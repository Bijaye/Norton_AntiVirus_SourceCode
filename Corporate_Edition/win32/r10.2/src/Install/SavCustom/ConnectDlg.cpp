// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavCustom.h"
#include "ConnectDlg.h"
#include "Domains.h"
#include "RegKey.h"
#include "FindDlg.h"
#include "clientreg.h"
#include "SymSaferStrings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog


CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_SERVER_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_INSERTITEM, IDC_SERVER_LIST, OnInsertitemServerList)
	ON_BN_CLICKED(IDC_FINDCOMPUTER_BUTTON, OnFindcomputerButton)
	ON_NOTIFY(LVN_KEYDOWN, IDC_SERVER_LIST, OnKeydownServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnItemchangedServerList)
	ON_BN_CLICKED(IDC_RET_BACK, OnRetBack)
	ON_BN_CLICKED(IDC_RET_NEXT, OnRetNext)
	ON_BN_CLICKED(IDC_RET_CANCEL, OnRetCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Center the dialog
	CenterWindow();

	// Load up our image list
	g_bitmap.LoadBitmap( IDB_BROWSER );
	g_imgList.Create( 16, 16, TRUE, 11, 0 );
	g_imgList.Add( &g_bitmap, RGB(128, 0, 128) );

	m_listCtrl.SetImageList( &g_imgList, LVSIL_SMALL );

	CString sServer, sVersion;

	sServer.LoadString( IDS_COL_SERVER );
	sVersion.LoadString( IDS_COL_VERSION );

	m_listCtrl.InsertColumn( 0, sServer, LVCFMT_LEFT, 235 );
	m_listCtrl.InsertColumn( 1, sVersion, LVCFMT_LEFT, 66 );

	GetServers( &m_listCtrl, TRUE );
	m_listCtrl.SetItemState(m_listCtrl.GetTopIndex(),LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlg::OnInsertitemServerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CString		sServer;
	DWORD		version;
	*pResult	= 0;
	DWORD		dwProductVersionFlag=0;
	TCHAR		szLog [MAX_PATH+1];

	m_listCtrl.GetItemText(pNMListView->iItem,0,sServer.GetBuffer(512),512);	
	sServer.ReleaseBuffer();
	sServer = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache\\" + sServer;

	if (RegKeyGetValue(HKLM,sServer,"ProductVersion",(LPBYTE)&version,4,NULL) == ERROR_FILE_NOT_FOUND)
	{
		sServer.LoadString(IDS_UNKNOWN_VERSION);
		m_listCtrl.SetItemText(pNMListView->iItem,1,sServer);
		return;
	}
	else
	{
		if (RegKeyGetValue(HKLM,sServer,_T(szReg_Val_Flags2),(LPBYTE)&dwProductVersionFlag,4,NULL) == ERROR_FILE_NOT_FOUND)
		{
			OutputDebugString("OnInsertitemServerList regread failed");
		}
		else
		{
			sssnprintf (szLog, sizeof(szLog), "OnInsertitemServerList VersionFlag %d", dwProductVersionFlag);
			OutputDebugString(szLog);
		}

		if (dwProductVersionFlag & PF2_SECURE_COMM_ENABLED)
		{
			// add version information to list if server does support Secure comm
			sServer.Format("%u.%u.%u.%u",LOWORD(version)/100,(LOWORD(version)%100)/10,LOWORD(version)%10,HIWORD(version));
			m_listCtrl.SetItemText(pNMListView->iItem,1,sServer);	
		}
		else
		{
			// remove from list if server does not support Secure comm
 			m_listCtrl.DeleteItem(pNMListView->iItem);
 			return;
		}
	}
}

void CConnectDlg::OnFindcomputerButton() 
{
	CFindDlg dlgFind;
	dlgFind.DoModal();
	CString sComputer(dlgFind.GetReportedName());

	if (!sComputer.IsEmpty())
	{
		sComputer.MakeUpper();

		LV_ITEM lvi;
		lvi.iItem = 32767;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iImage = 9;
		lvi.pszText = (LPTSTR)(LPCTSTR)sComputer;

		int item, index = -1;
		LV_FINDINFO lvf;
		lvf.psz = (LPTSTR)(LPCTSTR)sComputer;
		lvf.flags = LVFI_STRING;

		if ((item = m_listCtrl.FindItem(&lvf)) == -1)
			item = m_listCtrl.InsertItem(&lvi);

		if (item != -1)
		{
			while ((index = m_listCtrl.GetNextItem(index,LVNI_ALL | LVNI_SELECTED)) != -1)
				m_listCtrl.SetItemState(index,0,LVIS_SELECTED | LVIS_FOCUSED);

			m_listCtrl.SetItemState(item,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
			m_listCtrl.SetFocus();
			m_listCtrl.EnsureVisible(item,FALSE);
		}
	}	
}

void CConnectDlg::OnKeydownServerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	ListView_GetISearchString(m_listCtrl,0);	

	*pResult = 0;
}

void CConnectDlg::OnItemchangedServerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int index = pNMListView->iItem;
	*pResult = 0;

	CString sServer(m_listCtrl.GetItemText(index,0));
	CWnd *tmp = GetDlgItem(IDC_SERVER_NAME);
	tmp->SetWindowText(sServer);
	m_Selection = sServer;
	
}

void CConnectDlg::OnRetBack() 
{
	g_imgList.DeleteImageList();
	g_bitmap.DeleteObject();

	EndDialog( IDC_RET_BACK );	
}

