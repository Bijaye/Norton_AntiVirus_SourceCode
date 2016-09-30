/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/EXCLPAGE.CPv   1.23   13 Jul 1998 19:51:04   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// EXCLPAGE.CPP - Contains class implementation for CExcludePage
//
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/EXCLPAGE.CPv  $
// 
//    Rev 1.23   13 Jul 1998 19:51:04   jtaylor
// Changed the page to only load the complicated exclusion page on Windows95 non-OEM.
//
//    Rev 1.22   13 Jul 1998 12:09:42   DBuches
// Fixed more help stuff.
//
//    Rev 1.21   02 Jul 1998 15:21:08   mdunn
// Fixed breakages on help IDs for build 22
//
//    Rev 1.20   03 Apr 1998 13:24:44   MKEATIN
// Removed File Inoculation, Immediate Notify, and Sensor.
//
//    Rev 1.19   25 Mar 1998 15:07:56   PVENKAT
// Moved changes from ROSWELL.  Disable "Innoculation" checkbox on OEM build on
// W95 platforms.  Uncheck before disbling it.
//
//
//    Rev 1.18   19 Feb 1998 11:57:10   mdunn
// Ported NEC changes from ROSNEC.
//
//    Rev 1.3   05 Nov 1997 13:36:36   mdunn
// POST-SHIP: Fixed CExcludeEdit so that the OK button is greyed out if
// the edit box is empty (and on 95, if all the exclusion type checkboxes
// are unchecked).  This prevents the user from entering an empty exclusion
// which NAV treats the same as *.*
// This is currently in STS as incident #128184.
//
//    Rev 1.2   12 Oct 1997 19:02:26   mdunn
// Fixed context help on the add/edit exclusion dlg.
//
//    Rev 1.1   16 Sep 1997 19:54:30   MDUNN
// Added a NEC-specific add/edit exclusions dialog used on 95.
//
//    Rev 1.17   17 Aug 1997 17:23:00   DALLEE
// 1. Added necessary OemToAnsi() conversions before ExcludeSFN2LFN() calls.
// File API's are in ANSI and ExcludeSFN2LFN() will expect ANSI input and give
// ANSI output (regardless of the comments in the old NAVOPTS source.. eek!)
//
// 2. Check for no activities selected when adding/editing exclusions.
//
//    Rev 1.16   10 Aug 1997 21:54:36   DBUCHES
// Doh!  Fixed a typo which broke exclusion validation.
//
//    Rev 1.15   06 Aug 1997 14:19:52   DBUCHES
// Now, only allow up to 3 character extensions.
//
//    Rev 1.13   16 Jul 1997 18:11:18   DBUCHES
// Fixed problem with deleting last exclusion from list.
//
//    Rev 1.12   16 Jul 1997 12:56:40   DBUCHES
// Added code to validate exclusion text.
//
//    Rev 1.11   09 Jul 1997 15:32:34   DBUCHES
// Changed title of FileOpen dialog.
//
//    Rev 1.10   08 Jul 1997 16:55:16   DBUCHES
// Fixed another problem with updating the exclusion frame text.
//
//    Rev 1.9   02 Jul 1997 14:06:24   DBUCHES
// Added handler for double click on listbox.
//
//    Rev 1.8   25 Jun 1997 15:16:04   DBUCHES
// Converted buttons to owner draw buttons.
//
//    Rev 1.7   24 Jun 1997 15:59:00   DBUCHES
// Fixed text display.
//
//    Rev 1.6   20 Jun 1997 12:01:02   DBUCHES
// Added Help support.
//
//    Rev 1.5   10 Jun 1997 17:50:30   DBUCHES
// Fixed exclusion bug.
//
//    Rev 1.4   10 Jun 1997 15:21:40   DBUCHES
// Fixed file open dialog caption.  Now says 'open'
//
//    Rev 1.3   27 May 1997 20:55:48   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.2   16 May 1997 18:34:54   DBUCHES
// Changed GetData() and SetData() members to use ExcludeCreateCopy().
//
//    Rev 1.1   16 May 1997 15:27:42   DBUCHES
// Finished hooking up Exclusions.  1st pass.
//
//    Rev 1.0   14 May 1997 14:58:12   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
// ExclPage.cpp : implementation file
//

