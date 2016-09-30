////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVTaskWizard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#define INITIIDS
#include "ScanTaskInterface.h"
#include "NAVTrust.h"

#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "NAVTaskWizard.h"
#include "wizard.h"

#include "FolderBrowseDlg.h"    // We expose folder browse here
#include "DrivePickerDlg.h"

#include "isSymTheme.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymInstalledApps.h"
#include "SmartModuleHandle.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(L"NAVTskWz");
IMPLEMENT_CCTRACE(::g_DebugOutput);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace avScanTask;

BOOL    g_bUseHiColorBmps;

// Handle to resource instance NavTskWz.loc
StahlSoft::CSmartModuleHandle g_ResInst;

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
// CNAVTaskWizardApp

BEGIN_MESSAGE_MAP(CNAVTaskWizardApp, CWinApp)
	//{{AFX_MSG_MAP(CNAVTaskWizardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNAVTaskWizardApp construction

CNAVTaskWizardApp::CNAVTaskWizardApp()
{
    // Decide whether to use 256-color BMPs.  We do if the system is running
    // in _more_ than 256 colors right now.
HDC   hDC = GetDC ( ::GetDesktopWindow() );
int   nColorDepth;
DWORD dwDisplayableColors;

    g_bUseHiColorBmps = FALSE;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        {
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
        g_bUseHiColorBmps = ( dwDisplayableColors > 256 );
        }
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );

        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;

        g_bUseHiColorBmps = ( dwDisplayableColors > 256 );
        }

    ReleaseDC ( ::GetDesktopWindow(), hDC );
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNAVTaskWizardApp object

CNAVTaskWizardApp theApp;


extern "C"
HRESULT WINAPI NAVTaskWizard(avScanTask::IScanTask* pScanTask, int iMode)
{
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication() )
    {
        return E_ACCESSDENIED;
    }

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    pScanTask->AddRef();

    int iReturn = CTaskWizard( pScanTask, iMode, 0, AfxGetMainWnd (), 0).DoModal();
    
    pScanTask->Release();

    if ( ID_WIZFINISH == iReturn )
    	return S_OK;
    else
        return S_FALSE;
}

extern "C"
HRESULT WINAPI DrivesDialog(avScanTask::IScanTask* pScanTask)
{
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication() )
    {
        return E_ACCESSDENIED;
    }

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    pScanTask->AddRef();

	// Apply SymTheme 
	// Do not bail out if failed.
	// Should continue to display our window
	HRESULT hr1 = S_OK;
	CISSymTheme isSymTheme;
	hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
	if(FAILED(hr1))
	{
		CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
	}

    CDrivePickerDlg dlgDrives ( pScanTask );
    int iModalRet = 0;

    pScanTask->Release();

    iModalRet = dlgDrives.DoModal (); 

    if ( IDOK == iModalRet )
    {
        return S_OK;
    }
    else 
        return S_FALSE;
}

extern "C"
HRESULT WINAPI FolderDialog(avScanTask::IScanTask* pScanTask)
{
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication() )
    {
        return E_ACCESSDENIED;
    }

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hrRet = S_FALSE;
    pScanTask->AddRef();

	// Create SymTheme Skin window.
	// Do not bail out if failed.
	// Should continue to display our window
	HRESULT hr1 = S_OK;
	CISSymTheme isSymTheme;
	hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
	if(FAILED(hr1))
	{
		CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
	}

    CFolderBrowseDlg dlg;
    int              nModalRet;

    // We are about to scan these, not just add them.
    //
    dlg.m_bScanning = TRUE;

    // Display a wait cursor before DoModal() since the folder browse
    // dlg can take a while to show up.  The cursor will be restored 
    // automagically when the dialog is shown.
    {
        CWaitCursor w;

        nModalRet = dlg.DoModal();
    }


    if ( IDOK == nModalRet )
    {
        list<CNVBrowseItem>::iterator itFolders;
        int        nItemIndex = -1;
        int        nEntries = dlg.m_browse.m_listSelectedItems.size();

        ASSERT ( nEntries > 0 );

        for ( itFolders = dlg.m_browse.m_listSelectedItems.begin();
              itFolders != dlg.m_browse.m_listSelectedItems.end();
              itFolders++ )
        {
            ASSERT ( 0 != itFolders->GetName() );

            SCANITEMTYPE type = typeFolder;
            SCANITEMSUBTYPE subtype;
    
            if ( itFolders->GetSubDirs() )
            {
                subtype = subtypeFolderAndSubs;
            }
            else
            {
                subtype = subtypeFolder;
            }

            pScanTask->AddItem( type, subtype, _TCHAR(' '), itFolders->GetName(), 0);
        }
        
        hrRet = S_OK;
    }

    pScanTask->Release();
    return hrRet;

}

HINSTANCE GetResInst()
{
	if((HANDLE) NULL != (HANDLE) g_ResInst)
		return g_ResInst;

	// Setup path to NavTskWz.loc
	ccLib::CString sPath;
	if(!ccSym::CInstalledApps::GetNAVDirectory(sPath))
		return FALSE;
	sPath += L"\\NavTskWz.loc";

	g_ResInst = LoadLibraryEx(sPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if((HMODULE)NULL != (HMODULE)g_ResInst)
	{
		return g_ResInst;
	}
	else
	{
		CCTRCTXE2(L"Failed to load %s. Error: %d", 
			sPath.GetString(), GetLastError());
	}

	return g_ResInst;
}