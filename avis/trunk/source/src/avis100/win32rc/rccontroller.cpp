#include "stdafx.h"
#include <afxdisp.h>
#include "afxtempl.h"
#include <afxsock.h>
#include <stdlib.h>
#include <direct.h>
#include <process.h>
#include <io.h>
#include <regstr.h>
#include <winreg.h>

#include "defs.h"
#include "resource.h"
#include "rcerror.h"
#include "rckeys.h"
#include "polymath.h"
#include "win32rcdlg.h"
#include "win32rc.h"
#include "rcfile.h"
#include "rcfilelist.h"
#include "rccheck.h"
#include "rccontroller.h"
#include "win32rcDlg.h"

#define BUFF_SIZE 1000
#define MAXPATH 256
// classes of the "*.exe - application error" window
#define BugClassName "#32770"
#define PROCESS_STILL_ACTIVE 259

// registry fields we are interested in
#define RUN_REG_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RUNONCE_REG_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Runonce"
#define RUNSERVICES_REG_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Runservices"
#define RUNSERVICESONCE_REG_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Runservicesonce"

#define REPLICATE_KEY_NAME "Replicate"
//codes for the registry fields to use in collection
// HKEY_LOCAL_MACHINE\ ... \RUN   corresponds to HKEY_MACHINE_RUN
// HKEY_CURRENT_USER\  ...\RUNSERVICESONCE corresponds to HKEY_USER_RUNSERVICESONCE
#define HKEY_MACHINE_RUN             0
#define HKEY_MACHINE_RUNONCE         1
#define HKEY_MACHINE_RUNSERVICES     2
#define HKEY_MACHINE_RUNSERVICESONCE 3
#define HKEY_USER_RUN                4
#define HKEY_USER_RUNONCE            5
#define HKEY_USER_RUNSERVICES        6
#define HKEY_USER_RUNSERVICESONCE    7
  

BOOL CALLBACK EnumWindowsProc( HWND hwnd, 
 // handle to parent window 
 
       LPARAM lParam 
 // application-defined value 
 
)
{
	static CStringList WindowTitles;  
	long TitleLength;
     RCError rcTrace;
  
   int killWindow = (int) lParam; // are we killing or initializing?
   
   TitleLength = GetWindowTextLength(hwnd) + 1;

   if (TitleLength <= 1) return TRUE; //ignore

   char *pszTmp = new char[TitleLength];
   GetWindowText(hwnd, pszTmp, TitleLength);
   CString strTmp = pszTmp;

   if (!killWindow) //initializing
   {
	   WindowTitles.AddTail(strTmp);
   } else {
	   if (WindowTitles.Find(strTmp) > 0) { // is window on the list
		   delete pszTmp;
		   return TRUE;
	   }
	   RCKeys rcKeys;
       RCError rcTrace;
	   rcTrace.ReportError("trying to kill %s",strTmp);
       if (FindWindow(0, pszTmp)) {
		   ::SetForegroundWindow(hwnd);
           rcKeys.SendKeys("[ESC]", hwnd);
       } 
	   
       if (FindWindow(0, pszTmp)) {
  		   ::SetForegroundWindow(hwnd);
           rcKeys.SendKeys("[ALT]f", hwnd);
		   Sleep(100);
           rcKeys.SendKeys("x",hwnd);

	   }
	   if (FindWindow(0,pszTmp)) {
		   ::SetForegroundWindow(hwnd);
	       keybd_event(VK_RETURN,0,0,0);
	   }
   }
       delete pszTmp;
       return TRUE;
}

// thread function that runs in another thread and kills windows
// that appear during virus execution

//BOOL bKill = FALSE;  // set to TRUE to kill the thread
//UINT ThreadFunction(LPVOID pParam)
//{
//	while (!bKill)
//	{
//		EnumWindows(EnumWindowsProc, (LPARAM) 1);
//	}
//	return 0;
//}


// constructor
RCController::RCController(CString SampleName, CString GoatsDir, 
						   CStringList *pGoatsDirs2, bool trace, 
						   bool after_reboot, CSocket *socket, 
						   CString strIdentifier)
{
   m_GoatsDir = GoatsDir;
   m_pGoatsDirs2 = pGoatsDirs2;
   m_goats_infected = 0;
   m_SampleName = SampleName;
   m_trace = trace;
   m_after_reboot = after_reboot;
   m_socket = socket;
   m_strIdentifier = strIdentifier;
  
}

RCController::~RCController()
{
      // free up lists 
	m_ChangedFiles.RemoveAll();
	m_NewFiles.RemoveAll();
	m_GoatList.RemoveAll();
	m_NewFiles.RemoveAll();

	// if we haven't terminated the thread yet, e.g. this is the unexpected return
//	if (m_pThread != NULL) 
//	{
//		bKill = TRUE;
//		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
//		delete m_pThread;
//	}
	
}

