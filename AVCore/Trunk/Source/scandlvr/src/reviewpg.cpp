/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/reviewpg.cpv   1.24   28 Jul 1998 13:37:10   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// ReviewPg.cpp: implementation of the CReviewPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/reviewpg.cpv  $
// 
//    Rev 1.24   28 Jul 1998 13:37:10   jtaylor
// Added a final panel for all false positives.
// 
//    Rev 1.23   27 Jul 1998 14:04:00   mdunn
// Fixed 118634 - now displaying the summary text in a list control so it
// can scroll and accomodate big filenames.
//
//    Rev 1.22   16 Jul 1998 17:18:00   jtaylor
// Updated the string that is printed in the filelist.  This allows multiple files to be formatted nicely.
//
//    Rev 1.21   26 Jun 1998 15:08:36   SEDWARD
// Added 'OnSetActive' so we can refresh the summary text.
//
//    Rev 1.20   10 Jun 1998 23:21:48   jtaylor
// Added a function called TriggerAutodial which tries to establish and email connection to the SARC mailserver in order to trigger the IE4 AutoDial feature.
//
//    Rev 1.19   10 Jun 1998 21:50:44   jtaylor
// Added support for enhanced network connection detection and retry connection dialog.
//
//    Rev 1.18   08 Jun 1998 23:26:16   SEDWARD
// Started working on 'SetSummaryText'.
//
//    Rev 1.16   28 May 1998 18:28:36   jtaylor
// Added commented out support for AutoDial.
//
//    Rev 1.15   27 May 1998 21:36:10   SEDWARD
// Reset final wizard panel text if SARC APIs fail.
//
//    Rev 1.14   26 May 1998 18:28:08   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.13   25 May 1998 13:50:40   SEDWARD
// Set bitmap IDs and caption in constructor, call WizMessageBox instead of
// AfxMessageBox.
//
//    Rev 1.12   20 May 1998 23:23:54   jtaylor
// Added support for all files clean.
//
//    Rev 1.11   20 May 1998 20:46:52   jtaylor
// No change.
//
//    Rev 1.10   18 May 1998 18:30:02   SEDWARD
// New string default for SendToSarc() failure.
//
//    Rev 1.9   30 Apr 1998 20:19:26   jtaylor
// added an extra argument to SendToSarc. HWND for progress control parent window.
//
//    Rev 1.8   01 May 1998 22:19:58   jtaylor
// Added progress dialog control.
//
//    Rev 1.7   27 Apr 1998 20:25:48   SEDWARD
// Fix up "finish" dialog if user sheepishly admits to not having paid for
// their virus definitions.
//
//    Rev 1.6   27 Apr 1998 15:03:26   jtaylor
// Uncommented some lines.
//
//    Rev 1.5   27 Apr 1998 02:06:30   SEDWARD
// Added DefAnnuity stuff ("nagware"), OnInitDialog().
//
//    Rev 1.4   26 Apr 1998 17:27:44   SEDWARD
// Handle 'WizardNext' scenarios, construtor takes a pointer to a
// ScanDeliverDLL object.
//
//    Rev 1.3   24 Apr 1998 18:18:04   SEDWARD
// Added OnWizardNext()
//
//    Rev 1.2   20 Apr 1998 18:05:12   SEDWARD
// Added 'm_szSummary'.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ScanDeliverDLL.h"
#include "ScanWiz.h"
#include "ScanCfg.h"
#include "ScanDres.h"
#include "ScanDlvr.h"
#include "ReviewPg.h"
#include "DefAnnuity.h"
#include "WrapUpPg.h"

// Remote Access Services configuration (AutoDial)
#include "s32rasu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

                                        // local utility function
static void FillListCtrl ( CListCtrl* pList, LPTSTR szText );

/////////////////////////////////////////////////////////////////////////////
// CReviewPropertyPage property page

IMPLEMENT_DYNCREATE(CReviewPropertyPage, CWizardPropertyPage)

CReviewPropertyPage::CReviewPropertyPage(CScanDeliverDLL*  pDLL)
    : CWizardPropertyPage(pDLL, CReviewPropertyPage::IDD, IDS_CAPTION_REVIEW)
{
    m_bitmap16 = IDB_SDBMP_REVIEW_16;
    m_bitmap256 = IDB_SDBMP_REVIEW_256;
}

CReviewPropertyPage::CReviewPropertyPage()
    : CWizardPropertyPage(CReviewPropertyPage::IDD, IDS_CAPTION_REVIEW)
{
    //{{AFX_DATA_INIT(CReviewPropertyPage)
    m_szSummary = _T("");
    //}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_REVIEW_16;
    m_bitmap256 = IDB_SDBMP_REVIEW_256;
}

