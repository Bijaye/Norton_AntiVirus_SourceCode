// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ExtensionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ExtensionsDlg.h"

#ifdef __SRVCON
#include "SrvCon.h"
#endif /* __SRVCON */

#ifdef __CLNTCON
#include "ClntCon.h"
#endif /* __CLNTCON */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined( __SRVCON ) && !defined( __CLNTCON )
#define CVPString	CString
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtensionsDlg dialog


CExtensionsDlg::CExtensionsDlg(CWnd* pParent /*=NULL*/)
:	CDialog(CExtensionsDlg::IDD, pParent),
	m_description( _T( "" ) )
{
	//{{AFX_DATA_INIT(CExtensionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CExtensionsDlg::~CExtensionsDlg()
{
	m_extensions.RemoveAll();
}

void CExtensionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtensionsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtensionsDlg, CDialog)
	//{{AFX_MSG_MAP(CExtensionsDlg)
	ON_EN_CHANGE(IDC_EXTENSION, OnChangeExtension)
	ON_LBN_SELCHANGE(IDC_EXTENSION_LIST, OnSelchangeExtensionList)
	ON_BN_CLICKED(IDC_EXTENSION_EXECUTABLE, OnExtensionExecutable)
	ON_BN_CLICKED(IDC_EXTENSION_REMOVE, OnExtensionRemove)
	ON_BN_CLICKED(IDC_EXTENSION_DOCUMENTS, OnExtensionDocuments)
	ON_BN_CLICKED(IDC_EXTENSION_ADD, OnExtensionAdd)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtensionsDlg message handlers

BOOL CExtensionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pWnd;
	POSITION pos;
	
	//Set the Context-sensitive ID
	SetHelpID( IDD_EXTENSIONS );

	pos = m_extensions.GetHeadPosition();
	if ( pos )
	{
		CVPString str;

		VERIFY( pWnd = GetDlgItem( IDC_EXTENSIONS_LIST ) );
		while( pos )
		{
			str = m_extensions.GetNext( pos );
			((CListBox*)pWnd)->AddString( str );
		}
	}

	VERIFY( pWnd = GetDlgItem( IDC_DESCRIPTION ) );
	pWnd->SetWindowText( m_description );

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_ADD ) );
	pWnd->EnableWindow( FALSE );

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_REMOVE ) );
	pWnd->EnableWindow( FALSE );
	
	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION ) );
	((CEdit*)pWnd)->LimitText( 3 );
	pWnd->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExtensionsDlg::OnChangeExtension() 
{
	CWnd* pWnd;

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION ) );
	if ( ((CEdit*)pWnd)->LineLength() )
	{
		VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_ADD ) );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_ADD ) );
		pWnd->EnableWindow( FALSE );
	}
}

void CExtensionsDlg::OnSelchangeExtensionList() 
{
	CWnd* pWnd;
	
	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_LIST ) );
	if ( ((CListBox*)pWnd)->GetSelCount() )
	{
		VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_REMOVE ) );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_REMOVE ) );
		pWnd->EnableWindow( FALSE );
	}
}

void CExtensionsDlg::OnExtensionExecutable() 
{
	CVPString exts;
	CVPString newExt;
	CListBox* pWnd;
	int i;
	int item;
	
	VERIFY( pWnd = (CListBox*)GetDlgItem( IDC_EXTENSION_LIST ) );
	VERIFY( exts.LoadString( IDS_EXTENSION_EXECUTABLES ) );

	VERIFY( pWnd->SetSel( -1, FALSE ) >= 0 );

	i = exts.Find( ';' );
	while( i != -1 )
	{
		newExt = exts.Left( i );
		item = pWnd->FindString( 0, newExt );
		if ( item < 0 )
		{
			item = pWnd->AddString( newExt );
			ASSERT( item >= 0 );
			VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
		}
		else
		{
			VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
		}

		exts = exts.Mid( i + 1 );
		i = exts.Find( ';' );
	}

	newExt = exts;
	item = pWnd->FindString( 0, newExt );
	if ( item < 0 )
	{
		item = pWnd->AddString( newExt );
		ASSERT( item >= 0 );
		VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
	}
	else
	{
		VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
	}
	
	CWnd* pButton;
	VERIFY( pButton = GetDlgItem( IDC_EXTENSION_REMOVE ) );
	pButton->EnableWindow( TRUE );
}


