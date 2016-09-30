/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*------------------------------------------------------------------------
   UTIL.C
   
   Utility routines 
   
   Written by Jim Hill
------------------------------------------------------------------------*/


//#include  <windows.h>
//#include  <windowsx.h>
//#include  <tchar.h>   
#include <stdio.h>
#include  "localize.h"
#include  "util.h"


//#ifndef       STRICT
//    #define   STRICT
//#endif
//#ifndef SYM_WIN32
//   #define SYM_WIN32
//#endif
//#ifndef SYM_WIN
//   #define SYM_WIN
//#endif


#ifdef __cplusplus               // Define as "C" linkage
extern "C"
{
#endif



// CONTROL LOGGING   12/19/99 
#ifdef _DEBUG
    DWORD   dwUseDebugLogging = TRUE;
#else
    DWORD   dwUseDebugLogging = FALSE;
#endif
// DWORD dwUseDebugLogging   = TRUE;


// GLOBAL
HMODULE hModuleForDll = NULL;
TCHAR   g_ServerDisplayName[MAX_PATH];
TCHAR   g_szMorseFullPath[MAX_PATH];
//TCHAR   g_szMorseIniPath[MAX_PATH];
BYTE    szLogfileName[MAX_PATH];
TCHAR   szModuleFileNameAndDateTime[MAX_PATH];



//LOCAL
int WriteStatusMessage(BYTE *msg);
static DWORD dwFirstRun=TRUE;
DWORD GetLogFileName(void);


// DEFINES
#define _CHAR_NEXT(p)       CharNext(p)



HGLOBAL  MemAlloc(DWORD dwSize)
{
        return GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT , dwSize); 
}

HGLOBAL  MemFree(HGLOBAL hHandle)
{
        return GlobalFree( hHandle ); 
}




#define  USE_MILLISECONDS_IN_TIME_STRING     1

/*----------------------------------------------------------------------------
   GetTimeString()
   
   int iSeparatorChr     ','   Insert a comma
                         '|'   Insert a |
                          0    Use a space
   
   Passed buffer must be at least 20 bytes long.
   
   define TIME_STRING_SIZE  75
   
   LOCALE_SYSTEM_DEFAULT
   LCID GetSystemDefaultLCID(VOID);
   sServerInfo.dwLocaleSystemDefaultTime

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BYTE *GetTimeString(BYTE *lpszTimeString)
{
   SYSTEMTIME  SysTime;
   BYTE        szTime[50],szDate[50];
   //struct tm   *local;
   //time_t      Time;

   // GET THE LOCAL TIME ON THIS MACHINE
   GetLocalTime(&SysTime);
      
   if(GetDateFormatA(LOCALE_SYSTEM_DEFAULT,DATE_SHORTDATE,&SysTime,NULL,szDate,sizeof(szDate))==0)  {
      STRCPY(szDate,"00/00/00");
   }
   if(GetTimeFormatA(LOCALE_SYSTEM_DEFAULT,TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,&SysTime,NULL,szTime,sizeof(szTime))==0)  {
      STRCPY(szTime,"00:00");
   }

#if USE_MILLISECONDS_IN_TIME_STRING
   wsprintfA(lpszTimeString,"%s %s.%03d ",szDate,szTime,SysTime.wMilliseconds);  
#else      
   wsprintfA(lpszTimeString,"%s %s ",szDate,szTime);
#endif

   return(lpszTimeString);

}


//#include  <windows.h>

/*------------------------------------------------------------------------
   GetModuleNameAndDateTimeString     
   Get the exe Name and the file date and time.
   Written by: Jim Hill                          
------------------------------------------------------------------------*/
DWORD GetModuleNameAndDateTimeString(TCHAR *pszString)
{
   TCHAR            fullPath[MAX_PATH];
   HANDLE           findHandle;
   WIN32_FIND_DATA  findData;
   TCHAR            szDate[100];
   TCHAR            szTime[100];


   // if(GetModuleFileName(NULL,fullPath,sizeof(fullPath))==0) 
   if(GetModuleFileName( hModuleForDll, fullPath, sizeof(fullPath) ) == 0 ) 
      return(0);    

   memset(&findData,0,sizeof(WIN32_FIND_DATA));
   //memset(&FileTime,0,sizeof(FILETIME));
   if( (findHandle=FindFirstFile( fullPath, &findData)) != INVALID_HANDLE_VALUE)  
   {
      SYSTEMTIME SystemTime;    
      FileTimeToSystemTime( &findData.ftLastWriteTime, &SystemTime );
      FindClose(findHandle);

      GetDateFormat( LOCALE_SYSTEM_DEFAULT, 0, &SystemTime, _T("ddd, dd MMM yyyy"), szDate, 32 );               // NULL
      GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &SystemTime, _T("HH:mm:ss"), szTime, 32 );  // NULL

      if( pszString != NULL )
        wsprintf( pszString, _T("%s  %s  %s"), fullPath, szDate, szTime );

   }
   return(0);
}