int RCController::Initialize()
{
    CWin32rcDlg dlg;
#ifndef _DEBUG
	GetWindowsDirectory(m_WindowsDir, MAXPATH);
#else
	strcpy(m_WindowsDir,"C:\\WINDOWS");
#endif
	strupr(m_WindowsDir);
    if (InitGoatList(m_GoatsDir) != SUCCESS) //remember names of all goats in primary goat directory
		return FAILURE;
	// add goats from secondary directories to the list
	POSITION pos;
	for (pos = m_pGoatsDirs2->GetHeadPosition(); pos != NULL;)
		InitGoatList(m_pGoatsDirs2->GetNext(pos));


	ReportProgress(&dlg, "Initializing the goat list","","");
    
	rcCheck.Init(m_GoatsDir, m_pGoatsDirs2, m_WindowsDir, m_trace);
	
	chdir(m_GoatsDir);
	return SUCCESS;

}
int RCController::StandardReplication(int nFromCommandFile /*=0*/)
{ 
	CWin32rcDlg dlg;
    CString strText;

	ReportProgress(&dlg, "Initializing", "state","startup");

	if (!nFromCommandFile)
	  if (Initialize() != SUCCESS)
		  return FAILURE;
	/*
	GetWindowsDirectory(m_WindowsDir, MAXPATH);
	strupr(m_WindowsDir);
    InitGoatList(m_GoatsDir); //remember names of all goats in primary goat directory
	// add goats from secondary directories to the list
	POSITION pos;
	for (pos = m_pGoatsDirs2->GetHeadPosition(); pos != NULL;)
		InitGoatList(m_pGoatsDirs2->GetNext(pos));


    CString strText;
    
	rcCheck.Init(m_GoatsDir, m_pGoatsDirs2, m_WindowsDir, m_trace);
	
	chdir(m_GoatsDir);
    */
	ReportProgress(&dlg, "Obtaining system configuration information","state","checking");
	if (rcCheck.InitFileList(m_after_reboot) != SUCCESS)
	{
		ReportProgress(NULL, "", "message","unable to open database file after reboot");
		dlg.DestroyWindow();
		return FAILURE;
	}

    
	if (!m_after_reboot)
	  GetRegistryValues(m_RegistryValues);
      // start virus sample
	
    m_pThread = NULL;
	if (!m_after_reboot) {
    	EnumWindows(EnumWindowsProc, (LPARAM) 0); //get the list of windows open now
        
	   // start the thread to kill the windows displayed unexpectedly by the virus
//       m_pThread = AfxBeginThread(ThreadFunction, NULL);
//	   SetThreadPriority(m_pThread,THREAD_PRIORITY_ABOVE_NORMAL+1);
//       m_pThread->m_bAutoDelete = FALSE; // need to ensure the thread will always exist
	                                    // so we can wait for it
	   strText = "Running the virus sample";
       ReportProgress(&dlg, "Running the virus sample","state", "runsample");
	   
       int exitcode = RunProgram(m_SampleName,10,1);
	   if (exitcode == FAILURE) {
		  CString strTmp;
		  strTmp.Format("Unable to run the virus sample, error= %d\n",GetLastError());
          ReportProgress(NULL, "", "message", strTmp);
          return FAILURE;  // no goats infected 
	   }
	   Sleep(2000);
       if (m_trace) 
	      rcTrace.ReportError("run sample\n");

      // run checkup to check for changes
	   ReportProgress(&dlg, "Checking files for changes","state", "checking");

       rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
	            
      // check if infected
      if (m_trace)
         rcTrace.ReportError("run second checkup\n");

	  int changed_count = m_ChangedFiles.GetCount();
	  if (changed_count <= 8 && changed_count > 0 && exitcode != PROCESS_STILL_ACTIVE) {
        if (m_trace)
	        rcTrace.ReportError("in case 1-8 infected files");

		ReportProgress(&dlg, "Running the virus sample again","state", "runsample");
        
	    exitcode = RunProgram(m_SampleName, 10, 1);
        
		ReportProgress(&dlg, "Checking files for changes","state", "checking");
	    rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
	    int previous_infected = changed_count;
	    int currently_infected = m_ChangedFiles.GetCount();

	    // some viruses infect a couple of files at a time
	    while (currently_infected - previous_infected > 0 &&
		  GoatsInfected() < m_GoatList.GetCount())
		{
              ReportProgress(&dlg, "Running the virus sample again","state","runsample");
			  exitcode = RunProgram(m_SampleName, 10,1);
              ReportProgress(&dlg, "Checking files for changes","state","checking");
			  rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
			  previous_infected = currently_infected;
			  currently_infected = m_ChangedFiles.GetCount();
		}
	  }
	  
    
      if (GoatsInfected()) {
		  ReportProgress(&dlg, "Goats are infected","","");
      
	  
       if (m_trace)
	     rcTrace.ReportError("first return\n");
      
	   dlg.DestroyWindow();
       return m_goats_infected;
	  }  
	}
	ReportProgress(&dlg, "Checking files for changes","state", "checking");

    rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
    if (m_trace)
       rcTrace.ReportError("run third checkup\n");

    if (GoatsInfected()) {
        if (m_trace)
		   rcTrace.ReportError("files are infected \n");

        if (m_trace)
		   rcTrace.ReportError("second return\n");

		dlg.DestroyWindow();
		return m_goats_infected;
	}

	ReportProgress(&dlg, "executing goats","","");


//	if (m_pThread != NULL) //if we started the thread
//	{
//		bKill = TRUE;
//		WaitForSingleObject(m_pThread->m_hThread, INFINITE); //wait for the thread to terminate
//		delete m_pThread;
//	}
	if (m_after_reboot) //we haven't run the sample, so we didn't need to enumerate windows
   	  EnumWindows(EnumWindowsProc, (LPARAM) 0); //get the list of windows open now
    
    ExecuteGoats(m_GoatsDir);
	//* EnumWindows(EnumWindowsProc, (LPARAM) 1); //get the list of windows open now

	ReportProgress(&dlg, "Checking files for changes","state","checking");

    rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
    if (m_trace)
	   rcTrace.ReportError("run last checkup\n");

	if (!m_NewFiles.IsEmpty() || !m_ChangedFiles.IsEmpty()) { 
		ReportProgress(&dlg, "New/Changed files are found","","");
        

	} else {    //not seeing infection, but could be stealth virus
		ReportProgress(&dlg, "No changes detected","","");
	}

    if (m_trace)
	  rcTrace.ReportError("last return\n");

 	dlg.DestroyWindow();
	return m_goats_infected;
}

