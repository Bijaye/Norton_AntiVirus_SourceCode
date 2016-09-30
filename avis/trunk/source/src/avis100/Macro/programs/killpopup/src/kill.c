/*******************************************************************
*                                                                  *
* File name:     kill.c                                            *
*                                                                  *
* Description:   main killpopup processor                          *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      close windows accidentally opened during macro    *                        
*                replication                                       *
*                                                                  *
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>

#include <windows.h>
#include <winuser.h>
#include <winbase.h>


#include "appli.h"
#include "popup.h"
#include "keys.h"
#include "winjmb.h"

// classes of the main window for the targeted applications
#define WordClassName "OpusApp"
#define ExcelClassName "XLMain"
#define PowerPointClassName  "PP97FrameClass"

// classes of the "*.exe - application error" window
#define BugClassName "#32770"

/* some errors due to macros cause the display of User Dialog windows */
#define UserDialogClassName "#32770"

//in Word 97, errors in macro cause the VBA5 debugger to become visible
//as I don't know how to close it properly, the program kills it
#define VBA5DebugClassName "wndclass_desked_gsk"

/* sleeping time in ms between the killing of two windows */
#define DefaultSleepTime 250

// maximum time of activity in seconds
// the programs kills some crucial windows necessary to kill processes and exit programs
// so it has to desactivate itself after a certain amount of time if something went wrong
#define DefaultTimeOut 300

#define DefaultInactiveTimeOut 60

#define WinTitleLength 100

#define MAXPATH 256

char AppName [100];
char AppClassName [100];

int Version;
long MaxWaitTime;
long InactiveTimeOut;
long SleepTime;
char LabelsFile[100];
char LogFile[MAXPATH];

//regular windows that must be killed only if they remain after a timeout
char OpenTitle[50];
char SaveAsTitle[50];
char NewTitle[50];
char VBErrorReply[50];
char UpconvertReply[50];

char UserName[50];
char Password[50];

DWORD dwAppPid;
HWND hAppWin;
HWND hPrvAppWin;

void WriteLog(char *fmt,...);

void usage()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "killpopup -app ApplicationName -labels labelfile [-apppid Pid] [-timeout time] [-coma time] [-inactivetimeout time] [-user username] [-password password]\n"); 
	fprintf(stderr, " -app: used to specify the name of the application (Word, Excel or PowerPoint).\n"); 
	fprintf(stderr, " -labels: used to specify the path of the file containing the titles of the dialog boxes to spare (Open, SaveAs, New).\n"); 
	fprintf(stderr, " -apppid: specifies the hexa pid of the legal instance of the application. Any other instance will be killed if apppid is used.\n"); 
	fprintf(stderr, " -timeout: specifies the timeout in seconds after which the application will be killed if still running (default: 300).\n"); 
	fprintf(stderr, " -coma: specifies the time in milliseconds between two checks for a popup (default: 250)\n"); 
	fprintf(stderr, " -inactivetimeout: specifies the timeout in seconds after which the application will be killed if turns out to be inactive (default: 60)\n"); 
	fprintf(stderr, " -user: specifies the name of the user to impersonate if necessary.\n"); 
	fprintf(stderr, " -password: password of the user to impersonate if necessary.\n"); 
}