#include "stdafx.h"
#include "n32opts.h"
#include "ExclPage.h"
#include "sheet.h"
#include "xapi.h"
#include "n32behav.h"




#define PASSWORD_CHECK(i)( ((CNavOptionsSheet*)GetParent())->PasswordCheck(i) )
#define SHOW_SIMPLE_EXCLUDE_PAGE ( IS_NT() || NAV_IS_SCANONLY() )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CExcludePage::m_dwWhatsThisMap[] = {
//&^    IDC_EXCLUDE_LIST,   IDH_NAVW_WHATHIS_EXCLUSIONS_LIST_SETTINGS_DLG,
    IDC_NEW_BUTTON,     IDH_NAVW_WHATHIS_EXCLUSIONS_LIST_ADD_BTN,
    IDC_EDIT_BUTTON,    IDH_NAVW_WHATHIS_EXCLUSIONS_LIST_EDIT_BTN,
    IDC_REMOVE_BUTTON,  IDH_NAVW_WHATHIS_EXCLUSIONS_LIST_DELETE_BTN,
    0,0
    };
DWORD CExcludePage::m_dwHowToMap[] = { 0,0 };

DWORD CExcludeEdit::m_dwWhatsThisMap[] = {
    IDC_TEXT_EDIT,          IDH_NAVW_WHATHIS_ADD_EXCLUSION_ITEM,
    IDC_SUBDIR_CHECK,       IDH_NAVW_WHATHIS_INCLUDE_SUBFOLDERS_CHKBOX,
    IDC_FILEOPEN_BUTTON,    IDH_NAVW_WHATHIS_ADD_EXCLUSION_ITEM,
    IDC_KNOWN_CHECK,        IDH_NAVW_WHATHIS_KNOWN_VIRUS_DETECTION_CHKBOX,
    IDC_LOWLEVEL_CHECK,     IDH_NAVW_WHATHIS_LOW_LEVEL_FORMAT_OF_HARD_DISK_CHKBOX,
    IDC_WRITE_HARD_CHECK,   IDH_NAVW_WHATHIS_WRITE_TO_HARD_DISK_CHKBOX,
    IDC_WRITE_FLOPPY_CHECK, IDH_NAVW_WHATHIS_WRITE_TO_FLOPPY_DISK_CHKBOX,
    IDC_WRITE_PROTRAM_CHECK,IDH_NAVW_WHATHIS_WRITE_TO_PROGRAM_FILES_CHKBOX,
    IDC_DOS_READONLY_CHECK, IDH_NAVW_WHATHIS_READ_ONLY_ATTRIBUTE_CHKBOX,
    0, 0
    };
DWORD CExcludeEdit::m_dwHowToMap[] = { 0, 0 };

/////////////////////////////////////////////////////////////////////////////
// CExcludePage property page

IMPLEMENT_DYNCREATE(CExcludePage, CHelperPage)

CExcludePage::CExcludePage() : CHelperPage(SHOW_SIMPLE_EXCLUDE_PAGE ? IDD_EXCLUDE_PAGE_NT : CExcludePage::IDD )
{
	//{{AFX_DATA_INIT(CExcludePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_EXCLUSIONS_LIST_DLG_HELP_BTN;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWEXCLOPT;
    m_iMaxDisplayChars = 0;

}

CExcludePage::~CExcludePage()
{
}

void CExcludePage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExcludePage)

	DDX_Control(pDX, IDC_EXCLUDE_LIST, m_ListBox);
    //}}AFX_DATA_MAP
    if(!SHOW_SIMPLE_EXCLUDE_PAGE)
        DDX_Control(pDX, IDC_EXCLUDE_EDIT, m_Edit);
}


