// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ConnectWizardPage.cpp : implementation file

#include "stdafx.h"
#include "ScsPassMan.h"
#include "ScsPassManDlg.h"
#include "ConnectWizardPage.h"
#include ".\connectwizardpage.h"
#include "SymSaferRegistry.h"
//#include "securecommsmisc.h" //Included for hardcoded account names

// CConnectWizardPage dialog

//REMOVED_FOR_VC6//IMPLEMENT_DYNAMIC(CConnectWizardPage, CNewWizPage)
CConnectWizardPage::CConnectWizardPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CConnectWizardPage::IDD, pParent)
	, m_scsUsername(_T(""))
	, m_scsPassword(_T(""))
	, m_failureOccurred( FALSE )
	, m_serverGroup(_T(""))
{
}

CConnectWizardPage::~CConnectWizardPage()
{
}

void CConnectWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONNECT_USERNAME_EDITBOX, m_scsUsername);
	DDV_MaxChars(pDX, m_scsUsername, 20);
	DDX_Text(pDX, IDC_CONNECT_PASSWORD_EDITBOX, m_scsPassword);
	DDV_MaxChars(pDX, m_scsPassword, 20);
	DDX_Control(pDX, IDC_CONNECT_STATUS_TEXT, m_connectStatusText);
	DDX_Control(pDX, IDC_CONNECT_SERVERGROUP_COMBOBOX, m_serverGroupComboBox);
	DDX_CBString(pDX, IDC_CONNECT_SERVERGROUP_COMBOBOX, m_serverGroup);
}


BEGIN_MESSAGE_MAP(CConnectWizardPage, CNewWizPage)
	ON_WM_CTLCOLOR()
//	ON_EN_CHANGE(IDC_CONNECT_USERNAME_EDITBOX, OnEnChangeConnectUsernameEditbox)
//	ON_EN_CHANGE(IDC_CONNECT_PASSWORD_EDITBOX, OnEnChangeConnectPasswordEditbox)
	ON_WM_ENABLE()
//	ON_CBN_EDITCHANGE(IDC_CONNECT_SERVERGROUP_COMBOBOX, OnCbnEditchangeConnectServergroupCombobox)
//ON_CBN_SELCHANGE(IDC_CONNECT_SERVERGROUP_COMBOBOX, OnCbnSelchangeConnectServergroupCombobox)
END_MESSAGE_MAP()


// CConnectWizardPage message handlers

//Removed: we no longer have a Network Browse Button
/*void CConnectWizardPage::OnBnClickedConnectNetworkBrowseButton()
{
	// TODO: Add your control notification handler code here

	//This is the root of our browse window.  Once set, we are responsible for freeing the memory
	//when we are done with it
	LPITEMIDLIST pidlRoot = NULL;
	//The BROWSEINFO struct is required for the SHBrowseForFolder method
	BROWSEINFO browseInfo;

	//This method gets the location of a virtual folder, in this case "My Network Places".  What
	//What we really want is "Entire Network", but there is no CSIDL for that one.  My Network Places
	//includes recently visisted network folders.  Fortunatly, the OK button on the browser will not
	//be enabled until an actual computer is selected.
	SHGetFolderLocation( AfxGetMainWnd()->m_hWnd, CSIDL_NETWORK, NULL, NULL, &pidlRoot );

	browseInfo.hwndOwner	  = AfxGetMainWnd()->m_hWnd;
	browseInfo.pidlRoot		  = pidlRoot;
	browseInfo.pszDisplayName = ( char* ) malloc( MAX_PATH );
	browseInfo.lpszTitle	  = "Select a Primary Server machine:";
	browseInfo.ulFlags		  = BIF_BROWSEFORCOMPUTER;
	browseInfo.lpfn			  = NULL;
	browseInfo.lParam		  = NULL;
	LPITEMIDLIST pidl		  = SHBrowseForFolder( &browseInfo );

	//Free the allocated memory
	CoTaskMemFree( pidlRoot );
}*/

