//===========================================================================
//
//                               SOCKUTIL.CPP
//
//
// Common definitions and helper functions for socket-related networking.
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.
//---------------------------------------------------------------------------

//===========================================================================
//
// Returns:
//    -1:if not found
//    else:offset of given DWORD in buffer.

int   FindDWORDInDataBuffer  (DWORD dwSearchFor, char *pDataBuffer, DWORD dwBufferSize)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   if (dwBufferSize < sizeof(dwSearchFor))
      return (-1);

   for (i=0; i<(int)(dwBufferSize - sizeof(dwSearchFor) + 1); i++)
      if (dwSearchFor == *(DWORD *)(pDataBuffer + i))
         return (i);

   return (-1);
}
//===========================================================================

//===========================================================================
//
// Returns host name and IP address, using 'struct sockaddr_in *pAddr'
// structure.
// If 'pAddr' == NULL => get local machine's name and IP.
//
// 'pHostName' and 'pHostIPAddr' must be sufficient enough to hold info.
// 'pHostName'   - at least MAX_PATH chars.
// 'pHostIPAddr' - at least 20 chars.

VOID  GetHostNameAndIP  (char *pHostName, char *pHostIPAddr, struct sockaddr_in *pAddr /* = NULL*/)
{
//---------------------------------------------------------------------------
///!!!   WSADATA              wsData;
   struct hostent       *pHostEnt;
   struct in_addr       *pInAddr;
   char                 *pIPAddr;

   int                  iRetCode;
//---------------------------------------------------------------------------

 
///!!!   // Initialize socket library first.
///!!!   //
///!!!   iRetCode =
///!!!      WSAStartup (   // int. 0-success, error code otherwise.
///!!!         MAKEWORD (1, 1),  // WORD wVersionRequested
///!!!         &wsData           // LPWSADATA lpWSAData
///!!!         );   
///!!!   if (iRetCode != 0)
///!!!      {
///!!!      DBGprintf ("\nCannot initialize socket library.");
///!!!      goto  ABORT;
///!!!      }


// WINSOCK.H:
// Structure used by kernel to store most addresses.
// 
// struct sockaddr {
//       u_short sa_family;   // address family.
//       char    sa_data[14]; // up to 14 bytes of direct address.
//       };
//
// struct in_addr 
//    {
//    union 
//       {
//       struct { u_char  s_b1,s_b2,s_b3,s_b4; }   S_un_b;
//       struct { u_short s_w1,s_w2; }             S_un_w;
//       u_long S_addr;
//       } S_un;
//    };
//
// Socket address, internet style.
// For AF_INET 'sockaddr' will be the following 'sockaddr_in':
// struct sockaddr_in      // == sockaddr for TCP/IP:
//    {
//    short             sin_family;    // AF_[INET|*].
//    u_short           sin_port;      // 0:service provider assigned value: 1024-5000(use getsockname() to get assigned port number).
//    struct  in_addr   sin_addr;      // 4-byte internet addr. '.S_un.S_addr' = htonl(INADDR_ANY) - host machine.
//    char              sin_zero[8];   // Padding - must be 0 for 'bind()' call.
//    };
//
// struct hostent { 
//     char     *h_name;         // Official name of the host (PC).
//     char    **h_aliases;      // A NULL-terminated array of alternate names.
//     short     h_addrtype;     // The type of address being returned (WSock: PF_INET).
//     short     h_length;       // The length, in bytes, of each address (PF_INET: 4)
//     char    **h_addr_list;    // A NULL-terminated list of addresses in net byte order.
// }; 

   if (!pAddr)
      {
      // Get local machine's name right away.
      // 
      iRetCode =
         gethostname (  // int: SOCKET_ERROR:fail, 0:success.
            pHostName,  // char *name: standard host name for the local machine.
            MAX_PATH    // size of supplied buffer.
            );
      if (iRetCode == SOCKET_ERROR)
         goto  ABORT;

      // Get pointer to the hostent structure
      pHostEnt =
         gethostbyname (   // struct HOSTENT *: NULL:fail, else:success.
            pHostName      // const char *name
            );
      if (!pHostEnt)
         goto  ABORT;

      // Get local machine's dotted IP.      
      pInAddr = (struct in_addr *)pHostEnt->h_addr_list[0];
      if (!pInAddr)
         goto  ABORT;

      pIPAddr = inet_ntoa (*pInAddr);
      if (pIPAddr)
         strcpy (pHostIPAddr, pIPAddr);
      else
         goto  ABORT;

      goto  EXIT;
      }
   else
      {
      // 'pAddr' is non-NULL.
      //
      // Assemble host dotted IP address.
      pIPAddr =
         inet_ntoa ( // char FAR *: success-ptr to addr:"9.21.3.46", else NULL.
            pAddr->sin_addr // struct in_addr in: A structure which represents an Internet host address.
            );   
      if (pIPAddr)
         strcpy  (pHostIPAddr, pIPAddr);
      else
         goto  ABORT;
      
      // Get host's official name.
      pHostEnt =
         gethostbyaddr ( // struct hostent FAR *: error:NULL
            (char *)&pAddr->sin_addr.s_addr, // const char FAR *addr: A pointer to an address in network byte order.
            4,       // int len: The length of the address, which must be 4 for PF_INET addresses.
            PF_INET  // int type:   The type of the address, which must be PF_INET.
            );
      if (pHostEnt)
         strcpy (pHostName, pHostEnt->h_name);
      else
         strcpy (pHostName,   "***");

// Do not abort here.
//      else
//         goto  ABORT;

      goto  EXIT;
      }


EXIT:
///!!!   WSACleanup();
   return;

ABORT:
   // Reset hosts's name and IP addr.
   strcpy (pHostName,   "***");
   strcpy (pHostIPAddr, "*.*.*.*");

   goto  EXIT;
}
//===========================================================================

