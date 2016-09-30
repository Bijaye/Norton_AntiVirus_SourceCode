/*******************************************************************
*                                                                  *
* File name:     RCDdeApp.cpp                                      *
*                                                                  *
* Description:   Dde interface                                     *
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
* Function:      Initialize and terminate the DDE application      *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include <windows.h>
#include <ddeml.h>
#include <direct.h>
#include "RCdefs.h"
#include "errors.h"
#include "RCError.h"
#include "RCCommon.h"
#include "RCKeys.h"
#include "RCProcess.h"
#include "RCDdeApp.h"



HDDEDATA CALLBACK DDECallbackProc (UINT type, 
								   UINT /*format*/,
								   HCONV /*hConversation*/,
								   HSZ /*hsz1*/,
								   HSZ hsz2,
								   HDDEDATA hGlobalData,
								   DWORD /*dwData1*/,
								   DWORD /*dwData2*/)
{
	HDDEDATA ddeRC = 0;
 /*
	switch (type)
	{
	case XTYP_ADVDATA:
		
		if (itemHandle && !DdeCmpStringHandles(hsz2, itemHandle))
		{
			DdeGetData (hGlobalData,
				        ddeReceiveBuf,
						DDE_DATA_BUF_SIZE,
						0);

			if (ddeMsgWnd)
					ddeMsgWnd->PostMessage(CLIENT_ADVSTOP_TRANSACTION);
		}

		ddeRC = (HDDEDATA) DDE_FACK;
		break;

	default:
		ddeRC = (HDDEDATA) DDE_FNOTPROCESSED;
		break;

	}
	
	return (ddeRC);
	*/
    return (HDDEDATA)DDE_FACK;
			        
}
RCDdeApp::RCDdeApp(RCCommon *Common, CString AppName, int AppVersion) 
                : Common(Common),
				AppName(AppName)
{
	RCProcess::Init(Common);

	if (AppName == "Word") {
      sourceapp  =  "WinWord";
	  MainAppWinName =   "OpusApp";	  // word main class name
	} else {
      sourceapp = "Excel";
      MainAppWinName =  "XLmain"; // excel main class name
	}
}
void RCDdeApp::Start(CString FirstFile)
{

       CString commandline;

   // 'kill other instances of the application

     commandline = Common->SeekAppPath + " " + MainAppWinName;
    
     SyncSystem(commandline, -1, "");
    
	 if (!FirstFile.IsEmpty() && !Common->isFileReadable(FirstFile)) {
        Error.ReportError("Cannot access the file : %s", FirstFile);
        FirstFile.Empty();
	 }
        
    if (!Common->isFileReadable(Common->AppPath)) {
        Error.ExitError(RCBadPath, "Wrong path for App executable: %s", Common->AppPath);

    } else {
           

		// 'the /n flag prevents word from creating a new documents if it is started without a file as an argument
	   	char ShortPath[MAXPATH];

		GetShortPathName(Common->AppPath, ShortPath, MAXPATH);
		commandline = ShortPath;
	     if  (strcmp(FirstFile,"") )
		     commandline += " " + FirstFile;
		 else
			 if (AppName == "Word") {
                 commandline += " /n";

			 } else if (AppName == "Excel")
                commandline += " /e";
            
        
        StartProcess(commandline, 10, &Apppi);
                

        AppHwnd = FindWindow(MainAppWinName, NULL);
		
        if (AppHwnd == NULL) {
          Error.ExitError(RCAppStart,"The command: %s failed .", commandline);
		  return;
        }
        
    }  // if file is readable
    
   Error.ReportError("kill: %d", Common->nokillpopup);
    char ShortPath[MAXPATH];
    if (Common->nokillpopup == 0) {
         if  (!Common->isFileReadable(Common->PopupKillerPath)) {
           Error.ExitError(RCBadPath, "Bad path for the popup killer: %s", Common->PopupKillerPath);
         } else {
		     char LogPath[MAXPATH];
			 getcwd(LogPath, MAXPATH-1); // get current directory

			 GetShortPathName(Common->PopupKillerPath, ShortPath, MAXPATH);
             commandline.Format("%s -app %s -timeout %s -coma %s -labels %s -apppid %lx -rcpath %s",
                ShortPath, AppName, Common->PopupKillerTimeOut, 
                Common->PopupKillerSleepTime, Common->LangCfgFile, Apppi.dwProcessId, LogPath);

          
            Error.ReportError("running %s",commandline);
            if (WinExec (commandline,SW_SHOW) < 32) {
           
               Error.ExitError(RCKillPopupStart,"The command : %s failed.", 
				   commandline);
        
             }
         }
    }

	// the following 5 lines are for Win 95 only which use only the name of 
	// the program for title. Need to check it, might need to use the PIF file
	// or if only need to run on NT, can remove this code

	char ShortName[MAXPATH];
	char *p = strrchr(ShortPath,'\\');
	if (p == NULL) 
		strcpy(ShortName, ShortPath);
	else 
		strcpy(ShortName, p+1);
	p = strrchr (ShortName, '.');
	if (p != NULL) *p = '\0';
	int i=0;
	while(FindWindow(NULL, ShortPath) == NULL) {
	    if (FindWindow(NULL,ShortName) !=NULL) // if just pgm name is used
			break;
		Sleep(500);
	   i++;
	   if (i > 10) {  // just to avoid infinite loop in case some problem
		               // with the window
		   break;
	   }
	}	   

    SetForegroundWindow(AppHwnd);
	if  (strcmpi(AppName,"word")==0) {
        if (Common->DBCSMode) {
              Keys.SendKeys ("%`", AppHwnd);
        }
    }     
       //Open DDE connection
        UINT ddeRc;
		idInst = 0;
        if ((ddeRc = DdeInitialize(&idInst, (PFNCALLBACK) &DDECallbackProc, 
                      APPCLASS_STANDARD |APPCMD_CLIENTONLY, 0L))) {
          Error.ExitError(RCDdeInitialize, "%s", "could not initialize dde");
          return;
        }     
        hszService = DdeCreateStringHandle(idInst,sourceapp, CP_WINANSI);
        hszTopic = DdeCreateStringHandle(idInst, "System", CP_WINANSI);
        hConv = DdeConnect(idInst, hszService, hszTopic, NULL);
     
   
    
}