BEGIN_MESSAGE_MAP(CExcludePage, CHelperPage)
	//{{AFX_MSG_MAP(CExcludePage)
	ON_LBN_SELCHANGE(IDC_EXCLUDE_LIST, OnSelchangeExcludeList)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, OnEditButton)
	ON_BN_CLICKED(IDC_NEW_BUTTON, OnNewButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	ON_LBN_DBLCLK(IDC_EXCLUDE_LIST, OnDblclkExcludeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcludePage message handlers

BOOL CExcludePage::SetData(LPNAVOPTIONS pOptions)
{
    // Create a copy of the exclusions.
    if( FALSE == ExcludeCreateCopy( &pOptions->exclude, &m_Exclude ))
    	return FALSE;

    return TRUE;
}

BOOL CExcludePage::GetData(LPNAVOPTIONS pOptions)
{
    // Kill old copy of exclusions, and use new one.
    ExcludeKillCopy( &pOptions->exclude );
    ExcludeCreateCopy( &m_Exclude, &pOptions->exclude );

    return TRUE;
}

BOOL CExcludePage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    if(!SHOW_SIMPLE_EXCLUDE_PAGE)
        {
        CString s;
        GetDlgItem(IDC_EXCLUDE_STATIC)->GetWindowText(s);

        m_iMaxDisplayChars = s.GetLength();
        }

	// Need to populate list control with exclusions.
    PopulateList();

    // Set state of delete and edit buttons.
    return EnableButtons();

}

void CExcludePage::PopulateList()
{
    WORD wCount = ExcludeGetCount(&m_Exclude);
    WORD wFlags;
    BOOL bSubDirs;

    TCHAR szItemName[SYM_MAX_PATH + 1];
    TCHAR szLongName[SYM_MAX_PATH + 1];

    // Clear items from listbox.
    m_ListBox.ResetContent();

    for( WORD w = 0; w < wCount; w++ )
        {
        // Get item information and add to list
        if( ExcludeGet ( &m_Exclude,    // [in] returned from init.
                         w,             // [in] item index number to get.
                         szItemName,    // [out] name of item placed here.
                         &wFlags,       // [out] exclusions flags for this item
                         &bSubDirs ) )  // [out] flag to exclude sub dirs.
            {

            // only support exclusions for known virus detection.
            if(SHOW_SIMPLE_EXCLUDE_PAGE && wFlags != excVIRUS_FOUND )
                continue;

            // Get ANSI name - both for display and possible conversion to
            // LFN (if there IS and LFN, the SFN will have only valid
            // OEM to ANSI characters anyway.

            CString sItemName(szItemName);
            sItemName.OemToAnsi();

            // Make sure we display things correctly
            if( ExcludeIsValidSFN(szItemName) == STATUSEXCLUDE_OK )
                {
                 if (ExcludeSFN2LFN (sItemName.GetBuffer(0),
                                     szLongName,
                                     sizeof(szLongName)))
                    sItemName = szLongName;
                }

            // Add to LB.
            int iIndex = m_ListBox.AddString(sItemName);
            m_ListBox.SetItemData(iIndex, wFlags);
            }
        }

    // Select first item in list.
    m_ListBox.SetCurSel(0);
    SetScrollWidth();
    OnSelchangeExcludeList();

}

void CExcludePage::OnSelchangeExcludeList()
{
    // do nothing for NT platform.
    if(SHOW_SIMPLE_EXCLUDE_PAGE)
        return;

    static struct
        {
        int         iExcludeFlag;
        int         iExcludeText;
        } ExcludeToText[] =
           {
                {excVIRUS_FOUND,        IDS_EXCLUSIONS_KNOWN},
                {excLOW_FORMAT,         IDS_EXCLUSIONS_FORMAT},
                {excWR_HBOOT,           IDS_EXCLUSIONS_WRITEHARDBOOT},
                {excWR_FBOOT,           IDS_EXCLUSIONS_WRITEFLOPPYBOOT},
                {excWR_ATLIST,          IDS_EXCLUSIONS_WRITEEXECUTABLES},
                {excRO_CHANGE,          IDS_EXCLUSIONS_ROATTRIBCHANGE},
                {0,                     0}
           };

    CString sEditCtrlText;
    CString s;
    CString sFormat, sItemName;

    // Get flags from this item.  Remove exeVIOLATING flag since this is
    // a mask shared among several flags.
    int iIndex = m_ListBox.GetCurSel();
    int iFlags = m_ListBox.GetItemData(iIndex) & ~excVIOLATING;
    int iCount = m_ListBox.GetCount();

    if ( IS_NEC() )
        {
        // On NECs, force flags off for activities that we don't watch for.
        // Just a precaution - the user may bring over options files from an
        // AT install that has an exclusion for one of these activities, and
        // we shouldn't display, say "low level format" on a NEC system.

        iFlags &= ~ ( excLOW_FORMAT | excWR_HBOOT | excWR_FBOOT );
        }

    // Special case for empty listbox
    if( iCount == 0 )
        {
        sFormat.LoadString(IDS_EXCLUDE_PAGE_FORMAT);
        sItemName.Format(sFormat, "");
        GetDlgItem(IDC_EXCLUDE_STATIC)->SetWindowText(sItemName);
        GetDlgItem(IDC_EXCLUDE_EDIT)->SetWindowText("");
        return;
        }

    // Construct exclusion text.
    for ( int i = 0; ExcludeToText[i].iExcludeFlag; i++ )
        {
        if( iFlags & ExcludeToText[i].iExcludeFlag )
            {
            // Add this guy to our list of items.
            s.LoadString(ExcludeToText[i].iExcludeText);

            // Add CR/LF combo so the edit control doesn't get confused...
            sFormat.Format(_T("%s%c\n"), s, 0x0D);
            sEditCtrlText += sFormat;
            }
        }

    // Set groupbox text.
    LPTSTR szShortName = new TCHAR[m_iMaxDisplayChars + 1];
    sFormat.LoadString(IDS_EXCLUDE_PAGE_FORMAT);
    m_ListBox.GetText(iIndex, s);
    NameShortenFileName(s, szShortName, m_iMaxDisplayChars);
    sItemName.Format(sFormat, szShortName);
    GetDlgItem(IDC_EXCLUDE_STATIC)->SetWindowText(sItemName);

    delete [] szShortName;

    // Set exclude flags edit control text.
    GetDlgItem(IDC_EXCLUDE_EDIT)->SetWindowText(sEditCtrlText);

}

void CExcludePage::OnEditButton()
{
	if( PASSWORD_CHECK(PASSWORD_MODIEXCLUSIONS) == FALSE )
        return;


    EXCLUDEITEM ExcludeItem;
    UINT iExcludeIndex, iListBoxIndex;

    // Find the currently selected exclude item in our list box.
    if(GetSelectedExcludeItem(ExcludeItem, &iExcludeIndex, &iListBoxIndex) == FALSE)
        {
        CString sTitle, sText;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_ERROR_EDITING_EXCLUSION);
        MessageBox(sText, sTitle, MB_ICONEXCLAMATION | MB_OK);
        return;
        }

    // Create dialog and run with it.
    CExcludeEdit dlg;

    // Copy this item to the dialog.
    dlg.m_item = ExcludeItem;

    if(dlg.DoModal() != IDOK)
        return;

    // Record changes to this exclusion item.
    UINT uStatus = ExcludeEdit(&m_Exclude, &ExcludeItem, &dlg.m_item);

    // If something bad happened, let the user know.
    if(uStatus == FALSE)
        {
        CString sTitle, sText;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_ERROR_ADDING_EXCLUSION);
        MessageBox(sText, sTitle, MB_ICONEXCLAMATION | MB_OK);
        return;
        }

    // Update this list box item and call the OnSelChange message handler.
    PopulateList();
    m_ListBox.SetCurSel(iListBoxIndex);
    OnSelchangeExcludeList();
}

