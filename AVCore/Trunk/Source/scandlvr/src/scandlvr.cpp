// ScanDlvr.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ScanDlvr.h"
#include "ScanWiz.h"
#include "User1Pg.h"
#include "CorpPg.h"
#include "ReviewPg.h"
#include "WrapUpPg.h"
#include "AcceptPg.h"
#include "RejectPg.h"
#include "IScanDeliver.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// SCOTT -- this is only temporary!!
#define SCAN_DELIVER_CONFIG_FILE    "c:\\temp\\ScanCfg.dat"


extern HINSTANCE g_hInstance;;   // Global instance handle to this module.


//
//  Note!
//
//      If this DLL is dynamically linked against the MFC
//      DLLs, any functions exported from this DLL which
//      call into MFC must have the AFX_MANAGE_STATE macro
//      added at the very beginning of the function.
//
//      For example:
//
//      extern "C" BOOL PASCAL EXPORT ExportedFunction()
//      {
//          AFX_MANAGE_STATE(AfxGetStaticModuleState());
//          // normal function body here
//      }
//
//      It is very important that this macro appear in each
//      function, prior to any calls into MFC.  This means that
//      it must appear as the first statement within the
//      function, even before any object variable declarations
//      as their constructors may generate calls into the MFC
//      DLL.
//
//      Please see MFC Technical Notes 33 and 58 for additional
//      details.
//

/////////////////////////////////////////////////////////////////////////////
// CScanDlvrApp

BEGIN_MESSAGE_MAP(CScanDlvrApp, CWinApp)
    //{{AFX_MSG_MAP(CScanDlvrApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDlvrApp construction

CScanDlvrApp::CScanDlvrApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// global objects

CScanDlvrApp                theApp;
CScanDeliverConfiguration   g_ConfigInfo;


/////////////////////////////////////////////////////////////////////////////
// Exported Functions


// ==== StartSarcDeliver ==================================================
//
//  This function
//  Input:
//  Output:
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

extern "C" BOOL PASCAL EXPORT StartSarcDeliver(BOOL  bCreateDatFile)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    auto    BOOL        bResult = TRUE;

    // initialize the global configuration data object
    auto    DWORD       dwErrorBits = 0;
    g_ConfigInfo.Initialize(SCAN_DELIVER_CONFIG_FILE, bCreateDatFile);
    g_ConfigInfo.ReadConfigFile(&dwErrorBits);

    // display the wizard panels
    auto    CScanDeliverWizard*         pWizSheet;
    auto    CUserInfo1PropertyPage      wizPage1;
    auto    CorpInfoPropertyPage        wizPage3;
    auto    CReviewPropertyPage         wizPage5;
    auto    CWrapUpPropertyPage         wizPage6;
    auto    CAcceptedFilesPropertyPage  acceptedPage;
    auto    CRejectedFilesPropertyPage  rejectedPage;

    // create an instance of the property sheet
    pWizSheet = new CScanDeliverWizard(IDS_SCAN_WIZARD_CAPTION);

    // add the property pages to the property sheet
    pWizSheet->AddPage(&acceptedPage);
    pWizSheet->AddPage(&rejectedPage);
    pWizSheet->AddPage(&wizPage1);
    pWizSheet->AddPage(&wizPage3);
    pWizSheet->AddPage(&wizPage5);
    pWizSheet->AddPage(&wizPage6);

    // place the sheet in "wizard" mode
    pWizSheet->SetWizardMode();

    // display the wizard panels
    pWizSheet->DoModal();


    // free up alloated memory
    delete  pWizSheet;

    return (bResult);

}  // end of "StartSarcDeliver"

BOOL CScanDlvrApp::InitInstance()
{
    // TODO: Add your specialized code here and/or call the base class
    g_hInstance = AfxGetInstanceHandle();

    return CWinApp::InitInstance();
}