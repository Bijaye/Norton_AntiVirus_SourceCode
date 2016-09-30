// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// IdleOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "IdleOptionsPage.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include "ExcludePatternsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIdleOptionsPage property page

IMPLEMENT_DYNCREATE(CIdleOptionsPage, CWizardPage)

CIdleOptionsPage::CIdleOptionsPage() : CWizardPage(CIdleOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CIdleOptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	m_bInitialized = FALSE;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CIdleOptionsPage::~CIdleOptionsPage()
{
}

void CIdleOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleOptionsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CIdleOptionsPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSE_FILE_TYPES, OnBrowseFileTypes)
	ON_BN_CLICKED(IDC_MESSAGE, OnMessage)
	ON_BN_CLICKED(IDC_BROWSE_SELECTED_DRIVES, OnBrowseSelectedDrives)
	ON_BN_CLICKED(IDC_BROWSE_EXCLUDE_FILES, OnBrowseExcludeFiles)
	ON_BN_CLICKED(IDC_ONOFF, OnOnoff)
	ON_BN_CLICKED(IDC_TYPES_ALL, OnTypesAll)
	ON_BN_CLICKED(IDC_TYPES_SELECTED, OnTypesSelected)
	ON_BN_CLICKED(IDC_SELECTED_DRIVES, OnSelectedDrives)
	ON_BN_CLICKED(IDC_EXCLUDE_FILES, OnExcludeFiles)
	ON_BN_CLICKED(IDC_MESSAGE_BOX, OnMessageBox)
	ON_BN_CLICKED(IDC_ALL_DRIVES, OnAllDrives)
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYING, OnDestroying )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleOptionsPage message handlers

BOOL CIdleOptionsPage::OnSetActive() 
{
	CWizardPage::OnSetActive();

	CClientPropSheet	*ptrParent = (CClientPropSheet*)GetParent();
	
	if( m_bInitialized )
	{
		return TRUE;
	}

	//Set the Context-sensitive ID
	SetHelpID( IDD_IDLE_OPTIONS );

	
	//Load the options
	if( ptrParent->IsKindOf( RUNTIME_CLASS( CWizard ) ) )
	{
		ptrParent->HideBack();
		ptrParent->m_strTitle.LoadString( IDS_IDLE_SCAN );
		ptrParent->SetFinishText( IDS_CLOSE );
		ptrParent->SetWizardButtons( PSWIZB_FINISH );
	}

	//Now open the scan options
	ptrParent->OpenScan( FALSE );
	//And update the interface
	SetDialogOptions();
	UpdateInterface();

	m_bInitialized = TRUE;
	
	return TRUE;
}

void CIdleOptionsPage::OnDestroying()
{
	GetDialogOptions();
}

LRESULT CIdleOptionsPage::OnWizardNext() 
{
	GetDialogOptions();
		
	//I return -1 so the page doesn't change. This is because
	//	there is no page to change to!
	//	The ONLY way I can get the WizardNext message is from 
	//	the main app terminating the task, in which case
	//	I simply want to save out my options. The task will
	//	be terminated immediatly after this.
	return -1;
}

BOOL CIdleOptionsPage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

void CIdleOptionsPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	PaintTitle( &dc );
}

void CIdleOptionsPage::OnBrowseFileTypes() 
{
	GetExtensions();
}

void CIdleOptionsPage::SetDialogOptions()
{
	CButton		*ptrScanAllDrives = (CButton*)GetDlgItem( IDC_ALL_DRIVES );
	CButton		*ptrScanSelDrives = (CButton*)GetDlgItem( IDC_SELECTED_DRIVES );
	CButton		*ptrOnOff = (CButton*)GetDlgItem( IDC_ONOFF );
	CString		strTemp;
	BOOL		bDefault = TRUE, 
				*pBSet = NULL;
	CConfigObj	config( GetScanConfig() );

	//Set the proper items in the interface
	//First, call the base class method to update the
	//	interface for the common items
	CWizardPage::SetDialogOptions( config );

	//Now add my own stuff
	ptrScanAllDrives->SetCheck( config.GetOption( szReg_Val_ScanAllDrives, TRUE  ) );
	ptrScanSelDrives->SetCheck( !config.GetOption( szReg_Val_ScanAllDrives, TRUE  ) );

	ptrOnOff->SetCheck( config.GetOption( szReg_Val_RTSScanOnOff, TRUE ) );

	OnOnoff();
}

void CIdleOptionsPage::GetDialogOptions()
{
	CButton		*ptrScanAllDrives = (CButton*)GetDlgItem( IDC_ALL_DRIVES );
	CButton		*ptrOnOff = (CButton*)GetDlgItem( IDC_ONOFF );
	CConfigObj	config( GetScanConfig() );

	//Set the proper items in the interface
	//First, call the base class method to update the
	//	interface for the common items
	CWizardPage::GetDialogOptions( config );

	//Now add my own stuff
	config.SetOption( szReg_Val_ScanAllDrives, ptrScanAllDrives->GetCheck() );
	config.SetOption( szReg_Val_RTSScanOnOff, ptrOnOff->GetCheck() );
}


void CIdleOptionsPage::OnMessage() 
{
	CButton		*ptrButton = (CButton*)GetDlgItem( IDC_MESSAGE_BOX );
	CString		strTemp;
	CConfigObj	config( GetScanConfig() );

	GetMessage();

	config.GetOption( szReg_Val_Message, strTemp );

	ptrButton->SetCheck( !strTemp.IsEmpty() );
}

