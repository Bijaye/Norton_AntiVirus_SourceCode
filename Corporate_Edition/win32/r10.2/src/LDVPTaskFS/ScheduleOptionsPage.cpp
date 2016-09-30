// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScheduleOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ScheduleOptionsPage.h"
#include "Wizard.h"

#include "Scaninf.h"
#include "scaninf_i.c"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScheduleOptionsPage property page

IMPLEMENT_DYNCREATE(CScheduleOptionsPage, CWizardPage)

CScheduleOptionsPage::CScheduleOptionsPage()
 : CWizardPage(CScheduleOptionsPage::IDD), 
	m_useResumableScan(FALSE), m_scheduleControlInitialized(FALSE),
	m_poCtlSchedule(NULL), m_poCtlScheduleResumable(NULL)
{
	HKEY		keyHandle		= NULL;
	DWORD		valueType		= REG_DWORD;
	DWORD		valueData		= 0;
	DWORD		valueDataSize	= 0;
	DWORD		returnValDW		= ERROR_FILE_NOT_FOUND;

	//{{AFX_DATA_INIT(CScheduleOptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

	m_bInitialized = FALSE;

	returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_ProductControl), NULL, KEY_READ, &keyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		valueDataSize = sizeof(valueData);
		returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_Resumption_Enable), NULL, &valueType, (LPBYTE) &valueData, &valueDataSize);
		if ((returnValDW == ERROR_SUCCESS) && (valueType == REG_DWORD))
			m_useResumableScan = (valueData != 0);
		RegCloseKey(keyHandle);
		keyHandle = NULL;
	}
	if (m_useResumableScan)
		m_poCtlScheduleResumable = new CLDVPSchedule2();
	else
		m_poCtlSchedule = new CLDVPSchedule();
}

CScheduleOptionsPage::~CScheduleOptionsPage()
{
	if (m_poCtlSchedule)
		delete m_poCtlSchedule;
	if (m_poCtlScheduleResumable)
		delete m_poCtlScheduleResumable;
}

void CScheduleOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleOptionsPage)
	if (m_scheduleControlInitialized)
	{
		if (m_useResumableScan)
			DDX_Control(pDX, IDC_SCHEDULE_CONTROL, *m_poCtlScheduleResumable);
		else
			DDX_Control(pDX, IDC_SCHEDULE_CONTROL, *m_poCtlSchedule);
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleOptionsPage, CWizardPage)
	//{{AFX_MSG_MAP(CScheduleOptionsPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleOptionsPage message handlers
BOOL CScheduleOptionsPage::OnInitDialog()
{
	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
    CRect rect;
    CWnd* placeholder = NULL;

	CWizardPage::OnInitDialog();

	ptrWizParent->OpenScan( FALSE, szReg_Key_Scan_Defaults );

	placeholder = GetDlgItem(IDC_SCHEDULE);
	if (placeholder != NULL)
	{
		placeholder->GetWindowRect(&rect);
		ScreenToClient(&rect);
		placeholder->DestroyWindow();
	}
    if (m_useResumableScan)
    {
		m_poCtlScheduleResumable->Create("schedule", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_EX_CONTROLPARENT, rect, this, IDC_SCHEDULE_CONTROL);
        m_poCtlScheduleResumable->Load( GetScanConfig() );
    }
    else
    {
		m_poCtlSchedule->Create("schedule", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_EX_CONTROLPARENT, rect, this, IDC_SCHEDULE_CONTROL);
        m_poCtlSchedule->Load( GetScanConfig() );
    }
	m_scheduleControlInitialized = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CScheduleOptionsPage::OnSetActive()
{
	CWizardPage::OnSetActive();
	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();

	if( m_bInitialized )
		return TRUE;

	if( InWizardMode() )
	{
		//ptrWizParent->SetFinishText( IDS_NEXT );
		ptrWizParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
		ptrWizParent->ShowBack();

	}

	return TRUE;
}


void CScheduleOptionsPage::OnOK()
{
	SaveOptions();

	CWizardPage::OnOK();
}


void CScheduleOptionsPage::SaveOptions()
{
    if (m_useResumableScan)
    	m_poCtlScheduleResumable->Store( GetScanConfig() );
    else
        m_poCtlSchedule->Store( GetScanConfig() );
}

void CScheduleOptionsPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if( InWizardMode() )
		PaintTitle( &dc );
}

void CScheduleOptionsPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWizardPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CScheduleOptionsPage::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWizardPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

LRESULT CScheduleOptionsPage::OnWizardNext()
{
	// Call OnWizardNext instead of OnWizardFinish so that the text
	// on the 'next' button will be resourced by the OS on int'l platforms.

	SaveOptions();

	return CPropertyPage::OnWizardNext();
}
