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


#include  <windows.h>
#include  <windowsx.h>
#include  <tchar.h>   

#include  "util.h"




#ifdef __cplusplus               // Define as "C" linkage
extern "C"
{
#endif


#ifndef       STRICT
    #define   STRICT
#endif
#ifndef SYM_WIN32
   #define SYM_WIN32
#endif
#ifndef SYM_WIN
   #define SYM_WIN
#endif



// GLOBAL
BYTE   szLogfileName[MAX_PATH];
TCHAR  szModuleFileNameAndDateTime[MAX_PATH];




//LOCAL
int WriteStatusMessage(LPCSTR msg);
static DWORD dwFirstRun=TRUE;
DWORD GetLogFileName(void);




// DEFINES
#define _CHAR_NEXT(p)       CharNext(p)






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
TCHAR *GetTimeString(TCHAR *lpszTimeString)
{
   SYSTEMTIME  SysTime;
   TCHAR       szTime[50],szDate[50];
   //struct tm   *local;
   //time_t      Time;

   // GET THE LOCAL TIME ON THIS MACHINE
   GetLocalTime(&SysTime);
      
   if(GetDateFormat(LOCALE_SYSTEM_DEFAULT,DATE_SHORTDATE,&SysTime,NULL,szDate,sizeof(szDate))==0)  {
      strcpy(szDate,"00/00/00");
   }
   if(GetTimeFormat(LOCALE_SYSTEM_DEFAULT,TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,&SysTime,NULL,szTime,sizeof(szTime))==0)  {
      strcpy(szTime,"00:00");
   }

#if USE_MILLISECONDS_IN_TIME_STRING
   wsprintf(lpszTimeString,"%s %s.%03d ",szDate,szTime,SysTime.wMilliseconds);  
#else      
   wsprintf(lpszTimeString,"%s %s ",szDate,szTime);
#endif

   return(lpszTimeString);

}


#include "tchar.h"
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


   // if(GetModuleFileName( hModuleForDll, fullPath, sizeof(fullPath) ) == 0 ) 
   if(GetModuleFileName( NULL, fullPath, sizeof(fullPath) ) == 0 ) 
      return(0);    

   memset(&findData,0,sizeof(WIN32_FIND_DATA));
   //memset(&FileTime,0,sizeof(FILETIME));
   if( (findHandle=FindFirstFile( fullPath, &findData)) != INVALID_HANDLE_VALUE)  
   {
      SYSTEMTIME SystemTime;    
      FileTimeToSystemTime( &findData.ftLastWriteTime, &SystemTime );
      FindClose(findHandle);

      //GetDateFormat( LOCALE_SYSTEM_DEFAULT, 0, &SystemTime, _T("ddd, dd MMM yyyy"), szDate, 32 );               
      //GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &SystemTime, _T("HH:mm:ss"), szTime, 32 );  

      GetDateFormat( LOCALE_SYSTEM_DEFAULT, 0, &SystemTime, "ddd, dd MMM yyyy", szDate, 32 );               
      GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &SystemTime, "HH:mm:ss", szTime, 32 );  

      if( pszString != NULL )
        wsprintf( pszString, "%s  %s  %s", fullPath, szDate, szTime );

   }
   return(0);
}