LRESULT CConnectWizardPage::OnWizardBack( void )
{
	//Clear out the status text
	m_connectStatusText.SetWindowText( _T( "" ) );

	return 0;
}

LRESULT CConnectWizardPage::OnWizardNext( void )
{
	UpdateData();

//Removing verification of admin password as we are now only selecting the server group on this page
//	m_connectStatusText.SetWindowText( "Connecting to \"" + m_serverGroup + "\"..." );

	//Attempt to connect to the server group's primary server using the supplied information
//	if ( connectToPrimaryServer() )
//	{
		//We've successfully connected.

		//Put the servergroup name in our parent object so that we can access it in the next wizard page
		static_cast<CScsPassManDlg*>( GetParent() )->m_serverGroup = m_serverGroup;

		//Clear out the status text first
//		m_connectStatusText.SetWindowText( _T( "" ) );

		//Returning 0 takes us to the next wizard page
//		return 0;
//	}
//	else
//	{
		//Connection has failed
//		m_failureOccurred = TRUE;

//		m_connectStatusText.SetWindowText( "Connection to \"" + m_serverGroup + "\" failed." );

		//Returning -1 keeps us on the same wizard page
//		return -1;
//	}

	return 0;
}

/*
 * Attempt to connect ( via UnlockDomain() ) to the Primary Server of the supplied Server Group using the hardcoded 
 * admin account and the supplied admin password.  Only admins are allowed to change passwords.  If the
 * password checks out, then show the accounts.  If the password doesn't check out, don't show the accounts.
 */
/*BOOL CConnectWizardPage::connectToPrimaryServer()
{
	PasswordManager* pPasswordManager = NULL;

	//Grab the instance of PasswordManager
	if ( pPasswordManager = PasswordManager::getInstance() )
	{
		//If we can unlock the server group, the password is good
		if ( pPasswordManager->unlockServerGroup( m_serverGroup, m_scsPassword, ScsSecureComms::BUILTIN_USERNAME_ADMIN ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}*/

/*HBRUSH CConnectWizardPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNewWizPage::OnCtlColor(pDC, pWnd, nCtlColor);

	//If we're painting the connection status text...
	if ( pWnd->GetDlgCtrlID() == IDC_CONNECT_STATUS_TEXT )
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
}*/

/*void CConnectWizardPage::OnSetActive()
{
//	checkForEnableOrDisable();
	//Grab the current values
	//UpdateData();

	//If any of the fields are empty, then set the Next button inactive
	//We're currently using a the hardcoded admin account, so don't check the username
	//if ( m_scsPassword.IsEmpty() )
	//{
	//	CWnd* nextButton = GetParent()->GetDlgItem( ID_WIZNEXT );
	//	nextButton->EnableWindow( FALSE );
	//}
}*/

/*
 * Check to see if 'Next' should be enabled or disabled
 */
/*void CConnectWizardPage::checkForEnableOrDisable()
{
    CWnd* nextButton = GetParent()->GetDlgItem( ID_WIZNEXT );

	UpdateData();

	//If any of the fields are empty, then set the Next button inactive
	//Otherwise, set it to active ( in case it was previously deactivated )
	//We're currently using a the hardcoded admin account, so don't check the username
	if ( m_serverGroup.IsEmpty()   ||
		 m_scsPassword.IsEmpty() )
	{
		nextButton->EnableWindow( FALSE );
	}
	else
	{
		nextButton->EnableWindow();
	}
}*/

/*void CConnectWizardPage::OnEnChangeConnectUsernameEditbox()
{
	checkForEnableOrDisable();
}*/

/*void CConnectWizardPage::OnEnChangeConnectPasswordEditbox()
{
	checkForEnableOrDisable();
}*/

void CConnectWizardPage::OnEnable(BOOL bEnable)
{
	CNewWizPage::OnEnable(bEnable);
}