void CExtensionsDlg::OnExtensionDocuments() 
{
	CVPString exts;
	CVPString newExt;
	CListBox* pWnd;
	int i;
	int item;

	VERIFY( pWnd = (CListBox*)GetDlgItem( IDC_EXTENSION_LIST ) );
	VERIFY( exts.LoadString( IDS_EXTENSION_DOCUMENTS ) );

	VERIFY( pWnd->SetSel( -1, FALSE ) >= 0 );

	i = exts.Find( ';' );
	while( i != -1 )
	{
		newExt = exts.Left( i );
		item = pWnd->FindString( 0, newExt );
		if ( item < 0 )
		{
			item = pWnd->AddString( newExt );
			ASSERT( item >= 0 );
			VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
		}
		else
		{
			VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
		}

		exts = exts.Mid( i + 1 );
		i = exts.Find( ';' );
	}
	
	newExt = exts;
	item = pWnd->FindString( 0, newExt );
	if ( item < 0 )
	{
		item = pWnd->AddString( newExt );
		ASSERT( item >= 0 );
		VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
	}
	else
	{
		VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
	}

	CWnd* pButton;
	VERIFY( pButton = GetDlgItem( IDC_EXTENSION_REMOVE ) );
	pButton->EnableWindow( TRUE );
}

void CExtensionsDlg::OnExtensionRemove() 
{
	CListBox* pWnd = NULL;
	int count;
	int i;
	int itemCount;

	VERIFY( pWnd = (CListBox*)GetDlgItem( IDC_EXTENSION_LIST ) );
	VERIFY( itemCount = pWnd->GetCount() );
	VERIFY( count = pWnd->GetSelCount() );

	i = 0;
	while( count && ( i < itemCount ) )
	{
		ASSERT( i < itemCount );

		while( ( i < itemCount ) && !pWnd->GetSel( i ) )
			i++;

		if ( pWnd->GetSel(i) )
		{
			VERIFY( pWnd->DeleteString( i ) >= 0 );
			itemCount--;
		}

		count--;
	}

	if ( !pWnd->GetSelCount() )
	{
		CButton* pButton;

		VERIFY( pButton = (CButton*)GetDlgItem( IDC_EXTENSION_REMOVE ) );
		pButton->EnableWindow( FALSE );
	}

}

void CExtensionsDlg::OnExtensionAdd() 
{
	CWnd* pWnd;
	CVPString newText;
	int newTextLen;
	CVPString itemText;
	int itemCount;
	int i;
	BOOL rc = FALSE;

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION ) );
	pWnd->GetWindowText( newText );
	VERIFY( newTextLen = newText.GetLength() );
	ASSERT( newText.GetLength() );
	newText.MakeUpper();
	
	if ( newText.GetAt( 0 ) == '.' )
	{
		i = 0;
		while( ( i < newTextLen ) && ( newText.GetAt( i ) == '.' ) )
		{
			i++;
		}
		if ( i == newTextLen )
			goto cleanup;

		newText = newText.Mid( i );
		newTextLen = newText.GetLength();
	}

	i = 0;
	while( i < newTextLen )
	{
		if ( !_istalnum ( newText.GetAt( i ) ) )
		{
			if ( i != 0 )
				goto cleanup;
			else if ( newText.GetAt( i ) != '.' )
				goto cleanup;
		}	
		i++;
	}

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_LIST ) );
	itemCount = ((CListBox*)pWnd)->GetCount();

	VERIFY( ((CListBox*)pWnd)->SetSel( -1, FALSE ) >= 0 );

	// make sure it ain't already there
	while( itemCount )
	{
		itemCount--;
		((CListBox*)pWnd)->GetText( itemCount, itemText );
		if ( newText == itemText )
		{
			VERIFY( ((CListBox*)pWnd)->SetSel( itemCount, TRUE ) >= 0 );
			rc = TRUE;
			goto cleanup;
		}
	}

	itemCount = ((CListBox*)pWnd)->AddString( newText );
	ASSERT( itemCount >= 0 );
	VERIFY( ((CListBox*)pWnd)->SetSel( itemCount, TRUE ) >= 0 );

	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION_REMOVE ) );
	pWnd->EnableWindow( TRUE );

	rc = TRUE;