void CExcludePage::OnNewButton()
{
	if( PASSWORD_CHECK(PASSWORD_MODIEXCLUSIONS) == FALSE )
        return;

    // Create dialog and run with it.
    CExcludeEdit dlg;

    // Set new flag.
    dlg.m_bNew = TRUE;

    if(dlg.DoModal() != IDOK)
        return;

    // Add this item to our exclude list.
    UINT uStatus = ExcludeAdd(&m_Exclude, &dlg.m_item);

    // If something bad happened, let the user know.
    if(uStatus == FALSE)
        {
        CString sTitle, sText;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_ERROR_ADDING_EXCLUSION);
        MessageBox(sText, sTitle, MB_ICONEXCLAMATION | MB_OK);
        return;
        }

    // If this guy already exists, bail out.
    if(uStatus == EXCLUDE_DUPLICATE)
        return;

    // Add this new item to our list box.
    CString s = dlg.m_item.szText;

    // Convert to Ansi for display
    s.OemToAnsi();
    int iIndex = m_ListBox.AddString(s);
    m_ListBox.SetItemData(iIndex, dlg.m_item.wBits);
    m_ListBox.SetCurSel(iIndex);
    EnableButtons();

    SetScrollWidth();
    // Update onscreen display.
    OnSelchangeExcludeList();

}

