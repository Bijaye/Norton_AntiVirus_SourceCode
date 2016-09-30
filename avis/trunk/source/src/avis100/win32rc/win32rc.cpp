// win32rc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxdisp.h>
#include "afxtempl.h"
#include <stdlib.h>
#include <direct.h>
#include <process.h>
#include <io.h>
#include <afxsock.h>

#include "defs.h"
#include "drop.h"
#include "rcerror.h"
#include "polymath.h"
#include "rcfile.h"
#include "rcfilelist.h"
#include "rccheck.h"
#include "win32rc.h"
#include "win32rcdlg.h"
#include "rccontroller.h"
#include "rcframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



  
/////////////////////////////////////////////////////////////////////////////
// CWin32rcApp

BEGIN_MESSAGE_MAP(CWin32rcApp, CWinApp)
	//{{AFX_MSG_MAP(CWin32rcApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWin32rcApp construction

CWin32rcApp::CWin32rcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWin32rcApp object

CWin32rcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWin32rcApp initialization

BOOL CWin32rcApp::InitInstance()
{
#ifndef _NOSOCK
	if (!AfxSocketInit())
	{
        rcTrace.ReportError("Error:AfxSocketInit() failed\n");
		return FALSE;
	}
#endif

	static RcFrame rcFrame;
    m_pMainWnd = &rcFrame;
   
    rcFrame.ShowWindow(m_nCmdShow);

	//get parameters
	// initialize defaults
	m_SampleName = ""; // default sample name
	m_strResultsDir = "";
	m_CopyResults = FALSE;
	m_help = FALSE;
	m_strVarList = "";
	m_noshutdown = FALSE; //shutdown/restart by default
	m_nSocketPort = -1;
	m_strSocketAddr = "";
	m_strIdentifier = "";
#ifdef DEBUG
	m_trace = TRUE;
#else
    m_trace = FALSE; // no trace by default
#endif
	m_standard = TRUE;
	m_after_reboot = FALSE;

	if (ProcessParams() != SUCCESS || m_GoatsDir == "" 
#ifndef _NOSOCK
		 || m_nSocketPort == -1 || m_strSocketAddr == ""
#endif
			                     ) // error in parameters
	{
		rcTrace.ReportError("ERROR: invalid command-line arguments\n");
		rcTrace.ReportError("syntax: win32rc <sample-name> -goats=<goatsdir> [-results=<result-dir>]\n");
		rcTrace.ReportError("              -controlport <port> -controladdress <ip-address> -identifier <identifier>\n");
		rcTrace.ReportError("              [-trace] [-commands <comandsfile>] [-noshutdown]\n");
		return FALSE; //get out
	}

    if (m_help == TRUE)  //only asked for help
	{
		CString strHelp;
		strHelp = 	"win32rc <sample-name> -goats=<goatsdir> [-results=<result-dir>]\n"
			         "-controlport <port> -controladdress <ip-address> -identifier <identifier>\n"  
                    "         [-trace] [-commands <comandsfile>]";
		MessageBox(NULL, strHelp, "Syntax", MB_OK);

	}
    
#ifndef _NOSOCK
   if (!m_socket.Create()) 
   {
      rcTrace.ReportError("Unable to create socket err = %d\n",m_socket.GetLastError());
	  return FALSE; // get out
   } else if (!m_socket.Connect(m_strSocketAddr, m_nSocketPort)) 
   {
	   m_socket.Close();
	   rcTrace.ReportError("ERROR: Unable to connect to socket err = %d\n", m_socket.GetLastError());
	   return FALSE; //get out
   } 
#endif
	return TRUE;
}
 
// Use this to do main processing

