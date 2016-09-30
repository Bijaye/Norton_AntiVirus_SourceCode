/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/PASSPAGE.CPv   1.10   19 May 1998 13:10:28   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
//
// PassPage.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/PASSPAGE.CPv  $
// 
//    Rev 1.10   19 May 1998 13:10:28   DBuches
// Fixed misc password page problems.
// 
//    Rev 1.9   10 Apr 1998 00:27:36   MKEATIN
// Made inoculation related changes.
// 
//    Rev 1.7   19 Feb 1998 11:57:14   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.1   12 Oct 1997 19:05:00   mdunn
// Added context help to the Set Password button.
// 
//    Rev 1.0   12 Sep 1997 17:44:50   RFULLER
// Initial revision
// 
//    Rev 1.6   11 Jul 1997 12:30:20   DBUCHES
// Set horizontal scroll width of listbox correctly.
// 
//    Rev 1.5   24 Jun 1997 15:59:40   DBUCHES
// Removed unused password items.
// 
//    Rev 1.4   20 Jun 1997 12:03:56   DBUCHES
// Added help support.
// 
//    Rev 1.3   11 Jun 1997 14:05:10   DALLEE
// Removed redefinition of WM_PASSWORD_CHECK. Define is now in SHEET.H.
// 
//    Rev 1.2   10 Jun 1997 15:22:52   DBUCHES
// Added startup password stuff.
// 
//    Rev 1.1   27 May 1997 20:56:50   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 14:58:20   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "PassPage.h"
#include "sheet.h"
#include "ngui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Local data

    static UINT uList[] =
      {
      IDS_VIEW_REPORTS,
      IDS_CLEAR_LOG,
      IDS_VIRUS_LIST,
      //IDS_DELETE_VIRUS_LIST,          // no longer supported.
      IDS_VIEW_SCANNER,
      IDS_VIEW_AP,
      IDS_VIEW_ALERT,
      IDS_VIEW_EXCLUDE,
      IDS_EDIT_EXCLUDE,
      IDS_VIEW_LOG_SETTINGS,
      IDS_VIEW_INOC_SETTINGS,
      IDS_VIEW_GENERAL,
      IDS_VIEW_STARTUP,
      0
      };


    static UINT uListNT[] =
      {
      IDS_VIEW_REPORTS,
      IDS_CLEAR_LOG,
      IDS_VIRUS_LIST,
      //IDS_DELETE_VIRUS_LIST,
      IDS_VIEW_SCANNER,
      IDS_VIEW_AP,
      IDS_VIEW_ALERT,
      IDS_VIEW_EXCLUDE,
      IDS_EDIT_EXCLUDE,
      IDS_VIEW_LOG_SETTINGS,
      //IDS_VIEW_INOC_SETTINGS          // Non-existant on NT platform
      IDS_VIEW_GENERAL,
      0
      };


   static UINT uLBIndexMapList[] =
      {
      PASSWORD_VIEWACTIVITY,    // IDS_VIEWREPS,
      PASSWORD_CLEARLOG,        // IDS_CLEARLOG,
      PASSWORD_VIEWVIRUSLIST,   // IDS_VIEWVIRLIST,
      //PASSWORD_MODIVIRUSLIST,   // IDS_MODIVIRLIST,
      PASSWORD_VIEWSCANOPT,     // IDS_VIEWSCAN,
      PASSWORD_VIEWTSROPT,      // IDS_VIEWTSR,
      PASSWORD_VIEWALERTOPT,    // IDS_VIEWALERT,
      PASSWORD_VIEWEXCLOPT,     // IDS_VIEWEXCL,
      PASSWORD_MODIEXCLUSIONS,  // IDS_MODIEXCL,
      PASSWORD_VIEWACTIVITYOPT, // IDS_VIEWLOG,
      PASSWORD_VIEWINOCOPT,     // IDS_VIEWINOC,
      PASSWORD_VIEWGENOPT,      // IDS_VIEWGEN
      PASSWORD_VIEWSTARTUPOPT,  // IDS_VIEW_STARTUP
      0xFFFFFFFF
      };

   static UINT uLBIndexMapListNT[] =
      {
      PASSWORD_VIEWACTIVITY,    // IDS_VIEWREPS,
      PASSWORD_CLEARLOG,        // IDS_CLEARLOG,
      PASSWORD_VIEWVIRUSLIST,   // IDS_VIEWVIRLIST,
      //PASSWORD_MODIVIRUSLIST,   // IDS_MODIVIRLIST,
      PASSWORD_VIEWSCANOPT,     // IDS_VIEWSCAN,
      PASSWORD_VIEWTSROPT,      // IDS_VIEWTSR,
      PASSWORD_VIEWALERTOPT,    // IDS_VIEWALERT,
      PASSWORD_VIEWEXCLOPT,     // IDS_VIEWEXCL,
      PASSWORD_MODIEXCLUSIONS,  // IDS_MODIEXCL,
      PASSWORD_VIEWACTIVITYOPT, // IDS_VIEWLOG,
      //PASSWORD_VIEWINOCOPT,     // IDS_VIEWINOC,
      PASSWORD_VIEWGENOPT,      // IDS_VIEWGEN
      0xFFFFFFFF
      };