/*
 * If needed, get a list of server groups from the registry.
 */
void CConnectWizardPage::OnSetActive()
{
	//We are using a set for our list of server groups to prevent duplicates
	StringSet stringSet;

	CNewWizPage::OnSetActive();

	//Get the combo box's child windows as a CEdit control
	CEdit* pEdit = static_cast<CEdit*>( m_serverGroupComboBox.GetWindow( GW_CHILD ) );

	//Set to read only.  This forces the user to use the items in the drop down instead of typing in their own
	pEdit->SetReadOnly();

	//Only try to populate the combo box if it is empty.  This keeps us from repopulating it with the same
	//server groups over and over again everytime the page is viewed.
	if ( 0 == m_serverGroupComboBox.GetCount() )
	{
		//Get the server group names from the local registry
		if ( getServerGroupNames( stringSet ) )
		{
			//Put the server group names into our Combo Box
			for ( StringSet::iterator it = stringSet.begin(); it != stringSet.end(); ++it )
			{
				std::string serverGroup = *it;
				m_serverGroupComboBox.AddString( serverGroup.c_str() );
			}
		}
		else
		{
			AfxMessageBox( "Unable to retrieve Server Groups.  Make sure your are running on a machine with SSC installed." );
			
			//If we're here then we weren't able to get any server groups.  There is no point in continuing,
			//so gray out the Next button
			CWnd* nextButton = GetParent()->GetDlgItem( ID_WIZNEXT );
			nextButton->EnableWindow( FALSE );
		}
	}

	//Set the initially displayed item to the top item
	m_serverGroupComboBox.SetCurSel( 0 );
}

/*
 * Code stolen from ServerTree.cpp and modified slightly
 * Get all server group names from the address cache and put then in the passed in set
 * [OUT] stringSet - a set of server group names
 * [RETURN] true if success, false otherwise
*/
BOOL CConnectWizardPage::getServerGroupNames( StringSet& stringSet )
{
	BOOL ret			  = TRUE;
	char* consoleKey	  = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Console";
	char* addressCacheKey = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache";
    HKEY hkey			  = 0;

	//Try to open the console reg key.  If its not there, we're not on an SSC machine and won't work
    if( ERROR_SUCCESS != ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, consoleKey, 0, KEY_READ, &hkey ) )
    {
		return FALSE;
	}

	//Close the console key
    ::RegCloseKey( hkey );

	//Open the address cache reg key
    if( ERROR_SUCCESS == ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, addressCacheKey, 0, KEY_READ, &hkey ) )
    {
		char childName[50];
        DWORD childIndex = 0;
        DWORD childSize = 50;
		FILETIME fileTime;

        //Enumerate the server keys in the address cache
        while( ERROR_SUCCESS == ::RegEnumKeyEx( hkey, childIndex, childName, &childSize, NULL, NULL, NULL, &fileTime ) )
        {
            HKEY hChild = 0;

			//Open the current server key
            if( ERROR_SUCCESS == ::RegOpenKeyEx( hkey, childName, 0, KEY_READ, &hChild ) )
            {
				char  serverGroupName[100];
				DWORD sizeServerGroupName = 100;

				//Get server group name of server
				if( ERROR_SUCCESS == ::SymSaferRegQueryValueEx( hChild, "Domain", 0, NULL, ( LPBYTE ) serverGroupName, &sizeServerGroupName ) )
				{
					//Add the server group to our set
					stringSet.insert( serverGroupName );
				}
				else
				{
					ret = FALSE;
				}
                
				//Close the current server key
                ::RegCloseKey( hChild );
            }
			else
			{
				ret = FALSE;
			}

			//childSize is changed by RegEnumKeyEx, so reset it
            childSize = 50;
			//childIndex is our iterator, increment it
            ++childIndex;
        }

		//Close the address cache key
        ::RegCloseKey( hkey );
    }
	else
	{
		ret = FALSE;
	}

	return ret;
}