/*----------------------------------------------------------------------------
   WriteStatusMessage()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int WriteStatusMessage(BYTE *msg)
{
   HANDLE   dwFileHandle;
   int      errcode = NOERR;   
   DWORD    bytesWritten = 0;
   DWORD	dwMsgLength,dwFileSize;
   BYTE     szTimeString[100];                      //BYTE
   BYTE     szOutputString[MAX_PRINTSTRING+1024];   //TCHAR
   //BYTE     szThreadID[50];
   DWORD    dwThreadID = GetCurrentThreadId();


   // GET THE DATE AND TIME
   GetTimeString(szTimeString);

   if(msg==NULL || (dwMsgLength=strlen(msg))==0)
      return(0);

   //7/6/00 THIS ALSO SAVES THE PATH USED FOR THE HELP FILE
   if(szLogfileName[0] == 0)    // "AvisConsole.log"
       GetLogFileName();

   // CONTROL LOGGING
   if( !dwUseDebugLogging )
   {
	  // FORMAT THE LINE
      wsprintfA(szOutputString,"AvisCon  %s  ------ START LOCATION  \r\n",szTimeString);
      OutputDebugStringA(szOutputString);
      wsprintfA(szOutputString,"%08X %s %s\r\n", dwThreadID, szTimeString, msg);
      OutputDebugStringA(szOutputString);
      return(0);
   }


   dwFileHandle= CreateFileA( szLogfileName,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,                  // No security attributes
                              OPEN_ALWAYS,           // iF NOT EXIST, THEN CREATE
                              FILE_ATTRIBUTE_NORMAL,
                              NULL                   // No template file
                             );
   if(dwFileHandle!=INVALID_HANDLE_VALUE)
   {

      // MOVE TO THE END OF THE FILE
      dwFileSize = SetFilePointer(dwFileHandle, 0, (LPLONG) NULL, FILE_END);


	  // IS THIS THE FIRST TIME CALLED?
	  if(dwFirstRun) { 
          char szFileNameAndDateTime[255];
          WideCharToMultiByte(CP_ACP,0,(LPCWSTR)szModuleFileNameAndDateTime,-1,szFileNameAndDateTime,sizeof(szFileNameAndDateTime),NULL,NULL);
        
	      dwFirstRun=FALSE;
	      //wsprintfA(szOutputString,"AvisCon  %s  ----------- STARTING ------------------------\r\n",szTimeString);
	      //wsprintfA(szOutputString,"AvisCon  %s  ------ START LOCATION  %s \r\n",szTimeString,szLogfileName);
	   	  wsprintfA(szOutputString,"AvisCon  %s  ------ STARTING  %s \r\n", szTimeString, szFileNameAndDateTime );
	      WriteFile(dwFileHandle,szOutputString,strlen(szOutputString),&bytesWritten,NULL) ;
	      OutputDebugStringA(szOutputString);
	  }

	  // FORMAT THE LINE
      // wsprintfA(szOutputString,"AvisCon %s %s\r\n", szTimeString, msg);
      wsprintfA(szOutputString,"%08X %s %s\r\n", dwThreadID, szTimeString, msg);

      // WRITE DATA 
      WriteFile(dwFileHandle,szOutputString,strlen(szOutputString),&bytesWritten,NULL) ;

      // CLOSE THE FILE
      CloseHandle(dwFileHandle);

      OutputDebugStringA(szOutputString);
   }
   else {
      errcode=GetLastError();
   }         
   
   return(0);
}

/*----------------------------------------------------------------------------
   fWidePrintString
   Assumes the format string is ANSI and the args can be WCHAR
 ----------------------------------------------------------------------------*/