bool RCController::ReportResults(bool copy_results, CString strResultsDir)
{
	bool reboot_required = false;
	// report results 
	RCError rcResults("c:\\win32rc.log");
	CString strFile;
	
	POSITION pos = m_ChangedFiles.GetHeadPosition();
	while (pos != NULL)
	{
		strFile = m_ChangedFiles.GetNext(pos);
		rcResults.ReportError("CHANGED: %s\n", strFile);
	}
	
	
	pos = m_NewFiles.GetHeadPosition();
	while (pos != NULL)
	{
		strFile = m_NewFiles.GetNext(pos);
		rcResults.ReportError("NEW %s", strFile);
	}

	// check if reboot is required in which case enter ourselves
	// into the registry
	if (!m_after_reboot && (reboot_required = IsRebootRequired())) {
		CString strCommand;
		char szSrc[MAXPATH];
	    rcTrace.ReportError("reboot is required\n");
		GetModuleFileName(AfxGetInstanceHandle(), szSrc, MAXPATH);
		strCommand = szSrc ;
		strCommand += " " + m_SampleName;
		strCommand += " -afterreboot -goats " + m_GoatsDir;
		if (copy_results)
			strCommand += " -results " + strResultsDir;
		/*
		if (!WriteRegistryInfo(strCommand)) // enter ourselves into the registry
		{
			reboot_required = FALSE; // no point to reboot
			                         // if this happens too often
			                         // maybe worth while to use win.ini
			rcTrace.ReportError("Error: unable to enter ourselves into the registry");

		}
		*/
		  
		ReportProgress(NULL,"", "state", "writedb");
        rcCheck.WriteFilesInfo();
	}
	if (copy_results)
		CopyResults(strResultsDir);

	if (reboot_required)
		ReportProgress(NULL, "","state", "reboot");
	else 
		ReportProgress(NULL, "", "state","shutdown");

    return reboot_required;
}

