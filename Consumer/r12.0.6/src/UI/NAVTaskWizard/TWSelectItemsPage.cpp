// TWSelectItemsPage.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"

#include "ScanTask.h"
#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "wizard.h"


#include "FolderBrowseDlg.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

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

    m_ItemList.InsertColumn ( 0, "" );
    m_ItemList.SetImageList ( &m_ImgList, LVSIL_SMALL );

    ListView_SetExtendedListViewStyleEx ( m_ItemList.GetSafeHwnd(),
                                          LVS_EX_FULLROWSELECT,
                                          LVS_EX_FULLROWSELECT );

	AddScanItems();

	// Format body text with product name
	CString csText;
	CString csFormat;
	GetDlgItemText(IDC_STATIC_SELECTITEMS_INTO, csFormat);
	csText.Format(csFormat, m_csProductName);
	SetDlgItemText(IDC_STATIC_SELECTITEMS_INTO, csText);

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
    if (!pTask->IsEmpty())
    {
        pTask->EnumReset();
        SCANITEMTYPE type;
        SCANITEMSUBTYPE subtype;
        TCHAR chDrive = _TCHAR(' ');
        TCHAR szItemPath[2*MAX_PATH] = {0};
        CScanItem NewItem;
        unsigned long ulVID = 0;

        while (pTask->EnumNext(type, subtype, chDrive, szItemPath, 2*MAX_PATH, ulVID))
        {
            NewItem.SetItemType(type, subtype);
            NewItem.SetDrive(chDrive);
            NewItem.SetPath(szItemPath);

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
        MessageBox ( _S(IDS_NO_ITEMS_IN_TASK), m_csProductName,
                     MB_ICONEXCLAMATION );

        return -1;                      // stay on this page
        }
    
    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    IScanTask* pTask = pParent->GetScanTask ();

    pTask->Empty();

    for ( i = 0; i < nMaxIndex; i++ )
    {
        item = *(CScanItem*)(m_ItemList.GetItemData(i));

        SCANITEMTYPE type;
        SCANITEMSUBTYPE subtype;
        item.GetItemType(type, subtype);

        VERIFY ( pTask->AddItem ( type, subtype, item.GetDrive(), item.GetPath(), 0 ) );
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

void CTWSelectItemsPage::OnScanFile() 
{
CString		  strScanFilesTitle ( (LPCTSTR) IDS_SCAN_FILES );
CString		  strFileFilter ( (LPCTSTR) IDS_ALL_FILES_FILTER );
CFileDialog   dlg ( TRUE, NULL, NULL,
                    OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | 
                        OFN_ALLOWMULTISELECT | OFN_ENABLESIZING,
                    strFileFilter, this, 0  );
TCHAR         szzBuffer[4096] = _T("");
CScanItem     NewItem;
int           nItemIndex;

    dlg.m_ofn.lpstrFile = szzBuffer;
    dlg.m_ofn.nMaxFile  = sizeof(szzBuffer) / sizeof(TCHAR);
    dlg.m_ofn.lpstrTitle = strScanFilesTitle;

    if ( IDOK == dlg.DoModal() )
        {
        m_ItemList.SetRedraw ( FALSE );

        POSITION pos = dlg.GetStartPosition();

        while ( NULL != pos )
            {
            CScanItem NewItem;

            NewItem.SetItemType ( typeFile, subtypeFile );
            NewItem.SetPath ( dlg.GetNextPathName ( pos ) );

            nItemIndex = AddScanItem ( NewItem );

            }

        m_ItemList.SetRedraw ( TRUE );
        }
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
int              nImage;

    Item.GetItemType(type, subtype);

    string strNewItem;
    
    switch ( type )
    {
        case typeDrive :
            strNewItem = Item.GetDrive ();
            break;

        case typeFolder :
            if ( subtypeFolderAndSubs == subtype )
            {
                CString strDesc;        
                strDesc.Format ( IDS_FOLDER_AND_SUBS_ITEM_DESCRIPTION, 
                                 (LPCTSTR) Item.GetPath() );

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
            sDesc.Format ( IDS_DRIVE_ITEM_DESCRIPTION, Item.GetDrive() );
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
                sDesc.Format ( IDS_FOLDER_AND_SUBS_ITEM_DESCRIPTION, 
                               (LPCTSTR) Item.GetPath() );
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