void fWidePrintString(LPCSTR lpszFmt, ...)
{
    BYTE     szLine[MAX_PRINTSTRING];       //  TCHAR
	int      nBuf;
    WCHAR    szLineW[MAX_PRINTSTRING];
    WCHAR    szFormatBufferW[MAX_PRINTSTRING];
    //LPWSTR   lpWptr=szFormatBufferW;
	va_list args;

    memset(szLine,0,sizeof(szLine));
    memset(szLineW,0,sizeof(szLineW));
    memset(szFormatBufferW,0,sizeof(szFormatBufferW));

    //DebugBreak();

    // Convert the fmt string to wide
    if(!MultiByteToWideChar(CP_ACP,0,lpszFmt,-1,szFormatBufferW,sizeof(szFormatBufferW)/ sizeof(TCHAR)))   
       return;

	va_start(args, lpszFmt);

	nBuf = _vsnwprintf(szLineW, sizeof(szLineW) / sizeof(WCHAR), szFormatBufferW, args);

    if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)szLineW,-1,szLine,sizeof(szLine),NULL,NULL))
        WriteStatusMessage(szLine);

	va_end(args);

    return;
}


#include <locale.h>    // 7/15

/*----------------------------------------------------------------------------
    PrintLocaleInfo
 ----------------------------------------------------------------------------*/
void PrintLocaleInfo( LPCSTR lpszHeader )
{
    CHAR  szFmt[255];
    TCHAR *lpszLocale = NULL;
    TCHAR szLocale[259];
    lpszLocale = _tsetlocale( LC_ALL, NULL );   // English  _T("Japanese")
    szLocale[0] = 0;

    wsprintfA(szFmt, "%s  Locale= ", lpszHeader);   
    strcat(szFmt, "%s  %s");
    if( lpszLocale != NULL )
    {
        GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, sizeof(szLocale)); 
        fWidePrintString( szFmt, lpszLocale, szLocale );
    }

    return;
}


/*----------------------------------------------------------------------------
   fPrintString
 ----------------------------------------------------------------------------*/
void  fPrintString(char *lpszFmt, ...)    //  LPCSTR  
{
   CPINFO CodePageInfo;
   va_list ap;
   BYTE    szLine[MAX_PRINTSTRING],szFmt[512];
// TCHAR   szLine[MAX_PRINTSTRING],szFmt[512];

   // GET CODE PAGE INFO. TRANSLATE TO SINGLE BYTE?
   GetCPInfo(CP_ACP, &CodePageInfo);
   if(CodePageInfo.MaxCharSize == 1 && *(lpszFmt+1)==0)    {
      // LOOKS LIKE UNICODE, BETTER TRANSLATE IT TO ANSI
      if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)lpszFmt,-1,szFmt,sizeof(szFmt),NULL,NULL))
         lpszFmt=szFmt;
   }

   va_start(ap,lpszFmt);
//   wvsprintf(szLine,(LPCSTR)lpszFmt,ap);
   wsprintfA(szLine,lpszFmt,ap);
   va_end(ap);

   WriteStatusMessage(szLine);
   return;
}

#if 0
/*----------------------------------------------------------------------------
   fDebugString
 ----------------------------------------------------------------------------*/
void fDebugString(LPCSTR lpszFmt, ...)
{
   va_list ap;
   BYTE   szLine[MAX_PRINTSTRING];

   va_start(ap,lpszFmt);
   wvsprintf(szLine,lpszFmt,ap);
   va_end(ap);

   WriteStatusMessage(szLine);
   return;
}
#endif

/*----------------------------------------------------------------------------
   RemoveMultipleCharacters()

   Copy string s onto itself deleting any occurrances of the indicated chars
   in the second string.

   Returns:              Pointer to beginning of the original string.

   Double Byte Enabled 
   Written by: Jim Hill
----------------------------------------------------------------------------*/
LPTSTR RemoveMultipleCharacters(LPTSTR str,LPCTSTR chars)
{
  TCHAR      chr;
  LPTSTR     ptr,p;
  
  if(str==NULL)
     return(NULL);
  for(ptr=str;*chars;) {
     chr=*chars;
     p=str=ptr;                // RELOAD IT
     for(;*str;) {
        if(*str != (TCHAR)chr)  {
           *p=*str;
           p=_CHAR_NEXT(p);   
        }
        str=_CHAR_NEXT(str);   
     }
     *p='\0';               /* ALSO TERMINATE THE STRING  */
     p=_CHAR_NEXT(p);   
     chars=_CHAR_NEXT(chars);   
  }
  return(ptr);
}



