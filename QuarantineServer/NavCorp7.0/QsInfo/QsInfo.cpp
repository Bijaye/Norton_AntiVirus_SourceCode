// QsInfo.cpp : Defines the entry point for the DLL application.
//

//
//  Special defines & typedefs for NLM
//
#ifdef NLM
typedef unsigned long       ULONG;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef BYTE far            *LPBYTE;
typedef unsigned long       DWORD;	
typedef int                 BOOL;
typedef char*               LPSTR;

#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_PATH          260
#define MAX_COMPUTERNAME_LENGTH 48
#endif //.. NLM special defines

//  Note:
//  When using pre-compiled headers, for some curious reason, you cannot
//  put ifdefs around #include "stdafx.h". The precompiler will complain
//  about the endif. So I've put ifndef NLM _inside_ stdafx.h
//
#include "stdafx.h"

#ifndef NLM

#include "qspak.h"
#include "qsfields.h"
#include "qsinfo.h"
#include "global.h"
#include <tchar.h>
#include <winnls.h>
#include <winsock.h>
#include <wsipx.h>

#else

#include <iostream.h>
#include <assert.h>
#include <nwconn.h>
#include <tchar.h>
typedef LPSTR               LPTSTR;

#include <nwenvrn.h>
#include <nwadv.h>

#include "qspak.h"
#include "qsfields.h"
#include "qsinfo.h"

#endif  //.. NLM

//////////////////////////////////////////////////////////////////////////////
// Local defines
#define QSINFO_KEY_CURRENT_VER_NT   _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")
#define QSINFO_KEY_CURRENT_VER_9X   _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion")
#define QSINFO_KEY_HARDWARE         _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0")

// Common Registry Values
#define QSINFO_VAL_OWNER            _T("RegisteredOwner")
#define QSINFO_VAL_ORGANIZATION     _T("RegisteredOrganization")
#define QSINFO_VAL_PROCESSOR        _T("Identifier")
#define QSINFO_VAL_VENDOR           _T("VendorIdentifier")

// Win NT version info strings
#define QSINFO_VAL_CURRENTVERSION   _T("CurrentVersion")
#define QSINFO_VAL_CSDVERSION       _T("CSDVersion")
#define QSINFO_VAL_BUILD            _T("CurrentBuildNumber")
#define QSINFO_KEY_INTERNATIONAL    _T("ControlPanel\\International")
#define QSINFO_VAL_LOCALE           _T("Locale")


// Win 9x-specific strings
#define QSINFO_VAL_VERSION          _T("Version")
#define QSINFO_VAL_VERSIONNUMBER    _T("VersionNumber")
#define QSINFO_KEY_LOCALE           _T("System\\CurrentControlSet\\control\\Nls\\Locale")

#define QSINFO_MAX_BUFFER_SIZE      1024


//////////////////////////////////////////////////////////////////////////////
// typedefs for function pointers
#ifndef NLM //.. not used for NLM
typedef NET_API_STATUS ( NET_API_FUNCTION *PFNNetWkstaGetInfo)
                                (LPTSTR szServerName, DWORD level, LPBYTE* pBuffer);
#endif                                

typedef QSPAKSTATUS (*PFNQsPakQueryItemValue)( HQSERVERITEM hItem,
                                const char * szValue,
                                QSPAKDATATYPE eType,
                                void * pBuffer,
                                unsigned long * pulBufferLength );

typedef QSPAKSTATUS (*PFNQsPakSetItemValue)( HQSERVERITEM hItem,
                                const char * szValue,
                                QSPAKDATATYPE eType,
                                void * pData,
                                unsigned long ulDataLength );


//////////////////////////////////////////////////////////////////////////////
// Global data
#ifndef NLM

HINSTANCE                   g_hInstNetApi32 = NULL;
HINSTANCE                   g_hInstQSPak32 = NULL;

