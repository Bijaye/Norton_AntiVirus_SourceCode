// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LSLicensingView.cpp : implementation file
//

#include "stdafx.h"
#include <atlbase.h>
#include "ldvpview.h"
#include "Wizard.h"
#include "LSLicensingView.h"

#include "oem_obj.h"
#include <atlbase.h>
#include "slic.h"
#include "SLICWrap.h"
#include "SLICLicenseData.h"
#include <mbstring.h>
#include ".\lslicensingview.h"
WINOLEAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE SavOpenMutex(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR szMutexName)
{
	HANDLE hMutex = NULL;
	OSVERSIONINFO osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	CString csMutexName;
 
	if ((!szMutexName) || (_tcscmp(szMutexName, "") == 0))
		return NULL;

 	if (GetVersionEx(&osInfo))
 	{
 		if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
 		{
 			if (osInfo.dwMajorVersion <= 4)
 			{
 				// WinNT4: use names with no backslash character
 				csMutexName = szMutexName;
 			}
 			else
 			{
 				// Win2K, WinXP: use names with "Global\" prefix
 				csMutexName = GLOBAL;
 				csMutexName += szMutexName;
 			}
 			hMutex = OpenMutex(dwAccess, bInheritHandle, csMutexName);
 		}
 		else if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
 		{
 			// discard dwAccess flag. MUTEX_ALL_ACCESS is the only flag acceptable in Win9x
 			hMutex = OpenMutex(MUTEX_ALL_ACCESS, bInheritHandle, szMutexName);
 		}
 	}
   
 	return hMutex;
}


/////////////////////////////////////////////////////////////////////////////
// CLSLicensingView property page

IMPLEMENT_DYNCREATE(CLSLicensingView, CPropertyPage)

CLSLicensingView::CLSLicensingView() : CWizardPage(CLSLicensingView::IDD)
{
	//{{AFX_DATA_INIT(CLSLicensingView)
	m_InstructionLink = _T("");
	m_InstructionText = _T("");
	m_LicExpiryDate = _T("");
	m_LicStartDate = _T("");
	m_LicStatus = _T("");
	m_LicVeniceId = _T("");
	//}}AFX_DATA_INIT
}

CLSLicensingView::~CLSLicensingView()
{
}

void CLSLicensingView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLSLicensingView)
	DDX_Control(pDX, IDC_INSTRUCTIONS_LINK, m_InstructionLinkCtl);
	DDX_Text(pDX, IDC_INSTRUCTIONS_LINK, m_InstructionLink);
	DDX_Text(pDX, IDC_INSTRUCTIONS_TEXT, m_InstructionText);
	DDX_Text(pDX, IDC_LIC_EXPIRY_DATE, m_LicExpiryDate);
	DDX_Text(pDX, IDC_LIC_START_DATE, m_LicStartDate);
	DDX_Text(pDX, IDC_LIC_STATUS, m_LicStatus);
	DDX_Text(pDX, IDC_LIC_VENICE_ID, m_LicVeniceId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLSLicensingView, CPropertyPage)
	//{{AFX_MSG_MAP(CLSLicensingView)
	ON_BN_CLICKED(IDC_INSTALL_LICENSE, OnInstallLicense)
	ON_BN_CLICKED(IDC_SYNC, OnSync)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLSLicensingView message handlers