CReviewPropertyPage::~CReviewPropertyPage()
{
}

void CReviewPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CReviewPropertyPage)
//No longer using this for DDX: DDX_Text(pDX, IDC_EDIT_SUMMARY, m_szSummary);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReviewPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CReviewPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReviewPropertyPage message handlers



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CReviewPropertyPage::OnInitDialog(void)
{
CListCtrl* pList = (CListCtrl*) GetDlgItem ( IDC_EDIT_SUMMARY );
COLORREF   rgbBkColor = (COLORREF) GetSysColor ( COLOR_3DFACE );

    CWizardPropertyPage::OnInitDialog();

    // Insert one column in the list control, and set the background and
    // text background colors to make it look disabled.  We can't actually
    // disable the control, because that would make the scroll bars not work.

    pList->InsertColumn ( 0, _T(""), LVCFMT_LEFT, -1, 0 );
    pList->SetBkColor ( rgbBkColor );
    pList->SetTextBkColor ( rgbBkColor );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CReviewPropertyPage::OnInitDialog"



// ==== SetSummaryText ====================================================
//
//  This function is responsible for initializing the summary static control
//  with pertinent information (username, email address, files to be sent,
//  etc).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

void    CReviewPropertyPage::SetSummaryText(void)
{
    auto    CString         tempStr;
    auto    CString         formatStr;
    auto    TCHAR           tempBuf[MAX_INI_BUF_SIZE];

    // name
    if (0 != tempStr.LoadString(IDS_REVIEW_NAME))
        {
        formatStr.Format(_T("%s "), tempStr);
        m_szSummary = formatStr;
        g_ConfigInfo.GetFirstName(tempBuf, MAX_INI_BUF_SIZE);
        formatStr.Format(_T("%s "), tempBuf);
        m_szSummary += formatStr;
        g_ConfigInfo.GetLastName(tempBuf, MAX_INI_BUF_SIZE);
        formatStr.Format(_T("%s%c\n"), tempBuf, 0x0D);  // add a CR/LF
        m_szSummary += formatStr;
        }

    // email address
    if (0 != tempStr.LoadString(IDS_REVIEW_EMAIL_ADDRESS))
        {
        formatStr.Format(_T("%s "), tempStr);
        m_szSummary += formatStr;
        g_ConfigInfo.GetEmail(tempBuf, MAX_INI_BUF_SIZE);
        formatStr.Format(_T("%s%c\n"), tempBuf, 0x0D);
        m_szSummary += formatStr;
        }

    // strip file content
    auto    UINT        resID = 0;
    if (TRUE == g_ConfigInfo.GetStripFileContent())
        {
        resID = IDS_REVIEW_STRIP_FILE_CONTENT_YES;
        }
    else
        {
        resID = IDS_REVIEW_STRIP_FILE_CONTENT_NO;
        }

    if (0 != tempStr.LoadString(resID))
        {
        formatStr.Format(_T("%s%c\n"), tempStr, 0x0D);
        m_szSummary += formatStr;
        }

    // file list
    if (0 != tempStr.LoadString(IDS_REVIEW_FILES))
        {
        formatStr.Format(_T("%s%c\n"), tempStr, 0x0D);
        m_szSummary += formatStr;

        // get the number of files we're about to send
        auto    int                 numAcceptedFiles = m_pScanDeliverDLL->GetNumberAcceptedFiles();
        auto    int                 nIndex;
        auto    IQuarantineItem*    pQItem = NULL;
        if (numAcceptedFiles > 0)
            {
            for (nIndex = 0; nIndex < numAcceptedFiles; ++nIndex)
                {
                pQItem = m_pScanDeliverDLL->GetAcceptedQuarantineItem(nIndex);
                if (NULL != pQItem)
                    {
                    m_szSummary += FOUR_SPACE_TAB;
                    pQItem->GetOriginalAnsiFilename(tempBuf, MAX_INI_BUF_SIZE);
                    formatStr.Format(_T("%s%c\n"), tempBuf, 0x0d);
                    m_szSummary += formatStr;
                    }
                }
            }
        }

}  // end of "CReviewPropertyPage::SetSummaryText"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
//  5/28/98 JTAYLOR - Added commented out test dialup network support
//  6/10/98 JTAYLOR - Enhanced network connection test and retry support
//                    Removed commented out test dialup network support...
// ========================================================================

LRESULT CReviewPropertyPage::OnWizardNext(void)
{
    BOOL                    bSubmissionSucceeded = FALSE;
    CScanDeliverWizard*     pWizSheet;
    DWORD                   dwResult;
    HRESULT                 hResult;
    int                     nDlgResponse;
    UINT                    nResID = 0;
    BOOL                    bRASConnected = FALSE;
	BOOL					bRetry;
	CString					szStatusText, szStatusTitle;


    // check to see if the virus definitions have expired; if so, nag the user
    dwResult = DefAnnuityGetDownloadStatus();
    if (DefAnnuityDownloadNotAllowed == dwResult)
        {
        nDlgResponse = WizMessageBox(IDS_DEF_ANNUITY_MESSAGE
                                                    , IDS_CAPTION_STANDARD_MESGBOX
                                                    , MB_ICONWARNING | MB_YESNO);
        if (IDYES != nDlgResponse)
            {
            // get a pointer to the parent window (the property sheet)
            pWizSheet = (CScanDeliverWizard*)this->GetParent();

            // if the user sets the "no" radio button, wrap up the session
            if (NULL != pWizSheet)
                {
                pWizSheet->GoToWrapUpPage(WRAPUP_TYPE_VIRUS_SUBSCRIPTION_NOT_PAID);
                }
            goto  Exit_Function;
            }
        }

    // Create the package for delivery to SARC
    hResult = m_pScanDeliverDLL->CreatePackage(GetSafeHwnd());

    if (FAILED(hResult))
        {
        switch (hResult)
            {
            case  E_QUARANTINE_DIRECTORY_INVALID:
                nResID = IDS_SARC_ERROR_INVALID_QUARANTINE_DIRECTORY;
                break;

            case  E_UNABLE_TO_STRIP_FILE:
                nResID = IDS_SARC_ERROR_UNABLE_TO_STRIP_FILE;
                break;

            case  E_PACKAGE_OVER_MAX_SUBMISSION_SIZE:
                nResID = IDS_SARC_ERROR_PACKAGE_SIZE_TOO_BIG;
                break;

            default:
                nResID = IDS_SARC_ERROR_PACKAGE_CREATION;
                break;
            }

        // display a message and set the text in the final wizard dialog
        WizMessageBox(nResID);
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
            {
            pWizSheet->GoToWrapUpPage(WRAPUP_TYPE_SARC_ERROR_CREATE_PACKAGE);
            }
        goto  Exit_Function;
        }


    // Ping SARC to insure we have a working connection and to give the user
    // the ability to cancel if the connection is hung
	do
	{
		bRetry = FALSE;

		hResult = m_pScanDeliverDLL->PingSARC(GetSafeHwnd());

		if (FAILED(hResult))
		{
			switch (hResult)
			{
				case E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET:
				case E_USER_CANCELLED:
				case E_UNABLE_TO_CREATE_THREAD:
					szStatusText.LoadString(IDS_SARC_ERROR_BACKEND_NO_CONNECT);
					szStatusTitle.LoadString(IDS_CAPTION_STANDARD_MESGBOX);
					nDlgResponse = MessageBox(szStatusText, szStatusTitle,
												 MB_ICONWARNING | MB_RETRYCANCEL);
					if (IDCANCEL == nDlgResponse)
					{
						// set the final dialog text
						pWizSheet = (CScanDeliverWizard*)this->GetParent();
						if (NULL != pWizSheet)
							pWizSheet->GoToWrapUpPage(WRAPUP_TYPE_SARC_ERROR_SEND_TO_SARC);

						// display a message to the user and exit this function
						//WizMessageBox(nResID);

						goto Exit_Function;
					}
					bRetry = TRUE;

					break;

				case  E_UNABLE_TO_GET_BACKEND_STATUS:
					if (FALSE == g_ConfigInfo.IsCorporateMode())
						nResID = IDS_SARC_ERROR_RETAIL_BACKEND_NO_STATUS;
					else
						nResID = IDS_SARC_ERROR_CORPORATE_BACKEND_NO_STATUS;
					break;

				case  S_BACKEND_BUSY:
					if (FALSE == g_ConfigInfo.IsCorporateMode())
						nResID = IDS_SARC_ERROR_RETAIL_BACKEND_BUSY;
					else
						nResID = IDS_SARC_ERROR_CORPORATE_BACKEND_BUSY;
					break;

				case  S_BACKEND_FALSE_POSITIVE:
					if (FALSE == g_ConfigInfo.IsCorporateMode())
						nResID = IDS_SARC_ERROR_RETAIL_FALSE_POSITIVE;
					else
						nResID = IDS_SARC_ERROR_CORPORATE_FALSE_POSITIVE;
					break;

				case  S_BACKEND_ALL_CLEAN:
					if (FALSE == g_ConfigInfo.IsCorporateMode())
						nResID = IDS_SARC_ERROR_RETAIL_ALL_CLEAN;
					else
						nResID = IDS_SARC_ERROR_CORPORATE_ALL_CLEAN;
					break;

				default:
					break;
			}

		}
	}
	while (bRetry);

    // if we're supposed to display a message, do so and set the text for the final
    // wizard dialog
    if (nResID != 0)
    {
        auto    UINT        nWrapUpType = 0;

        // retail users simply exit, corporate users get a choice
        if (FALSE == g_ConfigInfo.IsCorporateMode())
        {
            if( S_BACKEND_FALSE_POSITIVE == hResult )
                nWrapUpType = WRAPUP_TYPE_ALL_FALSE_POSITIVES;
            else
                nWrapUpType = WRAPUP_TYPE_SARC_ERROR_CHECK_BACKEND_STATUS;

            WizMessageBox(nResID);
        }
        else
        {
            nDlgResponse = WizMessageBox(nResID, IDS_CAPTION_STANDARD_MESGBOX
                                                    , MB_ICONWARNING | MB_YESNO);
            if (IDYES != nDlgResponse)
            {
                if( S_BACKEND_FALSE_POSITIVE == hResult )
                    nWrapUpType = WRAPUP_TYPE_ALL_FALSE_POSITIVES;
                else
                    nWrapUpType = WRAPUP_TYPE_SARC_ERROR_CHECK_BACKEND_STATUS;
            }
        }

        // if we're to wrap it up, set the final dialog text and exit this function
        if (0 != nWrapUpType)
        {
            pWizSheet = (CScanDeliverWizard*)this->GetParent();

            if (NULL != pWizSheet)
                pWizSheet->GoToWrapUpPage(nWrapUpType);

            goto Exit_Function;
        }
    }


    // send the package off to SARC
    hResult = m_pScanDeliverDLL->SendToSarc(GetSafeHwnd());

    if (FAILED(hResult))
    {
        switch (hResult)
        {
            case  E_QUARANTINE_DIRECTORY_INVALID:
                nResID = IDS_SARC_ERROR_INVALID_QUARANTINE_DIRECTORY;
                break;

            case  E_FILE_CREATE_FAILED:
                nResID = IDS_SARC_ERROR_UNABLE_TO_STRIP_FILE;
                break;

            default:
                nResID = IDS_SARC_ERROR_PACKAGE_NOT_SENT;
                break;
        }

        // set the final dialog text
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
            pWizSheet->GoToWrapUpPage(WRAPUP_TYPE_SARC_ERROR_SEND_TO_SARC);

        // display a message to the user and exit this function
        WizMessageBox(nResID);
        goto Exit_Function;
    }
    else
        bSubmissionSucceeded = TRUE;

    // if we've successfully submitted our package AND we're running in file of the
    // day mode, update the registry value
    if  ((TRUE == m_pScanDeliverDLL->IsFileOfTheDayMode()) && (TRUE == bSubmissionSucceeded))
        m_pScanDeliverDLL->UpdateTimeLapseRegistryData();


Exit_Function:
    return CWizardPropertyPage::OnWizardNext();

}  // end of "CReviewPropertyPage::OnWizardNext"



// ==== OnSetActive =======================================================
//
//  This function updates the contents of the summary static control with
//  whatever information is currently in the global Scan and Deliver
//  configuration object.
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

BOOL    CReviewPropertyPage::OnSetActive(void)
{
CListCtrl* pList = (CListCtrl*) GetDlgItem (IDC_EDIT_SUMMARY);
	
    CWizardPropertyPage::OnSetActive();

    // we initialize the summary text in this handler instead of OnInitDialog
    // so that if the user should click on the "Back" button to change anything,
    // it will show up in the summary text
    SetSummaryText();

    FillListCtrl ( pList, m_szSummary.GetBuffer(1) );
    m_szSummary.ReleaseBuffer(-1);

    pList->SetColumnWidth ( 0, LVSCW_AUTOSIZE );

    return 1;                           // allow page to be set active
}  // end of "CReviewPropertyPage::OnSetActive"


//////////////////////////////////////////////////////////////////////////
//
// Function:    FillListControl
//
// Description:
//  Inserts a given string line-by-line into a list control.  Each line
//  goes in a separate item in the control.
//
// Input:
//  pList: [in] Pointer to the list control.
//  szText: [in] Text to insert.  This text is modified since the function
//               uses strtok().
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 7/27/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

static void FillListCtrl ( CListCtrl* pList, LPTSTR szText )
{
LPTSTR szLine;
int    nItem = 0;

    pList->DeleteAllItems();

    szLine = _tcstok ( szText, _T("\x0d\n") );

    while ( NULL != szLine )
        {
        pList->InsertItem ( nItem++, szLine );
        szLine = _tcstok ( NULL, _T("\x0d\n") );
        }
}