void CExcludePage::OnRemoveButton()
{
	if( PASSWORD_CHECK(PASSWORD_MODIEXCLUSIONS) == FALSE )
        return;

    EXCLUDEITEM ExcludeItem;
    UINT iExcludeIndex, iListBoxIndex;

    // Find the currently selected exclude item in our list box.
    if( GetSelectedExcludeItem(ExcludeItem, &iExcludeIndex, &iListBoxIndex) == FALSE )
        {
        // bugbug - error message here.
        MessageBeep(0);
        return;
        }

    // Remove this exclusion.
    ExcludeDelete( &m_Exclude,  &ExcludeItem );

    // Remove items from listbox.
    m_ListBox.DeleteString((int)iListBoxIndex);

    if(iListBoxIndex >= (UINT)m_ListBox.GetCount())
        m_ListBox.SetCurSel(0);
    else
        m_ListBox.SetCurSel(iListBoxIndex);

    SetScrollWidth();

    // Set states of edit and delete buttons.
    EnableButtons();

    // Update text.
    OnSelchangeExcludeList();
}


BOOL CExcludePage::EnableButtons()
{
    int iCount = m_ListBox.GetCount();

    // Disable buttons if no items in listbox.
    BOOL bEnable = iCount > 0 && iCount != LB_ERR;

    GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(bEnable);
    GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(bEnable);

    // If we've disabled these guys, put focus on the
    // ADD button.
    if(!bEnable)
        GetDlgItem(IDC_NEW_BUTTON)->SetFocus();

    return bEnable;
}

BOOL CExcludePage::GetSelectedExcludeItem(EXCLUDEITEM & ExcludeItem, UINT *pIndex, UINT *pListBoxIndex)
{
    BOOL bRet = FALSE;

    // Get name from listbox.
    CString sFileUpper;
    CString sItemText;
    int iListBoxIndex = m_ListBox.GetCurSel();
    *pListBoxIndex = (UINT) iListBoxIndex;

    m_ListBox.GetText(iListBoxIndex, sFileUpper);

    // Convert to uppercase.
    sFileUpper.MakeUpper();

    for ( UINT uIndex = 0;
          uIndex < ExcludeGetCount(&m_Exclude);
          uIndex++ )
        {
        BOOL bDir = FALSE;

        BOOL bStatus = ExcludeGet (
                                &m_Exclude,
                                uIndex,
                                ExcludeItem.szText,
                                &ExcludeItem.wBits,
                                &bDir
                                );

        // Copy item text and perform same conversion that the
        // display name in the listbox got in PopulateList().
        sItemText = ExcludeItem.szText;
        sItemText.OemToAnsi();

        if ( bStatus == TRUE
            &&  ExcludeIsValidSFN ( ExcludeItem.szText ) == STATUSEXCLUDE_OK )
            {
            TCHAR szItemName[SYM_MAX_PATH +1];

            bStatus = ExcludeSFN2LFN (
                                sItemText.GetBuffer(0),
                                szItemName,
                                sizeof(szItemName)
                                );

            if ( bStatus == TRUE )
                {
                // Found our guy...Should already be in ANSI .
                // bugbug - is this true??
                sItemText = szItemName;
                }
            }
        sItemText.MakeUpper();

        // Now that we've mangled the names in a similar fashion
        // (LFN, ANSI, and upper) - check if they're equivalent.

        if ( sItemText == sFileUpper  )
            {
            // Found a match.  Bail out now.
            ExcludeItem.bSubDirectory = bDir;
            bRet = TRUE;
            *pIndex = uIndex;
            break;
            }
        else
            {
            ZeroMemory( &ExcludeItem, sizeof(ExcludeItem) );
            }
        }

     return bRet;
}


