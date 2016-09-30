/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/sheet.cpv   1.8   18 Nov 1997 20:50:30   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
//
// Sheet.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/sheet.cpv  $
// 
//    Rev 1.8   18 Nov 1997 20:50:30   jtaylor
// Removed scanonly code
// 
//    Rev 1.6   20 Jun 1997 12:03:50   DBUCHES
// Added help support.
// 
//    Rev 1.5   10 Jun 1997 20:19:42   DBUCHES
// Added changes for alert password.
// 
//    Rev 1.4   10 Jun 1997 15:23:02   DBUCHES
// Added help button.
// 
//    Rev 1.3   27 May 1997 22:20:14   DBUCHES
// Added SaveNTAlertOptions.
// 
//    Rev 1.2   27 May 1997 20:57:18   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   18 May 1997 23:24:44   DBUCHES
// Made a global CIncludeDlg member.  This was needed because the
// file inclusion extension list may be changed from more than one
// property page.
//
//    Rev 1.0   14 May 1997 14:58:22   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "Sheet.h"
// page header files
#include "scanpage.h"   // Scanner Page
#include "appage.h"     // AutoProtect Page
#include "startpge.h"   // Startup Page
#include "alertpge.h"   // Alert Page
#include "logpage.h"    // Activity Log Page
#include "exclpage.h"   // Exclusions Page
#include "genpage.h"    // General Page
#include "inocpage.h"   // Inoculation Page
#include "passpage.h"   // Password page.

#include "n32behav.h"

// for password support.
#include "ngui.h"

// for NT alerting
#include "at_uiexp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavOptionsSheet

IMPLEMENT_DYNAMIC(CNavOptionsSheet, CPropertySheet)

CNavOptionsSheet::CNavOptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    Init();
}

CNavOptionsSheet::CNavOptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

    Init();
}

CNavOptionsSheet::~CNavOptionsSheet()
{
    DeletePages();
}


BEGIN_MESSAGE_MAP(CNavOptionsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNavOptionsSheet)
    ON_MESSAGE( WM_PASSWORD_CHECK, OnPasswordCheckMessage )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavOptionsSheet message handlers

void CNavOptionsSheet::Init()
{
    // Set some reasonable default values.
    m_pPages = NULL;
    ZeroMemory(&navOpts, sizeof(NAVOPTIONS));

    m_bPasswordValid = FALSE;
    m_dwPassword = 0;

    // Remove Apply now button.
    m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_HASHELP;

}


int CNavOptionsSheet::InsertPages(int iNumPages, int* piPageIDs)
{
    // Make sure we don't over-allocate
    if( iNumPages > N32OPTS_MAXPAGES )
        {
        ASSERT(iNumPages <= N32OPTS_MAXPAGES);
        return -1;
        }

   
    // Copy data
    m_iNumPages = iNumPages;
    CopyMemory(&m_iPageIDs, piPageIDs, iNumPages * sizeof(int));

    // Alloc array of pointers to property pages.
    m_pPages = new CPropertyPage*[m_iNumPages];
    ZeroMemory(m_pPages, sizeof(CPropertyPage*) * m_iNumPages);


    // Allocate the actual pages.
    for(int i=0; i < m_iNumPages; i++)
        {
        // Allocate the page.
        m_pPages[i] = AllocPage(i);
        if( m_pPages[i] == NULL )
            {
            ASSERT(FALSE);
            return ERROR_OUTOFMEMORY;
            }

        // Add the page to the property sheet.
        AddPage(m_pPages[i]);

        // Don't init Server page.  Not needed
        if( m_iPageIDs[i] == N32OPTS_ALERT_PAGE && 
                NAV_IS_SERVER() && IS_NT() )
            continue;
        
        // Init the page data members.
        ((CHelperPage*)m_pPages[i])->SetData(&navOpts);
        }
    
    
    // Set up inclusion dialog.
    m_IncludeDlg.m_sExtensions = navOpts.general.szExt;
    m_IncludeDlg.m_sDefaultExtensions = navOpts.general.szDef;


    return ERROR_SUCCESS;
}

void CNavOptionsSheet::DeletePages()
{
    if(m_pPages)
        {
        // delete each page
        for(int i=0; i < m_iNumPages; i++)
            delete m_pPages[i];

        // delete array of pointers
        delete [] m_pPages;
        m_pPages = NULL;
        }
}


CPropertyPage * CNavOptionsSheet::AllocPage(int iPage)
{
    CPropertyPage *pRet = NULL;

    // Find the correct page
    switch(m_iPageIDs[iPage])
        {
        // Add new page types here.
        case N32OPTS_SCANNER_PAGE:  pRet = new CScannerPage; break;
        case N32OPTS_AP_PAGE:       pRet = new CAutoProtectPage; break;
        case N32OPTS_STARTUP_PAGE:  pRet = new CStartupPage; break;
        case N32OPTS_LOG_PAGE:      pRet = new CLogPage; break;
        case N32OPTS_EXCLUDE_PAGE:  pRet = new CExcludePage; break;
        case N32OPTS_GENERAL_PAGE:  pRet = new CGeneralPage; break;
        case N32OPTS_PASSWORD_PAGE: pRet = new CPasswordPage; break;
        case N32OPTS_INOC_PAGE:     pRet = new CInocPage; break;
        case N32OPTS_ALERT_PAGE:    
            // If we are the server version, allocate a server alert page.
            // else allocate a normal alerts page.
            if( NAV_IS_SERVER() && IS_NT())
                pRet = CreateServerAlertPage();
            else
                pRet = new CAlertsPage; 
            break;
        }

    ASSERT(pRet);
    return pRet;
}


