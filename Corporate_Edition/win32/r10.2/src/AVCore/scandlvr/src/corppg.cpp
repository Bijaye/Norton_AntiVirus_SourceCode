// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/CorpPg.cpv   1.7   07 Jul 1998 18:21:22   sedward  $
/////////////////////////////////////////////////////////////////////////////
//
// CorpPg.cpp: implementation of the CorpInfoPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/CorpPg.cpv  $
// 
//    Rev 1.7   07 Jul 1998 18:21:22   sedward
// Nuked 'ReleaseListCtrlMemory' (we're no longer allocating those, the control
// does it for us), plus some general cleanup.
//
//    Rev 1.6   08 Jun 1998 16:17:44   SEDWARD
// Added support for a list control that displays target platforms for defs.
//
//    Rev 1.5   26 May 1998 18:27:50   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.4   25 May 1998 13:50:00   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.3   27 Apr 1998 18:11:48   SEDWARD
// Removed needless calls to CString::GetBuffer for ScanCfg "Set" calls.
//
//    Rev 1.2   20 Apr 1998 18:04:50   SEDWARD
// Added support for new UI.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ScanDeliverDLL.h"
#include "ScanDlvr.h"
#include "CorpPg.h"
#include "Global.h"
#include "xapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CorpInfoPropertyPage property page

IMPLEMENT_DYNCREATE(CorpInfoPropertyPage, CWizardPropertyPage)

CorpInfoPropertyPage::CorpInfoPropertyPage()
    : CWizardPropertyPage(CorpInfoPropertyPage::IDD, IDS_CAPTION_CORPORATE_INFO)
{
    //{{AFX_DATA_INIT(CorpInfoPropertyPage)
    m_szPlatGoldNumber = _T("");
	//}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_INFO_16;
    m_bitmap256 = IDB_SDBMP_INFO_256;
}

CorpInfoPropertyPage::~CorpInfoPropertyPage()
{
}

void CorpInfoPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CorpInfoPropertyPage)
	DDX_Control(pDX, IDC_LISTCTRL_PLATFORMS, m_listCtrlPlatforms);
    DDX_Text(pDX, IDC_EDIT_PLAT_OR_GOLD_NUMBER, m_szPlatGoldNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CorpInfoPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CorpInfoPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_LISTCTRL_PLATFORMS, OnClickListctrlPlatforms)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CorpInfoPropertyPage message handlers



// ==== UpdateConfigData ==================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CorpInfoPropertyPage::UpdateConfigData(void)
{
    auto    DWORD       dwResult = 0;
    auto    DWORD       dwOs = 0;

    // update the member variables with current field contents
    this->UpdateData(TRUE);

    // update the global configuration data object
    GetOsCheckboxes(&dwOs);
    g_ConfigInfo.SetOperatingSystem(dwOs);
    g_ConfigInfo.SetCorporateSupportNumber(m_szPlatGoldNumber);

    // write the current configuration data to disk
    g_ConfigInfo.WriteConfigFile(&dwResult);

    return (TRUE);

}  // end of "CorpInfoPropertyPage::UpdateConfigData"