BOOL CLSLicensingView::OnInitDialog() 
{
	CWizardPage::OnInitDialog();
	
   // Load the instructions and the link.
	TCHAR szUrl[1024] = "";
	TCHAR szAdminText[1024] = "";

	if (OEMObj.IsOEMBuild())
	{
		if (OEMObj.IsOEM_NoUrl())
		{
			// hide and disable link control
			m_InstructionLink.Empty();
			m_InstructionLinkCtl.EnableWindow(FALSE);
			m_InstructionLinkCtl.ShowWindow(SW_HIDE);
		}
		else
		{
			// display and enable link control
			m_InstructionLink.LoadString(IDS_COBRANDED_WEBSITE);
			if (OEMObj.GetOEM_Url(szUrl, 1024))
				m_InstructionLinkCtl.SetLink(CString(szUrl));
			m_InstructionLinkCtl.EnableWindow(TRUE);
			m_InstructionLinkCtl.ShowWindow(SW_SHOW);
		}
		if (OEMObj.GetOEM_AdminText(szAdminText, 1024))
			m_InstructionText = szAdminText;
		else
			m_InstructionText.LoadString( IDS_LS_LICENSE_ADMINTEXT );			
	}

	//if (OEMObj.IsOEMBuild() && OEMObj.GetOEM_Url(szUrl, 1024) == TRUE)
	//{
	//	m_InstructionLink.LoadString(IDS_COBRANDED_WEBSITE);
	//	m_InstructionLinkCtl.SetLink(CString(szUrl));
	//}



    // Initial display of license data.

	UpdateLicenseData();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLSLicensingView::OnInstallLicense() 
{
	// First, get an instance of the COM object that wraps SLIC.
	HRESULT hr = S_OK;

	// If we can't get an instance, we can assume that this is an unlicensed version.

	CComPtr<ISLICLicenseRepositorySCS> slic;

	if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
	{
		// Show the common license install wizard dialogs.

        SLIC_STATUS slic_status = SLICSTATUS_OK;

		hr = slic->InstallLicenseSCS( ((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->GetSafeHwnd(),
                                      &slic_status );

		if( SUCCEEDED( hr ) )
		{
			// If license install succeeded, update the display.

			if( SLIC_SUCCEEDED( slic_status ) )
			{
				UpdateLicenseData();
				UpdateData( FALSE );
				// change to the ELS license screen
				CWnd* pWnd = GetParent();
				if (pWnd)
				{
					::SendMessage(pWnd->GetSafeHwnd(), WM_LICENSE_MODE_CHANGE, 0, 0);
				}
			}
		}
	}
}

BOOL CLSLicensingView::OnWizardFinish() 
{
	// Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

BOOL CLSLicensingView::OnSetActive() 
{
	CWizardPage::OnSetActive();

	CWizard	*ptrParent = (CWizard*)GetParent();

	// Set the Context-sensitive ID
	SetHelpID( IDD_LICENSING );

	// Parse the raw resouce string to draw out the the menu and taskpad headings.
	CString sMenu, sTaskpad, sRaw;
	sRaw.LoadString( IDS_TASK_LICENSING  );
	ConvertRawMenuString( sRaw, sMenu, sTaskpad );
	
	// Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle = sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}

typedef struct LSLicenseThreadData_Struct
{
	HRESULT hr;
	CString	sLicExpiryDate;
	CString	sLicStartDate;
	CString	sLicStatus;
#if 0 // GLEE: Comment out venice ID
	CString	sLicVeniceId;
#endif
} LSLicenseThreadData;

UINT UpdateLicenseDataThread( LPARAM pResult)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	// If a sync with server is in progress, wait till it's finished
	if (OEMObj.IsOEMBuild())
	{
		HANDLE hMutex = SavOpenMutex(MUTEX_ALL_ACCESS, FALSE, SYNC_WITH_SERVER_MUTEX);
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			WaitForSingleObject(hMutex, INFINITE);
		}

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}

	LSLicenseThreadData *pLSLicenseThreadData = (LSLicenseThreadData *) pResult;

	if (pLSLicenseThreadData)
	{
		pLSLicenseThreadData->hr = S_OK;
		// If we can't get an instance, we can assume that this is an unlicensed version.
		CComPtr<ISLICLicenseRepositorySCS> slic;

		if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
		{
			
			CComPtr<ISLICLicense> lic;

			CComBSTR license_status_dscr;
			SLIC_STATUS slic_status = SLICSTATUS_OK;

			pLSLicenseThreadData->hr = slic->CheckLicenseSCS( &slic_status, &license_status_dscr, &lic.p );

			if( SUCCEEDED( pLSLicenseThreadData->hr ) )
			{
				// The call might succeed, but we might not get a license.
				SSLICLicenseData lic_data;

				if( lic != NULL )
				{
					// We have a license.
					pLSLicenseThreadData->hr = lic->GetLicenseData( &lic_data);

					// Convert license dates to localized strings.

					CComBSTR temp, bstrVeniceId;

					lic->GetDateInLocalFormat( lic_data.expiredate, &temp );
					pLSLicenseThreadData->sLicExpiryDate = temp;

					lic->GetDateInLocalFormat( lic_data.startdate, &temp );
					pLSLicenseThreadData->sLicStartDate = temp;

					// m_LicSerialNumber = lic_data.serial_id;
#if 0 // GLEE: comment out Venice ID 
					pLSLicenseThreadData->hr = lic->GetVeniceId(&bstrVeniceId);
					if (SUCCEEDED(pLSLicenseThreadData->hr))
					{
						pLSLicenseThreadData->sLicVeniceId = bstrVeniceId;
					}
					if (pLSLicenseThreadData->sLicVeniceId.IsEmpty())
					{
						pLSLicenseThreadData->sLicVeniceId.LoadString(IDS_NO_VENICEID);	
					}
#endif
					
				}

				pLSLicenseThreadData->sLicStatus = license_status_dscr;
			}
		}
	} // if pLSLicenseThreadData

	CoUninitialize();


	return 0;
}


CWinThread* g_pSlicThread;
LSLicenseThreadData g_stLicenseThreadData;
SSLICLicenseData g_lic_data;

void CLSLicensingView::UpdateLicenseData()
{
	// If we can't get an instance, we can assume that this is an unlicensed version.
	// Put on a different thread to prevent errors in MFC initialization when many double-clicks
	// are done.

	if (g_pSlicThread)
	{
		// Wait until previous thread dies
		if (g_pSlicThread && g_pSlicThread->m_hThread)
		{
			WaitForSingleObject(g_pSlicThread->m_hThread, INFINITE); // have to wait for the thread to finish to get the result
			g_pSlicThread = NULL;
		}	
	}
	
	g_pSlicThread = AfxBeginThread( (AFX_THREADPROC)UpdateLicenseDataThread, (void*)&g_stLicenseThreadData);

	if (g_pSlicThread && g_pSlicThread->m_hThread)
	{
		WaitForSingleObject(g_pSlicThread->m_hThread, INFINITE); // have to wait for the thread to finish to get the result
		g_pSlicThread = NULL;

		if (SUCCEEDED(g_stLicenseThreadData.hr))
		{
			// Convert license dates to localized strings.
			m_LicExpiryDate = g_stLicenseThreadData.sLicExpiryDate;

			m_LicStartDate = g_stLicenseThreadData.sLicStartDate;

#if 0 // GLEE: Comment out venice id
			m_LicVeniceId = g_stLicenseThreadData.sLicVeniceId;
#endif			
			m_LicStatus = g_stLicenseThreadData.sLicStatus;
		}
	}
}

void CLSLicensingView::OnSync() 
{
	CLSLicensingViewDlg dlg(this);
	dlg.SetTheParentDlg(this);
	dlg.DoModal();	
}


/////////////////////////////////////////////////////////////////////////////
// CLSLicensingViewDlg dialog

CLSLicensingViewDlg::CLSLicensingViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLSLicensingViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLSLicensingViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLSLicensingViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLSLicensingViewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLSLicensingViewDlg, CDialog)
	//{{AFX_MSG_MAP(CLSLicensingViewDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLSLicensingViewDlg message handlers

BOOL CLSLicensingViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ShowWindow(SW_HIDE);
	
	// TODO: Add extra initialization here
	HRESULT hr = S_OK;
	CComPtr<IOEMLicenseManager> spLicenseMgr;
	SLIC_STATUS slic_status = SLICSTATUS_OK;
	if (SUCCEEDED(spLicenseMgr.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER) ) )
	{
		hr = spLicenseMgr->SynchronizeSubscription();
		if( SUCCEEDED( hr ) )
		{
			if (m_pDlgLSLicensingView)
			{
				// Update the display.
				m_pDlgLSLicensingView->UpdateLicenseData();
				m_pDlgLSLicensingView->UpdateData( FALSE );	
			}
		}
	}	

	PostMessage(WM_CLOSE);
	return FALSE;

} 

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLSLicensingViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLSLicensingViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