int RCDdeApp::Execute(CString dde_command)
{
     int ret; 
     
    if (isAppRunning())
   {
       ret = WaitForInputIdle(Apppi.hProcess, 5000);
       if (ret != 0) {
        
        Error.ReportError ("The command: %s failed; the input is not idle.", dde_command );
        return 1;
       }
        
      DWORD cmdlength = dde_command.GetLength() + 1;
      char *tempcmd = new char[cmdlength];
	  strcpy(tempcmd, dde_command);
      DdeClientTransaction((LPBYTE)tempcmd, strlen(tempcmd)+1, hConv, 
                0L,0L,XTYP_EXECUTE,  10000, NULL);
      DWORD rc = DdeGetLastError(idInst);
	  delete tempcmd;
      if (rc == 0) 
         return 0;
      else {
       
        Error.ReportError ("The command :%s failed %lx",dde_command, rc);
        return 1;
    }
  } else
     return 10;
  
   return 0;
}

void RCDdeApp::Close(int IPCmode )
{
   long WaitTime;
   long SleepTime;
   CString dde_command;

       
   if (FExit(IPCmode) == 1) {  // same as in Basic code but does not
	                            // seem to ever return 1
        if (strcmpi(AppName,"word")==0) {
            if  (isAppRunning()) {
                dde_command = "[";
		        dde_command +=  Common->ExitCommand + " 1]";
               Execute (dde_command);
             
           }
        }
   }
    DdeUninitialize(idInst);
         SleepTime = Common->TimeoutInterval;
         WaitTime = 0;
        while (isAppRunning() && WaitTime < 10000)
        {
            Sleep (SleepTime);
            WaitTime += SleepTime;
        }
                
        if (WaitTime >= 10000) 
            TerminateProcess (Apppi.hProcess, 1);
        
        
        //'it takes several seconds between the moment where the Word window closes
        //'and the moment when you can transfer the goat files
        SleepTime = Common->TimeoutInterval;
        WaitTime = 0;
        while (isAppRunning() && WaitTime < 10000) {
            Sleep (SleepTime);
            WaitTime +=  SleepTime;
        }
        
       

}

int RCDdeApp::FExit(int IPCmode)  
{
   int seconds;
   double totaltime;
   int SleepTime;
   int ret;

    //'check whether the process is busy
    ret = WaitForInputIdle(Apppi.hProcess, 5000);
    if (ret != 0) {
        Error.ReportError ("FExit failed ; the application is busy.");
        return Err_TimeOut;
	}
    
    totaltime = 0;
    seconds = 20;
    SleepTime = Common->TimeoutInterval/4;
    while (totaltime < seconds && isAppRunning()
		&& AppHwnd != GetLastActivePopup(AppHwnd)) {
       Sleep (SleepTime);
       totaltime += SleepTime / 1000;
    }

    if (!isAppRunning())
        return Err_App_Not_Running;
        

    if (totaltime >= seconds)
        return Err_Unknown;
      

    DdeUninitialize(idInst);     
    if (IPCmode == IPC_KS) {
        if (SetForegroundWindow(AppHwnd) != 0) 
            Keys.SendKeys(Common->ExitShortcut, AppHwnd);

    } else if (IPCmode == IPC_WM) {
        PostMessage (AppHwnd, WM_COMMAND, Common->ExitWP, 0);
	}
    
    
    totaltime = 0;
    seconds = 10;
    SleepTime = Common->TimeoutInterval;
    while (totaltime < seconds && isAppRunning()) {
        Sleep (SleepTime);
        totaltime += SleepTime / 1000;
    }
        
    if (totaltime >= seconds) {
        Error.ReportError("Exit failed.");
        return  Err_TimeOut;
    } else
        return Err_OK;
}

HANDLE RCDdeApp::GetProcessHandle()
{
	return Apppi.hProcess;
}

HWND RCDdeApp::GetApplicationHandle()
{
	return AppHwnd;
}

BOOL RCDdeApp::isAppRunning() 
{
   unsigned long exitcode;

    if (GetExitCodeProcess(Apppi.hProcess, &exitcode))
        if (exitcode == STILL_ACTIVE)
            return TRUE;
        else
            return FALSE;

		else {
          Error.ReportError("Error trying to get the exit code of the application.");
          return TRUE;
		}
    
}
