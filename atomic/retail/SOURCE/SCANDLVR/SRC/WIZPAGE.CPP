/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/WizPage.cpv   1.5   26 May 1998 18:27:58   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// WizPage.cpp: implementation of the CWizardPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/WizPage.cpv  $
// 
//    Rev 1.5   26 May 1998 18:27:58   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.4   26 May 1998 18:18:40   SEDWARD
// Added support for swapping 16/256 color bitmaps at runtime.
//
//    Rev 1.3   25 May 1998 13:52:56   SEDWARD
// Added m_staticBitmap, DisplayableColors(), OnInitDialog() and
// WizMessageBox().
//
//    Rev 1.2   24 Apr 1998 18:23:18   SEDWARD
// Added logfile headers.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "WizPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardPropertyPage property page

IMPLEMENT_DYNCREATE(CWizardPropertyPage, CPropertyPage)

CWizardPropertyPage::CWizardPropertyPage() : CPropertyPage()
{
}

CWizardPropertyPage::CWizardPropertyPage(CScanDeliverDLL*  pDLL, UINT nIDTemplate
                                            , UINT nIDCaption)
                                            : CPropertyPage(nIDTemplate, nIDCaption)
{
    m_pScanDeliverDLL = pDLL;
}

CWizardPropertyPage::CWizardPropertyPage(UINT nIDTemplate, UINT nIDCaption)
    : CPropertyPage(nIDTemplate, nIDCaption)
{
    //{{AFX_DATA_INIT(CWizardPropertyPage)
	//}}AFX_DATA_INIT
}

CWizardPropertyPage::~CWizardPropertyPage()
{
}

void CWizardPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWizardPropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPropertyPage, CPropertyPage)
    //{{AFX_MSG_MAP(CWizardPropertyPage)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPropertyPage message handlers

BOOL CWizardPropertyPage::UpdateConfigData()
{
    return (TRUE);
}



// ==== DisplayableColors =================================================
//
//  This function determines how many colors can be displayed on screen.
//  Should be used internally by wizard pages when deciding whether or not
//  to display 256 color bitmaps.
//
//  NOTE: this function was ripped-off directly from navwbmp.cpp (proj=NAVW)
//
//  Input:  nothing
//
//  Output: number of display colors for the screen
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

DWORD   CWizardPropertyPage::DisplayableColors(void)
{
    HDC         hDC = ::GetDC ( ::GetDesktopWindow () );
    int         nColorDepth;
    DWORD       dwDisplayableColors;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );
        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;
        }

    ::ReleaseDC ( ::GetDesktopWindow (), hDC );
    return ( dwDisplayableColors );

}  // end of "CWizardPropertyPage::DisplayableColors"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

BOOL    CWizardPropertyPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    // use the appropriate bitmap
    auto    DWORD           dwNumColors;
    auto    int             iResID;

    // determine which bitmap ID to use
    dwNumColors = DisplayableColors();
    if (dwNumColors < 256)
        {
        iResID = m_bitmap16;
        }
    else
        {
        iResID = m_bitmap256;
        }

    // load the bitmap and attach it to the static control
    auto    CStatic*    ctrlPtr = NULL;
    auto    HINSTANCE   hRes = AfxFindResourceHandle( MAKEINTRESOURCE( iResID ), RT_BITMAP );
    m_hBitmap = ::LoadBitmap( hRes, MAKEINTRESOURCE( iResID ) );
    ctrlPtr = (CStatic*)GetDlgItem(IDC_BITMAP);
    if ((NULL != m_hBitmap)  &&  (NULL != ctrlPtr))
        {
        ctrlPtr->SetBitmap( m_hBitmap );
        }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CWizardPropertyPage::OnInitDialog"



// ==== WizMessageBox =====================================================
//
//  This function displays a message box to the user.
//
//  Input:
//      nMessageID  -- the resource ID of an entry in the string table that
//                     contains the message to display
//
//      nCaptionID  -- the resource ID of an entry in the string table that
//                     contains the caption to display
//
//      nFlags      -- the "flags" argument used for the standard Win32
//                     "MessageBox" function
//
//  Output:
//      the result of the call to the "MessageBox" function
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

int CWizardPropertyPage::WizMessageBox(UINT  nMessageID, UINT  nCaptionID, UINT  nFlags)
{
    auto    CString     captionStr;
    auto    CString     mesgStr;
    auto    int         nResult = 0;

    captionStr.LoadString(nCaptionID);
    mesgStr.LoadString(nMessageID);
    nResult = MessageBox(mesgStr, captionStr, nFlags);

    return (nResult);

}  // end of "CWizardPropertyPage::WizMessageBox"