// Copies src file to dst file in binary mode
int RCController::FileCopy (LPCTSTR src, LPCTSTR dst)
{
 int numItems;
 FILE *fp_src, *fp_dst;
 char *buffr;  // Use 16k buffer for faster file copy

  buffr = (char *) malloc (BUFF_SIZE);

  if (buffr == NULL) {
    return FALSE;
  }

  if ((fp_src = fopen (src,"rb"))==NULL)
  {
    free (buffr);
    return FALSE;
  }
  if ((fp_dst = fopen (dst,"wb"))==NULL)
  {
     free (buffr);
    return FALSE;
  }


  while ((numItems = fread (buffr, 1, BUFF_SIZE, fp_src)) == BUFF_SIZE)
  {
        fwrite (buffr, 1, BUFF_SIZE, fp_dst);
   }


  fwrite (buffr, 1, numItems, fp_dst);

  free (buffr);

  fflush (fp_dst);
//  setftime (fileno (fp_dst), &ft);

  fclose (fp_src);

  fclose (fp_dst);

  return TRUE;

}
/*-----------------------------------------------------------------------------
Function Name:  void AppendPath(char * strFirst, char * strSec)

Input Parameters:
        strFirst: Specifies the first path to which second path is to be appended.
        strSecond: Specifies the path to be appenced to lpszFirst

Output Parameters:
        strFirst: Containst he resultant path on return.

Return Value:
        None

Description:
        This function combines two paths and takes care of trailing
        slashes in both of them. It ensures that when the two
        paths are appended, there is a single slash separating
        them. For ex: if first path has a trailing slash and
        second path has a starting slash, it ensures to remove
        one of the slashes. Similarly, if first path does not
        have a trailing slash and second path does not have
        a starrting slash, a single slash is appended to the first

  path before appending the second path.

-----------------------------------------------------------------------------*/

// Appends sec path to first and takes care of trailing slashes.
void RCController::AppendPath(char *strFirst,LPCTSTR strSec)
{
        if(strFirst == NULL || strSec == NULL) return;

        int nLen1 = strlen(strFirst);

        // If last char in First is not a slash and first char
        // in Sec is not a slash, append slash.

        if(strFirst[nLen1-1] != '\\' && strSec[0] != '\\')
                strcat(strFirst,"\\");

        // If both of then have a slash, remove one of them.

        else if (strFirst[nLen1-1] == '\\' && strSec[0] == '\\')
                strFirst[nLen1-1] = 0;

        strcat(strFirst,strSec);
}
/*
 * get file name without the directory 
 * filename - full name of the file
 * result - resulting name
 */
void RCController::GetFileName(CString filename, CString &result) 
{
   char *p, *s;

   char *temp;

   temp = new char[filename.GetLength() +1];
   strcpy(temp,filename);
	

   s = strrchr(temp,'\\');
   p = NULL;
   if (s == NULL) s = temp;
   else s++;

   result = s;

   delete temp;
}


int RCController::InitGoatList(CString DirName)
{

     // Check if directory exists.
      if(DirName == "" || access(DirName,00) != 0) {
		CString strTmp;
		strTmp.Format("Unable to access goat directory %s\n",DirName);
		ReportProgress(NULL, "", "message", strTmp);
        return FAILURE;
	  }
      
	  m_GoatsDir = DirName;
     //Search thru the directory and add all goat*.* files to goat list

        struct _finddata_t tFileBlk;

        char szTmp[MAXPATH]; // temporary for directory
        char szTmpBuf[MAXPATH]; // temporary for directory
        unsigned int nRet; // return code from findnext
        long hFile;

        strcpy(szTmp,DirName);
        strcpy(szTmpBuf,DirName);
        AppendPath(szTmp,"\\*.*");

        hFile = _findfirst(szTmp,&tFileBlk);
        nRet = (hFile == -1) ? 1 : 0;

        
        while(nRet == 0 )
        {
                // skip . and .. entries

                strupr(tFileBlk.name);

                if (strcmp(tFileBlk.name,".") && strcmp(tFileBlk.name,".."))
                {
                  if(tFileBlk.attrib & _A_SUBDIR )
                  {
                          // subdirectory
                          //Nothing to do now.
                  }
                  else
                  {
                         // not a subdirectory
                         CString strName;
						 strName = m_GoatsDir + '\\' + tFileBlk.name;
                         strName.MakeUpper();   
						 CString strPrefix = tFileBlk.name;
						 if (strName != m_SampleName && strPrefix.Left(4)==GOAT_PREFIX)
                           m_GoatList.AddTail(strName);
                  }
                }

                nRet = _findnext(hFile, &tFileBlk);

        }

        _findclose(hFile);

        return SUCCESS;
}
 
void RCController::CopyAllGoats(CString strResultsDir)
{
	CString strSrc;
	CString strDst;
	CString strTmp;

    	POSITION pos = m_GoatList.GetHeadPosition(); //** later move to separate fn
    	while (pos != NULL) {
	      strSrc = m_GoatList.GetNext(pos);
          GetFileName(strSrc, strTmp); // extract file name
	      strDst = strResultsDir + '\\' + strTmp;
	      FileCopy(strSrc, strDst);
		}


}