// ==== OnWizardBack ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CorpInfoPropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();
    return CWizardPropertyPage::OnWizardBack();

}  // end of "CorpInfoPropertyPage::OnWizardBack"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CorpInfoPropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();
    return CWizardPropertyPage::OnWizardNext();

}  // end of "CorpInfoPropertyPage::OnWizardNext"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CorpInfoPropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here


    // limit the length in the edit controls...

    auto    CEdit*      pEditCtrl;
    auto    DWORD       dwOs;

    // technical support number
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_PLAT_OR_GOLD_NUMBER);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_CORPORATE_SUPPORT_NUMBER_LEN);
        }


    // initialize this dialog's member variables...

    // corporate support number
    g_ConfigInfo.GetCorporateSupportNumber(m_szPlatGoldNumber.GetBuffer(
                                                MAX_CORPORATE_SUPPORT_NUMBER_LEN)
                                                , MAX_CORPORATE_SUPPORT_NUMBER_LEN);
    m_szPlatGoldNumber.ReleaseBuffer();



    // insert a column into the list control
    auto    int             result;
    auto    LV_COLUMN       newCol;
    auto    RECT            listRect;

    newCol.mask = LVCF_FMT | LVCF_WIDTH;
    newCol.fmt = LVCFMT_LEFT;
    m_listCtrlPlatforms.GetClientRect(&listRect);
    newCol.cx = (listRect.right - listRect.left);
    result = m_listCtrlPlatforms.InsertColumn(0, &newCol);

    // create the image list object
    m_imageList.Create(IDB_SDBMP_CHECKBOXES, 16, 1, RGB(255, 0, 255));
    m_listCtrlPlatforms.SetImageList(&m_imageList, LVSIL_SMALL);

    // populate the "platform" list control with the SARC file and save the number
    // of platforms in the configuration object
    auto    int         numPlatforms = PopulateListCtrl();
    g_ConfigInfo.SetNumPlatforms(numPlatforms);

    // operating system checkboxes
    g_ConfigInfo.GetOperatingSystem(&dwOs);
    SetOsCheckboxes(dwOs);

    // refresh the fields
    this->UpdateData(FALSE);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CorpInfoPropertyPage::OnInitDialog"



// ==== GetOsCheckboxes ===================================================
//
//  This function examines the current state of each operating system
//  checkbox and updates the global configuration object accordingly.
//
//  Input:
//      dwOS    -- a pointer to a DWORD that is to store the bit flags
//                 for each OS (ie, whether or not it's selected)
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CorpInfoPropertyPage::GetOsCheckboxes(DWORD*  dwOS)
{
    auto    DWORD       dwPlatformID;
    auto    int         nIndex = 0;
    auto    int         numItems = 0;
    auto    LV_ITEM     listItem;

    // TODO: Add your control notification handler code here

    // initialize the list item structure
    memset((void*)&listItem, 0, sizeof(LV_ITEM));
    listItem.mask = LVIF_IMAGE;

    // clear the input argument
    *dwOS = 0;

    // get the number of items in the list control
    numItems = m_listCtrlPlatforms.GetItemCount();

    // loop and set or clear bits in the configuration object
    for (nIndex = 0; nIndex < numItems; ++nIndex)
        {
        // fetch the platform ID associated with this item
        dwPlatformID = m_listCtrlPlatforms.GetItemData(nIndex);

        // see which bitmap is currently in use for this item
        listItem.iItem = nIndex;
        if (0 != m_listCtrlPlatforms.GetItem(&listItem))
            {
            if (SD_CHECKED_BITMAP == listItem.iImage)
                {
                // set the corresponding bit
                *dwOS |= dwPlatformID;
                }
            else
                {
                // clear the corresponding bit
                *dwOS &= ~dwPlatformID;
                }
            }
        }

}  // end of "CorpInfoPropertyPage::GetOsCheckboxes"



// ==== SetOsCheckboxes ===================================================
//
//  This function sets the state for each of the operating system
//  checkboxes based on what's currently stored in the global configuration
//  object.
//
//  Input:
//      dwOS    -- a DWORD containing the bit flag set/cleared state for
//                 each OS
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CorpInfoPropertyPage::SetOsCheckboxes(DWORD  dwOS)
{
    auto    DWORD       dwConfigOS = 0;
    auto    DWORD       dwPlatform = 0;
    auto    int         nIndex;
    auto    int         numListCtrlItems = 0;
    auto    LV_ITEM     listItem;

    // initialize the list item structure
    memset((void*)&listItem, 0, sizeof(LV_ITEM));
    listItem.mask = LVIF_IMAGE;

    // loop and set bitmaps
    numListCtrlItems = m_listCtrlPlatforms.GetItemCount();
    for (nIndex = 0; nIndex < numListCtrlItems; ++nIndex)
        {
        // set the item index
        listItem.iItem = nIndex;

        // get the item data value
        dwPlatform = m_listCtrlPlatforms.GetItemData(nIndex);
        if (dwOS & dwPlatform)
            {
            // use a "checked" bitmap
            listItem.iImage = SD_CHECKED_BITMAP;
            }
        else
            {
            // use a "unchecked" bitmap
            listItem.iImage = SD_UNCHECKED_BITMAP;
            }

        m_listCtrlPlatforms.SetItem(&listItem);
        }

}  // end of "CorpInfoPropertyPage::SetOsCheckboxes"