void CConnectDlg::OnRetNext() 
{
	g_imgList.DeleteImageList();
	g_bitmap.DeleteObject();

	EndDialog( IDC_RET_NEXT );	
}

void CConnectDlg::OnRetCancel() 
{
	g_imgList.DeleteImageList();
	g_bitmap.DeleteObject();

	EndDialog( IDC_RET_CANCEL );	
}

void CConnectDlg::OnPaint()
{
	CPaintDC dc(this);

	// Get coordinates
	CWnd* pCWndTemp = NULL;
	pCWndTemp = GetDlgItem(IDC_STATIC_LOGO);

	RECT tempRect;

	if ( pCWndTemp )
		pCWndTemp->GetWindowRect(&tempRect);

	POINT pt;
	pt.x = tempRect.left;
	pt.y = tempRect.top;

	int nWidth = tempRect.right - tempRect.left;
	int nHeight = tempRect.bottom - tempRect.top;

	ScreenToClient(&pt);

	// Save the new coordinates
	RECT rect;
	rect.left = pt.x;
	rect.top = pt.y;
	rect.right = rect.left + nWidth;
	rect.bottom = rect.top + nHeight;

	// Load the bitmap 
	CBitmap bmp;
	CBitmap* pOldBitmap = NULL;
	
	bmp.LoadBitmap(IDB_IS_BITMAP);
	
	// Paint the dialog
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	pOldBitmap = dcMem.SelectObject(&bmp);
	dc.BitBlt(rect.left, rect.top, nWidth, nHeight, &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();


    // Draw the 'cutin' line around rectangle 
    DrawCutInLine( &dc, (LPRECT) &rect, FALSE );
    
    // Draw the cutin line just above the push buttons
	pCWndTemp = GetDlgItem( IDC_CUTIN_LINE );
    
    if ( pCWndTemp )
	    pCWndTemp->GetWindowRect( &tempRect );
    
	pt.x = tempRect.left;
    pt.y = tempRect.top;

    nWidth  = tempRect.right - tempRect.left;
    nHeight = tempRect.bottom - tempRect.top;
    ScreenToClient( &pt );

    rect.left   = pt.x;
    rect.top    = pt.y;
    rect.right  = rect.left + nWidth;
    rect.bottom = rect.top + nHeight;

    DrawCutInLine( &dc, (LPRECT) &rect, TRUE );
}


BOOL CConnectDlg::DrawCutInLine( CDC* pPdc, LPRECT pRect, BOOL bSingle )
{
    // Use the color defined for SHADOW to create DARK pen and create a pen
    CPen DarkPen;
    DarkPen.CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ));
    
    // Select the New pen, storing the old one.
    CPen* pOldPen = NULL;
    pOldPen       = pPdc -> SelectObject( &DarkPen );

    // Draw the lines on the left side corner of the rectangle
    if( bSingle )
        pPdc -> MoveTo( pRect -> left,  pRect -> top );
    else
    {
        pPdc -> MoveTo( pRect -> left,  pRect -> bottom );
        pPdc -> LineTo( pRect -> left,  pRect -> top );
    }
    pPdc -> LineTo( pRect -> right, pRect -> top );
    
    // Create a light pen, and draw lines on the right corner of the rectangle.
    CPen LightPen;
    if( GetSysColor( COLOR_BTNFACE ) == RGB( 255, 255, 255 ) )
        LightPen.CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ));
    else
        LightPen.CreatePen( PS_SOLID, 1, RGB( 255, 255, 255));
  
    pPdc -> SelectObject( &LightPen );

    if( bSingle )
    {
       pPdc -> MoveTo( pRect -> left,  pRect -> top+1 );
       pPdc -> LineTo( pRect -> right, pRect -> top+1 );
    }
    else
    {
      pPdc -> MoveTo( pRect -> left+1,  pRect -> bottom );
      pPdc -> LineTo( pRect -> right-1, pRect -> bottom );
      pPdc -> LineTo( pRect -> right-1, pRect -> top );
    }
    
    // Select the original Pen value.
    pPdc -> SelectObject( pOldPen );
    return( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
//
// Function: DoServerListDlg
//
// Description: Main entry point for calling the MFC dialog from Install
// Shield.  CConnectDlg is the custom dialog that displays Symantec AntiVirus
// servers in a listview control.
//
// IN:  Buffer to receive the servername.
//
// OUT: Returns the following:
//
//      IDC_RET_NEXT when the user pressed the 'Next' button.
//      IDC_RET_BACK when the user pressed the 'Back' button.
//      IDC_RET_CANCEL when the user pressed the 'Cancel' button.
//      NULL if there was an exception.
//      szSelectedServer = Name of server if found.
// 
/////////////////////////////////////////////////////////////////////////////
// 9/1/99 MHOTTA Function Created :: Based off of Install Shield's
// example on how to create a custom wizard panel.
/////////////////////////////////////////////////////////////////////////////
LONG __stdcall DoServerListDlg( HWND hWndSetup, TCHAR* lpszSelectedServer )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CConnectDlg connectDlg( CWnd::FromHandle( hWndSetup ));
	LONG dwRtn=0;

	TRY
	{
		dwRtn = (LONG) connectDlg.DoModal();

		_tcscpy( lpszSelectedServer, connectDlg.m_Selection );

		return( dwRtn );
	}
	CATCH_ALL(e)
	{
		// Return NULL (0) if we catch any execptions.
		return 0;
	}
	END_CATCH_ALL

	return( dwRtn );
}