// Run the specified program
int RCController::RunProgram(CString Path, int timeout_seconds, BOOL virus)
{
RCKeys rcKeys;
STARTUPINFO si;
PROCESS_INFORMATION pi;
unsigned long exitcode;
static HANDLE hVirusProcess = 0;
bool crashed = FALSE;

   if (hVirusProcess != 0 && virus) // try to invoke virus sample multiple times
   {
       GetExitCodeProcess( hVirusProcess, &exitcode); // see if still running

       if (exitcode == PROCESS_STILL_ACTIVE) { // kill it so we can run the next one
          TerminateProcess(hVirusProcess, 1);
	   }

   }
//initialization of the STARTUPINFO structure

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	
	si.lpReserved = NULL;
	si.lpDesktop = NULL; //crashes with an empty string
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOWMAXIMIZED;
	si.wShowWindow = SW_SHOW;
	si.cbReserved2 = 0;
	si.lpReserved2 = NULL;

	si.cb = sizeof (si);
     
	bool retc = CreateProcess(
		Path, //lpApplication name
		NULL,	// command line
		NULL,	// handle cannot be inherited by child processes
		NULL,	// handle cannot be inherited by threads
		FALSE,	// new process does not inherit handles form the calling process
		CREATE_DEFAULT_ERROR_MODE & CREATE_NEW_PROCESS_GROUP & NORMAL_PRIORITY_CLASS, 
		NULL,	// same environment as the calling process
		NULL,	// same directory as the calling process
		&si,
		&pi);

	if (retc == FALSE) // CreateProcess failed
	{
		return FAILURE;
	}
     exitcode = PROCESS_STILL_ACTIVE;
     int RunTime = 0;
    
   while (exitcode == PROCESS_STILL_ACTIVE && (RunTime < 1000 * timeout_seconds || timeout_seconds < 0)) {
    GetExitCodeProcess( pi.hProcess, &exitcode);
    Sleep (1000);
    RunTime +=  1000;
  }
   // see if there is a crash
   //look for a crash of the application
   HWND lastbughwnd = 0;
   HWND bughwnd;
   while ((bughwnd = FindWindowEx(0, lastbughwnd, BugClassName, 0)) != NULL)
   {	
      crashed = TRUE;                     
	   	//rcKeys.SendKeys("[ENTER]", bughwnd);
	   if (::SetForegroundWindow(bughwnd))
	      keybd_event(VK_RETURN,0,0,0);
	  
       	lastbughwnd = bughwnd;
   }
   
   // One of the Cabanas variants displays a bunch of dialogs to OK
   int cabanas_dlgs = 0;
   while ((bughwnd = FindWindow(0,"Win32.Cabanas Monitor Box")) != NULL) 
   {
	   cabanas_dlgs++; //every 2 dialogs mean an extra infected file
       if (::SetForegroundWindow(bughwnd))
		   keybd_event (VK_RETURN,0,0,0);
	   Sleep(200); //give the sample time to show another dialog
   }
   EnumWindows(EnumWindowsProc, (LPARAM) 1);
//   Sleep(1000); // let another thread handle this
   if (cabanas_dlgs > 0 || crashed) //if saw cabanas dialog or virus crashed
	                           // CreateProcess worked
	   return SUCCESS; //no need to run the virus sample again

   GetExitCodeProcess( pi.hProcess, &exitcode);

   if (exitcode == PROCESS_STILL_ACTIVE && !virus ) { // will not kill virus process yet
	rcTrace.ReportError("killing process %s\n", Path);
    TerminateProcess(pi.hProcess, 1);
   } 
   if (exitcode != PROCESS_STILL_ACTIVE && virus) // some viruses do not infect
	                                      // when run with CreateProcess
   {
         system(Path);
		    // see if there is a crash
        HWND lastbughwnd = 0;
        HWND bughwnd;
        while ((bughwnd = FindWindowEx(0, lastbughwnd, BugClassName, 0)) != NULL)
		{	
                           
	   	//rcKeys.SendKeys("[ENTER]", bughwnd);
	        if (::SetForegroundWindow(bughwnd))
	          keybd_event(VK_RETURN,0,0,0);
	 
       	     lastbughwnd = bughwnd;
		}

   }
   EnumWindows(EnumWindowsProc, (LPARAM) 1); 
   Sleep(1000); //let another thread kill windows 
   return SUCCESS;
}

int RCController::GoatsInfected()
{
	//first parse checkup.rep for possibly infected files
    			  
    // loop through goat list and see if file has been changed
     POSITION pos = m_GoatList.GetHeadPosition();
	 POSITION pos1;
	 CString strTmp;
	 while (pos != NULL) {
        strTmp = m_GoatList.GetNext(pos);
        if ((pos1 = m_ChangedFiles.Find(strTmp)) != NULL)
		{                                     // found
			rcTrace.ReportError("Changed goat file: %s\n", strTmp);
			m_ChangedGoats.AddTail(strTmp);
			m_goats_infected ++;
		}

	 }
   
   return m_goats_infected;
}