BOOL CWin32rcApp::OnIdle(LONG lCount) 
{
	int goats_infected;
   // make sure do not do this more than once
   if(lCount > 0) return FALSE;
    //   perform main replication 
    RCController rcController(m_SampleName, m_GoatsDir, &m_GoatsDirs2, m_trace, m_after_reboot,
		                       &m_socket, m_strIdentifier);   
	if (m_standard)
	  goats_infected = rcController.StandardReplication();
	else
	  goats_infected = rcController.ProcessCommands(m_strCommandFile);
	if (goats_infected == FAILURE) // error encountered
	{
		PostQuitMessage(0);
		return FALSE;
	}
	m_reboot_required = rcController.ReportResults(m_copy_results, m_strResultsDir);
	PostQuitMessage(0);
	return CWinApp::OnIdle(lCount);
}

int CWin32rcApp::ExitInstance() 
{
  
    if (m_reboot_required)
	  doDrop(1,1, 0);
    else if (!m_noshutdown)
	  doDrop(1, 0, 0);

    return CWinApp::ExitInstance();
}
// Process command-line parameters
int CWin32rcApp::ProcessParams()
{
    CStringList Params;
	CString strParam;
	CString strKeyword;
    
	GetArgs(Params);
	
	if (Params.GetCount() == 0)
		return FAILURE; //at least the name of virus sample must be specified
	POSITION pos = Params.GetHeadPosition();
	CString strSample = Params.GetNext(pos);
	if (access(strSample, 00) != 0) // invalid sample
      return FAILURE; //no need to continue
	m_SampleName = strSample;
	m_SampleName.MakeUpper();
    while (pos != NULL)
	{
		strParam = Params.GetNext(pos);
		strParam.MakeLower();
		if (strParam == "-trace") 
			m_trace = TRUE;
		else if (strParam == "-commands") {
			m_standard = FALSE;
			if (pos != NULL)
			  m_strCommandFile = Params.GetNext(pos);
			else
			  return FAILURE; //invalid params
		} else if (strParam == "-goats") { // primary goats directory
			if (pos != NULL)
			   m_GoatsDir = Params.GetNext(pos);
			else
			   return FAILURE;
		} else if (strParam == "-goats2") {
			if (pos != NULL)
				m_GoatsDirs2.AddHead(Params.GetNext(pos));
		     else
				 return FAILURE;
		} else if (strParam == "-results") {
			m_copy_results = TRUE;
			if (pos != NULL)
			  m_strResultsDir = Params.GetNext(pos);
			else
			  return FAILURE;
		} else if (strParam == "-afterreboot") {
			m_after_reboot = TRUE;
		} else if (strParam == "-noshutdown") {
			m_noshutdown = TRUE;
		} else if (strParam == "-controlport") {
			if (pos != NULL)
		       m_nSocketPort = atoi(Params.GetNext(pos));
			else
				return FAILURE;
		} else if (strParam == "-controladdress") {
			if(pos != NULL)
			   m_strSocketAddr = Params.GetNext(pos);
			else
			   return FAILURE;
		} else if (strParam == "-identifier") {
			if(pos != NULL)
			   m_strIdentifier = Params.GetNext(pos);
			else
			   return FAILURE;
		} else if (strParam.GetAt(1) == '?' || strParam == "-help")
			m_help = TRUE;
	}	

  
	Params.RemoveAll();
	return SUCCESS;
}

// Convert the command line into CStringList arguments
// Converts Windows CommanLine into argc, argv arguments
void CWin32rcApp::GetArgs (CStringList &Params)
{
  
  //int index;
  char *p;
  char temp[MAXPATH];
  int length;
  char *q;

  p = m_lpCmdLine;
  while (*p != '\0' && *p == ' ') p++; //skip blanks
  while (*p != '\0')
  {
     length = 0;
     q = temp;
     while (*p != '\"' && *p != ' ' && *p != '\0' && length < MAXPATH)
     {
       *q++ = *p++;

       length++;
     }
     if (*p == '\"')  // found a quote
     {
       p++;        // skip it
       while (*p != '\"' && *p != '\0' && length < MAXPATH) // copy till end of quote
       {
        *q++ = *p++;

        length++;
       }
           if (*p == '\"') p++;
      }
      *q = '\0';
      Params.AddTail(temp);
      while (*p != '\0' && *p == ' ') p++; //skip blanks
     }
   return;

}

