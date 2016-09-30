// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PasswordManageWizardPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScsPassMan.h"
#include "ScsPassManDlg.h"
#include "PasswordManageWizardPage.h"
#include ".\passwordmanagewizardpage.h"
//#include "securecommsmisc.h"


// CPasswordManageWizardPage dialog

//REMOVED_FOR_VC6/IMPLEMENT_DYNAMIC(CPasswordManageWizardPage, CNewWizPage)
CPasswordManageWizardPage::CPasswordManageWizardPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CPasswordManageWizardPage::IDD, pParent)
	, m_failureOccurred(FALSE)
	, m_oldPassword(_T(""))
	, m_newPassword(_T(""))
	, m_confirmPassword(_T(""))
	, m_account(_T(""))
{
}

CPasswordManageWizardPage::~CPasswordManageWizardPage()
{
}

void CPasswordManageWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_MANAGE_ACCOUNT_COMBOBOX, m_accountComboBox);
	DDX_Text(pDX, IDC_MANAGE_OLDPASS_EDITBOX, m_oldPassword);
	DDV_MaxChars(pDX, m_oldPassword, 20);
	DDX_Text(pDX, IDC_MANAGE_NEWPASS_TEXTBOX, m_newPassword);
	DDV_MaxChars(pDX, m_newPassword, 20);
	DDX_Text(pDX, IDC_MANAGE_CONFIRMPASS_TEXTBOX, m_confirmPassword);
	DDX_Control(pDX, IDC_MANAGE_CHANGE_BUTTON, m_passwordChangeButton);
	DDX_Control(pDX, IDC_MANAGE_STATUS_TEXT, m_manageStatusText);
//	DDX_CBString(pDX, IDC_MANAGE_ACCOUNT_COMBOBOX, m_account);
	DDX_Text(pDX, IDC_MANAGE_ACCOUNT_EDITBOX, m_account);
	DDV_MaxChars(pDX, m_account, 32);
}


BEGIN_MESSAGE_MAP(CPasswordManageWizardPage, CNewWizPage)
	ON_WM_CTLCOLOR()
//	ON_CBN_EDITCHANGE(IDC_MANAGE_ACCOUNT_COMBOBOX, OnCbnEditchangeManageAccountCombobox)
	ON_EN_CHANGE(IDC_MANAGE_OLDPASS_EDITBOX, OnEnChangeManageOldpassEditbox)
	ON_EN_CHANGE(IDC_MANAGE_NEWPASS_TEXTBOX, OnEnChangeManageNewpassTextbox)
	ON_EN_CHANGE(IDC_MANAGE_ACCOUNT_EDITBOX, OnEnChangeManageAccountEditbox)
	ON_EN_CHANGE(IDC_MANAGE_CONFIRMPASS_TEXTBOX, OnEnChangeManageConfirmpassTextbox)
	ON_BN_CLICKED(IDC_MANAGE_CHANGE_BUTTON, OnBnClickedManageChangeButton)
	ON_WM_ENABLE()
END_MESSAGE_MAP()


// CPasswordManageWizardPage message handlers

HBRUSH CPasswordManageWizardPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNewWizPage::OnCtlColor(pDC, pWnd, nCtlColor);

	//If we're painting the connection status text...
	if ( pWnd->GetDlgCtrlID() == IDC_MANAGE_STATUS_TEXT )
	{
		//If a failure ocurred, make the text red and reset the failure flag
		//If no failure occurred, make sure the text is black
		if ( ! m_failureOccurred )
		{
			pDC->SetTextColor( RGB( 0, 0, 0 ) );
		}
		else
		{
			pDC->SetTextColor( RGB( 175, 0, 0 ) );
			m_failureOccurred = FALSE;
		}
	}

	return hbr;
}

/*void CPasswordManageWizardPage::OnCbnEditchangeManageAccountCombobox()
{
	checkForEnableOrDisable();
}*/