/*--------------------------------------------------------------------
   strncpyExt()
   
   Use STRNCPY, but make certain it is null terminated.
   
   Double Byte Enabled 
   Written by: Jim Hill
--------------------------------------------------------------------*/
LPTSTR strncpyExt(LPTSTR str1,LPTSTR str2,int len)
{
   LPTSTR      lpRetString;
   if(len<=0 || str1==NULL || str2==NULL)
      return(str1);                   // CAN'T DO ANYTHING
   lpRetString=STRNCPY(str1,str2,len);   
   *(str1+(len-1))=0;                 // TERMINATE THE SUCKER
   return(lpRetString);
}



/*----------------------------------------------------------------------------
   STRIINC()
      
   determines if string1 is included in string2
   no regard for case

   Returns:   The address where string1 is included in string2.
              Else NULL if not included.

   Double Byte Enabled 
   Written by: Jim Hill
----------------------------------------------------------------------------*/
#if 0
LPTSTR striinc(LPTSTR str1,LPTSTR str2)
{
    register int  max;
    LPTSTR        p;

    if(str1==NULL || str2==NULL)
        return(NULL);

    max=STRLEN(str1);

    for(p=str2;*p;) {
       if(STRNICMP(str1,p,max)==0)
         return(p);
       p=_CHAR_NEXT(p);   
    }
    return(NULL);                       /* string1 not found in string2 */
}
#endif


/*--------------------------------------------------------------------

   Written by: Jim Hill                 szLogfileName
--------------------------------------------------------------------*/
DWORD GetLogFileName(void)
{
    BYTE   szStartDir[MAX_PATH];
    BYTE  *lpszLastSlash = NULL;
    BOOL   bRet = 0;

    // GET PATH AND FILE NAME TO THE EXE
    if(GetModuleFileNameA(hModuleForDll,szStartDir,sizeof(szStartDir))==0) 
        return(1);
    GetModuleNameAndDateTimeString( szModuleFileNameAndDateTime );

    lpszLastSlash = strrchr(szStartDir, '\\');
    if(lpszLastSlash == NULL)
        return(1);

    ++lpszLastSlash;      // point at filename 
    *lpszLastSlash = 0;   // trim

    // SAVE DIR
#ifdef _UNICODE
    bRet= MultiByteToWideChar(CP_ACP,0,szStartDir,-1,g_szMorseFullPath,sizeof(g_szMorseFullPath));   
    if( bRet == FALSE)
        g_szMorseFullPath[0] = 0;
#else
    strcpy(g_szMorseFullPath, szStartDir);

#endif

    // CREATE LOG FILENAME
    wsprintfA(szLogfileName, "%sAvisConsole.log", szStartDir);

    // IF IT EXISTS ALREADY, CLEAR IT
    DeleteFileA( szLogfileName );

    return(NOERR);
}


//DWORD GetPrivateProfileString(
//    LPCSTR      lpszSection,
//    LPCSTR      lpszEntry,
//    LPCSTR      lpszDefault,
//    LPSTR       lpszReturnBuffer,
//    DWORD       cbReturnBuffer,
//    LPCSTR      lpszFileName
//);

//DWORD GetLoginInfoFromIni( LPTSTR lpszServerName, LPTSTR lpszUserID, LPTSTR lpszPassword)
//{
//    DWORD  dwRet = 0;
//
//    dwRet = GetPrivateProfileString( L"Login", L"ServerAddress", L"", lpszServerName, MAX_PATH, g_szMorseIniPath);
//    if( dwRet == 0)
//        return(S_FALSE);
//
//    dwRet = GetPrivateProfileString( L"Login", L"UserID", L"", lpszUserID, MAX_PATH, g_szMorseIniPath);
//    if( dwRet == 0)
//        return(S_FALSE);
//
//    dwRet = GetPrivateProfileString( L"Login", L"Password", L"", lpszPassword, MAX_PATH, g_szMorseIniPath);
//    if( dwRet == 0)
//        return(S_FALSE);
//
//    return(S_OK);
//}




















#ifdef __cplusplus                                  
}
#endif

