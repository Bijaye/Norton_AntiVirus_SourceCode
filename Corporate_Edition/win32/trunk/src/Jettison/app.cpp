// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//-----------------------------------------------------------------------------
// App: Implementation of the following CApp Class:
//
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Tedn@Thuridion.com      12/22/03    Replaces S32UI as Main so that
//										the Application now supports
//										writing to the Console.
//										This allows us to:
//										
//										1)  Writes "usage" string to
//											Console.
//										2)  Echo processed Script filenames
//											to Console.
//					
//										Note: Detailed application results
//											  continue to be written to
//											  Status.ini file.
//
//										Should a developer wish to add a UI
//										to this application in the future,
//										you can use the S32UI project.
//
//  Tedn@Thuridion.com      12/30/03    Added /IP switch to usage line.  This
//										gives the user the option to ignore the
//										platform specifier in the script file.
//										This allows older script files to be 
//										run against all platforms.
//										Made call to new Utils::DisplayUsage func.
//-----------------------------------------------------------------------------
#include <afxwin.h>

#include "app.h"
#include "rfile.h"
#include "utils.h"
#include "DarwinResCommon.h"

#if !defined(_CONSOLE)
   #error Make it a console application project
#endif

int gArgc;

DARWIN_DECLARE_RESOURCE_LOADER(_T("ConsoleRes.dll"), _T("Console"))

/////////////////////////////////////////////////////////////////////////////
// DECLARE_CONSOLEAPP - enables MFC-like console app

BOOL WINAPI LMFC_ConsoleCtrlHandler(DWORD dwCtrlType)
{
   switch(dwCtrlType)
   {
      case CTRL_C_EVENT:
      case CTRL_BREAK_EVENT:
      case CTRL_CLOSE_EVENT:
      case CTRL_LOGOFF_EVENT:
      case CTRL_SHUTDOWN_EVENT:
         AfxGetApp()->ExitInstance();
         break;
      default:
         break;
   }
   return FALSE;
}      

#ifdef _CONSOLE
   #include <wincon.h>
   #define DECLARE_CONSOLEAPP()\
      extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nCmdShow); \
      extern "C" int _tmain( int argc, LPTSTR /*argv*/[ ], LPTSTR /*envp*/[]) \
      { gArgc = argc;\
		 SetConsoleCtrlHandler(LMFC_ConsoleCtrlHandler, TRUE); \
         return AfxWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW); \
      }
      // remember to instantiate a CWinApp
#endif // _CONSOLE

/////////////////////////////////////////////////////////////////////////////
// CMyCommandLineInfo implementation 

CMyCommandLineInfo::CMyCommandLineInfo(void) : CCommandLineInfo()
{
	m_bProcessAll = false;
	m_bHelp = false;
}

void CMyCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bSwitch, BOOL /*bLast*/)
{

	CString strParam = lpszParam;

	strParam.MakeUpper();
	
   if (bSwitch)
   {
      if (strParam == "HELP")
         m_bHelp = TRUE;
	  else if (strParam == "ALL")
      {
         m_bProcessAll = TRUE;
      }
	  else
	  {
         // the for loop enables 'compound' switches like "/XYZ"
         for(int i = 0; (i < lstrlen(lpszParam)); i++)
         {
            switch(lpszParam[i])
            {
               case _T('?'):
               case _T('h'):
               case _T('H'):
                  m_bHelp = TRUE;
                  break;
            }
         }
      }
   }
   else
   {
      m_strParameter = lpszParam;
   }
}

/////////////////////////////////////////////////////////////////////////////
// Instatiate the application class

DECLARE_CONSOLEAPP()

/*static*/ CApp  CApp::m_app;
/*static*/ CApp& CApp::GetApp(void)
{ 
   return m_app; 
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of the application class

CApp::CApp(void) : CWinApp()
{
}

CApp::~CApp(void)
{
}

BOOL CApp::InitInstance()
{
   if (!CWinApp::InitInstance())
   {
      return FALSE;
   }
   if(SUCCEEDED(g_ResLoader.Initialize()))
   {
	   AfxSetResourceHandle(g_ResLoader.GetResourceInstance());
   }
   else
   {
	   return FALSE;
   }
   
   //  If nothing was entered on the command line, show help
   if (1 == gArgc) 
	   m_cmdInfo.m_bHelp = true;
   else
	   ParseCommandLine();
   
   return TRUE;
}

int CApp::Run()
{
   if (m_cmdInfo.m_bHelp)
	   Utils::DisplayUsage ();
   else
   {
      m_cmdInfo.m_strParameter;
   	  CProcessor processor;

	  processor.DoWork();
   }   

   return CWinApp::Run(); // calls ExitInstance and exits right away when m_pMainWnd=NULL
}

int CApp::ExitInstance()
{
   return CWinApp::ExitInstance();
}

