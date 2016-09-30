/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/STARTPGE.CPv   1.7   19 Feb 1998 11:57:14   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
//
// Startpge.cpp : implementation file
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
// $Log:   S:/N32OPTS/VCS/STARTPGE.CPv  $
// 
//    Rev 1.7   19 Feb 1998 11:57:14   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.6   18 Nov 1997 20:36:40   jtaylor
// Removed most recent version.
// 
//    Rev 1.4   20 Jun 1997 12:03:58   DBUCHES
// Added help support.
// 
//    Rev 1.3   10 Jun 1997 15:21:56   DBUCHES
// Added password stuff
// 
//    Rev 1.2   04 Jun 1997 22:15:10   DBUCHES
// Removed TSR checkbox.
// 
//    Rev 1.1   27 May 1997 20:57:28   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 14:58:22   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "n32opts.h"
#include "Startpge.h"
#include "alignwnd.h"
#include "sheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Set up help maps
/////////////////////////////////////////////////////////////////////////////
DWORD CStartupPage::m_dwWhatsThisMap[] = { 
    IDC_MEMORY_CHECK,       IDH_NAVW_WHATHIS_STARTUP_MEMORY,
    IDC_MBR_CHECK,          IDH_NAVW_WHATHIS_STARTUP_MASTER_BOOT_RECORD,
    IDC_BOOTRECORD_CHECK,   IDH_NAVW_WHATHIS_STARTUP_BOOT_RECORDS,
    IDC_SYSFILES_CHECK,     IDH_NAVW_WHATHIS_STARTUP_DOS_SYSTEM_FILES,
    IDC_BYPASS_RADIO,       IDH_NAVW_WHATHIS_STARTUP_BYPASS_NONE,
    IDC_RADIO2,             IDH_NAVW_WHATHIS_STARTUP_BYPASS_BOTH_SHIFT,
    IDC_RADIO3,             IDH_NAVW_WHATHIS_STARTUP_BYPASS_BOTH_ALT,
    IDC_RADIO4,             IDH_NAVW_WHATHIS_STARTUP_BYPASS_BOTH_CTRL,
    0,0 
    };
DWORD CStartupPage::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// CStartupPage property page

IMPLEMENT_DYNCREATE(CStartupPage, CHelperPage)

CStartupPage::CStartupPage() : CHelperPage(CStartupPage::IDD)
{
	//{{AFX_DATA_INIT(CStartupPage)
	m_bBootRecord = FALSE;
	m_iBypassKey = -1;
	m_bMBR = FALSE;
	m_bMemory = FALSE;
	m_bSysFiles = FALSE;
	//}}AFX_DATA_INIT
    // Set up help maps.
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_STARTUP_DLG_HELP_BTN;
    // Set up Password ID
    m_iPasswordFeature = PASSWORD_VIEWSTARTUPOPT;

}

CStartupPage::~CStartupPage()
{
}

void CStartupPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStartupPage)
	DDX_Check(pDX, IDC_BOOTRECORD_CHECK, m_bBootRecord);
	DDX_Radio(pDX, IDC_BYPASS_RADIO, m_iBypassKey);
	DDX_Check(pDX, IDC_MBR_CHECK, m_bMBR);
	DDX_Check(pDX, IDC_MEMORY_CHECK, m_bMemory);
	DDX_Check(pDX, IDC_SYSFILES_CHECK, m_bSysFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStartupPage, CHelperPage)
	//{{AFX_MSG_MAP(CStartupPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartupPage message handlers

// I needed to create an OnInitDialog() for this page in order to hide &
// move checkboxes.  This resulted in the controls coming up blank, so
// I put in the GetData() and UpdateData() calls to correct that.
// For some reason MSVC won't let me set breakpoints, so I can't investigate
// this further.  Those 2 extra function calls don't seem to have any
// Bad side effects, so I'll go with them for now.  9/23/97 Mike Dunn

BOOL CStartupPage::OnInitDialog()
{
    CHelperPage::OnInitDialog();

    if ( IS_NEC() )
        {
        HWND hMBRCheck, hSysFilesCheck;

        // Hide the MBR and BR checkboxes, and move the system files
        // checkbox up.

        hMBRCheck = GetDlgItem ( IDC_MBR_CHECK ) -> GetSafeHwnd();
        hSysFilesCheck = GetDlgItem ( IDC_SYSFILES_CHECK ) -> GetSafeHwnd();

        AlignWnd ( hSysFilesCheck, hMBRCheck, gAlignWndBottom, NULL, 0 );

        GetDlgItem ( IDC_MBR_CHECK ) -> ShowWindow ( SW_HIDE );
        GetDlgItem ( IDC_BOOTRECORD_CHECK ) -> ShowWindow ( SW_HIDE );
        }

                                        // (see comments above)
                                        // re-set the page's member variables
    GetData( &(((CNavOptionsSheet*) GetParent()) -> navOpts) );
    
                                        // update the dlg controls from the
                                        // member variables
    UpdateData ( FALSE );

    return TRUE;
}

BOOL CStartupPage::SetData(LPNAVOPTIONS pOptions)
{
    m_bBootRecord   = pOptions->tsr.bScanBootRecs;
    m_bMemory       = pOptions->tsr.bScanMem;
    m_bMBR          = pOptions->tsr.bScanMBoot;
    m_bSysFiles     = pOptions->startup.bScanWindowsFiles;
    m_iBypassKey    = pOptions->tsr.uBypassKey;

    return TRUE;
}

BOOL CStartupPage::GetData(LPNAVOPTIONS pOptions)
{
    pOptions->tsr.bScanBootRecs          = m_bBootRecord;
    pOptions->tsr.bScanMem               = m_bMemory;
    pOptions->tsr.bScanMBoot             = m_bMBR;
    pOptions->startup.bScanWindowsFiles  = m_bSysFiles;
    pOptions->tsr.uBypassKey             = m_iBypassKey;

    return TRUE;
}
