/*******************************************************************
*                                                                  *
* File name:     rc.cpp                                            *
*                                                                  *
* Description:   The main application for the replication          *
*                controller for the macro viruses                  *
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
* Function:      Attempts to replicates macro viruses              *
*                                                                  *
*******************************************************************/

#include "stdafx.h"
#include "afxtempl.h"
#include "afxole.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <ddeml.h>
#include <direct.h>
#include <string.h>
#include "rcdefs.h"
#include "errors.h"
#include "RCCommon.h"
#include "RCError.h"
#include "RCFile.h"
#include "rcmsgfilter.h"
#include "rc.h"
#include "RCKeys.h"
#include "RcFrame.h"
#include "RCConfig.h"
#include "RCGoatList.h"
#include "RCPilot.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRcApp

BEGIN_MESSAGE_MAP(CRcApp, CWinApp)
	//{{AFX_MSG_MAP(CRcApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRcApp construction

CRcApp::CRcApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRcApp object

CRcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRcApp initialization

BOOL CRcApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


		// Initialize OLE 2.0 libraries
	if (!AfxOleInit())
	{
		AfxMessageBox("Ole initialization failed");
		return FALSE;
	}
	    m_pMsgFilter  =NULL;
		    m_pMsgFilter=new RCMessageFilter();

    if (NULL!=m_pMsgFilter)
        {
        m_pMsgFilter->AddRef();

        if (FAILED(CoRegisterMessageFilter(m_pMsgFilter, NULL)))
            //ReleaseInterface(m_pMsgFilter);
			m_pMsgFilter->Release();
        }

	static RcFrame rcFrame;
    m_pMainWnd = &rcFrame;
    
    rcFrame.ShowWindow(m_nCmdShow);
 
   
    getcwd(ResultPath, MAXPATH-1); // get current directory
        
	char tmpPath[MAXPATH];
    strcpy(tmpPath, ResultPath);
    Common.AppendPath(tmpPath,"replic.lst");
    unlink(tmpPath);
    strcpy(tmpPath, ResultPath);
	// initialize configuration
    RCConfig rcConfig(&Common);
    rcConfig.InitConfig(m_lpCmdLine, AppName, AppVersion);
    IdleCount = 0;
	return TRUE;
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

}

void CRcApp::Triage(CString AppName)
{
	CString DocExt;
                       
      
    RCPilot rcPilot(&Common, ResultPath, AppName, AppVersion);
    rcPilot.ExecuteCommands();

}

void CRcApp::Clean(char *szDir)
{
 
   // search thru the TempPath directory and delete all its contents
          struct _finddata_t tFileBlk;  // structure returned from findfirst/findnext
          long hFile;
  
       char szTmp[MAXPATH]; // temporary for directory
        int nRet; // return code from findfirst
      
        strcpy(szTmp, szDir);
        Common.AppendPath(szTmp,"\\*.*");
        hFile =_findfirst(szTmp,&tFileBlk);
        nRet = (hFile == -1) ? 1 : 0;           
        while(!nRet)
        {
                // skip . and .. entries

                strupr(tFileBlk.name);

                if (strcmp(tFileBlk.name,".") && strcmp(tFileBlk.name,".."))
                {
                  if (tFileBlk.attrib & _A_SUBDIR)
                  {
                          // subdirectory
                          //Nothing to do now.
                  }
                  else
                  {
                         // not a subdirectory
                         strcpy(szTmp,szDir);
                         Common.AppendPath(szTmp,tFileBlk.name);
                         remove(szTmp);
                  }
                }
                nRet = _findnext(hFile, &tFileBlk);
        }
   _findclose(hFile);

// remove the possibly infected normal.dot
    unlink(Common.GlobalPath);

}

int CRcApp::ExitInstance() 
{
        if (NULL!=m_pMsgFilter)
        {
        CoRegisterMessageFilter(NULL, NULL);
        m_pMsgFilter->Release();
        }
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::ExitInstance();
}

BOOL CRcApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
   if(IdleCount <= 2) IdleCount++;
   if(IdleCount > 1) return FALSE;
    Triage(AppName);   
	PostQuitMessage( RCOk );       

	return CWinApp::OnIdle(lCount);
}