// ==== PopulateListCtrl ==================================================
//
//  This function reads the contents of the "platform.dat" file and uses it
//  to populate the list control with displayable strings and associated
//  data values.
//
//  Each data value is a bit flag that uniquely identifies the corresponding
//  platform (these values are defined by SARC).  The selected state of the
//  platforms is stored in the "scancfg.dat" file.
//
//  Input:
//      nothing
//
//  Output:
//      If all goes well, the total number of items added to the list control
//      is returned; if an error occurs, an invalid value (-1) is returned.
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

int CorpInfoPropertyPage::PopulateListCtrl(void)
{
    auto    CStdioFile      platformFile;
    auto    CString         szPlatform;
    auto    CString         szPlatformData;
    auto    int             numItems = 0;
    auto    int             nResult = 0;
    auto    TCHAR           szPlatformDataFile[MAX_QUARANTINE_FILENAME_LEN] = "";

    // get the location of the main NAV directory
    if (FALSE == GetMainNavDir(szPlatformDataFile, sizeof (szPlatformDataFile)))
        {
        numItems = -1;
        goto  Exit_Function;
        }

    // derive a fully-qualified path to the platform data file and open it
    AppendFileName(szPlatformDataFile, PLATFORM_DAT_FILENAME);
    if (FALSE == platformFile.Open(szPlatformDataFile, CFile::modeRead))
        {
        numItems = -1;
        goto  Exit_Function;
        }

    // loop and populate the list control with items
    while (TRUE)
        {
        // get the platform string
        if (FALSE == platformFile.ReadString(szPlatform))
            {
            break;
            }

        // get the platform's corresponding data (ie, the platform ID)
        if (FALSE == platformFile.ReadString(szPlatformData))
            {
            break;
            }

        // add a new item to the list control
        nResult = AddPlatformItemToListCtrl(szPlatform, szPlatformData);
        if (-1 == nResult)
            {
            break;
            }
        else
            {
            numItems = nResult + 1;     // nResult is zero-based
            }
        }


Exit_Function:

    platformFile.Close();
    return (numItems);

}  // end of "CorpInfoPropertyPage::PopulateListCtrl"



// ==== AddPlatformItemToListCtrl =========================================
//
//  This function adds a new item to the list control.  It adds both the
//  text string to be displayed, and also its associated data.
//
//  Input:
//      szPlatform      -- a reference to a CString containing the string
//                         to display
//
//      szPlatformData  -- a reference to a CString containing the string
//                         equivalent of the item's data (ie, a bit value
//                         that uniquely identifies the platform to SARC's
//                         back end system)
//
//  Output:
//      The index of the new item being added to the list control.
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

int CorpInfoPropertyPage::AddPlatformItemToListCtrl(CString&  szPlatform
                                                    , CString&  szPlatformData)
{
    auto    int             nIndex;

    // get the new item index
    nIndex = m_listCtrlPlatforms.GetItemCount();
    nIndex = m_listCtrlPlatforms.InsertItem(nIndex, szPlatform);

    // set the item's data
    m_listCtrlPlatforms.SetItemData(nIndex, atol(szPlatformData));

    return (nIndex);

}  // end of "CorpInfoPropertyPage::AddPlatformItemToListCtrl"



// ==== OnClickListctrlPlatforms ==========================================
//
//  This function will toggle the checkbox if the user clicks on the bitmap.
//
//  Input:  the usual MFC fare
//  Output: nothing
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

void CorpInfoPropertyPage::OnClickListctrlPlatforms(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    auto    BOOL            iconHit;
    auto    int             clickedItemIndex;

    // if an item was hit...
    iconHit = GetClickedItemIndex(m_listCtrlPlatforms, clickedItemIndex);
    if  ((-1 != clickedItemIndex)  &&  (TRUE == iconHit))
        {
        // ... toggle the checkbox
        ToggleListItemCheckbox(m_listCtrlPlatforms, clickedItemIndex);
        }

   *pResult = 0;

}  // end of "CorpInfoPropertyPage::OnClickListctrlPlatforms"






