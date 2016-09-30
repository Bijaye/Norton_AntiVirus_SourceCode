// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanPCHealthPage.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ScanPCHealthPage.h"
#include "Wizard.h"

#include "ci.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Definitions
//
#define REG_KEY_TO_COPY		szReg_Key_Scan_Defaults

/////////////////////////////////////////////////////////////////////////////
// IScanPCHealthCallbackImp
STDMETHODIMP_(ULONG) IScanPCHealthCallbackImp::Progress(PPROGRESSBLOCK Progress)
{ 
	static HANDLE hEvent = NULL;

	if ( Progress->Status == S_STARTING )
	{
		if ( !hEvent )
		{
			hEvent = CreateEvent( NULL, FALSE, FALSE, DMI_SCANNING_EVENT );
		}
	}
	else if ( Progress->Status == S_DONE || Progress->Status == S_SUSPENDED )
	{
		if ( !hEvent || WaitForSingleObject( hEvent, 0 ) != WAIT_OBJECT_0 )
		{
			if ( !m_infectedCount )
			{
				IGenericConfig* pConfig;
				BOOL bDefault = TRUE;			

				// we need to initialize because we are on the scan thread now
				CoInitialize( NULL );

				if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IGenericConfig,
						(void**)&pConfig ) ) )
				{
					DWORD dwValue;
					pConfig->Open( NULL, HKEY_VP_MAIN_ROOT, REG_DMI_CUT_CURRENT_STATE_BASE, GC_MODEL_SINGLE );	

					// check that no other virus was found by realtime while we were scanning
					dwValue = 0;
					pConfig->SetOption( (LPSTR)(LPCTSTR)REG_DMI_INFECTED_VALUE, REG_DWORD, (BYTE*)&dwValue, sizeof( DWORD ) );
	
					pConfig->Release();
				}	

				CoUninitialize();
			}
		}
	}
	
	if ( ( Progress->Status == S_ABORTED ) 
			|| ( Progress->Status == S_DONE ) || ( Progress->Status == S_SUSPENDED )
			|| ( Progress->Status == S_DELAYED ) )
	{
		if ( hEvent )
		{
			while( WaitForSingleObject( hEvent, 0 ) == WAIT_OBJECT_0 )
				// clear any pending events
				;
			CloseHandle( hEvent );
			hEvent = NULL;
		}
	}

	return S_OK; 
}

STDMETHODIMP_(ULONG) IScanPCHealthCallbackImp::Virus(const char *line)   
{ m_infectedCount++; return S_OK; }

/////////////////////////////////////////////////////////////////////////////
// CScanPCHealthPage property page

IMPLEMENT_DYNCREATE(CScanPCHealthPage, CWizardPage)

CScanPCHealthPage::CScanPCHealthPage() : CWizardPage(CScanPCHealthPage::IDD)
{
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CScanPCHealthPage::~CScanPCHealthPage()
{
}

void CScanPCHealthPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanPCHealthPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CScanPCHealthPage::OnDestroying()
{
	return;
}

BEGIN_MESSAGE_MAP(CScanPCHealthPage, CWizardPage)
	//{{AFX_MSG_MAP(CScanPCHealthPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CScanPCHealthPage::OnSetActive()
{	
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();

	//Set up the Context-sensitive help
	SetHelpID( IDD_SCAN_PCHEALTH );

	ptrWizParent->HideBack();
	ptrWizParent->SetFinishText( IDS_SCAN );

	ptrWizParent->m_strTitle.LoadString( IDS_SCAN_PCHEALTH );

	return CWizardPage::OnSetActive();
}

BOOL CScanPCHealthPage::OnWizardFinish()
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();
	CConfigObj			config( GetScanConfig() );

	// drop the scan configuration
//	config.SetOption( szReg_Val_ScanAllDrives, (DWORD)1 );

	// start the scan
	VERIFY( CopyRealtimeScan() );

	((CWizard*)ptrParent)->Scan( &m_callback );

	return TRUE;
}

BOOL CScanPCHealthPage::OnKillActive()
{
	return TRUE;
}

BOOL CScanPCHealthPage::CopyRealtimeScan()
{
	BOOL rc = FALSE;

	// we need to copy the HKEY_LOCAL_MACHINE realtime scan key over

	rc = TRUE;

	return rc;
}

/////////////////////////////////////////////////////////////////////////////
// CScanPCHealthPage message handlers
BOOL CScanPCHealthPage::OnInitDialog()
{
	if ( !CWizardPage::OnInitDialog() )
		return FALSE;

	CClientPropSheet *ptrParent = (CClientPropSheet*)GetParent();

	//I need to open the scan before I try to add the scan item
	ptrParent->OpenScan( TRUE, REG_KEY_TO_COPY );

	return TRUE;
}