/*----------------------------------------------------------------------------
   WriteStatusMessage()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int WriteStatusMessage(LPCSTR msg)
{
   HANDLE   dwFileHandle;
   int      errcode = NOERR;   
   DWORD    bytesWritten = 0;
   DWORD	dwMsgLength,dwFileSize;
   BYTE    szTimeString[50];
   BYTE    szOutputString[MAX_PRINTSTRING];



   // GET THE DATE AND TIME
   GetTimeString(szTimeString);

   if(msg==NULL || (dwMsgLength=strlen(msg))==0)
      return(0);

   if(szLogfileName[0] == 0)    // "Qserver.log"
       GetLogFileName();

   dwFileHandle= CreateFile( szLogfileName,
                         GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL,                  // No security attributes
                         OPEN_ALWAYS,           // iF NOT EXIST, THEN CREATE
                         FILE_ATTRIBUTE_NORMAL,
                         NULL                   // No template file
                         );
   if(dwFileHandle!=INVALID_HANDLE_VALUE)
   {
      //DWORD dwFileSize = GetFileSize( dwFileHandle, NULL) ;
      //if( dwFileSize > NEFEVENT_MAX_LOGFILE_SIZE )
      //   ShrinkLogFile( dwFileHandle );

      // MOVE TO THE END OF THE FILE
      dwFileSize = SetFilePointer(dwFileHandle, 0, (LPLONG) NULL, FILE_END);


	  // IS THIS THE FIRST TIME CALLED?
	  if(dwFirstRun) {
          //char szFileNameAndDateTime[255];
          //WideCharToMultiByte(CP_ACP,0,(LPCWSTR)szModuleFileNameAndDateTime,-1,szFileNameAndDateTime,sizeof(szFileNameAndDateTime),NULL,NULL);
        
		 dwFirstRun=FALSE;
		 //wsprintf(szOutputString,"Qserver %s  ----------- STARTING ------------------------\r\n",szTimeString);
		 wsprintf(szOutputString,"Qserver %s  ----------- STARTING  %s \r\n",szTimeString, szModuleFileNameAndDateTime );
	     WriteFile(dwFileHandle,szOutputString,strlen(szOutputString),&bytesWritten,NULL) ;
	     OutputDebugString(szOutputString);
	  }

	  // FORMAT THE LINE
      wsprintf(szOutputString,"Qserver %s %s\r\n", szTimeString, msg);

      // WRITE DATA 
      WriteFile(dwFileHandle,szOutputString,strlen(szOutputString),&bytesWritten,NULL) ;

      // CLOSE THE FILE
      CloseHandle(dwFileHandle);

      OutputDebugString(szOutputString);
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
    TCHAR    szLine[MAX_PRINTSTRING];
	int nBuf;
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

    if(WideCharToMultiByte(CP_ACP,0,szLineW,-1,szLine,sizeof(szLine),NULL,NULL))
        WriteStatusMessage(szLine);

	va_end(args);

    return;
}


/*----------------------------------------------------------------------------
   fPrintString
 ----------------------------------------------------------------------------*/
void  fPrintString(LPCSTR lpszFmt, ...)
{
   CPINFO CodePageInfo;
   va_list ap;
// BYTE    szLine[MAX_PRINTSTRING];
   TCHAR   szLine[MAX_PRINTSTRING],szFmt[512];

   // GET CODE PAGE INFO. TRANSLATE TO SINGLE BYTE?
   GetCPInfo(CP_ACP, &CodePageInfo);
   if(CodePageInfo.MaxCharSize == 1 && *(lpszFmt+1)==0)    {
      // LOOKS LIKE UNICODE, BETTER TRANSLATE IT TO ANSI
      if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)lpszFmt,-1,szFmt,sizeof(szFmt),NULL,NULL))
         lpszFmt=szFmt;
   }

   va_start(ap,lpszFmt);
   wvsprintf(szLine,lpszFmt,ap);
   va_end(ap);

   WriteStatusMessage(szLine);
   return;
}


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


#if 0
/*--------------------------------------------------------------------
   strncpyExt()
   
   Use STRNCPY, but make certain it is null terminated.
   
   Double Byte Enabled 4/28/95
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
#endif


/*--------------------------------------------------------------------

   Written by: Jim Hill                 szLogfileName
--------------------------------------------------------------------*/
DWORD GetLogFileName(void)
{
    BYTE   szStartDir[MAX_PATH];
    BYTE  *lpszLastSlash = NULL;


    // GET PATH AND FILE NAME TO THE EXE
    if(GetModuleFileNameA(NULL,szStartDir,sizeof(szStartDir))==0) 
        return(1);
    GetModuleNameAndDateTimeString( szModuleFileNameAndDateTime );

    lpszLastSlash = strrchr(szStartDir, '\\');    // strrchr  wcsrchr
    if(lpszLastSlash == NULL)
        return(1);

    ++lpszLastSlash;      // point at filename 
    *lpszLastSlash = 0;   // trim

    //wsprintf(szLogfileName, "%sQserverr.log", szStartDir);
    wsprintfA(szLogfileName, "%sQserverr.log", szStartDir);

    // IF IT EXISTS ALREADY, CLEAR IT
    DeleteFileA( szLogfileName );

    return(NOERR);
}



























#ifdef __cplusplus                                  
}
#endif