// to avoid unnecessary infections in \windows, only execute goats in temp dir
void RCController::ExecuteGoats(CString strGoatsDir)
{
	POSITION pos = m_GoatList.GetHeadPosition();
	CString strTmp;
	strGoatsDir.MakeUpper();
	while (pos != NULL) {
		strTmp = m_GoatList.GetNext(pos);
		if (strTmp.Find(strGoatsDir) == 0) {
		   ReportProgress(NULL, "", "state","rungoat");
		   system(strTmp);
		   Sleep(500);
          // see if there is a crash
           HWND lastbughwnd = 0;
           HWND bughwnd;
           while ((bughwnd = FindWindowEx(0, lastbughwnd, BugClassName, 0)) != NULL)
		   {	
                           
	   	//rcKeys.SendKeys("[ENTER]", bughwnd);
	          if (::SetForegroundWindow(bughwnd))
	            keybd_event(VK_RETURN,0,0,0);
	 
       	      lastbughwnd = bughwnd;
		   }

           EnumWindows(EnumWindowsProc, (LPARAM) 1); //kill windows that popped up by the goat
		}
	}
}

// for now we'll use hardcoded dir name
void RCController::CopyResults(CString strResultsDir)
{
	CString strSrc;
	CString strDst;
	CString strTmp;

    if(access(strResultsDir,00) != 0) // no directory -- we'll change later
        mkdir(strResultsDir);
     
	// copy new files
	POSITION pos = m_NewFiles.GetHeadPosition();
	if (m_trace)
      rcTrace.ReportError("New files\n");
	while (pos != NULL) {
		strSrc = m_NewFiles.GetNext(pos);
        GetFileName(strSrc, strTmp); //extract file name
		strDst = strResultsDir + '\\' + strTmp;
		if (m_trace)
		  rcTrace.ReportError(" %s\n", strSrc);
		FileCopy(strSrc, strDst);
    }
     
	if (m_trace)
	  rcTrace.ReportError("Goats changed:\n");
	if (m_goats_infected > 0) // if there are goats infected copy them, else
		                    // copy all changed files
	{
    	POSITION pos = m_ChangedGoats.GetHeadPosition(); //** later move to separate fn
    	while (pos != NULL) {
	      strSrc = m_ChangedGoats.GetNext(pos);
          GetFileName(strSrc, strTmp); //extract file name
		  if (m_trace)
		     rcTrace.ReportError("%s\n", strSrc);
	      strDst = strResultsDir + '\\' + strTmp;
	      FileCopy(strSrc, strDst);
		}


	} else {
		if (m_trace)
		  rcTrace.ReportError("Files changed\n");
    	POSITION pos = m_ChangedFiles.GetHeadPosition(); //** later move to separate fn
    	while (pos != NULL) {
	      strSrc = m_ChangedFiles.GetNext(pos);
          GetFileName(strSrc, strTmp); //extract file name
		  if (m_trace)
		     rcTrace.ReportError("%s\n", strSrc);
	      strDst = strResultsDir + '\\' + strTmp;
	      FileCopy(strSrc, strDst);
		}

	}
}

void RCController::CopyAllFiles(CString strSrc, CString strDst)
{

     //Search thru the directory and delete all its contents.

        struct _finddata_t tFileBlk;

        char szTmp[MAXPATH]; // temporary for directory
        unsigned int nRet; // return code from findnext
        long hFile;

        strcpy(szTmp,strSrc);
        AppendPath(szTmp,"\\*.*");

        hFile = _findfirst(szTmp,&tFileBlk);
        nRet = (hFile == -1) ? 1 : 0;


        while(nRet == 0 )
        {
                // skip . and .. entries

                strupr(tFileBlk.name);

                if (strcmp(tFileBlk.name,".") && strcmp(tFileBlk.name,".."))
                {
                  if(tFileBlk.attrib & _A_SUBDIR )
                  {
                          // subdirectory
                          //Nothing to do now.
                  }
                  else
                  {
                         // not a subdirectory
                         CString tmpSrc = strSrc + '\\' + tFileBlk.name;
						 CString tmpDst = strDst + '\\' + tFileBlk.name;
                                                
                         FileCopy(tmpSrc, tmpDst);
                  }
                }

                nRet = _findnext(hFile, &tFileBlk);

        }

        _findclose(hFile);

        return;
}

// Remember values of registry keys, a virus is most likely
// to change
void RCController::GetRegistryValues(CMapStringToString RegistryValues[])
{
   GetValuesOfRegKey(RegistryValues[HKEY_MACHINE_RUN], HKEY_LOCAL_MACHINE, RUN_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_MACHINE_RUNONCE],HKEY_LOCAL_MACHINE, RUNONCE_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_MACHINE_RUNSERVICES],HKEY_LOCAL_MACHINE, RUNSERVICES_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_MACHINE_RUNSERVICESONCE],HKEY_LOCAL_MACHINE, RUNSERVICESONCE_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_USER_RUN],HKEY_CURRENT_USER, RUN_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_USER_RUNONCE],HKEY_CURRENT_USER, RUNONCE_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_USER_RUNSERVICES],HKEY_CURRENT_USER, RUNSERVICES_REG_PATH);
   GetValuesOfRegKey(RegistryValues[HKEY_USER_RUNSERVICESONCE],HKEY_CURRENT_USER, RUNSERVICESONCE_REG_PATH);
}