void CExcludePage::SetScrollWidth()
{
    CDC *pdc = m_ListBox.GetDC();
    CRect rect;
    int iMaxWidth = 0;
    int iCount = m_ListBox.GetCount();
    CString sText;

    for (int i = 0; i < iCount; i++)
        {
        m_ListBox.GetText(i, sText);
        pdc->DrawText(sText, -1, &rect, DT_LEFT | DT_CALCRECT);
        iMaxWidth = max( rect.Width(), iMaxWidth);
        }

    m_ListBox.SetHorizontalExtent( iMaxWidth );
}

void CExcludePage::OnDblclkExcludeList()
{
	// Call exclude edit code
    OnEditButton();
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDedit  - Dead Edit control used for displaying exclusion flags.

CDedit::CDedit()
{
}

CDedit::~CDedit()
{
}

BEGIN_MESSAGE_MAP(CDedit, CEdit)
	//{{AFX_MSG_MAP(CDedit)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDedit message handlers

void CDedit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return;
	//CEdit::OnLButtonDown(nFlags, point);
}

void CDedit::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;// TODO: Add your message handler code here and/or call default

	//CEdit::OnRButtonDown(nFlags, point);
}

void CDedit::OnMButtonDown(UINT nFlags, CPoint point)
{
	return;// TODO: Add your message handler code here and/or call default

	//CEdit::OnMButtonDown(nFlags, point);
}

void CDedit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return;
	//CEdit::OnMouseMove(nFlags, point);
}

BOOL CDedit::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CExcludeEdit dialog


CExcludeEdit::CExcludeEdit(CWnd* pParent /*=NULL*/)
: CHelperDialog(SHOW_SIMPLE_EXCLUDE_PAGE ? IDD_EXCLUDE_EDIT_NT :
                  IS_NEC() ? IDD_EXCLUDE_EDIT95_NEC : CExcludeEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExcludeEdit)
	m_DOSReadOnly = FALSE;
	m_bKnown = FALSE;
	m_bLowLevel = FALSE;
	m_bSubDir = FALSE;
	m_sFileName = _T("");
	m_bWriteFloppy = FALSE;
	m_bWriteHD = FALSE;
	m_bWriteProgram = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_ADD_EXCLUSION_DLG_HELP_BTN;
    m_bNew = FALSE;
    ZeroMemory(&m_item, sizeof(EXCLUDEITEM));

}


void CExcludeEdit::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExcludeEdit)
	DDX_Check(pDX, IDC_SUBDIR_CHECK, m_bSubDir);
	DDX_Text(pDX, IDC_TEXT_EDIT, m_sFileName);
	DDV_MaxChars(pDX, m_sFileName, SYM_MAX_PATH);
    DDX_Control(pDX, IDC_FILEOPEN_BUTTON, m_BrowseBtn);
	//}}AFX_DATA_MAP
    if(!SHOW_SIMPLE_EXCLUDE_PAGE)
        {
        if ( ! IS_NEC() )
            {
	        DDX_Check(pDX, IDC_LOWLEVEL_CHECK, m_bLowLevel);
	        DDX_Check(pDX, IDC_WRITE_FLOPPY_CHECK, m_bWriteFloppy);
	        DDX_Check(pDX, IDC_WRITE_HARD_CHECK, m_bWriteHD);
            }
        else
            {
            // On a NEC, force the flags off for activities that we don't
            // watch for (viz. formatting and MBR/BR writes).

            m_bLowLevel = m_bWriteFloppy = m_bWriteHD = FALSE;
            }

        DDX_Check(pDX, IDC_DOS_READONLY_CHECK, m_DOSReadOnly);
	    DDX_Check(pDX, IDC_KNOWN_CHECK, m_bKnown);
	    DDX_Check(pDX, IDC_WRITE_PROTRAM_CHECK, m_bWriteProgram);
        }
}


