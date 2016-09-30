/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/n32opts.cpv   1.4   19 Feb 1998 11:57:12   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
//
// n32opts.cpp : Defines the initialization routines for the DLL.
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/n32opts.cpv  $
// 
//    Rev 1.4   19 Feb 1998 11:57:12   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.1   16 Sep 1997 19:55:36   MDUNN
// Added the IS_NEC() function to the app class.
// 
//    Rev 1.3   11 Jul 1997 16:55:32   DBUCHES
// Few changes for Savanna folks.  Now, clients can specify whether to use
// the NT specific pages."
// 
//    Rev 1.2   20 Jun 1997 12:03:54   DBUCHES
// Added help support.
// 
//    Rev 1.1   27 May 1997 22:19:14   DBUCHES
// Added support for dynamically loading ALERTUI.DLL
// 
//    Rev 1.0   14 May 1997 14:58:18   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//


#include "stdafx.h"

#include "n32opts.h"
#include "sheet.h"
#include "navsheet.h"


// testtest
#include "navutil.h"
#include "navbin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CN32optsApp

BEGIN_MESSAGE_MAP(CN32optsApp, CWinApp)
	//{{AFX_MSG_MAP(CN32optsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN32optsApp construction

CN32optsApp::CN32optsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    m_hInstAlertUI = NULL;
    m_bUseNTStuff = !(GetVersion() & 0x80000000);

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CN32optsApp object

CN32optsApp theApp;



BOOL CN32optsApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
    m_hInstAlertUI = AfxLoadLibrary(_T("ALERTUI.DLL")); // IPE Ignore
	
    // Setup help file
    // Need to free old string allocation
    free((void*)m_pszHelpFilePath);
    
    //Change the name of the .HLP file.
    //The CWinApp destructor will free the memory.
    m_pszHelpFilePath=_tcsdup(IS_NT() ? _T("navwnt.hlp") : _T("navw32.hlp"));// IPE Ignore


	return CWinApp::InitInstance();
}

int CN32optsApp::ExitInstance() 
{
    // Free ALERTUI module.
    if(m_hInstAlertUI)
        {
        AfxFreeLibrary(m_hInstAlertUI);
        m_hInstAlertUI = NULL;
        }
	
	return CWinApp::ExitInstance();
}

int CN32optsApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt) 
{
    // Cover AfxMessageBox() with our own.
	CString sTitle;
	sTitle.LoadString(IDS_ERROR_TITLE);
    
    return m_pOptionsWindow->MessageBox( lpszPrompt, sTitle, nType );
}

BOOL CN32optsApp::IsNT()
{
    return m_bUseNTStuff;
}

// This is a copy of HWIsNEC() from Quake, pulled local here since this
// DLL doesn't use Quake.
BOOL CN32optsApp::IsNEC()
{
static BOOL bTested = FALSE;
static BOOL bIsNEC  = FALSE;

    if ( !bTested )
        {
        UINT uType;

        bTested = TRUE;
                                        // Get main KB type
        uType = GetKeyboardType(0);

        if ( uType == 7 )               // '7' is japanese keyboard type
            {
                                        // Get the sub-type.
            uType = GetKeyboardType(1);

            if ( uType >= 0x0D01 )
                {
                bIsNEC = TRUE;
                }
            }
        }

    return bIsNEC;
}

void CN32optsApp::SetNTFlag(BOOL bFlag)
{
    m_bUseNTStuff = bFlag;
}
