////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FolderBrowseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FolderBrowseDlg.h"
#include "resource.h"
#include "..\\NavTaskWizardRes\\resource.h"
#include "globals.h"
#include "ISVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderBrowseDlg dialog


CFolderBrowseDlg::CFolderBrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderBrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolderBrowseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    
    m_bScanning = TRUE;
    m_rectMin.SetRectEmpty();
}


void CFolderBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderBrowseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolderBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CFolderBrowseDlg)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
    ON_BN_CLICKED(IDC_SCAN, OnScan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderBrowseDlg message handlers

BOOL CFolderBrowseDlg::OnInitDialog() 
{
CWinApp* pApp = AfxGetApp();

	CDialog::OnInitDialog();

    // Set caption icon. We always have an icon since we have the 
    // WS_THICKFRAME style.

	HICON hIcon = NULL;
	if(CISVersion::GetProductIcon(hIcon))
	{
		SetIcon ( hIcon, TRUE );
		SetIcon ( hIcon, FALSE );
	}

	// Set body text
	// Format body text with product name
	CString csText;

	csText.LoadString(GetResInst(), IDS_TREE_HEADER_FOLDER);
	SetDlgItemText(IDC_TREE_HEADER, csText);

	csText.LoadString(GetResInst(), IDS_CANCEL);
	SetDlgItemText(IDCANCEL, csText);

	csText.LoadString(GetResInst(), IDS_SCAN);
	SetDlgItemText(IDC_SCAN, csText);

    if ( !m_bScanning )
        {
        // Reset the text above the tree and on the Scan button if we are
        // being called from the task wizard.

        CString sText;

        VERIFY ( sText.LoadString ( GetResInst(), IDS_SELECT_ADD_FOLDERS ));
        SetDlgItemText ( IDC_TREE_HEADER, sText );

        VERIFY ( sText.LoadString ( GetResInst(), IDS_ADD_FOLDERS_BTN_TEXT ));
        SetDlgItemText ( IDC_SCAN, sText );
        }

    // Set the RECT that'll hold the minimum size of the dialog.

	GetClientRect ( m_rectMin );

    // Set up the window resizer.

    m_resizer.SetParent ( GetSafeHwnd() );
    m_resizer.UseSizeGrip ( TRUE );
    m_resizer.RegisterWindow ( IDC_SCAN, 1, 1, 0, 0 );
    m_resizer.RegisterWindow ( IDCANCEL, 1, 1, 0, 0 );
    m_resizer.RegisterWindow ( IDC_FOLDER_TREE , 0, 0, 1, 1 );
    m_resizer.RegisterWindow ( IDC_TREE_HEADER, 0, 0, 1, 0 );

    m_resizer.SetInitialized ( TRUE );

    m_browse.Attach ( GetDlgItem ( IDC_FOLDER_TREE )->GetSafeHwnd() );
    m_browse.Init ( AfxFindResourceHandle (
                        MAKEINTRESOURCE(IDB_SCAN_FOLDER_CHECKBOXES), 
                        RT_BITMAP ));

    m_browse.SetValidatingFunction ( this );
    OnItemChanged ();

	m_csProductName = CISVersion::GetProductName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFolderBrowseDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
    // If the minimum-size rect hasn't been set yet, pass the message
    // on to the base class.
    if ( m_rectMin.IsRectEmpty() )
        {
        CDialog::OnGetMinMaxInfo ( lpMMI );
        }
    else
        {
        lpMMI->ptMinTrackSize.x = m_rectMin.right;
        lpMMI->ptMinTrackSize.y = m_rectMin.bottom;
        }
}

void CFolderBrowseDlg::OnSize(UINT nType, int cx, int cy) 
{
    // If the window resizer has been initialized, pass the size message
    // along to it so it can work its magic.  The IDC_TREE_HEADER needs to
    // be redrawn, or else the word-wrapping will get munged up.

    if ( !m_rectMin.IsRectEmpty() )
        {
	    m_resizer.UpdateSize ( nType, cx, cy );
        GetDlgItem ( IDC_TREE_HEADER )->RedrawWindow();
        }

    CDialog::OnSize(nType, cx, cy);
}

void CFolderBrowseDlg::OnScan() 
{
    m_browse.UpdateSelectedList();
   	
    // If no folders are selected, pop up a message box with some helpful
    // text telling the user to use the checkboxes.

    if ( 0 == m_browse.GetTotalSelectedItems() )
        {
		CString sMsg;
		sMsg.LoadString(GetResInst(), IDS_NO_FOLDERS_SELECTED);
        MessageBox ( sMsg, m_csProductName,
                     MB_ICONEXCLAMATION );
        }
    else
        {
        EndDialog ( IDOK );
        }
}


void CFolderBrowseDlg::OnItemChanged(void)
{
    // If the user unselected all the items we want to grey out
    // the scan button. It seems a little expensive CPU-wise
    // to do this, but I don't think it will be noticeable even
    // on the slowest machine.
    //
    m_browse.UpdateSelectedList ();

    if ( 0 == m_browse.m_listSelectedItems.size () )
        GetDlgItem (IDC_SCAN )->EnableWindow (FALSE);
    else
        GetDlgItem (IDC_SCAN )->EnableWindow (TRUE);
}


BOOL CFolderBrowseDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    // Pass WM_NOTIFY messages on to the NVBrowse object.

    m_browse.OnNotify ( wParam, lParam );
    
	return CDialog::OnNotify(wParam, lParam, pResult);
}