BEGIN_MESSAGE_MAP(CExcludeEdit, CHelperDialog)
	//{{AFX_MSG_MAP(CExcludeEdit)
	ON_BN_CLICKED(IDC_FILEOPEN_BUTTON, OnFileopenButton)
    ON_EN_CHANGE(IDC_TEXT_EDIT, OnChangeExclusionText)
    ON_BN_CLICKED(IDC_KNOWN_CHECK, OnClickedExclusionCheckbox)
    ON_BN_CLICKED(IDC_LOWLEVEL_CHECK, OnClickedExclusionCheckbox)
    ON_BN_CLICKED(IDC_WRITE_HARD_CHECK, OnClickedExclusionCheckbox)
    ON_BN_CLICKED(IDC_WRITE_FLOPPY_CHECK, OnClickedExclusionCheckbox)
    ON_BN_CLICKED(IDC_WRITE_PROTRAM_CHECK, OnClickedExclusionCheckbox)
    ON_BN_CLICKED(IDC_DOS_READONLY_CHECK, OnClickedExclusionCheckbox)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcludeEdit message handlers

BOOL CExcludeEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

    // Set icon for browse button.
    m_BrowseBtn.SetIconID( IDI_BROWSE );

    // Set window title.
    if(m_bNew)
        {
        CString s;
        s.LoadString(IDS_NEW_EXCLUSION);
        SetWindowText(s);
        }
    else
        {
        // Set up contols
        TCHAR szItemName[SYM_MAX_PATH + 1];

        // Text is in OEM at this point.  Need to convert.
        m_sFileName = m_item.szText;
        m_sFileName.OemToAnsi();

        // Make sure we show the long file name component of a short entry.
        if( ExcludeIsValidSFN ( m_item.szText ) == STATUSEXCLUDE_OK )
            {
            BOOL bStatus = ExcludeSFN2LFN (
                                m_item.szText,
                                szItemName,
                                sizeof(szItemName)
                                );

            // LFN should be in ANSI. Shouldnt have to convert.
            if ( bStatus == TRUE )
                {
                m_sFileName = szItemName;
                }

            }

        // Remove excVIOLATING bit, since this is a type mask shared between
        // several of these guys.
        m_item.wBits &= ~excVIOLATING;

        m_bSubDir           = (BOOL) (m_item.bSubDirectory) != 0;
        m_DOSReadOnly       = (BOOL) (m_item.wBits & excRO_CHANGE) != 0;
        m_bKnown            = (BOOL) (m_item.wBits & excVIRUS_FOUND) != 0;
        m_bLowLevel         = (BOOL) (m_item.wBits & excLOW_FORMAT) != 0;
        m_bWriteFloppy      = (BOOL) (m_item.wBits & excWR_FBOOT) != 0;
        m_bWriteHD          = (BOOL) (m_item.wBits & excWR_HBOOT) != 0;
        m_bWriteProgram     = (BOOL) (m_item.wBits & excWR_ATLIST) != 0;
        UpdateData(FALSE);
        }

    UpdateOKButtonStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CExcludeEdit::OnFileopenButton()
{
    // Get filename from user.
    CString sFilter;
    sFilter.LoadString(IDS_ALLFILES_FILTER);
    CFileDialog fileDlg(TRUE,
                        NULL,
                        m_sFileName,
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                        sFilter);
    CString sTitle;
    sTitle.LoadString(IDS_EXCLUDE_FILE_OPEN_TITLE);
    fileDlg.m_ofn.lpstrTitle = sTitle.GetBuffer(0);

    // Get the filename.
    if(fileDlg.DoModal() == IDOK)
        {
        UpdateData();
        m_sFileName = fileDlg.GetPathName();
        UpdateData(FALSE);
        UpdateOKButtonStatus();
        }

}

void CExcludeEdit::OnChangeExclusionText()
{
    UpdateOKButtonStatus();
}

void CExcludeEdit::OnClickedExclusionCheckbox()
{
    UpdateOKButtonStatus();
}


