/*******************************************************************
*                                                                  *
* File name:     RCProcess.cpp                                     *
*                                                                  *
* Description:   RCProcess class implementation file               *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Start process for the target application          *
*                                                                  *
*******************************************************************/

#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>

#include "errors.h"
#include "rcdefs.h"
#include "RCError.h"
#include "RCCommon.h"
#include "RCProcess.h"

void RCProcess::Init(RCCommon *rcCommon) 
{
	Common = rcCommon;
}
int RCProcess::StartProcess(CString command, int InitializeTime,  PROCESS_INFORMATION *pPI) 
{
char PgmPath[MAXPATH];
char ShortPath[MAXPATH];
char Args[MAXLINE];
char *commandline;
SECURITY_ATTRIBUTES SA;
STARTUPINFO SI;
int success;
long errcode;

commandline = new char[command.GetLength()+1];
strcpy(commandline,command);
   //'SI.cb = SizeofSI + Len("WinSta0\Default") + 1
SI.cb = sizeof SI;
SI.lpReserved = NULL;
SI.lpTitle = NULL;
//'SI.dwFlags = STARTF_USESHOWWINDOW
SI.dwFlags = 0;
SI.wShowWindow = SW_SHOWNORMAL;
SI.cbReserved2 = 0;
SI.lpReserved2 = NULL;
SI.lpDesktop = NULL;

SA.nLength = sizeof SA;
SA.lpSecurityDescriptor = NULL;
SA.bInheritHandle = NULL;
char *p;
    
  //  'split command line
    p  = strchr(commandline, ' ');
    if (p == NULL) {
        strcpy(Args, "");
        strcpy(PgmPath, commandline);
     } else {
        *p = '\0'; 
        strcpy(PgmPath,commandline);
        p++;
        if (p != NULL && *p != '\0') 
            strcpy(Args,p);
        else strcpy(Args, "");
    }

   
   GetShortPathName(PgmPath, ShortPath, MAXPATH);
   strcpy(commandline, ShortPath);
   strcat(commandline, " ");
   strcat(commandline,Args);
//    'SetErrorMode (SEM_FAILCRITICALERRORS Or SEM_NOGPFAULTERRORBOX Or SEM_NOOPENFILEERRORBOX)
//'                GetFileName(PgmPath) & "." & GetFileExt(PgmPath) & " " & Args,
// do not forget to change args of GetFileXxx...
    
    success = CreateProcess(ShortPath,
                commandline, 
                &SA, &SA, 
                0, 
                HIGH_PRIORITY_CLASS, 
                 0,
                NULL, 
                &SI, 
                pPI);
    
    if (success)
       WaitForInputIdle (pPI->hProcess, 1000 * InitializeTime);
    else {
 
       errcode = GetLastError();
       Error.ReportError("CreateProcess failed for %s;%s;%d",
		   ShortPath,commandline, errcode   );

    }
	delete commandline;
   return success;
}



long RCProcess::SyncSystem(CString commandline, long Timeout, char *Usname) 
{
// 'timeout : in seconds (-1 if no timeout)

  PROCESS_INFORMATION PI;
  unsigned long exitcode;
  long RunTime;
  long SleepTime;
  int ret;

  SleepTime = Common->TimeoutInterval/2;

  ret = StartProcess(commandline, 0, &PI);
  if (ret == 0)
  {
     
       Error.ReportError("Failed to start the process: %s.",commandline);
       return 0; 
  }

   exitcode = PROCESS_STILL_ACTIVE;
   RunTime = 0;
   while (exitcode == PROCESS_STILL_ACTIVE && (RunTime < 1000 * Timeout || Timeout < 0)) {
    GetExitCodeProcess( PI.hProcess, &exitcode);
    Sleep (SleepTime);
    RunTime +=  SleepTime;
  }
  if (exitcode == PROCESS_STILL_ACTIVE)
    TerminateProcess(PI.hProcess, 1);
  

  GetExitCodeProcess (PI.hProcess, &exitcode);

   return exitcode;
}