// check if reboot is required
bool RCController::IsRebootRequired()
{
  CMapStringToString NewRegistryValues[8];
  GetRegistryValues(NewRegistryValues);
  CString strName;
  CString strValue;
  int i;
  POSITION pos;
 
  // if aleady have enough replicants no need to reboot
  // Note: this may change if need to report registry changes
  // to the next stage
  if (m_ChangedFiles.GetCount() > MIN_REPLICANTS) // enough replicants
    return FALSE; //no need to reboot and try again

  else if (m_ChangedFiles.GetCount() > 0) // see if changed kernel32.dll
  {                                       // may need to add other conditions
	  CString strFileName;
	  CString strKernel = m_WindowsDir;
	  strKernel += "\\SYSTEM\\KERNEL32.DLL";
      for (pos = m_ChangedFiles.GetHeadPosition(); pos != NULL;)
	  {
		  strFileName = m_ChangedFiles.GetNext(pos);
		  if (strFileName == strKernel)
			  return TRUE; //reboot required
	  }
  }

  // see if new files in \windows or its subdirectories
  if (m_NewFiles.GetCount() > 0)
  {
	 CString strFullName;
	 CString strFileName;
	 CString strDirName;
	// CString strWinSys = m_WindowsDir;
	// strWinSys += "\\SYSTEM";
	 for (pos = m_NewFiles.GetHeadPosition(); pos != NULL;)
	 {
        strFullName =m_NewFiles.GetNext(pos);
        int i = strFullName.ReverseFind('\\'); // last backslash
		strFileName = strFullName.Right(strFullName.GetLength() - i-1);
		strDirName = strFullName.Left(i);
		if (strDirName.Find(m_WindowsDir) == 0) //do it if any new file is present
			                               // can be a dll, a vxd or an exe or an ini
			return TRUE; //reboot required
	 }
  } 
  // loop through new registry values to see if anything was added
  for (i = 0; i < 8; i++)
  {
	  for (pos = NewRegistryValues[i].GetStartPosition(); pos != NULL;)
	  {
          NewRegistryValues[i].GetNextAssoc(pos, strName, strValue);
          if (m_RegistryValues[i][strName] != strValue) // something changed
		  {
			  if (m_trace)
				  rcTrace.ReportError("In key %d, name %s, new value %s\n",
				              i, strName, strValue);
			  return TRUE;
		  }
	  }
  }
	// check win.ini file to see if anything added there
    // since we know our configuration, we know there is
    // nothing on run= and load= lines initially
   CString strWinIni;
   strWinIni = m_WindowsDir;
   strWinIni += "\\win.ini";
   FILE *win;
   char input_line[1000];
   
   if ((win = fopen(strWinIni, "r")) == NULL)
     return FALSE; //cannot read win.ini assume reboot not required

   while (fgets(input_line, 1000, win) != NULL) {
	 strupr(input_line);
     char *p = input_line;
     while (*p == ' ') p++;
     if (!memcmp(p,"RUN",3) || !memcmp(p, "LOAD",4)) { // got to run= line
	    p = strchr(input_line, '=') +1;
		while (p != NULL && *p != '\n' && *p != '\0' (*p == ' ' || *p == '\t')) p++; 
        if (p!= NULL && *p != '\n' && *p != '\0') //something added
			return TRUE; //reboot required
	 }

   } /* endwhile */
   fclose(win);
  
  
  return FALSE;
}
// Get all the leaves off a specified registry key
void RCController::GetValuesOfRegKey(CMapStringToString &KeyValues, HKEY hKey, char* path)
{
	CString strVar;
	CString strValue;
    HKEY hKeyReg;
	char keyname[200];
	DWORD keylength = sizeof keyname;
	FILETIME ftLastWriteTime;

	if(RegOpenKeyEx(hKey,path,0,KEY_READ, &hKeyReg) == ERROR_SUCCESS) 
	{
       DWORD dwIndex = 0;
       DWORD dType;
	   char keyvalue[1000];
       DWORD dSize = sizeof keyvalue;
	   
	   while (RegEnumKeyEx(hKeyReg, dwIndex, (LPTSTR) keyname, &keylength, NULL,
		   NULL, NULL, &ftLastWriteTime) == ERROR_SUCCESS)
	   {

          if(RegQueryValueEx(hKeyReg,keyname,0,(LPDWORD)&dType,
                (LPBYTE)(keyvalue),(LPDWORD)&dSize) == ERROR_SUCCESS)
			 KeyValues[keyname] = keyvalue; // add value to list

	   }	   
	}	   
  
     return;
}

