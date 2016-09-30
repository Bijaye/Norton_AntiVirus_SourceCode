
/*------------------------------------------------------------------------
   UTIL.H
   
   Utility routines 
   
   Written by Jim Hill
------------------------------------------------------------------------*/



#ifndef _UTIL_H_DEFINED
   #define  _UTIL_H_DEFINED

#ifdef __cplusplus               // Define as "C" linkage
extern "C"
{
#endif



// GLOBAL DATA
extern TCHAR g_ServerDisplayName[MAX_PATH];
extern TCHAR g_szMorseFullPath[MAX_PATH];
//TCHAR g_szMorseIniPath[];





#define MAX_PRINTSTRING     2048

#ifndef NOERR
#   define NOERR        0               /* No error.                 */
#endif


// IN UTIL.C
HGLOBAL  MemAlloc(DWORD dwSize);
HGLOBAL  MemFree(HGLOBAL hHandle);


//void fPrintString(LPCSTR lpszFmt, ...);
void fPrintString(char *lpszFmt, ...);
void fDebugString(LPCSTR lpszFmt, ...);
void fWidePrintString(LPCSTR lpszFmt, ...);
LPTSTR RemoveMultipleCharacters(LPTSTR str,LPCTSTR chars);
void PrintLocaleInfo( LPCSTR lpszHeader );























#ifdef __cplusplus                                  
}
#endif

#endif