//===========================================================================
// 

BOOL  StringIsDottedIPAddress (char *pString)
{
//---------------------------------------------------------------------------
   int            i, j;
   char           acOctet[4]; // "123";
   int            iValue;
//---------------------------------------------------------------------------

/***
/////!!!!!!
Use: MS-0recommented techineuqe:
unsigned long inet_addr (

const char FAR * cp);
INADDR_NONE
***/


   if (!pString)
      return (FALSE);

   i=0;

//-----------------------------
// 1-st octet checkup.
   for (j=0; j<3; j++, i++)
      if (pString[i] >= '0' && pString[i] <= '9')
         acOctet[j] = pString[i];
      else
         break;
   acOctet[j] = 0;
   if (acOctet[0] == 0)
      return (FALSE);

   iValue = atoi (acOctet);
   if (iValue < 1 || iValue > 254)
      return (FALSE);
//-----------------------------

//-----------------------------
// 2-nd octet checkup.
   if (pString[i++] != '.')
      return (FALSE);

   for (j=0; j<3; j++, i++)
      if (pString[i] >= '0' && pString[i] <= '9')
         acOctet[j] = pString[i];
      else
         break;
   acOctet[j] = 0;
   if (acOctet[0] == 0)
      return (FALSE);

   iValue = atoi (acOctet);
   if (iValue < 0 || iValue > 254)
      return (FALSE);
//-----------------------------

//-----------------------------
// 3-rd octet checkup.
   if (pString[i++] != '.')
      return (FALSE);

   for (j=0; j<3; j++, i++)
      if (pString[i] >= '0' && pString[i] <= '9')
         acOctet[j] = pString[i];
      else
         break;
   acOctet[j] = 0;
   if (acOctet[0] == 0)
      return (FALSE);

   iValue = atoi (acOctet);
   if (iValue < 0 || iValue > 254)
      return (FALSE);
//-----------------------------

//-----------------------------
// 4-th octet checkup.
   if (pString[i++] != '.')
      return (FALSE);

   for (j=0; j<3; j++, i++)
      if (pString[i] >= '0' && pString[i] <= '9')
         acOctet[j] = pString[i];
      else
         break;
   acOctet[j] = 0;
   if (acOctet[0] == 0)
      return (FALSE);

   iValue = atoi (acOctet);
   if (iValue < 0 || iValue > 254)
      return (FALSE);
//-----------------------------

   return (TRUE);
}
//===========================================================================

//===========================================================================
//
// Function will try to resolve name to IP if name is given and then 
// initialize 'sockaddr_in' structure in a proper way.
// Returns:
//    TRUE  - structure is successfully initialized and (*piLastError) returns 0.
//    FALSE - initialization failed. Most likely because input couldn't be 
//            resolved.
//
// Note: 'WSAStartup()' must already be successfully called before using this
//       function.