void Process_params(int argc, char * argv[])
{
int i;

	if (argc ==1)
	{
		usage();
		exit(1);
	}

	MaxWaitTime = DefaultTimeOut;
	SleepTime = DefaultSleepTime;
	InactiveTimeOut = 1000L * DefaultInactiveTimeOut;
	UserName[0] = 0; Password[0] = 0;
	dwAppPid = 0;

	for (i = 1; i < argc; ++i)
	{
		if (stricmp(argv[i], "-app") ==0)
			strcpy(AppName, argv[++i]);
//		else
//			if (stricmp(argv[i], "-version") ==0)
//				AppVersion = atoi(argv[++i]);
			else
				if (stricmp(argv[i], "-timeout") ==0)
					MaxWaitTime = atoi(argv[++i]);
				else
					if (stricmp(argv[i], "-coma") ==0)
						SleepTime = atoi(argv[++i]);
					else
						if (stricmp(argv[i], "-inactivetimeout") ==0)
							InactiveTimeOut = 1000L * atoi(argv[++i]);
						else
							if (stricmp(argv[i], "-labels") ==0)
								strcpy(LabelsFile, argv[++i]);
							else
								if (stricmp(argv[i], "-user") ==0)
									strcpy(UserName, argv[++i]);
								else
									if (stricmp(argv[i], "-password") ==0)
										strcpy(Password, argv[++i]);
									else
										if (stricmp(argv[i], "-apppid") ==0)
										{
											sscanf(strlwr(argv[++i]), "%x", &dwAppPid);
										}
										else								
										 if (stricmp(argv[i], "-rcpath") ==0)
										 {
											 strcpy(LogFile, argv[++i]);
                                             strcat(LogFile, "\\killwarn.log");											 
										 }
     										else
											{
											  usage();
											  exit(1);
											}
	}


	//checks if the application number is correct
	//and initializes AppClassName
	if (!stricmp(AppName, "word"))
		strcpy(AppClassName, WordClassName);
	else
		if (!stricmp(AppName, "excel"))
			strcpy(AppClassName, ExcelClassName);
		else 
	        if (!stricmp(AppName, "powerpoint"))
				strcpy(AppClassName, PowerPointClassName);
     		else
		{
			fprintf(stderr, "The application name: %s is incorrect.\n", AppName);
			exit(1);
		}
}


void GetLabels(void)
{
	FILE * F;
	char line [100];
	char type[20];
	char operation[20];
	char label[50];
	int labellg;

	F = fopen(LabelsFile, "rt");
	if (!F)
	{
		fprintf(stderr, "Unable to open the label file: %s. Aborting.\n", LabelsFile);
	    exit(1);
	}
	while (feof(F) ==0)
	{
		fgets(line,99,F);		
		strcpy(type, strtok(line,"|"));	
		if (stricmp(type, "t") == 0 || stricmp(type, "s") == 0) {
			strcpy(operation,strtok(NULL,"|"));
			strcpy(label,strtok(NULL,"|"));
			labellg = strlen(label);
			label[labellg-1] = 0;
		}
		if (stricmp(type, "t") ==0)
		{
	
			if (stricmp(operation, "open") ==0)
				strcpy(OpenTitle,label);
			else if (stricmp(operation, "saveas") ==0)
				strcpy(SaveAsTitle,label);
			else if (stricmp(operation, "new") ==0) 
				strcpy(NewTitle,label);
		} else if (stricmp(type, "s") == 0) {
			if (stricmp(operation, "upconvertreply") == 0)
				strcpy(UpconvertReply, label);
			else if (stricmp(operation, "vberrorreply") == 0)
				strcpy(VBErrorReply, label);
		}
	}
	fclose(F);
}