/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CPasswordPage::m_dwWhatsThisMap[] = { 
    IDC_PASSWORD_CHECK,     IDH_NAVW_WHATHIS_PASSWORD_PROTECT,
    IDC_MAXIMUM_RADIO,      IDH_NAVW_WHATHIS_MAXIMUM_PASSWORD_PROTECTION,
    IDC_CUSTOM_RADIO,       IDH_NAVW_WHATHIS_CUSTOM_PASSWORD_PROTECTION,
    IDC_LIST,               IDH_NAVW_WHATHIS_PASSWORD_LSTBOX,
    IDC_SETPW_BUTTON,       IDH_NAVW_WHATHIS_PASSWORD_SETUP_BROWSE_BTN,
    0,0 
    };
DWORD CPasswordPage::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// CPasswordPage dialog
CPasswordPage::CPasswordPage() : CHelperPage(CPasswordPage::IDD)
{
	//{{AFX_DATA_INIT(CPasswordPage)
	m_iMaximum = -1;
	m_bPassword = FALSE;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_PASSWORD_SETTINGS_DLG_HELP_BTN;
}

CPasswordPage::~CPasswordPage()
{
}


void CPasswordPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordPage)
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Radio(pDX, IDC_MAXIMUM_RADIO, m_iMaximum);
	DDX_Check(pDX, IDC_PASSWORD_CHECK, m_bPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordPage, CHelperPage)
	//{{AFX_MSG_MAP(CPasswordPage)
	ON_BN_CLICKED(IDC_PASSWORD_CHECK, OnPasswordCheck)
	ON_BN_CLICKED(IDC_MAXIMUM_RADIO, OnMaximumRadio)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
    ON_BN_CLICKED(IDC_CUSTOM_RADIO, OnMaximumRadio)
	ON_BN_CLICKED(IDC_SETPW_BUTTON, OnSetpwButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordPage message handlers

BOOL CPasswordPage::SetData(LPNAVOPTIONS pOptions)
{
    LPPASWOPT pPwOpts = &pOptions->password;

    m_bPassword = pPwOpts->bProtect;
    m_iMaximum = !pPwOpts->bMaxProtection;

    m_dwPassword = pPwOpts->dwPassword;

    CopyMemory(m_abProtectFeature, pPwOpts->bProtectFeature, MAX_PROTECT_FEATURE);

    return TRUE;

}

BOOL CPasswordPage::GetData(LPNAVOPTIONS pOptions)
{
    LPPASWOPT pPwOpts = &pOptions->password;

    pPwOpts->bProtect = m_bPassword;
    pPwOpts->bMaxProtection = !m_iMaximum;

    if( m_bPassword )
        pPwOpts->dwPassword = m_dwPassword;
    else
        pPwOpts->dwPassword = 0;

    CopyMemory(pPwOpts->bProtectFeature, m_abProtectFeature, MAX_PROTECT_FEATURE);

    return TRUE;

}

BOOL CPasswordPage::OnInitDialog()
{
	CHelperPage::OnInitDialog();

    // Fill list with strings
    PopulateList();

    // Update our selection array.
    UpdateSelections(FALSE);

    // Set states of controls
    OnPasswordCheck();
    
    // If password protected, display password now.
    if(m_bPassword)
        PostMessage(WM_PASSWORD_CHECK, -1, 0L);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordPage::OnPasswordCheck()
{
    UpdateData();
    BOOL bEnable = IsDlgButtonChecked(IDC_PASSWORD_CHECK);

    // Enable everybody
    m_ListBox.EnableWindow( (BOOL) m_iMaximum && bEnable);
    GetDlgItem(IDC_CUSTOM_RADIO)->EnableWindow(bEnable);
    GetDlgItem(IDC_MAXIMUM_RADIO)->EnableWindow(bEnable);
    GetDlgItem(IDC_SETPW_BUTTON)->EnableWindow(bEnable);
    HighlightSelectedItems(bEnable);

}

void CPasswordPage::UpdateSelections(BOOL bUpdate)
{
    int i;
    int iCount = m_ListBox.GetCount();
    UINT * pList;

    if(bUpdate)
        {
        // Get pointer to correct list.
        pList = IS_NT() ? uListNT : uList;

        // copy selected items from list to array.
        ZeroMemory(m_abProtectFeature, sizeof(m_abProtectFeature));
        for(i = 0; pList[i]; i++)
            {
            int iSelected = (int) m_ListBox.GetItemData(i);
            m_abProtectFeature[MapItemToFeature( i )] = (BYTE) iSelected;
            }
        }
    else
        {
        // Get pointer to correct list.
        pList = IS_NT() ? uLBIndexMapListNT : uLBIndexMapList;

        // Copy selected items from array to list.
        for(i = 0; pList[i] != 0xFFFFFFFF; i++)
            {
            if(m_abProtectFeature[pList[i]])
                m_ListBox.SetItemData(i,TRUE);
            else
                m_ListBox.SetItemData(i,FALSE);
            }
        }

}

void CPasswordPage::PopulateList()
{
    // Get pointer to correct list.
    UINT * pList = IS_NT() ? uListNT : uList;

    // Populate list box with password protect items.
    while( *pList )
        {
        CString s;
        s.LoadString(*pList);
        m_ListBox.AddString(s);
        pList++;
        }

    // Set up correct scroll width.
    SetScrollWidth();

}

UINT CPasswordPage::MapItemToFeature(int iIndex)
{
    // Get pointer to correct list.
    UINT * pList = IS_NT() ? uLBIndexMapListNT : uLBIndexMapList;

    ASSERT(iIndex <= MAX_PROTECT_FEATURE);

    return pList[iIndex];

}


void CPasswordPage::HighlightSelectedItems(BOOL bEnable)
{
    UpdateData();

    // If not enabled, remove highlight from selected items.
    if(!bEnable)
        {
        m_ListBox.SetSel(-1, FALSE);
        return;
        }

    if(!m_iMaximum)
        {
        m_ListBox.SetSel(-1, TRUE);
        return;
        }

    // We are enabled, so light ourselvs up.
    int iCount = m_ListBox.GetCount();
    for(int i=0; i < iCount; i++)
        m_ListBox.SetSel(i, m_ListBox.GetItemData(i));

}

void CPasswordPage::OnMaximumRadio()
{
	if(IsDlgButtonChecked(IDC_MAXIMUM_RADIO))
        {
        // Disable the window, but select all items.
        m_ListBox.EnableWindow(FALSE);
        m_ListBox.SetSel(-1, TRUE);
        }
    else
        {
        // Enable the window, and show selected items.
        m_ListBox.EnableWindow(TRUE);
        HighlightSelectedItems(TRUE);
        }

}

void CPasswordPage::OnSelchangeList()
{
    int iNumItems = m_ListBox.GetCount();

    // Clear all selections.
    for(int i = 0; i < iNumItems; i++)
        m_ListBox.SetItemData(i, FALSE);

    // Flag selected items.
    int aSelected[MAX_PROTECT_FEATURE];
    int iCount = m_ListBox.GetSelItems(MAX_PROTECT_FEATURE, aSelected);
    for(i = 0; i < iCount; i++)
        m_ListBox.SetItemData(aSelected[i], TRUE);

    UpdateSelections(TRUE);
}

void CPasswordPage::OnSetpwButton()
{
    auto    LPPASSWORDSTRUCT        lpPass;

    lpPass = new PASSWORDSTRUCT;
    ZeroMemory(lpPass, sizeof(PASSWORDSTRUCT));

    lpPass->dwSize                 = sizeof(PASSWORDSTRUCT);
    lpPass->hParent                = GetSafeHwnd();
    lpPass->dwPassword             = m_dwPassword;
    lpPass->uMaxCharsToAccept      = 16;
    lpPass->uHelpID                = IDH_NAVW_CHANGE_PASSWORD_DLG_HELP_BTN;
    lpPass->bCaseSensitive         = FALSE;
    lpPass->bOemConvert            = TRUE;

    BOOL bRet = PasswordChange (lpPass);

    if ( lpPass->dwPassword == NO_PASSWORD || !bRet)
        {
        // No password.
        if( m_dwPassword == 0 )
            {
            CheckDlgButton(IDC_PASSWORD_CHECK, FALSE);
            OnPasswordCheck();
            }
        }
    else
        m_dwPassword = lpPass->dwPassword;

    delete lpPass;


}

BOOL CPasswordPage::OnSetActive()
{
    BOOL bRet = CPropertyPage::OnSetActive();

    // If we have been disabled because the user did not enter the correct
    // password, give him another chance.
    if( m_bPassDisabled )
        {
        if( ((CNavOptionsSheet*)GetParent())->PasswordCheck( -1 ))
            {
            EnableChildWindows(TRUE);
            m_bPassDisabled = FALSE;
            }
        }

    return bRet;
}


void CPasswordPage::SetScrollWidth()
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