PFNNetWkstaGetInfo          g_pfnNetWkstaGetInfo = NULL;
PFNQsPakSetItemValue        g_pfnSetItemValue = NULL;
PFNQsPakQueryItemValue      g_pfnQueryItemValue = NULL;

#else //.. for NLM we have the functions locally

PFNQsPakSetItemValue        g_pfnSetItemValue = QsPakSetItemValue;
PFNQsPakQueryItemValue      g_pfnQueryItemValue = QsPakQueryItemValue;

#endif

TCHAR                       g_szMiscInfo1[QSINFO_MAX_BUFFER_SIZE] = {0};
TCHAR                       g_szMiscInfo2[QSINFO_MAX_BUFFER_SIZE] = {0};
DWORD                       g_dwMiscInfo1Size = QSINFO_MAX_BUFFER_SIZE;
DWORD                       g_dwMiscInfo2Size = QSINFO_MAX_BUFFER_SIZE;


//////////////////////////////////////////////////////////////////////////////
// Local functions
BOOL GetStandardData(HQSERVERITEM hItem);
BOOL GetOptionalData(HQSERVERITEM hItem);
BOOL GetFileData(HQSERVERITEM hItem, LPTSTR lpFileName);
#ifndef NLM
BOOL GetRegInfo(HKEY hTopKey, LPTSTR lpRegKey, LPTSTR lpRegValue, LPTSTR lpOutString, LPDWORD lpdwOutSize);
BOOL GetIPAddress(LPTSTR lpIPAddress);
#endif
BOOL GetIPXAddress(LPTSTR lpIPAddress);
void ResetMiscBuffers();

#ifndef NLM
////////////////////////////////////////////////////////////////////////////
// Function name    : DllMain
//
// Description      : 
//                    
// Arguments        : 
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    BOOL    bReturn = TRUE;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:

            // NT? We need to get extra help.
            if( IsWinNT() )
            {

                // Load our helper library for NT
                g_hInstNetApi32 = LoadLibrary( _T("NetApi32.DLL") );                
            
                if (!g_hInstNetApi32)
                    bReturn = FALSE;
                else
                {
                    // Get our function pointer
                    g_pfnNetWkstaGetInfo = (PFNNetWkstaGetInfo) GetProcAddress( g_hInstNetApi32,
                                                                _T( "NetWkstaGetInfo") );
                }
            }

            // Load the packaging library            
            if (bReturn)
            {
                g_hInstQSPak32 = LoadLibrary( _T("QSPAK32.DLL") );
                
                if (!g_hInstQSPak32)
                    bReturn = FALSE;
                else
                {
                    // Get function pointers to manipulate package items.
                    g_pfnSetItemValue = (PFNQsPakSetItemValue) GetProcAddress( g_hInstQSPak32, _T( "QsPakSetItemValue") );
                    g_pfnQueryItemValue = (PFNQsPakQueryItemValue) GetProcAddress( g_hInstQSPak32, _T( "QsPakQueryItemValue") );
                }
            }

            break;

		case DLL_THREAD_ATTACH:
            break;

		case DLL_THREAD_DETACH:
            break;

		case DLL_PROCESS_DETACH:

            // Free all the DLLs loaded when we detach.
            if( g_hInstQSPak32 )
            {
                FreeLibrary(g_hInstQSPak32);
                g_hInstQSPak32 = NULL;
            }

            if( g_hInstNetApi32 )
            {
                FreeLibrary( g_hInstNetApi32 );
                g_hInstNetApi32 = NULL;
            }

			break;
    }
    return (bReturn);
}

#endif

////////////////////////////////////////////////////////////////////////////
// Function name    : QsInfoGetSystemData
//
// Description      : 
//                    
// Arguments        : 
//
// Return type      : 
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
QSINFOSTATUS QSINFO_API QsInfoGetSystemData (HQSERVERITEM hItem, LPTSTR lpFileToQuarantine)
{

    // Get the standard QSInfo data
    if ( !GetStandardData(hItem) )
    {
        // Return some error
    }


    // Get the optional QSInfo data.
    GetOptionalData(hItem);


    // If we have a file name, get the file data.
    if (lpFileToQuarantine)
    {
        GetFileData(hItem, lpFileToQuarantine);
    }


    return QSINFOSTATUS_OK; 
}