cleanup:
	if ( !rc )
#if ( defined( __SRVCON) || defined( __CLNTCON ) )
		THIS_APP()->MessageBox( IDS_EXTENSION_BAD_EXTENSION );
#else
		AfxMessageBox( IDS_EXTENSION_BAD_EXTENSION );
#endif
	VERIFY( pWnd = GetDlgItem( IDC_EXTENSION ) );
	pWnd->SetFocus();
	((CEdit*)pWnd)->SetSel( 0, -1 );
	
	return;
}

void CExtensionsDlg::OnOK() 
{
	CListBox* pList;
	int i;
	CVPString str;

	VERIFY( pList = (CListBox*)GetDlgItem( IDC_EXTENSIONS_LIST ) );
	i = pList->GetCount();
	if ( i == LB_ERR )
		goto cleanup;

	m_extensions.RemoveAll();
	while( i-- )
	{
		pList->GetText( i, str );
		m_extensions.AddTail( str );
	}

cleanup:
	CDialog::OnOK();
}

//Copy the member list to the passed list
void CExtensionsDlg::SetExtensions( CStringList *ptrList )
{
	POSITION pos;

	//First, empty out the list
	m_extensions.RemoveAll();

	//Set my extensions into the dialog
	//If I have any items in my list, set them now
	if( !ptrList->IsEmpty() )
	{
		pos = ptrList->GetHeadPosition();

		while( pos )
		{
			m_extensions.AddTail( ptrList->GetAt( pos ) );
			ptrList->GetNext( pos );
		}
	}
	
}

//Copy the member list from the passed list
void CExtensionsDlg::GetExtensions( CStringList *ptrList )
{
	POSITION pos;

	//First, empty the passed list
	ptrList->RemoveAll();

	//Get the extensions from the dialog
	if( !m_extensions.IsEmpty() )
	{
		pos = m_extensions.GetHeadPosition();

		while( pos )
		{
			ptrList->AddTail( m_extensions.GetAt( pos ) );
			m_extensions.GetNext( pos );
		}
	}
}

void CExtensionsDlg::OnDefaults() 
{
	CVPString exts;
	CVPString newExt;
	CListBox* pWnd;
	int i;
	int item;

	VERIFY( pWnd = (CListBox*)GetDlgItem( IDC_EXTENSION_LIST ) );
	VERIFY( exts.LoadString( IDS_DEFAULT_EXTENSIONS ) );

	// remove every one
	if ( pWnd->GetCount() )
	{
		VERIFY( pWnd->SetSel( -1, TRUE ) >= 0 );
		OnExtensionRemove();
	}

	// add the defaults
	i = exts.Find( ';' );
	while( i != -1 )
	{
		newExt = exts.Left( i );
		item = pWnd->FindString( 0, newExt );
		if ( item < 0 )
		{
			item = pWnd->AddString( newExt );
			ASSERT( item >= 0 );
			VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
		}
		else
		{
			VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
		}

		exts = exts.Mid( i + 1 );
		i = exts.Find( ';' );
	}

	newExt = exts;
	item = pWnd->FindString( 0, newExt );
	if ( item < 0 )
	{
		item = pWnd->AddString( newExt );
		ASSERT( item >= 0 );
		VERIFY( pWnd->SetSel( item, TRUE ) >= 0 );
	}
	else
	{
		VERIFY( pWnd->SetSel( item, TRUE ) >=  0 );
	}

	VERIFY( pWnd->SetSel( -1, TRUE ) >= 0 );
	
	CWnd* pButton;
	VERIFY( pButton = GetDlgItem( IDC_EXTENSION_REMOVE ) );
	pButton->EnableWindow( TRUE );
}
