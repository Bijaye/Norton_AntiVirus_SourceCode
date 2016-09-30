// DrivePickerDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "DrivePickerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrivePickerDlg dialog


CDrivePickerDlg::CDrivePickerDlg(IScanTask* pScanTask, CWnd* pParent /*=NULL*/)
    : CDialog(CDrivePickerDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CDrivePickerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_pScanTask = pScanTask;
    m_pScanTask->AddRef();
    m_nSelected = 0;
}

CDrivePickerDlg::~CDrivePickerDlg()
{
    m_pScanTask->Release();
}

void CDrivePickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrivePickerDlg)
	DDX_Control(pDX, IDC_DRIVE_LIST, m_listDrives);
	DDX_Control(pDX, IDC_SCAN, m_btnScan);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrivePickerDlg, CDialog)
	//{{AFX_MSG_MAP(CDrivePickerDlg)
    ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_NOTIFY(NM_CLICK, IDC_DRIVE_LIST, OnClickDriveList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DRIVE_LIST, OnKeydownDriveList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrivePickerDlg message handlers

BOOL CDrivePickerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

	SHFILEINFO sfi;
	HIMAGELIST hImgList;

	// Insert a column.
	ListView_InsertColumn(m_listDrives.m_hWnd, 0, _T(""));

	// Get the handle of the system image list
	hImgList = reinterpret_cast<HIMAGELIST>(::SHGetFileInfo(_T("buffy.com"), FILE_ATTRIBUTE_NORMAL
	                                                                       , &sfi
	                                                                       , sizeof(SHFILEINFO)
	                                                                       , SHGFI_USEFILEATTRIBUTES
	                                                                       | SHGFI_SYSICONINDEX
	                                                                       | SHGFI_SMALLICON));

	// Set the control to use the new image list.
	ListView_SetImageList(m_listDrives.m_hWnd, hImgList, LVSIL_SMALL);

	// Set the state image list, which holds the checkboxes.  They look
	// nicer than what the control provides with the LVS_EX_CHECKBOXES style.
	hImgList = ImageList_LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DRIVE_LIST_CHECKBOXES)
	                                                            , 16
	                                                            , 2
	                                                            , RGB(0xFF,0x00,0xFF)
	                                                            , IMAGE_BITMAP
	                                                            , LR_LOADTRANSPARENT);
	ListView_SetImageList(m_listDrives.m_hWnd, hImgList, LVSIL_STATE);
    
    ::DestroyIcon ( sfi.hIcon );

	// Now, get the display name and icon for all drives on the system.
	TCHAR szDriveRoot[] = _T("A:\\");
	DWORD dwDrivesOnSystem = ::GetLogicalDrives();

	// Go through the alphabate
	for ( ; szDriveRoot[0] <= 'Z'; szDriveRoot[0]++, dwDrivesOnSystem >>= 1 )
	{
		// Not a valid drive letter
		if (!( dwDrivesOnSystem & 1))
			continue;

		// Get the type for the next drive, and check dwFlags to determine
		// if we should show it in the list.
		switch(::GetDriveType(szDriveRoot))
		{
		case DRIVE_NO_ROOT_DIR:
		case DRIVE_UNKNOWN:
			// Skip disconnected network drives and drives that Windows
			// can't figure out.
			continue;
		}


		// Now get the display name for the drive, and the position of the
		// drive's icon in the system image list.  The drive letter is stored
		// in each item's LPARAM for easy retrieval later.  Once all that
		// info has been retrieved, add an item to the list.
		if (::SHGetFileInfo(szDriveRoot, 0
		                               , &sfi
		                               , sizeof(SHFILEINFO)
		                               , SHGFI_SYSICONINDEX
		                               | SHGFI_DISPLAYNAME
		                               | SHGFI_ICON
		                               | SHGFI_SMALLICON))
		{
			LVITEM lvi = { LVIF_PARAM       // mask
			             | LVIF_IMAGE
			             | LVIF_TEXT
			             | LVIF_STATE
			             , m_nSelected++    // iIndex
			             , 0                // iSubItem
			             , INDEXTOSTATEIMAGEMASK(2) // state
			             , LVIS_STATEIMAGEMASK      // stateMask
			             , sfi.szDisplayName // pszText; 
			             , 0                 // cchTextMax; 
			             , sfi.iIcon         // iImage; 
			             , szDriveRoot[0] }; // lParam

			ListView_InsertItem(m_listDrives.m_hWnd, &lvi);
            
            ::DestroyIcon ( sfi.hIcon );
		}
	}   // end for

	// Set the scan button state
	m_btnScan.EnableWindow ( m_nSelected > 0 );

	ListView_SetColumnWidth(m_listDrives.m_hWnd, 0, LVSCW_AUTOSIZE_USEHEADER);

    return TRUE;
}

void CDrivePickerDlg::OnScan()
{
    for (int i = 0, nMax = ListView_GetItemCount(m_listDrives.m_hWnd); i < nMax; i++)
	{
		LVITEM lvi = { LVIF_PARAM   // mask
		             | LVIF_STATE
					 , i };         // iIndex

		lvi.stateMask = LVIS_STATEIMAGEMASK;
		ListView_GetItem(m_listDrives.m_hWnd, &lvi);

		if (INDEXTOSTATEIMAGEMASK(2) == lvi.state)
		{
			m_pScanTask->AddItem(typeDrive, subtypeDriveHD, lvi.lParam, NULL, 0);
		}
	}

	EndDialog(IDOK);
}

void CDrivePickerDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}


void CDrivePickerDlg::OnClickDriveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW*   pNMLV = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
	LVHITTESTINFO rHitTestInfo;

	rHitTestInfo.pt = pNMLV->ptAction;

	ListView_HitTest(m_listDrives.m_hWnd, &rHitTestInfo);

	if (LVHT_ONITEMSTATEICON == rHitTestInfo.flags)
	{
		UINT uState = ListView_GetItemState(m_listDrives.m_hWnd, rHitTestInfo.iItem, LVIS_STATEIMAGEMASK);

		uState = ( uState == INDEXTOSTATEIMAGEMASK(1) ) ? 2 : 1;    // swap state icon
		ListView_SetItemState(m_listDrives.m_hWnd, rHitTestInfo.iItem, INDEXTOSTATEIMAGEMASK(uState), LVIS_STATEIMAGEMASK);

		// Set the scan button state
		if (1 == uState)
			m_nSelected--;
		else
			m_nSelected++;

		m_btnScan.EnableWindow ( m_nSelected > 0 );
	}

	
	*pResult = 0;
}

void CDrivePickerDlg::OnKeydownDriveList(NMHDR* pNMHDR, LRESULT* pResult) 
{

    NMLVKEYDOWN* pLVKD = reinterpret_cast<NMLVKEYDOWN*>(pNMHDR);
	if (VK_SPACE == pLVKD->wVKey)
	{
		int nItem = ListView_GetNextItem(m_listDrives.m_hWnd, -1, LVNI_FOCUSED);

		if ( -1 != nItem )
		{
			UINT uState = ListView_GetItemState(m_listDrives.m_hWnd, nItem, LVIS_STATEIMAGEMASK);

			uState = (uState == INDEXTOSTATEIMAGEMASK(1)) ? 2 : 1; // swap state icon
			ListView_SetItemState(m_listDrives.m_hWnd, nItem, INDEXTOSTATEIMAGEMASK(uState), LVIS_STATEIMAGEMASK);

			// Set the scan button state
			if (1 == uState)
				m_nSelected--;
			else
				m_nSelected++;

			m_btnScan.EnableWindow ( m_nSelected > 0 );
		}
	}

	*pResult = 0;
}