void RCController::ReportProgress(CWin32rcDlg *dlg, CString strMsg, CString strType,CString strPhase)
{
	if (dlg != NULL && strMsg != "")
	{
      dlg->SendMessage(ON_SETTEXT, (WPARAM) &strMsg, 0);
     
	  dlg->UpdateWindow();
	}
	if (m_trace)
		if (strMsg != "")
		  rcTrace.ReportError("%s\n", strMsg);
		else
		  rcTrace.ReportError("%s\n",strPhase);

#ifndef _NOSOCK
	if (strPhase != "")
	{
		CString strTxt;
      
		strTxt = "win32rc|";
		strTxt +=  m_strIdentifier + "|" + strType + "|" + strPhase + "\n";
        if (m_socket->Send(strTxt, strTxt.GetLength(),0) == SOCKET_ERROR)
		  rcTrace.ReportError("Socket error sending %s rc= %d\n",
		                      strTxt, m_socket->GetLastError());
	}
#endif

} 

#define MAXLINE 300
int RCController::ProcessCommands(CString strCommandFile)
{
    FILE *commandsfile;	
	char TextLine[300];
      
	// open the commands file
    if ((commandsfile = fopen (strCommandFile,"r")) == NULL)
    {
        rcTrace.ReportError("Unable to open the commands file : %s.", strCommandFile);
        ReportProgress(NULL, "","message","Unable to open the commands file");
		return 0;
    }
    CWin32rcDlg dlg;

	// do the standard initialization
	ReportProgress(&dlg, "Initializing", "state","startup");
	Initialize();
    
	while (fgets(TextLine, MAXLINE-1, commandsfile) != NULL) {
		//get rid of leading/trailing blanks
      CString strTmp(TextLine);
      strTmp.TrimLeft();
	  strTmp.TrimRight();
	  if (strTmp.IsEmpty() || strTmp[0] == '#') continue;

	  CString cmd;
	  CString param;
      
	  int i = strTmp.Find('|');
	  if (i >=0) {
		  cmd = strTmp.Left(i);
		  param = strTmp.Right(strTmp.GetLength() - i);
	  } else {
		  cmd = strTmp;
		  param.Empty();
	  }
	  cmd.MakeUpper();
      if (cmd == "STANDARD")
		  StandardReplication(1);
	  else if (cmd == "GETINFO") {
 	  	ReportProgress(&dlg, "Obtaining system configuration information","state","checking");
	    if (rcCheck.InitFileList(m_after_reboot) != SUCCESS)
		{
		   ReportProgress(NULL, "", "message","unable to open database file after reboot");
		   dlg.DestroyWindow();
		   return FAILURE;
		}
	  } else if (cmd == "RUNSAMPLE") {
          EnumWindows(EnumWindowsProc, (LPARAM) 0); //get the list of windows open now
          ReportProgress(&dlg, "Running the virus sample","state", "runsample");
          int exitcode = RunProgram(m_SampleName,10,1);
          if (m_trace) 
	         rcTrace.ReportError("run sample\n");
	  } else if (cmd == "RUNGOATS") {
	     	EnumWindows(EnumWindowsProc, (LPARAM) 0); //get the list of windows open now
            ExecuteGoats(m_GoatsDir);
	      //*  EnumWindows(EnumWindowsProc, (LPARAM) 1); //get the list of windows open now
	  } else if (cmd == "RUN") {
		  if (!param.IsEmpty()) {
            EnumWindows(EnumWindowsProc, (LPARAM) 0); //get the list of windows open now
            ReportProgress(&dlg, "Running the program","state", "runprogram");
		    int exitcode = RunProgram(param,10,0 );
            if (m_trace)
			   rcTrace.ReportError("run program %s\n", param);
		  } else 
			  ReportProgress(NULL, "", "message", "error - program is not specified");
	  } else if (cmd == "CHECKFILES") {
	        // check files for changes
	      ReportProgress(&dlg, "Checking files for changes","state", "checking");
          rcCheck.CheckFiles(&m_NewFiles, &m_ChangedFiles);
	  } else if (cmd == "SLEEP") {
		  if (param.IsEmpty())
			  ReportProgress(NULL, "","message", "sleep interval is not specified");
		  else
			  Sleep(atoi(param));
	  } else if (cmd == "EXITIFDONE") {
		  if (GoatsInfected()) {
			  ReportProgress(NULL, "", "state","shutdown");
			  return m_goats_infected;
		  }
	  }

	}
   return m_goats_infected;
}