BOOL  Fill_sockaddr_in_Structure (
         char           *pHostNameOrIPAddress,
         unsigned short usPortNumber,
         struct sockaddr_in    *pSockAddr,    // Caller' allocated space for structure.
         int            *piLastError   /* = NULL */ // WinSock's last error or -1.
         )
{
//---------------------------------------------------------------------------
   struct hostent *pHostEnt;
   BOOL           bRetCode = TRUE;
   int            iLastError = 0;
//---------------------------------------------------------------------------

   memset (pSockAddr, 0, sizeof (*pSockAddr));

   pSockAddr->sin_family = AF_INET;
   pSockAddr->sin_port   = htons (usPortNumber);

   // Initialize 'sin_addr' element of sockaddr_in structure.
   if (StringIsDottedIPAddress (pHostNameOrIPAddress))
      {
      // Initialize in_addr structure.
      // inet_addr() converts a string containing a dotted 
      // address into an 'in_addr' (out in network byte order).
      pSockAddr->sin_addr.S_un.S_addr =
         inet_addr ( // unsigned long: (already in network byte order).
            pHostNameOrIPAddress // const char FAR *cp: "9.21.3.46"
            );
      if (pSockAddr->sin_addr.S_un.S_addr == INADDR_NONE)
         {
         iLastError = -1;
         goto  ABORT;
         }
      }
   else
      {
      pHostEnt = gethostbyname (pHostNameOrIPAddress); // Convert it to number.
      if (pHostEnt == NULL)
         {
         iLastError = WSAGetLastError();
         goto  ABORT;
         }

      pSockAddr->sin_addr.S_un.S_addr = *((u_long *)pHostEnt->h_addr_list[0]);
      }


// WINSOCK.H:
// Structure used by kernel to store most addresses.
// 
// struct sockaddr {
//       u_short sa_family;   // address family.
//       char    sa_data[14]; // up to 14 bytes of direct address.
//       };
//
// struct in_addr 
//    {
//    union 
//       {
//       struct { u_char  s_b1,s_b2,s_b3,s_b4; }   S_un_b;
//       struct { u_short s_w1,s_w2; }             S_un_w;
//       u_long S_addr;
//       } S_un;
//    };
//
// Socket address, internet style.
// For AF_INET 'sockaddr' will be the following 'sockaddr_in':
// struct sockaddr_in      // == sockaddr for TCP/IP:
//    {
//    short             sin_family;    // AF_[INET|*].
//    u_short           sin_port;      // 0:service provider assigned value: 1024-5000(use getsockname() to get assigned port number).
//    struct  in_addr   sin_addr;      // 4-byte internet addr. '.S_un.S_addr' = htonl(INADDR_ANY) - host machine.
//    char              sin_zero[8];   // Padding - must be 0 for 'bind()' call.
//    };
//
// struct hostent { 
//     char     *h_name;         // Official name of the host (PC).
//     char    **h_aliases;      // A NULL-terminated array of alternate names.
//     short     h_addrtype;     // The type of address being returned (WSock: PF_INET).
//     short     h_length;       // The length, in bytes, of each address (PF_INET: 4)
//     char    **h_addr_list;    // A NULL-terminated list of addresses in net byte order.
// }; 

   bRetCode = TRUE;

EXIT:
   if (piLastError)
      *piLastError = iLastError;
   return (bRetCode);


ABORT:
   memset (pSockAddr, 0, sizeof (*pSockAddr));
   bRetCode = FALSE;
   goto  EXIT;
}
//===========================================================================

//===========================================================================
//
// Returns: TRUE: match, FALSE: - no match.

BOOL  DataBeginsWith (const char *pPatternZ, const char *pData, DWORD dwDataSize, BOOL bCaseSensitive /* = FALSE */)
{
//---------------------------------------------------------------------------
   int            iPatternLen;
   int            i;
//---------------------------------------------------------------------------

   iPatternLen = strlen (pPatternZ);

   if (iPatternLen > (int)dwDataSize)
      return (FALSE);

   for (i=0; i<iPatternLen; i++)
      {
      if (bCaseSensitive)
         {
         if (pPatternZ[i] != pData[i])
            return (FALSE);
         }
      else
         {
         if (toupper(pPatternZ[i]) != toupper(pData[i]))
            return (FALSE);
         }
      }

   return (TRUE);
}
//===========================================================================


//===========================================================================
//
// Returns: TRUE: match, FALSE: - no match.

BOOL  DataEndsWith (const char *pPatternZ, const char *pData, DWORD dwDataSize, BOOL bCaseSensitive /* = FALSE */)
{
//---------------------------------------------------------------------------
   int            iPatternLen;
   int            i, j;
//---------------------------------------------------------------------------

   iPatternLen = strlen (pPatternZ);

   if (iPatternLen > (int)dwDataSize)
      return (FALSE);

   for (i=0, j=dwDataSize-iPatternLen; i<iPatternLen; i++, j++)
      {
      if (bCaseSensitive)
         {
         if (pPatternZ[i] != pData[j])
            return (FALSE);
         }
      else
         {
         if (toupper(pPatternZ[i]) != toupper(pData[j]))
            return (FALSE);
         }
      }

   return (TRUE);
}
//===========================================================================

//===========================================================================
//
// 

VOID  PrintfStreamingData (const char *pData, DWORD dwDataSize, WORD wColor)
{
//---------------------------------------------------------------------------
   char              *pTextBuffer;
   int               i;
//---------------------------------------------------------------------------

   // Printf forwarded data on screen now.
   pTextBuffer = new char [dwDataSize + 1];
   memcpy (pTextBuffer, pData, dwDataSize);
   for (i=0; i<(int)dwDataSize; i++)
      if (pTextBuffer[i] < 0x0A)
         pTextBuffer[i] = '?';
   pTextBuffer[dwDataSize] = 0;
   textattr (wColor);
   printf ("\n---------------------- [%08X] ----------------------", GetCurrentThreadId());
   printf ("\n%s", pTextBuffer);
   textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE);
   delete [] pTextBuffer;
   pTextBuffer = NULL;
}
//===========================================================================