void main(int argc, char * argv[])
{
long WaitTime;

char windowtitle[WinTitleLength];
char lastwindowtitle[WinTitleLength];

char AppTitle[WinTitleLength];
char LastAppTitle[WinTitleLength];

long wt;
long InactiveTime =0;
int nCloseAttempts = 0;

HWND hwnd;
HWND lasthwnd;
HWND bughwnd;
HWND lastbughwnd;
HWND hCurAppWin;
HANDLE hToken;
DWORD dwCurAppPid;
BOOL Err1057;


Process_params(argc, argv);
GetLabels();

Err1057 = FALSE;
lastwindowtitle[0] = 0;
LastAppTitle[0] = 0;

if (UserName[0] != 0)
	if (LogonUser(
        UserName,
        ".",
        Password,
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken))
		ImpersonateLoggedOnUser(hToken);

/*
    WaitTime = 0;
    while ((hAppWin = FindWindowEx(0, 0, WordClassName, 0)) == NULL)
    {
	    if (WaitTime > (MaxWaitTime * 1000L))
	    {
		    exit(101);
	    }
	    Sleep(SleepTime);
	    WaitTime += SleepTime;
    }
*/

    lasthwnd = 0;
	lastbughwnd = 0;
    WaitTime = 0;
	while ((hCurAppWin = FindWindowEx(0, 0, AppClassName, 0)) != 0)
    {
		
					
		if (dwAppPid)
		{
			hPrvAppWin = 0;
			//iter through the list of instances of the application and kill non legal instances
			while ((hCurAppWin = FindWindowEx(0, hPrvAppWin, AppClassName, 0)) != NULL)
			{
				GetWindowThreadProcessId(hCurAppWin, &dwCurAppPid);
				if (dwCurAppPid && (dwCurAppPid != dwAppPid))
				{
                    #ifdef _DEBUG
					WriteLog("killing application\n");
                    #endif
					kill_app_instance(hCurAppWin);
					hPrvAppWin = 0;
				}
				else
				{
					hAppWin = hPrvAppWin = hCurAppWin;
				}
			}
			if (!hAppWin) {
				//all instances killed
                #ifdef _DEBUG
				WriteLog("all instances of the application are killed, getting out\n");
                #endif
				exit(0);
			}
		}
		else
		{
			hAppWin = hCurAppWin;
		}
		
		if (WaitTime > (MaxWaitTime * 1000L))
	    {
			//	kill the running copy of the application
			#ifdef _DEBUG
				fprintf(stderr, "max time elapsed\n");
                WriteLog("timed out, killing the running copy of the application\n");
            #endif
			kill_app_instance(hAppWin);
		    exit (100);
	    }

		//look for a crash of the application
		lastbughwnd = 0;
		while ((bughwnd = FindWindowEx(0, lastbughwnd, BugClassName, 0)) != NULL)
		{	
			/*test
			GetWinTitle(bughwnd, windowtitle, WinTitleLength);
			strlwr(windowtitle);
			if (strstr(windowtitle," - application error") != NULL 
				  || strstr(windowtitle, "server busy") != NULL) {
#ifdef _DEBUG
				WriteLog("trying to send [ENTER] to the window %s\n",windowtitle);
#endif
                if (SetForegroundWindow(bughwnd)) 
				  SendKeys("[ENTER]",bughwnd);
                else
				   WriteLog("Unable to set %s window to the foreground\n", windowtitle);
			}
			

            if (strstr(windowtitle, "microsoft word") // sometimes in Word 2000 these windows
				                                       // have bug class
               || strstr(windowtitle, "virus")) // some viruses display the window with this title
			{
				if (SetForegroundWindow(bughwnd)) {
					SendKeys("[ENTER]", bughwnd);
					WriteLog("sending ENTER to %s\n", windowtitle);
				}
			} else
			    WriteLog("unable to set %s to the foreground", windowtitle);
				*endtest*/
            if (strstr(windowtitle, "microsoft visual basic") != NULL) {
				 char MainWinTitle[WinTitleLength];
              if (SetForegroundWindow(bughwnd)) {
                      SendKeys(VBErrorReply, bughwnd); //will be tricky to NLS this thing
			   GetWinTitle(hAppWin, MainWinTitle, WinTitleLength);
#ifdef _DEBUG
               WriteLog("Microsoft Visual Basic error on window %s\n",MainWinTitle);
			   WriteLog("sent %s to window\n", VBErrorReply);
#endif
			  }
			} else                                           

			if (SetForegroundWindow(bughwnd)) {
				SendKeys("[ENTER]", bughwnd);
				WriteLog("sending ENTER to %s\n", windowtitle);
			} else
				WriteLog("unable to set %s to the foreground\n", windowtitle);
           	lastbughwnd = bughwnd;
		}
		// look for an instance of the VBA5 debugger

		hwnd = FindWindowEx(0,0,VBA5DebugClassName,0);
		if (hwnd)
		{
			/* a hidden instance of the VBA5 debugger is running each time a macro runs
			if it becomes visible, there is a bug in the macro */
            	if (IsWindowVisible(hwnd))
			{
				#ifdef _DEBUG
     				fprintf(stderr, "found vba5 debugger\n");
	                WriteLog("found vba5 debugger\n",windowtitle);
					GetWinTitle(hwnd, windowtitle,WinTitleLength);
					fprintf(stderr,"%s\n",windowtitle);
				#endif
				//retrieve the process id and kill the app
				kill_app_instance(hwnd);
			}
		}

		windowtitle[0] = 0;
		hwnd = GetLastActivePopup(hAppWin);
		if (hwnd != hAppWin)
	    {
			GetWinTitle(hwnd, windowtitle, WinTitleLength);
			if ((stricmp(windowtitle, SaveAsTitle)) &&(stricmp(windowtitle, NewTitle)) &&(stricmp(windowtitle, OpenTitle)))
	        {
		        if (lasthwnd == hwnd)
				{
					++nCloseAttempts;
					if (nCloseAttempts ==1)
					{
						#ifdef _DEBUG
							fprintf(stderr, "first attempt to close %s\n", windowtitle);
							WriteLog("first attempt to close %s\n", windowtitle);
						#endif

						if (SetForegroundWindow(hwnd)) {
	//non-OLErc						   WriteLog("sending [ENTER] to the window\n");
    //non-OLErc							   SendKeys("[ENTER]", hwnd);
							WriteLog("sending [ESC] to the window\n");
							SendKeys("[ESC]",hwnd);
						} else
							fprintf(stderr, "unable to set focus to %s: %d\n", windowtitle, GetLastError());

					}
					else if (nCloseAttempts == 2)
					{
						#ifdef _DEBUG
							fprintf(stderr, "closing \"hard\" %s\n", windowtitle);
						#endif
						WriteLog("closing \"hard\" %s\n", windowtitle);
						Close_Window(hwnd);
					}
				}
				else
				{
					nCloseAttempts = 0;
					#ifdef _DEBUG
						fprintf(stderr, "seen an unknown window: %s\n", windowtitle);
     	                WriteLog("seen an unknown window: %s\n", windowtitle);
         			#endif
	            if (strstr(windowtitle, "Err=1057")!=NULL) //nasty virus
                  {  
                     WriteLog("Seen error 1057\n");
                     Err1057=TRUE;
                  }      

							
			   	if (SetForegroundWindow(hwnd))
/*
		           if (!stricmp(windowtitle, "Microsoft Word") ||
					   !stricmp(windowtitle, "Microsoft Excel")) {

#ifdef _DEBUG
				         WriteLog("sending %s to application\n", UpconvertReply);     		
#endif
					     SendKeys(UpconvertReply,hwnd); 
					} else 
*/
					SendKeys("[ENTER]",hwnd);
				else
					fprintf(stderr, "unable to set focus to %s: %d\n", windowtitle, GetLastError());
				}
                lasthwnd = hwnd;
                wt = 0;
            }
            else
            {
				#ifdef _DEBUG
					printf ("Seen a known window: %s.\n",windowtitle);
     				WriteLog("Seen a known window: %s.\n",windowtitle);

				#endif

				if (Err1057 == TRUE) { //don't wait, just hit ESC
					WriteLog("need to kill SaveAs because of Err1057\n");
					if (SetForegroundWindow(hwnd)) {
					   SendKeys("[ESC]", hwnd);
                       Err1057 = FALSE;  
#ifdef _DEBUG
             	       WriteLog("Sent [ESC] to window %s",windowtitle);
#endif
					   wt = 0;
					   Sleep(1000);
					   continue;
					}
                }
                wt = wt + SleepTime;
                if (wt > 10000)
                {
			        if (lasthwnd == hwnd)
					{
						#ifdef _DEBUG
							fprintf(stderr, "closing \"hard\" %s.\n", windowtitle);
							WriteLog("closing \"hard\" %s.\n", windowtitle);
						#endif
						Close_Window(hwnd);
					}
					else
						if (SetForegroundWindow(hwnd)) {
#ifdef _DEBUG
						     WriteLog("Sent [ENTER] to window %s",windowtitle);
#endif
						     SendKeys("[ENTER]",hwnd);
							}
                    lasthwnd = hwnd;
                    Sleep(1000);
                }
            }
        }
        else
        {
            wt = 0;
        }

    Sleep(SleepTime);
	WaitTime += SleepTime;
    
    GetWinTitle(hAppWin, AppTitle, WinTitleLength);
	if (strcmp(LastAppTitle, AppTitle) ==0)
		InactiveTime += SleepTime;
	else
		InactiveTime = 0;
	strcpy(LastAppTitle, AppTitle);

	if (InactiveTime > InactiveTimeOut)
	{
		#ifdef _DEBUG
			fprintf(stderr, "inactive timeout\n");
			WriteLog("inactive timeout\n");

		#endif
		kill_app_instance(hAppWin);
	}
}
exit(0);
}
void WriteLog(char *fmt,...)
{
   va_list ap;
   char msg[200];
   FILE *f;
    
   va_start(ap, fmt);
   vsprintf(msg, fmt, ap);
   va_end( ap);
   if ((f =fopen(LogFile, "a")) !=NULL)
   {
				  
    fprintf(  f, msg);
    fclose(f);
   }

}
