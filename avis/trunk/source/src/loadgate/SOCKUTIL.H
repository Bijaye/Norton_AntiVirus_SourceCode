//===========================================================================
// 
//                               SOCKUTIL.H
// 
// All possible message types are defined here.
// 
//===========================================================================



#ifndef  _SOCKUTIL_H_
#define  _SOCKUTIL_H_


//---------------------------------------------------------------------------
// Definitions.

// Makes 'DBGprintf' active only for debug builds.
#if defined (_DEBUG) || defined (FORCE_DEBUG_PRINTF)
   #define  DBGprintf  printf
#else
   #pragma warning (disable : 4710)
   inline VOID DBGprintf (char *, ...) {}
#endif

//---------------------------------------------------------------------------
// Classes.

//====================================
// Convenience syncronization class.
// This critical section has 2 behaviours:
//    1. Just as critical section. Usage: 
//          CriticalSection   *pCriticalSection = new CriticalSection;  // Definition.
//   // or: CriticalSection   MyCriticalSection;                        // or like this.
//
//          pCriticalSection->EnterCriticalSection(); // Usage in.
//          ...
//          pCriticalSection->LeaveCriticalSection(); // Out.
//
//    2. Stack base convenience critical section, that uses already existing one:
//          CriticalSection g_csExistingCS;    // Definition.
//          ...
//          CriticalSection StackBasedCS (&g_csExistingCS); // Usage.
//          This will be placed at a stack-level declarations, so that function will
//          be executed only after entering to already existing critical section.
//          Upon exiting the function - critical section will be "left" automatically.
   
class CriticalSection
{
private:
   CRITICAL_SECTION  CS;   // Maybe will be used (depends on constructor).
   CriticalSection   *pCS; // Will points to the real critical section.
public:
   CriticalSection   (CriticalSection *pCS_)    {  pCS = pCS_; EnterCriticalSection();    }
   CriticalSection   ()             { ::InitializeCriticalSection (&CS); pCS = this;      }
   ~CriticalSection  ()             { if (pCS==this) ::DeleteCriticalSection (&CS); else LeaveCriticalSection();  }
   CRITICAL_SECTION *GetCriticalSection (VOID)  {  return (&CS);                          }
   VOID EnterCriticalSection (VOID) { ::EnterCriticalSection (pCS->GetCriticalSection()); }
   VOID LeaveCriticalSection (VOID) { ::LeaveCriticalSection (pCS->GetCriticalSection()); }
};
//====================================
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Function prototypes.

extern int     FindDWORDInDataBuffer   (DWORD dwSearchFor, char *pDataBuffer, DWORD dwBufferSize);
extern VOID    GetHostNameAndIP        (char *pHostName, char *pHostIPAddr, struct sockaddr_in *pAddr = NULL);
extern BOOL    StringIsDottedIPAddress (char *pString);
extern BOOL    Fill_sockaddr_in_Structure (
                                       char           *pHostNameOrIPAddress,
                                       unsigned short usPortNumber,
                                       struct sockaddr_in *pSockAddr,
                                       int            *piLastError = NULL
                                       );
extern BOOL    DataBeginsWith          (const char *pPatternZ, const char *pData, DWORD dwDataSize, BOOL bCaseSensitive = FALSE);
extern BOOL    DataEndsWith            (const char *pPatternZ, const char *pData, DWORD dwDataSize, BOOL bCaseSensitive = FALSE);
extern VOID    PrintfStreamingData     (const char *pData, DWORD dwDataSize, WORD wColor);
//---------------------------------------------------------------------------


#endif   // _SOCKUTIL_H_
