/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/CINCLUDE.CPv   1.5   09 Jul 1998 16:49:14   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// CINCLUDE.CPP - Contains CInclude class implementation.
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
// $Log:   S:/N32OPTS/VCS/CINCLUDE.CPv  $
// 
//    Rev 1.5   09 Jul 1998 16:49:14   DBuches
// Fixed help.
// 
//    Rev 1.4   02 Jul 1998 15:21:06   mdunn
// Fixed breakages on help IDs for build 22
// 
//    Rev 1.3   20 Jun 1997 12:00:58   DBUCHES
// Added Help support.
// 
//    Rev 1.2   27 May 1997 20:55:28   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   21 May 1997 14:36:14   DBUCHES
// Fixed problem with cancel button.  Really cancels now.
//
//    Rev 1.0   14 May 1997 14:58:10   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "cinclude.h"

/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CIncludeDlg::m_dwWhatsThisMap[] = { 
    IDC_NEW_BUTTON,     IDH_NAVW_WHATHIS_PROGRAM_FILE_EXTENSIONS_ADD_BTN,
    IDC_REMOVE_BUTTON,  IDH_NAVW_WHATHIS_PROGRAM_FILE_EXTENSIONS_DELETE_BTN,
    IDC_DEFAULT_BUTTON, IDH_NAVW_WHATHIS_PROGRAM_FILE_EXTENSIONS_DEFAULT_BTN,
    0,0 
    };
DWORD CIncludeDlg::m_dwHowToMap[] = { 0,0 };

/////////////////////////////////////////////////////////////////////////////
// CIncludeDlg dialog
CIncludeDlg::CIncludeDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CIncludeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIncludeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_WHATHIS_FILE_EXTENSIONS_LIST_DLG_HELP_BTN;
}


void CIncludeDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIncludeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIncludeDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CIncludeDlg)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	ON_BN_CLICKED(IDC_DEFAULT_BUTTON, OnDefaultButton)
	ON_BN_CLICKED(IDC_NEW_BUTTON, OnNewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIncludeDlg message handlers


BOOL CIncludeDlg::OnInitDialog()
{
	CHelperDialog::OnInitDialog();

    // Initialize our list of extensions.
    ext.Init(m_sExtensions, m_bScanNoExtFile);

    // Populate list.
    PopulateList();

    // Set the checkbox for scanning files with no extension
    CButton *pButton = (CButton*)GetDlgItem(IDC_NOEXTENSION);
    pButton->SetCheck(m_bScanNoExtFile);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIncludeDlg::OnRemoveButton()
{
    // Get listbox
    CListBox *pList = (CListBox*) GetDlgItem(IDC_EXTENSION_LIST);
    int index = pList->GetCurSel();

    CString sTemp, sItem;
    pList->GetText(index, sTemp);

    // Remove leading period.
    sItem = (LPTSTR) (sTemp.GetBuffer(0) + 1);

    // Remove item from list
    ext.RemoveItem(sItem);

    // Remove item from listbox.
    pList->DeleteString(index);

    // Set focus back to list.
    pList->SetCurSel(index);
    pList->SetFocus();


}

void CIncludeDlg::OnDefaultButton()
{
	// Use default extensions and repopulate
    ext.Init(m_sDefaultExtensions, m_bScanNoExtFile);

    PopulateList();

}

void CIncludeDlg::PopulateList()
{
    // Populate list control with extensions.
    CListBox *pList = (CListBox*) GetDlgItem(IDC_EXTENSION_LIST);
    pList->ResetContent();

    CString sFormat;

    POSITION tPos = ext.m_extItems.GetHeadPosition();

    while(tPos != NULL)
        {
        CString s = ext.m_extItems.GetNext(tPos);

        sFormat.Format(_T(".%s"),s);

        pList->AddString(sFormat);
        }

    pList->SetCurSel(0);

}


void CIncludeDlg::OnNewButton()
{
	CNewExtDlg dlg;

    // Send in a pointer to the extension list so
    // we can perform validation operations.
    dlg.pExt = &ext;

    if(dlg.DoModal() != IDOK)
        return;

    // Add item to our list
    ext.AddItem(dlg.m_sExtText);

    // Add item to list box
    CListBox *pList = (CListBox*) GetDlgItem(IDC_EXTENSION_LIST);
    CString sFormat;
    sFormat.Format(_T(".%s"),dlg.m_sExtText);
    int index = pList->AddString(sFormat);
    pList->SetCurSel(index);

}

void CIncludeDlg::OnOK()
{
	// Build list of extensions.
    m_sExtensions.Empty();

    POSITION tPos = ext.m_extItems.GetHeadPosition();

    while(tPos != NULL)
        {
        CString s = ext.m_extItems.GetNext(tPos);
        m_sExtensions += s;
        m_sExtensions += _T(" ");
        }

    // Get the state of the no extension checkbox
    CButton *pButton = (CButton*)GetDlgItem(IDC_NOEXTENSION);
    m_bScanNoExtFile = pButton->GetCheck();

	// Set the flag indicating that options changed to TRUE
	m_bOptionsChanged = TRUE;

	CHelperDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CNewExtDlg dialog


CNewExtDlg::CNewExtDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CNewExtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewExtDlg)
	m_sExtText = _T("");
	//}}AFX_DATA_INIT
    m_dwHelpButtonHelpID = IDH_NAVW_NEW_FILE_EXTENSIONS_DLG_HELP_BTN;
}


void CNewExtDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewExtDlg)
	DDX_Text(pDX, IDC_EDIT1, m_sExtText);
	DDV_MaxChars(pDX, m_sExtText, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewExtDlg, CDialog)
	//{{AFX_MSG_MAP(CNewExtDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewExtDlg message handlers


void CNewExtDlg::OnOK()
{
	// Get data
    UpdateData();
    m_sExtText.MakeUpper();

    // Make sure data is valid
    int iRet = pExt->IsItemValid(m_sExtText);
    int iErrorID = 0;

    switch(iRet)
        {
        case ITEM_INVALID_NAME:
            iErrorID = IDS_ERROR_INVALID_EXT;
            break;

        case ITEM_DUPLICATE_NAME:
            iErrorID = IDS_ERROR_DUPLICATE_EXT;
            break;
        }

    if(iErrorID)
        {
        // Display an error dialog informing the user of the
        // error of his ways...
        CString sErrorText, sTitle;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sErrorText.LoadString(iErrorID);
        MessageBox(sErrorText, sTitle, MB_ICONSTOP | MB_OK);

        // Set focus to edit control.
        CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
        pEdit->SetFocus();
        pEdit->SetSel(0, -1);
        return;
        }



	CHelperDialog::OnOK();
}

