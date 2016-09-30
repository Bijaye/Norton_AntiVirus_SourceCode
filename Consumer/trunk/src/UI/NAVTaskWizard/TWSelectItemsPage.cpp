////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TWSelectItemsPage.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"

#include "ScanTaskInterface.h"
#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "wizard.h"
#include "ccCoInitialize.h"
#include "isSymTheme.h"

#include "FolderBrowseDlg.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace avScanTask;

/////////////////////////////////////////////////////////////////////////////
// CTWSelectItemsPage property page

IMPLEMENT_DYNCREATE(CTWSelectItemsPage, CBaseWizPage)

CTWSelectItemsPage::CTWSelectItemsPage() : CBaseWizPage(IDD_TW_SELECTITEMS)
{
	//{{AFX_DATA_INIT(CTWSelectItemsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_nNextDriveIndex   = 0;
    m_nNextFolderIndex  = 0;
    m_nNextFileIndex    = 0;
}

CTWSelectItemsPage::~CTWSelectItemsPage()
{
}

void CTWSelectItemsPage::DoDataExchange(CDataExchange* pDX)
{
	CBaseWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTWSelectItemsPage)
	DDX_Control(pDX, IDC_ITEMLIST, m_ItemList);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTWSelectItemsPage, CBaseWizPage)
	//{{AFX_MSG_MAP(CTWSelectItemsPage)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEMLIST, OnItemchangedItemlist)
	ON_BN_CLICKED(IDC_SCAN_FOLDER, OnScanFolder)
	ON_BN_CLICKED(IDC_SCAN_FILE, OnScanFile)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTWSelectItemsPage message handlers

BOOL CTWSelectItemsPage::OnInitDialog() 
{
	CBaseWizPage::OnInitDialog();
	
    m_Picture.SetBitmap ( g_bUseHiColorBmps ? IDB_WIZ256_2  : IDB_WIZ16_2,
                          111, 236 );	

    m_ImgList.Create ( IDB_SCANITEM_IMGLIST, 16, 1, RGB(255,0,255) );

    m_ItemList.InsertColumn ( 0, _T("") );
    m_ItemList.SetImageList ( &m_ImgList, LVSIL_SMALL );

    ListView_SetExtendedListViewStyleEx ( m_ItemList.GetSafeHwnd(),
                                          LVS_EX_FULLROWSELECT,
                                          LVS_EX_FULLROWSELECT );

	AddScanItems();

	// Format body text with product name
	CString csText;
	CString csFormat;
	csFormat.LoadString(GetResInst(), IDS_SELECTITEMS_INTO);
	csText.Format(csFormat, m_csProductName);
	SetDlgItemText(IDC_STATIC_SELECTITEMS_INTO, csText);

	csText.LoadString(GetResInst(), IDS_STATIC_ITEMS_TO_SCAN);
	SetDlgItemText(IDC_STATIC_ITEMS_TO_SCAN, csText);

	csText.LoadString(GetResInst(), IDS_STATIC_PROMPT_REMOVE);
	SetDlgItemText(IDC_STATIC_PROMPT_REMOVE, csText);

	csText.LoadString(GetResInst(), IDS_ADD_FOLDERS);
	SetDlgItemText(IDC_SCAN_FOLDER, csText);

	csText.LoadString(GetResInst(), IDS_ADD_FILES);
	SetDlgItemText(IDC_SCAN_FILE, csText);

	csText.LoadString(GetResInst(), IDS_REMOVE);
	SetDlgItemText(IDC_REMOVE, csText);

	csText.LoadString(GetResInst(), IDS_CANCEL);
	GetParentSheet()->SetDlgItemText(IDCANCEL, csText);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTWSelectItemsPage::AddScanItems(void)
{
	m_ItemList.DeleteAllItems();

    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    IScanTask* pTask = pParent->GetScanTask ();
    
    if ( !pTask )
        return;

    // If the scan task already has items in it, we need to put the items
    // in our list control.
	HRESULT hr = S_OK;
	bool bEmpty = false;
	
	hr = pTask->IsEmpty(bEmpty);
	if( FAILED(hr) )
		return;

    if ( !bEmpty )
    {
        pTask->EnumReset();
        SCANITEMTYPE type;
        SCANITEMSUBTYPE subtype;
        TCHAR chDrive = _TCHAR(' ');
        cc::IStringPtr pPath;
        CScanItem NewItem;
        unsigned long ulVID = 0;

        while (SUCCEEDED(pTask->EnumNext(type, subtype, chDrive, &pPath, ulVID)))
        {
            NewItem.SetItemType(type, subtype);
            NewItem.SetDrive(chDrive);
            NewItem.SetPath(pPath->GetStringW());

            AddScanItem ( NewItem );
        }
    }

    UpdateControls();	
}

BOOL CTWSelectItemsPage::OnSetActive() 
{
    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());

    if ( pParent->IsEditing() )
    {
        GetParentSheet()->SetWizardButtons ( PSWIZB_NEXT );
    }
    else
    {
        GetParentSheet()->SetWizardButtons ( PSWIZB_BACK | PSWIZB_NEXT );
    }

    UpdateControls();	

	return TRUE;
}