void CPasswordManageWizardPage::OnEnChangeManageAccountEditbox()
{
	checkForEnableOrDisable();
}

void CPasswordManageWizardPage::OnEnChangeManageOldpassEditbox()
{
	checkForEnableOrDisable();
}

void CPasswordManageWizardPage::OnEnChangeManageNewpassTextbox()
{
	checkForEnableOrDisable();
}

void CPasswordManageWizardPage::OnEnChangeManageConfirmpassTextbox()
{
	checkForEnableOrDisable();
}

void CPasswordManageWizardPage::checkForEnableOrDisable()
{
    CWnd* changePasswordButton = GetDlgItem( IDC_MANAGE_CHANGE_BUTTON );

	UpdateData();

	//If any of the fields are empty, then set the Change Password button inactive
	//Otherwise, set it to active ( in case it was previously deactivated )
	if ( m_account.IsEmpty()  ||
		 m_oldPassword.IsEmpty() ||
		 m_newPassword.IsEmpty() ||
		 m_confirmPassword.IsEmpty() )
	{
		changePasswordButton->EnableWindow( FALSE );
	}
	else
	{
		changePasswordButton->EnableWindow();
	}
}

void CPasswordManageWizardPage::OnSetActive()
{
	//Get the servergroup from our parent dialog.  It was put there by CConnectWizardPage.
	//This is a somewhat fragile way of doing this, if these wizard pages don't come in this order this won't work.
	m_serverGroup = static_cast<CScsPassManDlg*>( GetParent() )->m_serverGroup;
	m_manageStatusText.SetWindowText( "Managing \"" + m_serverGroup + "\"..." );

	UpdateData();

	//If any of the fields are empty, then set the Change Password button inactive
	if ( m_account.IsEmpty()  ||
		 m_oldPassword.IsEmpty() ||
		 m_newPassword.IsEmpty() ||
		 m_confirmPassword.IsEmpty() )
	{
		CWnd* changePasswordButton = GetDlgItem( IDC_MANAGE_CHANGE_BUTTON );
		changePasswordButton->EnableWindow( FALSE );
	}
}

void CPasswordManageWizardPage::OnBnClickedManageChangeButton()
{
	UpdateData();

	if ( m_oldPassword == m_newPassword )
	{
		m_failureOccurred = TRUE;
		m_manageStatusText.SetWindowText( "The old password and new password cannot be the same." );
		return;
	}

	if ( m_newPassword != m_confirmPassword )
	{
		m_failureOccurred = TRUE;
		m_manageStatusText.SetWindowText( "The new password and the confirmation don't match." );
		return;
	}

	if ( changePassword() )
	{
		m_manageStatusText.SetWindowText( "Password changed successfully." );
	}
	else
	{
		m_failureOccurred = TRUE;
		m_manageStatusText.SetWindowText( "An error occurred while attempting to change the password." );
	}
}

BOOL CPasswordManageWizardPage::changePassword()
{
	PasswordManager* pPasswordManager = NULL;

	//Grab the instance of PasswordManager
	if ( pPasswordManager = PasswordManager::getInstance() )
	{
		//Attempt to change the password for the supplied account
		if ( pPasswordManager->changeServerGroupPassword( m_serverGroup, m_account, m_oldPassword, m_newPassword ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}
void CPasswordManageWizardPage::OnEnable(BOOL bEnable)
{
	CNewWizPage::OnEnable(bEnable);

	//Get the combo box's child windows as a CEdit control
//	CEdit* pEdit = static_cast<CEdit*>( m_accountComboBox.GetWindow( GW_CHILD ) );

	//Set to read only.  This forces the user to use the items in the drop down instead of typing in their own
//	pEdit->SetReadOnly();

//	m_accountComboBox.AddString( "admin" );
//	m_accountComboBox.AddString( "readonly" );

	//Set the initially displayed item to the top item
//	m_accountComboBox.SetCurSel( 0 );
}