void CIdleOptionsPage::OnBrowseSelectedDrives() 
{
/*	CObList			tempList;
	CShellObject	*pData;
	CString			strItem;
	BOOL			bRecursive;
	DWORD			dwIndex;

	dlg.m_ctrlFlags = SHELLTREE_FLAG_DRIVES_ONLY | SHELLTREE_FLAG_SEL_MULTI;

	//First, get the items out of the OCX

	//Files first
	dwIndex = 0;
	do{
		strItem = GetScanObj().EnumScanFiles( dwIndex++ );

		if( !strItem.IsEmpty() )
		{
			if( pData = new CShellObject )
			{
				pData->m_name = strItem;
				pData->m_type = SHELLDATA_FILE;
				pData->m_children = FALSE;

				tempList.AddTail( pData );
			}
		}

	}while( !strItem.IsEmpty() );

	//Now get the Directories
	dwIndex = 0;
	do{
		strItem = GetScanObj().EnumScanDirectories( dwIndex++, &bRecursive );

		if( !strItem.IsEmpty() )
		{
			if( pData = new CShellObject )
			{
				pData->m_name = strItem;
				pData->m_type = SHELLDATA_DIRECTORY;
				pData->m_children = bRecursive;

				tempList.AddTail( pData );
			}
		}

	}while( !strItem.IsEmpty() );

	//Now set the list into the ShellSel control
	dlg.SetObjectList( &tempList );

	if( IDOK == dlg.DoModal() )
	{
		POSITION		pos;

		//Remove all of the pointers in the list.
		GetScanObj().RemoveAllScanFiles();
		GetScanObj().RemoveAllScanDirectories();

		//Now, fill my temp list with the items that were
		//	selected.
		dlg.GetObjectList( &tempList );

		pos = tempList.GetHeadPosition();
		while( pos )
		{
			pData = (CShellObject*)tempList.GetNext( pos );
			
			if( pData->m_type == SHELLDATA_FILE )
				GetScanObj().AddScanFile( pData->m_name );
			else
				GetScanObj().AddScanDirectory( pData->m_name, pData->m_children );

			delete pData;
		}
	}
	else
	{
		dlg.FreeObjectList();
	}
*/}

void CIdleOptionsPage::OnBrowseExcludeFiles() 
{
	GetExcludeFiles();
}


void CIdleOptionsPage::OnOnoff() 
{
	CButton		*ptrIdle = (CButton*)GetDlgItem( IDC_ONOFF );
	CWnd		*ptrWnd;
	BOOL		bEnabled = FALSE;
	int			iLoop,
				iCount = 20;
	CConfigObj	config( GetScanConfig() );
	static int	iControls[] ={	IDC_TYPES_ALL,
								IDC_TYPES_SELECTED,
								IDC_BROWSE_FILE_TYPES,
								IDC_COMPRESSED_STATIC,
								IDC_MINUTE_SPIN,
								IDC_COMPRESSED,
								IDC_ACTIONS_FOR,
								IDC_ACTION,
								IDC_IF_ACTION_FAILS,
								IDC_ALL_DRIVES,
								IDC_SELECTED_DRIVES,
								IDC_BROWSE_SELECTED_DRIVES,
								IDC_EXCLUDE_FILES,
								IDC_EXCLUDE_SIGS,
								IDC_BROWSE_EXCLUDE_FILES,
								IDC_BROWSE_EXCLUDE_SIGS,
								IDC_MESSAGE_BOX,
								IDC_CHECKSUM,
								IDC_MESSAGE,
								IDC_MOVE_FOLDER,
								IDC_BROWSE_MOVE_FOLDER };

	//If the Idle Options are disabled,
	//	then disable all controls on the dialog
	//	and set the option to OFF.
	if( ptrIdle )
	{
		config.SetOption( szReg_Val_RTSScanOnOff, (BOOL)ptrIdle->GetCheck() );
	}

	for( iLoop = 0; iLoop < iCount; iLoop ++ )
	{
		ptrWnd = GetDlgItem( iControls[iLoop] );
		if( ptrWnd && ::IsWindow( ptrWnd->m_hWnd ) )
		{
			ptrWnd->EnableWindow( bEnabled );
		}
	}

	//Now look for a couple of special cases
	CComboBox *ptrCombo = (CComboBox*)GetDlgItem( IDC_ACTION );

	if( ptrCombo )
	{
		//If the promary action is "Leave Alone", then
		//	we must grey out the second combo box
		if( ptrCombo->GetCurSel() == 3 )
		{
			if( ptrWnd = GetDlgItem( IDC_IF_ACTION_FAILS ) )
			{
				ptrWnd->EnableWindow( FALSE );
			}
		}
	}

	UpdateInterface();
}

void CIdleOptionsPage::OnOK() 
{
	//When modal, we get an OnOK. This is where
	//	we need to save out the options.
	GetDialogOptions();
	
	CPropertyPage::OnOK();
}

void CIdleOptionsPage::OnTypesAll() 
{
	UpdateInterface();
}

void CIdleOptionsPage::OnTypesSelected() 
{
	UpdateInterface();
}

void CIdleOptionsPage::OnSelectedDrives() 
{
	UpdateInterface();
}

void CIdleOptionsPage::OnExcludeFiles() 
{
	UpdateInterface();
}

void CIdleOptionsPage::OnMessageBox() 
{
	UpdateInterface();
}

void CIdleOptionsPage::OnAllDrives() 
{
	UpdateInterface();	
}