// This function enables the OK button if the exclusion edit box is
// not empty and at least one of the exclusion type checkboxes is checked.
// If neither of those conditions are true, OK is disabled.  This prevents
// the user from entering a blank exclusion, which NAV treats the same
// as *.* (scary!)
// On NT, just the filename is checked, since there are no exclusion
// type checkboxes.
// 11/5/97 Mike Dunn -- function created.

void CExcludeEdit::UpdateOKButtonStatus()
{
BOOL bEnable;

    UpdateData ( TRUE );

    // Remove leading and trailing spaces, so that an exclusion of
    // all spaces is rejected.

    m_sFileName.TrimRight();
    m_sFileName.TrimLeft();

    // Make sure the string has at least one nonspace char.

    bEnable = m_sFileName.GetLength() > 0;

    // On 95, check that at least one exclusion type checkbox is checked.

    if ( !SHOW_SIMPLE_EXCLUDE_PAGE )
        {
        bEnable = bEnable && ( m_bKnown    || m_bLowLevel || m_bWriteHD ||
                               m_bWriteFloppy || m_bWriteProgram ||
                               m_DOSReadOnly );
        }

    GetDlgItem ( IDOK ) -> EnableWindow ( bEnable );
}

void CExcludeEdit::OnOK()
{
	// TODO: Add extra validation here
    UpdateData();

    // Remove those unsightly extra spaces off the front & end of the
    // filename.  This works around the error that pops up if you type
    // in, say, " c:\*.com" as the exclusion (with leading space).

    m_sFileName.TrimLeft();
    m_sFileName.TrimRight();


    // Check for valid exclusion name.
    if(!IsExcludeNameValid())
        {
        CString sText, sTitle;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_EXCLUDE_EDIT_ERROR);
        MessageBox(sText, sTitle, MB_ICONSTOP | MB_OK);
        GetDlgItem(IDC_TEXT_EDIT)->SetFocus();
        ((CEdit*)GetDlgItem(IDC_TEXT_EDIT))->SetSel(0, -1);
        return;
        }

    // Need to convert back to OEM.
    m_sFileName.AnsiToOem();

    // Gather data from controls
    _tcscpy(m_item.szText, m_sFileName);
    m_item.bSubDirectory = m_bSubDir;

    // If running under NT, we only allow known virus exclusions.
    if(SHOW_SIMPLE_EXCLUDE_PAGE)
        {
        m_item.wBits = excVIRUS_FOUND;
        }
    else
        {
        m_item.wBits = 0;
        m_item.wBits |= (WORD) m_bKnown ? excVIRUS_FOUND : 0;
        m_item.wBits |= (WORD) m_DOSReadOnly ? excRO_CHANGE : 0;
        m_item.wBits |= (WORD) m_bLowLevel ? excLOW_FORMAT : 0;
        m_item.wBits |= (WORD) m_bWriteFloppy ? excWR_FBOOT : 0;
        m_item.wBits |= (WORD) m_bWriteHD ? excWR_HBOOT : 0;
        m_item.wBits |= (WORD) m_bWriteProgram ? excWR_ATLIST : 0;

        // Must be excluded from at least one activity.
        if ( 0 == m_item.wBits )
            {
            CString sText, sTitle;

            sTitle.LoadString(IDS_ERROR_TITLE);
            sText.LoadString(IDS_EXCLUDE_EDIT_ERROR_NO_BITS);

            MessageBox(sText, sTitle, MB_ICONSTOP | MB_OK);

            GetDlgItem(IDC_KNOWN_CHECK)->SetFocus();

            return;
            }
        }

    CDialog::OnOK();
}



BOOL CExcludeEdit::IsExcludeNameValid()
{
    // Check for UNC path.  UNC paths are not supported.
    if(m_sFileName.Find(_T("\\\\")) != -1)
        return FALSE;

    // Make sure we don't have an extension with more
    // than 3 characters.
    TCHAR szExt[_MAX_PATH];
    ZeroMemory( szExt, sizeof(TCHAR) * _MAX_PATH);
    NameReturnExtensionEx( m_sFileName,
                           szExt,
                           _MAX_PATH );

    if( lstrlen( szExt ) > 3 )
        return FALSE;

    return TRUE;
}