LRESULT CTWSelectItemsPage::OnWizardNext() 
{
CScanItem item;
int       i, nMaxIndex;

    nMaxIndex = m_ItemList.GetItemCount();

    if ( 0 == nMaxIndex )
        {
		CString sMsg;
		sMsg.LoadString( GetResInst(), IDS_NO_ITEMS_IN_TASK );
        MessageBox ( sMsg, m_csProductName, MB_ICONEXCLAMATION );

        return -1;                      // stay on this page
        }
    
    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    avScanTask::IScanTask* pTask = pParent->GetScanTask ();
	HRESULT hr = S_OK;

    pTask->Empty();

    for ( i = 0; i < nMaxIndex; i++ )
    {
        item = *(CScanItem*)(m_ItemList.GetItemData(i));

        SCANITEMTYPE type;
        SCANITEMSUBTYPE subtype;
        item.GetItemType(type, subtype);

        hr = pTask->AddItem ( type, subtype, item.GetDrive(), item.GetPath(), 0 );
        ATLASSERT(SUCCEEDED(hr));
    }

	return CBaseWizPage::OnWizardNext();
}

void CTWSelectItemsPage::OnDestroy() 
{
int i, nMax;
CScanItem* pItem;

    nMax = m_ItemList.GetItemCount();

    for ( i = 0; i < nMax; i++ )
        {
        pItem = reinterpret_cast<CScanItem*>( m_ItemList.GetItemData(i) );
        delete pItem;
        }

	CBaseWizPage::OnDestroy();
}

// Handler for LVN_ITEMCHANGED: When the selection in the list changes,
// we update the Remove and Next buttons (kind of a poor man's 
// UPDATE_COMMAND_UI).
void CTWSelectItemsPage::OnItemchangedItemlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UNREFERENCED_PARAMETER(pNMHDR);

    UpdateControls();	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTWSelectItemsPage command message handlers

void CTWSelectItemsPage::OnScanFolder() 
{
CFolderBrowseDlg dlg;
int              nModalRet;

    dlg.m_bScanning = FALSE;

    // Display a wait cursor before DoModal() since the folder browse
    // dlg can take a while to show up.  The cursor will be restored 
    // automagically when the dialog is shown.
    {
    CWaitCursor w;

    nModalRet = dlg.DoModal();
    }

    if ( IDOK == nModalRet )
        {
        list<CNVBrowseItem>::iterator itFolders;
        int        nItemIndex = -1;
        int        nEntries = dlg.m_browse.m_listSelectedItems.size();

        ASSERT ( nEntries > 0 );

        for ( itFolders = dlg.m_browse.m_listSelectedItems.begin();
              itFolders != dlg.m_browse.m_listSelectedItems.end();
              itFolders++ )
            {
            ASSERT ( 0 != itFolders->GetName() );

            CScanItem NewItem;
    
            if ( itFolders->GetSubDirs() )
                {
                NewItem.SetItemType ( typeFolder, subtypeFolderAndSubs );
                }
            else
                {
                NewItem.SetItemType ( typeFolder, subtypeFolder );
                }

            NewItem.SetPath ( itFolders->GetName() );

            nItemIndex = AddScanItem ( NewItem );

            }
        }
}