BOOL CNavOptionsSheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

    // Add "?" to title bar.
    ModifyStyleEx(0,WS_EX_CONTEXTHELP);

    ((CN32optsApp*)AfxGetApp())->m_pOptionsWindow = this;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CNavOptionsSheet::GatherData()
{
    // Loop thru each page, updating NAVOPTIONS as we go.
    for(int i=0; i < m_iNumPages; i++)
        {
        // Init the page data members.
        if( m_pPages[i] )
            {
            if( m_iPageIDs[i] == N32OPTS_ALERT_PAGE && 
                NAV_IS_SERVER() && IS_NT() )
                {
                // Save off server alert data.
                SaveNTServerAlerts();
                }
            else
                ((CHelperPage*)m_pPages[i])->GetData(&navOpts);
            }
        }

    // Save inclusion dialog data.
    FillMemory(navOpts.general.szExt, SIZE_EXTENSION_LIST + 1, _T(' '));
    _tcscpy(navOpts.general.szExt, m_IncludeDlg.m_sExtensions);

}

BOOL CNavOptionsSheet::PasswordCheck(int iFeature)
{
    // Check to see if user has already been here...
    if(m_bPasswordValid)
        return TRUE;

    if( iFeature > MAX_PROTECT_FEATURE && iFeature != -1)
        return TRUE;

    // -1 siganls that we want the user prompted for a password.
    if(iFeature != -1)
        {
        // Is this item password protected?
        if( navOpts.password.bProtectFeature[iFeature] == FALSE &&
            navOpts.password.bMaxProtection == FALSE )
            return TRUE;
        }

    // Veryfy password from user.
    LPPASSWORDSTRUCT lpPass;
    lpPass = new PASSWORDSTRUCT;
    ZeroMemory(lpPass, sizeof(PASSWORDSTRUCT));

    lpPass->dwSize            = sizeof(PASSWORDSTRUCT);
    lpPass->hParent           = GetSafeHwnd();
    lpPass->dwPassword        = m_dwPassword;
    lpPass->uMaxCharsToAccept = 16;
    lpPass->bCaseSensitive    = FALSE;
    lpPass->bOemConvert       = TRUE;
    lpPass->uHelpID           = IDH_NAVW_VERIFY_PASSWORD_DLG_HELP_BTN;

    m_bPasswordValid = PasswordVerify(lpPass);

    delete lpPass;

    return m_bPasswordValid;

}


CPropertyPage * CNavOptionsSheet::CreateServerAlertPage()
{
    CPropertyPage* pRet = NULL;    

    // Get proc addresses of exported functions.
    LPFNINITALERTEXTDLL pInitDll = (LPFNINITALERTEXTDLL)GetProcAddress(ALERTUI_INSTANCE(), "InitAlertExtDll");    
    ASSERT(pInitDll);
    LPFNCREATEALERTOPTSDLG pCreatePage = (LPFNCREATEALERTOPTSDLG)GetProcAddress(ALERTUI_INSTANCE(), "CreateAlertOptsDlg");
    ASSERT(pCreatePage);

    if(ALERTUI_INSTANCE() == NULL ||
       pInitDll == NULL || 
       pCreatePage == NULL )
        {
        // Tell the user that something bad happened.
        CString sTitle, sText;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_CANT_LOAD_ALERTUI);
        MessageBox(sText, sTitle, MB_ICONSTOP | MB_OK);
        return NULL;
        }

    // Call the init function
    pInitDll();

    // Create the page.
    pRet = (CPropertyPage*) pCreatePage(&navOpts);
    return pRet;

}

void CNavOptionsSheet::SaveNTServerAlerts()
{

    LPFNSAVEALERTOPTS pSaveOpts = (LPFNSAVEALERTOPTS)GetProcAddress(ALERTUI_INSTANCE(), "SaveAlertOptions");    
    ASSERT(pSaveOpts);

    if( ALERTUI_INSTANCE() == NULL ||
        pSaveOpts == NULL )
        {
        // Tell the user that something bad happened.
        CString sTitle, sText;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sText.LoadString(IDS_CANT_LOAD_ALERTUI);
        MessageBox(sText, sTitle, MB_ICONSTOP | MB_OK);
        return;
        }
    
    pSaveOpts();
}


afx_msg LONG CNavOptionsSheet::OnPasswordCheckMessage( WPARAM wParam, LPARAM lParam )
    {

    // Perform password check.
    return PasswordCheck((int) wParam);
    }
