// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ExcludePatternsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ExcludePatternsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExcludePatternsDlg dialog


CExcludePatternsDlg::CExcludePatternsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExcludePatternsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExcludePatternsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CExcludePatternsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExcludePatternsDlg)
	DDX_Control(pDX, IDC_PATTENRNS, m_ctrlPatterns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExcludePatternsDlg, CDialog)
	//{{AFX_MSG_MAP(CExcludePatternsDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcludePatternsDlg message handlers

BOOL CExcludePatternsDlg::OnInitDialog() 
{
	POSITION pos;
	CString strTemp;
	int		iIndex;

	//Set the Context-sensitive ID
	SetHelpID( IDD_EXCLUDE_PATTERNS );

	CDialog::OnInitDialog();

	//Load my image list
/*	m_ImageList = new CImageList;
	if( m_ImageList )
	{
		m_ImageList->Create( IDB_OBJECTS, 16, 16, COLORREF( RGB(0, 255, 0) ) );

		//m_ctrlPatterns.SetImageList( m_ImageList );
	}
*/
	//Now, copy the internal CStringList
	pos = m_PatternsList->GetHeadPosition();

	while( pos )
	{
		m_ctrlPatterns.AddString( (LPCTSTR)m_PatternsList->GetNext( pos ) );
	}

	//Now walk through the items and select the ones that need to be selected
	pos = m_SelPatternsList->GetHeadPosition();

	while( pos )
	{
		strTemp = m_SelPatternsList->GetNext( pos );

		iIndex = m_ctrlPatterns.FindStringExact( -1, (LPCTSTR)strTemp );
		if( iIndex != LB_ERR )
		{
			m_ctrlPatterns.SetCheck( iIndex, TRUE );
		}
	}

	return TRUE;  
}

void CExcludePatternsDlg::OnOK() 
{
	int		iIndex = 0;
	CString	strTemp;

	//First, empty out the list
	m_SelPatternsList->RemoveAll();

	//Fill the CString list with items from the ImageCheckList	
	while(	iIndex < m_ctrlPatterns.GetCount() )
	{	
		//If the item is checked, add it to the internal list
		if( m_ctrlPatterns.GetCheck( iIndex ) )
		{
			m_ctrlPatterns.GetText( iIndex, strTemp );
			m_SelPatternsList->AddTail( strTemp );
		}

		iIndex++;
	}
	
	CDialog::OnOK();
}

void CExcludePatternsDlg::OnDestroy() 
{
	//Destroy my listbox
	m_ctrlPatterns.DestroyWindow();

	CDialog::OnDestroy();
	
}

void CExcludePatternsDlg::SetPatterns( CStringList *lstAll, CStringList *ptrList )
{
	m_PatternsList = lstAll;
	m_SelPatternsList = ptrList;
}

