// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: SaveTask.cpp
//  Purpose: SaveTask Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "SaveTask.h"
#include "Wizard.h"
#include "acta.h"
#include "ls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveTask property page

IMPLEMENT_DYNCREATE(CSaveTask, CPropertyPage)

CSaveTask::CSaveTask() : CWizardPage(CSaveTask::IDD)
{
	//{{AFX_DATA_INIT(CSaveTask)
	m_strname = _T("");
	m_strDescription = _T("");
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CSaveTask::~CSaveTask()
{
}

void CSaveTask::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveTask)
	DDX_Text(pDX, IDC_NAME, m_strname);
	DDV_MaxChars(pDX, m_strname, 30);
	DDX_Text(pDX, IDC_TASK_DESCRIPTION, m_strDescription);
	DDV_MaxChars(pDX, m_strDescription, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveTask, CPropertyPage)
	//{{AFX_MSG_MAP(CSaveTask)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
void CSaveTask::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( InWizardMode() )
		PaintTitle( &dc );
	
}

//----------------------------------------------------------------
// OnSetActive
//----------------------------------------------------------------
BOOL CSaveTask::OnSetActive() 
{
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();

	CWizardPage::OnSetActive();
 
	//HACK ALERT
	// For some reason when pressing the Back button to get to this dialog,
	//	my windows hook has been removed from this dialog ( it should have been
	//	set in the ClientPropSheet baser class ). If I Unhook then re-hook it here, 
	//	it works.
	UnHookWindow();
	HookWindow();

	if( InWizardMode() )
	{
		//ptrWizParent->HideBack();
		//ptrWizParent->SetFinishText( IDS_SAVE );
		//ptrWizParent->SetWizardButtons( PSWIZB_FINISH );
		//ptrWizParent->SetWizardButtons( PSWIZB_NEXT );
		ptrWizParent->SetWizardButtons( PSWIZB_NEXT | PSWIZB_BACK );
		ptrWizParent->ShowBack();
		//We want to show the back button now. To show it,
		//we will have to remove the last page(selected scan type page) from the property sheet.
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CSaveTask::OnInitDialog() 
{
	CEdit				*ptrWnd;
	CClientPropSheet	*ptrWizParent = (CClientPropSheet*)GetParent();
	CLDVPTaskFSCtrl		*ptrCtrl = ((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl;
	CString				strScanName,
						strDescription,
						strStaticText;

	CPropertyPage::OnInitDialog();

	if( ptrWizParent )
		ptrWizParent->OpenScan( FALSE, szReg_Key_Scan_Defaults );

	//Set the values
//	ptrWizParent->m_strTitle = ptrCtrl->GetTaskDescriptiveName();
//	ptrWizParent->m_strDescription = ptrCtrl->GetTaskDescription();

	if( InWizardMode() )
	{
        // If we're in wizard mode recording a new task, set close mode to autodelete
        // so we won't leave inactive scheduled scans in the registry on cancel.
        //
        // This is reset in CWizard::Scan() -- CWizard::Scan() is called by the
        // final wizard page of a task, not just to perform scans, but apparantly to save
        // recorded task data as well.

        if ( ptrWizParent && ptrWizParent->m_ptrCtrl->Recording() )
        {
            // Note: GetScanConfig() does not AddRef on the interface returned, so no Release.
    	    IConfig		*pConfig = GetScanConfig();

	        if( pConfig )
	        {
                pConfig->ChangeCloseMode( TRUE );
	        }
        }

		strScanName.LoadString( IDS_TASK_NAME_DEFAULT );
		strDescription = LS( IDS_TASK_DESCRIPTION_DEFAULT );

		strStaticText.LoadString( IDS_TASK_NAME_WIZARDMODE );
	
		CStatic* pStatic = (CStatic*)GetDlgItem(IDC_NAME_STATIC);
		ASSERT( pStatic );
		pStatic->SetWindowText( strStaticText );
	}
	else
	{
		strScanName = ptrCtrl->GetTaskDescriptiveName();
		strDescription = ptrCtrl->GetTaskDescription();

		strStaticText.LoadString( IDS_TASK_NAME_NONWIZARDMODE );
		
		CStatic* pStatic = (CStatic*)GetDlgItem( IDC_NAME_STATIC );
		ASSERT( pStatic );
		pStatic->SetWindowText( strStaticText );

	}

	ptrWizParent->m_strTitle = strScanName;

	//And set the default text in the fields
	if( ptrWnd = (CEdit*)GetDlgItem( IDC_TASK_DESCRIPTION ))
	{
		ptrWnd->SetWindowText( strDescription );
	}
	
	if( ptrWnd = (CEdit*)GetDlgItem( IDC_NAME ))
	{
		ptrWnd->SetWindowText( strScanName );

		//Simulate a click on the field to activate it.
		if( InWizardMode() )
			ptrWnd->PostMessage( WM_ACTIVATE, MAKEWPARAM( WA_CLICKACTIVE, 0 ), 0L );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//----------------------------------------------------------------
// OnWizardNext
//----------------------------------------------------------------
LRESULT CSaveTask::OnWizardNext() 
{
	//It is OK to use CWizard here, because we can only get this
	//	message if we are in Wizard mode.
	CWizard *ptrWizParent = (CWizard*)GetParent();

	ptrWizParent->SetRecordedData( (CEdit*)GetDlgItem( IDC_NAME ), (CEdit*)GetDlgItem( IDC_TASK_DESCRIPTION ) );

	//Fire my event telling the container that we are recording a task
//	ptrWizParent->m_ptrCtrl->BeginRecordingSession();
	
	ptrWizParent->RemoveSelectedScanTypePage();
	ptrWizParent->AddSelectedScanTypePage();

	return CPropertyPage::OnWizardNext();
}


//----------------------------------------------------------------
// SizePage
//----------------------------------------------------------------
void CSaveTask::SizePage( int cx, int cy )
{
	CWnd	*ptrWnd;
	CRect	dlgRect,
			buttonRect;

	//Start by calling the base class version
	CWizardPage::SizePage( cx, cy );

	//Now size my controls
	//Figure out the current client rect
	GetClientRect( &dlgRect );

	if( ptrWnd = GetDlgItem( IDC_NAME ) )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( buttonRect.left, buttonRect.top, dlgRect.right - buttonRect.left - 12, buttonRect.bottom - buttonRect.top );
	}

	if( ptrWnd = GetDlgItem( IDC_TASK_DESCRIPTION ) )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( buttonRect.left, buttonRect.top, dlgRect.right - buttonRect.left - 12, dlgRect.bottom - buttonRect.top );
	}
}


void CSaveTask::OnOK() 
{
	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();

	//Now, tell my parent to inform the container of the new names
	//	& description
	ptrParent->RenameTask( (CEdit*)GetDlgItem( IDC_NAME ), (CEdit*)GetDlgItem( IDC_TASK_DESCRIPTION ) );

	CPropertyPage::OnOK();
}


void CSaveTask::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CSaveTask::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/

