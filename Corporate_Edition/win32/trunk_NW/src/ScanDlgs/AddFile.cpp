// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// AddFile.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "AddFile.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFile

IMPLEMENT_DYNAMIC(CAddFile, CFileDialog)

CAddFile::CAddFile(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    // Set title
    m_sTitle.LoadString( IDS_ADD_FILE_TITLE );
    m_ofn.lpstrTitle = m_sTitle.GetBuffer(0);

    // Set up extension template.
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
    m_ofn.hInstance = AfxGetResourceHandle();
    // m_ofn.hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( IDD_MOVE_FILE_TEMPLATE ), RT_DIALOG );
    m_ofn.lpTemplateName = MAKEINTRESOURCE( IDD_MOVE_FILE_TEMPLATE );

    // Set initial dir to root of system volume.
    TCHAR szBuff[ MAX_PATH ];                    
    GetWindowsDirectory( szBuff, MAX_PATH );
    lstrcpyn( m_szInitialDir, szBuff, 3 );
    m_ofn.lpstrInitialDir = m_szInitialDir;

    m_bDelete = TRUE;

    m_iCtrlID = -1;
}


BEGIN_MESSAGE_MAP(CAddFile, CFileDialog)
	//{{AFX_MSG_MAP(CAddFile)
	ON_WM_DESTROY()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAddFile::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
    // Set check state
    CheckDlgButton( IDC_DELETE_CHECK, m_bDelete );

    // Change Open button text to Add
    CWnd *p = GetParent()->GetDlgItem( 1 );
    ASSERT( *p );
    if( p )
        {
        CString s;
        s.LoadString( IDS_ADD_BUTTON );
        p->SetWindowText( s );
        }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAddFile::OnDestroy() 
{
    // Get state of delete checkbox.
    m_bDelete = IsDlgButtonChecked( IDC_DELETE_CHECK );

	CFileDialog::OnDestroy();	
}

void CAddFile::OnCancelMode() 
{
	CFileDialog::OnCancelMode();	
}