////////////////////////////////////////////////////////////////////////////
// Function name    : GetStandardData
//
// Description      : 
//                    
// Arguments        : 
//
// Return type      : 
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
#ifndef NLM
BOOL GetStandardData(HQSERVERITEM hItem)
{
    QSPAKSTATUS     qsError    = QSPAKSTATUS_OK;
    OSVERSIONINFO   stOsVersionInfo = {0};
    TCHAR           *pLangInfo = NULL;
    LPTSTR          lpCurrentVer = NULL;
    LPHOSTENT       lpHostEnt = NULL;

    ////////////////////////////////////////////////////////////////////////////
    // Processor and Vendor
    ////////////////////////////////////////////////////////////////////////////
    if ( GetRegInfo( HKEY_LOCAL_MACHINE, QSINFO_KEY_HARDWARE, QSINFO_VAL_PROCESSOR, 
                     g_szMiscInfo1, &g_dwMiscInfo1Size) )
    {
        if ( GetRegInfo( HKEY_LOCAL_MACHINE, QSINFO_KEY_HARDWARE, QSINFO_VAL_VENDOR, 
                         g_szMiscInfo2, &g_dwMiscInfo2Size) )
        {
            // Build up a concat'd string ...
            TCHAR szHardware[QSINFO_MAX_BUFFER_SIZE] = {0};

            wsprintf(szHardware,"%s %s", g_szMiscInfo2, g_szMiscInfo1);

            // Add it to the file
            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_PROCESSOR,
                             QSPAK_TYPE_STRING,
                             (void*) szHardware,
                             _tcslen(szHardware));                        
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Get the owner and organization
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();

    if ( IsWinNT() )
        lpCurrentVer = QSINFO_KEY_CURRENT_VER_NT;
    else
        lpCurrentVer = QSINFO_KEY_CURRENT_VER_9X;
        
    if ( GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_OWNER, 
                     g_szMiscInfo1, &g_dwMiscInfo1Size) )
    {
        if ( GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_ORGANIZATION, 
                         g_szMiscInfo2, &g_dwMiscInfo2Size) )
        {
            // Build up a concat'd string ...
            TCHAR szOwnerOrg[QSINFO_MAX_BUFFER_SIZE] = {0};

            wsprintf(szOwnerOrg,"%s  %s", g_szMiscInfo1, g_szMiscInfo2);

            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_OWNER,
                             QSPAK_TYPE_STRING,
                             (void*) szOwnerOrg,
                             _tcslen(szOwnerOrg));                        
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Get OS and version
    ////////////////////////////////////////////////////////////////////////////
    ResetMiscBuffers();
    TCHAR szOSInfo[QSINFO_MAX_BUFFER_SIZE] = {0};

    if ( IsWinNT() )
    {
        TCHAR   szCSD[QSINFO_MAX_BUFFER_SIZE] = {0};
        DWORD   dwCSDSize = QSINFO_MAX_BUFFER_SIZE;

        lpCurrentVer = QSINFO_KEY_CURRENT_VER_NT;

        if ( GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_CURRENTVERSION, g_szMiscInfo1, &g_dwMiscInfo1Size ) 
            && GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_BUILD, g_szMiscInfo2, &g_dwMiscInfo2Size ) )
        {
            GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_CSDVERSION, szCSD, &dwCSDSize );

            wsprintf(szOSInfo,
                     "%s  %s %s %s", 
                     _T("Windows NT"), g_szMiscInfo1, _T("Build"), g_szMiscInfo2, szCSD );
        }
    }
    else
    {
        lpCurrentVer = QSINFO_KEY_CURRENT_VER_9X;

        if ( GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_VERSION, g_szMiscInfo1, &g_dwMiscInfo1Size )
            && GetRegInfo( HKEY_LOCAL_MACHINE, lpCurrentVer, QSINFO_VAL_VERSIONNUMBER, g_szMiscInfo2, &g_dwMiscInfo2Size ) )
        {
            wsprintf(szOSInfo,"%s %s %s", g_szMiscInfo1, _T("Version"), g_szMiscInfo2); 
        }
    }

    if ( _tcslen(szOSInfo) )
    {
        qsError = g_pfnSetItemValue(
                        hItem, 
                        QSERVER_ITEM_INFO_SYSTEM,
                        QSPAK_TYPE_STRING,
                        (void*) szOSInfo,
                        _tcslen(szOSInfo));                        
    }


    ////////////////////////////////////////////////////////////////////////////
    // Domain (i.e. LDVP Domain)
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();

    if ( GetDomainName(g_szMiscInfo1, g_dwMiscInfo1Size) )
    {
        // If we have the domain info, fill in  
        // the fields in the package.

        if ( _tcslen(g_szMiscInfo1) )
        {
            qsError = g_pfnSetItemValue(
                            hItem, 
                            QSERVER_ITEM_INFO_DOMAINNAME,
                            QSPAK_TYPE_STRING,
                            (void*) g_szMiscInfo1,
                            _tcslen(g_szMiscInfo1));                        
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Correlator ... not right now
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    // Host
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();

    if (GetComputerName(g_szMiscInfo1, &g_dwMiscInfo1Size))
    {
        lpHostEnt = gethostbyname(g_szMiscInfo1);

        if (lpHostEnt)
        {
            // Add it to the file
            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_HOST,
                             QSPAK_TYPE_STRING,
                             (void*) lpHostEnt->h_name,
                             _tcslen(lpHostEnt->h_name));                        
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Network addresses
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();

    if ( GetIPAddress(g_szMiscInfo1) && GetIPXAddress(g_szMiscInfo2) )
    {
        TCHAR szAddressList[QSINFO_MAX_BUFFER_SIZE] = {0};

        wsprintf(szAddressList, "%s %s", g_szMiscInfo1, g_szMiscInfo2);

        if ( _tcslen(szAddressList) )
        {
            // Add it to the file
            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_MACHINE_ADDRESS,
                             QSPAK_TYPE_STRING,
                             (void*) szAddressList,
                             _tcslen(szAddressList));                        
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Language
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();

    if ( GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, g_szMiscInfo1, g_dwMiscInfo1Size) )
    {
        if ( _tcslen(g_szMiscInfo1) )
        {
            // Add it to the file
            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_LANGUAGE,
                             QSPAK_TYPE_STRING,
                             (void*) g_szMiscInfo1,
                             g_dwMiscInfo1Size );    
        }
    }

    return TRUE;
}
#else //.. if this is NLM

BOOL GetStandardData(HQSERVERITEM hItem)
{
    QSPAKSTATUS     qsError    = QSPAKSTATUS_OK;


	
    ////////////////////////////////////////////////////////////////////////////
    // Processor and Vendor
    ////////////////////////////////////////////////////////////////////////////
   
	//dont know got to find a way???

    ////////////////////////////////////////////////////////////////////////////
    // Get the owner and organization
    ////////////////////////////////////////////////////////////////////////////
	
	//not available in nw

    ////////////////////////////////////////////////////////////////////////////
    // Get OS and version
    ////////////////////////////////////////////////////////////////////////////
   
    ResetMiscBuffers();

	TCHAR szOSInfo[QSINFO_MAX_BUFFER_SIZE] = {0};
	TCHAR szCompanyName[80];
	TCHAR szRevision[80];
	TCHAR szRevisionDate[24];
	TCHAR szCopyrightNotice[80];
	int nCcode;
	memset(szCompanyName, 0 , 80);
	memset(szRevision, 0 , 80);
	nCcode = GetFileServerDescriptionStrings(szCompanyName, szRevision, szRevisionDate, szCopyrightNotice);
	if(nCcode == 0)
	{
		sprintf(g_szMiscInfo1,"%s %s", szCompanyName, szRevision);
		_tcscpy(szOSInfo, g_szMiscInfo1);
		if( _tcslen(szOSInfo) )
		{
			qsError = g_pfnSetItemValue(
            	            hItem, 
                	        QSERVER_ITEM_INFO_SYSTEM,
                    	    QSPAK_TYPE_STRING,
                        	(void*) szOSInfo,
	                        _tcslen(szOSInfo));  
	            
		}
		
		
			
	}
	
	////////////////////////////////////////////////////////////////////////////
    // Domain (i.e. LDVP Domain)
    ////////////////////////////////////////////////////////////////////////////
	
	//not available in nw

    ////////////////////////////////////////////////////////////////////////////
    // Correlator ... not right now
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    // Host
    ////////////////////////////////////////////////////////////////////////////

	//not available in nw		
		
    ////////////////////////////////////////////////////////////////////////////
    // Network addresses
    ////////////////////////////////////////////////////////////////////////////

	ResetMiscBuffers();
	//for nw only ipx address
	if ( GetIPXAddress(g_szMiscInfo2) )
	{
		// Add it to the file
            qsError = g_pfnSetItemValue(
                             hItem, 
                             QSERVER_ITEM_INFO_MACHINE_ADDRESS,
                             QSPAK_TYPE_STRING,
                             (void*) g_szMiscInfo2,
                             _tcslen(g_szMiscInfo2));         
	}

    ////////////////////////////////////////////////////////////////////////////
    // Language
    ////////////////////////////////////////////////////////////////////////////

    ResetMiscBuffers();
	int nLanguageID;
	BYTE bLangName[255];
	nLanguageID = GetCurrentOSLanguageID();
	if(!ReturnLanguageName(nLanguageID, bLangName))
	{
		_tcscpy(g_szMiscInfo1, (TCHAR*)bLangName);
		// Add it to the file
        qsError = g_pfnSetItemValue(
                        hItem, 
                        QSERVER_ITEM_INFO_LANGUAGE,
                        QSPAK_TYPE_STRING,
                        (void*) g_szMiscInfo1,
                        _tcslen(g_szMiscInfo1) );  
	}

    return TRUE;
}
#endif

////////////////////////////////////////////////////////////////////////////
// Function name    : GetOptionalData
//
// Description      : 
//                    
// Arguments        : 
//
// Return type      : 
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetOptionalData(HQSERVERITEM hItem)
{
    QSPAKSTATUS     qsError = QSPAKSTATUS_OK;
    char            szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
    unsigned long   uComputerNameSize = MAX_COMPUTERNAME_LENGTH + 1;
    
    // Machine Name
    qsError = g_pfnQueryItemValue(hItem, 
                                  QSERVER_ITEM_INFO_MACHINENAME,
                                  QSPAK_TYPE_STRING,
                                  (void*)szComputerName,
                                  &uComputerNameSize);

	
#ifndef NLM
    
    if (qsError == QSPAKSTATUS_NO_SUCH_FIELD)
    {
        if (GetComputerName(szComputerName, &uComputerNameSize))
        {
            // If we get the name, then fill in the 
            // value in the package.
        }
    }
#else
	if (qsError == QSPAKSTATUS_NO_SUCH_FIELD)
    {
		int nCcode, nStructSize = 128;
		FILE_SERV_INFO siBuf;
		memset(szComputerName, 0 , uComputerNameSize);
		nCcode = GetServerInformation(nStructSize, &siBuf);
		if( nCcode == 0)
		{
			// If we get the name, then fill in the 
	        // value in the package.
    	    _tcscpy( szComputerName, siBuf.serverName);
		}	
	}
#endif
		
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : GetFileData
//
// Description      : 
//                    
// Arguments        : 
//
// Return type      : 
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetFileData(HQSERVERITEM hItem, LPTSTR lpFileName)
{
    return TRUE;
}


#ifndef NLM
////////////////////////////////////////////////////////////////////////////
// Function name    : GetRegInfo
//
// Description      : 
//                    
// Arguments        : 
//
// Return type      : 
//
////////////////////////////////////////////////////////////////////////////
// 2/17/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetRegInfo(HKEY hTopKey, LPTSTR lpRegKey, LPTSTR lpRegValue, LPTSTR lpOutString, LPDWORD lpdwOutSize)
{
    HKEY    hKey = NULL;
    BOOL    bRet = FALSE;
    DWORD   dwType = REG_SZ;
    long    lResult;

    lResult = RegOpenKeyEx( hTopKey,
                    lpRegKey,
                    0,
                    KEY_READ,
                    &hKey );

    if( ERROR_SUCCESS == lResult )
        {
        // Read data.
        lResult = RegQueryValueEx( hKey,
                                    lpRegValue,
                                    NULL,
                                    &dwType,
                                    (LPBYTE) lpOutString,
                                    lpdwOutSize );

        if( ERROR_SUCCESS == lResult  && *lpdwOutSize )
            bRet = TRUE;

        // Cleanup.
        RegCloseKey(hKey);
        }

    return bRet;
}
#endif

#ifndef NLM
////////////////////////////////////////////////////////////////////////////////
//
// Function name	: GetIPXAddress()
// Description	    : Uses WinSock to retrieve our IPX address.
// Argument         : LPTSTR lpIPXAddress
//
////////////////////////////////////////////////////////////////////////////////
// 11/30/97 - DBUCHES function created / header added
// 02/24/99 - TCASHIN took this from NSDU and updated it for use here.
////////////////////////////////////////////////////////////////////////////////
BOOL GetIPXAddress(LPTSTR lpIPXAddress)
{
    WSADATA             WSAData;
    SOCKET              TempSock;
    sockaddr_ipx        SockAddr;
    BOOL                bRet = FALSE;
    WORD                wWinsockVersion = 0x0101;    // need Winsock 1.1
    TCHAR               szAddress[MAX_PATH] = {0};

    // Init WinSock.
    int iResult = WSAStartup (wWinsockVersion, &WSAData);
    if( iResult != 0 )
        return FALSE;

    // Construct a local socket
    TempSock = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if( TempSock == INVALID_SOCKET )
        goto cleanup;

    // Clear sock address struct
    ZeroMemory(&SockAddr, sizeof(sockaddr_ipx));     //Clear adress
    SockAddr.sa_family = AF_IPX;

    // Bind to this object
    iResult = bind(TempSock, (PSOCKADDR)&SockAddr, sizeof(SOCKADDR_IPX));
    if( iResult == 0 )
        {
        int iSize = sizeof(SOCKADDR_IPX);

        // Get IPX address
        if( getsockname ( TempSock, (LPSOCKADDR)&SockAddr, &iSize ) == 0 )
            {

            // Build RPC compatible address string
            wsprintf ( szAddress,
                       "%02X%02X%02X%02X:%02X%02X%02X%02X%02X%02X",
                       (unsigned char)SockAddr.sa_netnum[0],
                       (unsigned char)SockAddr.sa_netnum[1],
                       (unsigned char)SockAddr.sa_netnum[2],
                       (unsigned char)SockAddr.sa_netnum[3],
                       (unsigned char)SockAddr.sa_nodenum[0],
                       (unsigned char)SockAddr.sa_nodenum[1],
                       (unsigned char)SockAddr.sa_nodenum[2],
                       (unsigned char)SockAddr.sa_nodenum[3],
                       (unsigned char)SockAddr.sa_nodenum[4],
                       (unsigned char)SockAddr.sa_nodenum[5] );

            _tcscpy( lpIPXAddress, LPCTSTR(szAddress) );

            bRet = TRUE;
            }
        }

    // Cleanup
    closesocket (TempSock);

cleanup:

    // Shut down WinSock
    WSACleanup();

    return bRet;
}
#else // ..if this is NLM

BOOL GetIPXAddress(LPTSTR lpIPXAddress)
{
	
	TCHAR           szAddress[MAX_PATH] = {0} ;
	int nCcode;
	TCHAR 		   szNetworkNumber[4];
	TCHAR           szNodeAddress[6]; 
	nCcode = GetInternetAddress( GetConnectionNumber(), szNetworkNumber, szNodeAddress);
	if( nCcode  != 0)
		return FALSE;
	memset(szAddress, 0, MAX_PATH);
	sprintf (szAddress, "%02X%02X%02X%02X:%02X%02X%02X%02X%02X%02X",
          				szNetworkNumber[0], szNetworkNumber[1], 
          				szNetworkNumber[2], szNetworkNumber[3],
          				szNodeAddress[0], szNodeAddress[1],
          				szNodeAddress[2], szNodeAddress[3],
          				szNodeAddress[4], szNodeAddress[5]
           );
	assert( lpIPXAddress != NULL);
   _tcscpy( lpIPXAddress, szAddress );       
   return TRUE;
}
#endif

#ifndef NLM
////////////////////////////////////////////////////////////////////////////////
//
// Function name    : GetIPAddress()
// Description      : Uses WinSock to retrieve our IP address.
// Argument         : LPTSTR lpIPAddress
//
////////////////////////////////////////////////////////////////////////////////
// 11/30/97 - DBUCHES function created / header added
// 02/24/99 - TCASHIN took this from NSDU and updated it for use here.
////////////////////////////////////////////////////////////////////////////////
BOOL GetIPAddress(LPTSTR lpIPAddress)
{
    WSADATA             WSAData;
    char                szHostName [MAX_PATH];
    struct hostent      *pHostEntry;                // pointer to host entry
    BOOL                bRet = FALSE;
    WORD                wWinsockVersion = 0x0101;    // need Winsock 1.1
    TCHAR               szAddress[MAX_PATH] = {0};


    // Init WinSock.
    int iResult = WSAStartup (wWinsockVersion, &WSAData);
    if( iResult != 0 )
        return FALSE;

    //
    //  Get the host name and the IP address from of the local machine.
    //
	gethostname (szHostName, sizeof (szHostName));
    pHostEntry = gethostbyname (szHostName);

    //
    //  If gethostbyname worked, construct the IP address string.
    //  Otherwise, cleanup and exit.
    //
    if (NULL != pHostEntry)
        {
        //
        //  Construct the IP address string
        //
        wsprintf ( szAddress, 
                 _T( "%u.%u.%u.%u" ),
			     (unsigned char) pHostEntry->h_addr_list[0][0],
    			 (unsigned char) pHostEntry->h_addr_list[0][1],
	    		 (unsigned char) pHostEntry->h_addr_list[0][2],
		    	 (unsigned char) pHostEntry->h_addr_list[0][3] );

        _tcscpy(lpIPAddress, szAddress);

        bRet = TRUE;
        }


    // Shut down WinSock
    WSACleanup();

    return bRet;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : 
// Description      : 
// Argument         : None
//
////////////////////////////////////////////////////////////////////////////////
// 02/24/99 - TCASHIN function created / header added
////////////////////////////////////////////////////////////////////////////////
void ResetMiscBuffers()
{
    memset(g_szMiscInfo1, 0, QSINFO_MAX_BUFFER_SIZE);
    memset(g_szMiscInfo2, 0, QSINFO_MAX_BUFFER_SIZE);

    g_dwMiscInfo1Size = QSINFO_MAX_BUFFER_SIZE;
    g_dwMiscInfo2Size = QSINFO_MAX_BUFFER_SIZE;
}