// FileOpenDlgThread - Launch File Open dialog
unsigned __stdcall CTWSelectItemsPage::FileOpenDlgThread(void * pParam)
{	
	CCTRCTXI0(L"Enter");

	// Must initialize COM with COINIT_APARTMENTTHREADED flag; 
	// otherwise, clicking on "My computer" from File Open dialog 
	// will not show any item under My computer. See MSDN ShBrowseForFolder()
	ccLib::CCoInitialize coinit(ccLib::CCoInitialize::eSTAModel);

	// Get pointer to "this" object via argument
	CTWSelectItemsPage* pThis = (CTWSelectItemsPage*)(pParam);

	// Apply SymTheme. Do not bail out if failed.
	HRESULT hr1 = S_OK;
	CISSymTheme isSymTheme;
	hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
	if(FAILED(hr1))
	{
		CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
	}

	CCTRCTXI0(L"Setup FileOpenDialog");

	CString	strScanFilesTitle;
	CString	strFileFilter;

	// Get title and filter for the file open dialog
	strScanFilesTitle.LoadString(GetResInst(), IDS_SCAN_FILES);
	strFileFilter.LoadString(GetResInst(), IDS_ALL_FILES_FILTER);

	// Clean up previous dialog and its data
	pThis->m_spFileDlg.Release();
	ZeroMemory(pThis->m_szFilesBuffer, sizeof(pThis->m_szFilesBuffer));

	// Construct new dialog
	pThis->m_spFileDlg = new CCustomFileDialog ( TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | 
		OFN_ALLOWMULTISELECT | OFN_ENABLESIZING,
		strFileFilter, pThis, 0  );

	pThis->m_spFileDlg->m_ofn.lpstrFile = pThis->m_szFilesBuffer;
	pThis->m_spFileDlg->m_ofn.nMaxFile  = sizeof(pThis->m_szFilesBuffer)/sizeof(WCHAR);
	pThis->m_spFileDlg->m_ofn.lpstrTitle = strScanFilesTitle;

	CCTRCTXI0(L"Show FileOpenDialog");

	// Show file open dialog
	if(IDOK == pThis->m_spFileDlg->DoModal())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void CTWSelectItemsPage::OnScanFile() 
{	
	CCTRCTXI0(L"Begin FileOpenDialog thread");

	// Disable "Add Files" button
	CCTRCTXI0(L"Disable \"Add Files\" button");
	GetDlgItem(IDC_SCAN_FILE)->EnableWindow(FALSE);

	HANDLE hFileOpenThread = NULL;
	try
	{
		// We are currently running in a thread that is initialized with COINIT_MULTITHREADED
		// while File Open dialog CFileDialog requires to run in a thread that is 
		// initialized with COINIT_APARTMENTTHREADED (see MSDN ShBrowseForFolder).
		// To support Files Open dialog, a new thread is created and is initialized with
		// COINIT_APARTMENTTHREADED to display the File Open dialog. If we don't do this,
		// Clicking "My computer" icon on the dialog will not display any item.

		bool bExit = false;
		unsigned threadID = 0;

		hFileOpenThread = (HANDLE)_beginthreadex(NULL, 0, FileOpenDlgThread, (LPVOID)(this), 0, &threadID);

		while(!bExit)
		{
			DWORD dwRet = MsgWaitForMultipleObjects(1, &hFileOpenThread, FALSE, INFINITE, QS_ALLINPUT); 
			switch (dwRet)
			{
			default:
				break;

			case WAIT_OBJECT_0:
				{
					bExit = true;
					CCTRCTXI0(L"File Open dialog is closed. Bail out.");
				}
				break;

			case WAIT_OBJECT_0 + 1:
				{
					MSG msg;
					while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
					{
						if (msg.message == WM_QUIT)
						{
							bExit = true;
							CCTRCTXI0(L"On WM_QUIT. Bail out.");
							break;
						}

						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				break;
			}  

			if(bExit)
				break;
		} 

		CCTRCTXI0(L"Exit FileOpenDialog thread");

		DWORD dwExitCode = 0;
		if(GetExitCodeThread(hFileOpenThread, &dwExitCode) && 
			(0 == dwExitCode))
		{
			m_ItemList.SetRedraw(FALSE);

			CScanItem NewItem;
			
			POSITION pos = m_spFileDlg->GetStartPosition();

			while(NULL != pos)
			{
				CScanItem NewItem;

				NewItem.SetItemType(typeFile, subtypeFile);
				NewItem.SetPath(m_spFileDlg->GetNextPathName(pos));

				AddScanItem(NewItem);
			}

			m_ItemList.SetRedraw(TRUE);
		}
	}
	catch(_com_error& e)
	{
		CCTRCTXE1(L"Exception thrown! Error: %d", e.Error());
	}

	CloseHandle(hFileOpenThread);

	// Enable "Add Files" button
	CCTRCTXI0(L"Enable \"Add Files\" button");
	GetDlgItem(IDC_SCAN_FILE)->EnableWindow(TRUE);

	// Set focus to the scan list 
	GetDlgItem(IDC_ITEMLIST)->SetFocus();
}

void CTWSelectItemsPage::OnRemove() 
{
int nSel = m_ItemList.GetNextItem ( -1, LVNI_SELECTED );

    if ( -1 != nSel )
        {
        RemoveScanItem ( nSel );
        }
    else
        {
        MessageBeep(MB_OK);
        }

    UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTWSelectItemsPage other functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    AddScanItem
//
// Description:
//  Adds a new scan item to the list, making sure the item goes in the
//  right position (ie, drives grouped first, then folders, then files).
//
// Input:
//  pItem: [in] Pointer to the scan item being added.
//
// Returns:
//  The item's index in the list control, or -1 if the item was not added.
//  This will only occur if the item is for a drive and the drive cannot
//  be accessed at the time this function is called.
//
//////////////////////////////////////////////////////////////////////////

int CTWSelectItemsPage::AddScanItem ( CScanItem& Item )
{
int              nItemIndex;
SCANITEMTYPE     type;
SCANITEMSUBTYPE  subtype;
CString          sDesc;
int              nImage = -1;

    Item.GetItemType(type, subtype);

    tstring strNewItem;
    
    switch ( type )
    {
        case typeDrive :
            strNewItem = Item.GetDrive ();
            break;

        case typeFolder :
            if ( subtypeFolderAndSubs == subtype )
            {
                CString strDesc; 
				CString strFormat;

				strFormat.LoadString( GetResInst(), IDS_FOLDER_AND_SUBS_ITEM_DESCRIPTION );
                strDesc.Format ( strFormat, (LPCTSTR) Item.GetPath() );

                strNewItem = strDesc;
            }
            else
                strNewItem = Item.GetPath ();
            break;

        default:
            strNewItem = Item.GetPath ();
            break;
    }

    CString strTempItem;

    // Scan the current list of items and make sure this isn't a dup.
    //
    for ( int iIndex = 0; iIndex < m_ItemList.GetItemCount (); iIndex ++ )
    {
        strTempItem.Empty ();

        strTempItem = m_ItemList.GetItemText ( iIndex, 0 );

        if ( !strTempItem.IsEmpty () &&
             0 == strTempItem.Compare ( strNewItem.c_str () ))
            return -1;
    }
    
    switch ( type )
        {
        case typeDrive:
            {
			CString sFormat;
			sFormat.LoadString( GetResInst(), IDS_DRIVE_ITEM_DESCRIPTION );

            sDesc.Format ( sFormat.GetString(), Item.GetDrive() );
            CString sRoot;
            sRoot.Format ( _T("%c:\\"), Item.GetDrive() );

            switch ( GetDriveType ( sRoot ) )
                {
                case DRIVE_UNKNOWN:
                case DRIVE_FIXED:
                case DRIVE_RAMDISK:
                    nImage = 1;
                break;

                case DRIVE_REMOVABLE:
                    nImage = 0;
                break;

                case DRIVE_CDROM:
                    nImage = 2;
                break;

                case DRIVE_REMOTE:
                    nImage = 3;
                break;

                case DRIVE_NO_ROOT_DIR:
                    // The network drive might have been unmapped, or some
                    // other bad error happened, so don't add the item.
                    return -1;
                break;
                }

            nItemIndex = m_ItemList.InsertItem ( m_nNextDriveIndex++,
                                                 sDesc, nImage );
            m_nNextFolderIndex++;
            m_nNextFileIndex++;
            }
        break;  // end case typeDrive

        case typeFolder:
            if ( subtypeFolderAndSubs == subtype )
                {
				CString sFormat;
				sFormat.LoadString( GetResInst() , IDS_FOLDER_AND_SUBS_ITEM_DESCRIPTION );
                sDesc.Format ( sFormat.GetString(), (LPCTSTR) Item.GetPath() );
                nImage = 5;
                }
            else
                {
                sDesc = Item.GetPath();
                nImage = 4;
                }

            nItemIndex = m_ItemList.InsertItem ( m_nNextFolderIndex++,
                                                 sDesc, nImage );
            m_nNextFileIndex++;
        break;

        case typeFile:
            sDesc = Item.GetPath();
            nImage = 6;
            
            nItemIndex = m_ItemList.InsertItem ( m_nNextFileIndex++,
                                                 sDesc, nImage );
        break;

        DEFAULT_UNREACHABLE;
        }
            
    ASSERT ( -1 != nItemIndex );        // make sure the insert worked.

    m_ItemList.SetColumnWidth ( 0, LVSCW_AUTOSIZE_USEHEADER );
    m_ItemList.EnsureVisible ( nItemIndex, FALSE );
    SetScanItemData(nItemIndex, Item);

    return nItemIndex;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveScanItem
//
// Description:
//  Removes a scan item from the list control, frees memory used by
//  the scan item, and updates internal counters that track which groups
//  of items (ie, drives/folders/files) are where in the list.
//
// Input:
//  nItemIndex: [in] Index of the item to remove.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////

void CTWSelectItemsPage::RemoveScanItem ( int nItemIndex )
{
SCANITEMTYPE     type;
SCANITEMSUBTYPE  subtype;
CScanItem*       pItem;

    ASSERT ( nItemIndex >= 0  &&
             nItemIndex < m_ItemList.GetItemCount() );

    pItem = reinterpret_cast<CScanItem*> ( 
                m_ItemList.GetItemData ( nItemIndex ));

    ASSERT ( NULL != pItem );

    pItem->GetItemType (type, subtype);

    switch ( type )
        {
        case typeDrive:
            m_nNextDriveIndex--;
        // fall through

        case typeFolder:
            m_nNextFolderIndex--;
        // fall through

        case typeFile:
            m_nNextFileIndex--;
        break;

        DEFAULT_UNREACHABLE;
        }

    m_ItemList.DeleteItem ( nItemIndex );
    delete pItem;
    UpdateControls();	
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetScanItemData
//
// Description:
//  Stores a pointer to a CScanItem object as the LPARAM of the specified
//  item in the list control.
//
// Input:
//  nItemIndex: [in] The index of the item in the list.
//  pItem: [in] Pointer to be stored.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////

void CTWSelectItemsPage::SetScanItemData ( int nItemIndex,
                                           const CScanItem& Item )
{
    ASSERT ( nItemIndex >= 0  &&  nItemIndex < m_ItemList.GetItemCount() );

    m_ItemList.SetItemData(nItemIndex, reinterpret_cast<DWORD>(new CScanItem(Item)));
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    UpdateControls
//
// Description:
//  Does UPDATE_COMMAND_UI-like stuff, handling enabling/disabling of the
//  Remove and Next buttons.  Note that the other functions have to
//  call this manually - it isn't automatic like a real UPDATE_COMMAND_UI.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////

void CTWSelectItemsPage::UpdateControls()
{
UINT uItemCount = m_ItemList.GetItemCount();
UINT uSelCount = m_ItemList.GetSelectedCount();

    // Enable the Remove button if an item in the list is selected.
    GetDlgItem (IDC_REMOVE)->EnableWindow ( uSelCount > 0 );
    
    if ( uSelCount == 0 )
        m_ItemList.SetFocus ();

    // Enable the Next button if there are any items in the list.
    GetParentSheet()->GetDlgItem (ID_WIZNEXT)->EnableWindow ( uItemCount > 0 );
}

