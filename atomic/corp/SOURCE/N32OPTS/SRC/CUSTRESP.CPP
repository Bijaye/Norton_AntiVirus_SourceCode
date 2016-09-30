/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/custresp.cpv   1.12   24 Jun 1998 18:09:54   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
// CustResp.cpp : implementation file
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/custresp.cpv  $
// 
//    Rev 1.12   24 Jun 1998 18:09:54   jtaylor
// Fixed a cut and past error in the code to remove macro quarantine options"
// 
//    Rev 1.11   24 Jun 1998 17:54:46   jtaylor
// Removed more Quarantine Controls when Quarantine not installed.  Uncheck Qurantine checkboxes.  Removed unused first window parameter on AlignWindow calls.
// 
//    Rev 1.10   30 Apr 1998 03:27:08   tcashin
// Changed a string in the macro section to "delete file automatically"
// (#112172).
// 
//    Rev 1.9   01 Apr 1998 16:41:48   DALLEE
// Added AP option for Quarantine if can't repair.
// 
//    Rev 1.8   30 Mar 1998 15:08:16   tcashin
// Added quarantine to combo boxes for file and macro viruses.
// 
//    Rev 1.7   20 Aug 1997 18:14:38   DBUCHES
// Changed string for NT platform.
//
//    Rev 1.6   25 Jun 1997 11:54:38   DBUCHES
// Hooked up more help topics.
//
//    Rev 1.5   24 Jun 1997 15:59:20   DBUCHES
// Added items to response combo lists.
//
//    Rev 1.4   18 Jun 1997 14:02:40   DBUCHES
// Fixed problem with retrieving data from comboboxes.
//
//    Rev 1.3   10 Jun 1997 17:50:54   DBUCHES
// Removed quarantine options.
//
//    Rev 1.2   09 Jun 1997 12:37:06   DBUCHES
// Set some reasonable default values in custom response dialog contstructor.
//
//    Rev 1.1   27 May 1997 20:55:38   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   21 May 1997 14:37:08   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n32opts.h"
#include "n32behav.h"     // Personality DLL
#include "CustResp.h"
#include "platform.h"
#include "navopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CCustomResponseDlg::m_dwWhatsThisMap[] = {
    IDC_FILE_COMBO,     IDH_NAVW_WHATHIS_WHEN_A_VIRUS_IS_FOUND_IN_A_FILE_LSTBX,
    IDC_MACRO_COMBO,    IDH_NAVW_WHATHIS_WHEN_A_MACRO_VIRUS_IS_FOUND_LSTBX,
    IDC_BOOT_COMBO,    IDH_NAVW_WHATHIS_WHEN_A_BOOT_RECORD_VIRUS_IS_FOUND_LSTBX,
    0,0
    };
DWORD CCustomResponseDlg::m_dwHowToMap[] = { 0,0 };


/////////////////////////////////////////////////////////////////////////////
// Action string maps
// Note that these guys need to be kept up to date!
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// File
static int aFileActionsNT[] = { IDS_PROMPT,         SCAN_PROMPT,
                                IDS_NOTIFY,         SCAN_REPORT,
                                IDS_REPAIR,         SCAN_REPAUTO,
                                IDS_DELETE,         SCAN_DELAUTO,
                                IDS_QUARAUTO,       SCAN_QUARAUTO,
                                IDS_QUAR_CANT_REP,  SCAN_QUAR_CANT_REP,
                                0,0
                              };

static int aAPFileActionsNT[] = { IDS_DENY,         TSR_DENY,
                                IDS_REPAIR,         TSR_REPAIR,
                                IDS_DELETE,         TSR_DELETE,
                                IDS_QUARAUTO,       TSR_QUARANTINEQUIET,
                                IDS_QUAR_CANT_REP,  TSR_QUAR_CANT_REP,
                                0,0
                              };


static int aFileActions[] = {   IDS_PROMPT,         SCAN_PROMPT,
                                IDS_NOTIFY,         SCAN_REPORT,
                                IDS_REPAIR,         SCAN_REPAUTO,
                                IDS_DELETE,         SCAN_DELAUTO,
                                IDS_SHUTDOWN,       SCAN_HALTCPU,
                                IDS_QUARAUTO,       SCAN_QUARAUTO,
                                IDS_QUAR_CANT_REP,  SCAN_QUAR_CANT_REP,
                                0,0
                              };

static int aAPFileActions[] = { IDS_PROMPT,         TSR_PROMPT,
                                IDS_DENY,           TSR_DENY,
                                IDS_REPAIR,         TSR_REPAIR,
                                IDS_DELETE,         TSR_DELETE,
                                IDS_SHUTDOWN,       TSR_HALTCPU,
                                IDS_QUARAUTO,       TSR_QUARANTINEQUIET,
                                IDS_QUAR_CANT_REP,  TSR_QUAR_CANT_REP,
                                0,0
                              };

////////////////////////////////////////////////////////////////////////////////////
// Macro
static int aMacroActions[] = { IDS_PROMPT,          SCAN_PROMPT,
                               IDS_NOTIFY,          SCAN_REPORT,
                               IDS_REPAIR,          SCAN_REPAUTO,
                               IDS_DELETE_FILE,     SCAN_DELAUTO,
                               IDS_QUARAUTO,        SCAN_QUARAUTO,
                               IDS_QUAR_CANT_REP,   SCAN_QUAR_CANT_REP,
                               0,0
                             };

static int aAPMacroActions[] = { IDS_PROMPT,        TSR_PROMPT,
                                 IDS_DENY,          TSR_DENY,
                                 IDS_REPAIR,        TSR_REPAIR,
                                 IDS_DELETE_FILE,   TSR_DELETE,
                                 IDS_SHUTDOWN,      TSR_HALTCPU,
                                 IDS_QUARAUTO,      TSR_QUARANTINEQUIET,
                                 IDS_QUAR_CANT_REP, TSR_QUAR_CANT_REP,
                                 0,0
                             };

static int aAPMacroActionsNT[] = { IDS_DENY,        TSR_DENY,
                               IDS_REPAIR,          TSR_REPAIR,
                               IDS_DELETE_FILE,     TSR_DELETE,
                               IDS_QUARAUTO,        TSR_QUARANTINEQUIET,
                               IDS_QUAR_CANT_REP,   TSR_QUAR_CANT_REP,
                               0,0
                              };

////////////////////////////////////////////////////////////////////////////////////
// Boot

static int aBootActions[] =   { IDS_PROMPT,         SCAN_PROMPT,
                                IDS_NOTIFY,         SCAN_REPORT,
                                IDS_REPAIR,         SCAN_REPAUTO,
                                IDS_SHUTDOWN,       SCAN_HALTCPU,
                                0,0
                              };

static int aBootActionsNT[] = {   IDS_PROMPT,       SCAN_PROMPT,
                                IDS_NOTIFY,         SCAN_REPORT,
                                IDS_REPAIR,         SCAN_REPAUTO,
                                0,0
                              };

static int aAPBootActions[] = { IDS_PROMPT,         TSR_PROMPT,
                                IDS_REPAIR,         TSR_REPAIR,
                                IDS_SHUTDOWN,       TSR_HALTCPU,
                                0, 0
                              };

static int aAPBootActionsNT[] = { IDS_NOTIFY,       TSR_PROMPT,
                                IDS_REPAIR,         TSR_REPAIR,
                                0, 0
                              };



/////////////////////////////////////////////////////////////////////////////
// CCustomResponseDlg dialog


CCustomResponseDlg::CCustomResponseDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CCustomResponseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomResponseDlg)
	m_iFileAction = 0;
	m_iMacroAction = 0;
	m_iBootAction = 0;
	//}}AFX_DATA_INIT
    m_bAP = FALSE;
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_CUSTOM_RESPONSE_DLG_HLPBTN;
}


void CCustomResponseDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomResponseDlg)
	DDX_CBIndex(pDX, IDC_FILE_COMBO, m_iFileAction);
	DDX_CBIndex(pDX, IDC_MACRO_COMBO, m_iMacroAction);
	DDX_CBIndex(pDX, IDC_BOOT_COMBO, m_iBootAction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomResponseDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CCustomResponseDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomResponseDlg message handlers

BOOL CCustomResponseDlg::OnInitDialog()
{
	CHelperDialog::OnInitDialog();

    int *pFile;
    int *pMacro;
    int *pBoot;
    int i, index;
    CString s;
    CComboBox* pComboBox;
    BOOL bQuarantine = NAV_IS_QUARANTINE();

	// Setup contents of combo boxes.
    if(IS_NT())
        {
        pFile = m_bAP ? aAPFileActionsNT : aFileActionsNT;
        pMacro = m_bAP ? aAPMacroActionsNT : aMacroActions;
        pBoot = m_bAP ? aAPBootActionsNT : aBootActionsNT;
        }
    else
        {
        pFile = m_bAP ? aAPFileActions : aFileActions;
        pMacro = m_bAP ? aAPMacroActions : aMacroActions;
        pBoot = m_bAP ? aAPBootActions : aBootActions;
        }

    // File
    pComboBox = (CComboBox*)GetDlgItem(IDC_FILE_COMBO);
    for(i=0; pFile[i]; i += 2)
        {
        // If Quarantine is not enabled then do not display the
        // Quarantine items in the list.
        if( bQuarantine || 
            ( IDS_QUARAUTO      != pFile[i] &&
              IDS_QUAR_CANT_REP != pFile[i] ) )
            {
            s.LoadString(pFile[i]);
            index = pComboBox->AddString(s);
            if(m_iFile == pFile[i+1])
                m_iFileAction = index;
            }
        }

    // Macro
    pComboBox = (CComboBox*)GetDlgItem(IDC_MACRO_COMBO);
    for(i=0; pMacro[i]; i += 2)
        {
        // If Quarantine is not enabled then do not display the
        // Quarantine items in the list.
        if( bQuarantine || 
            ( IDS_QUARAUTO      != pMacro[i] &&
              IDS_QUAR_CANT_REP != pMacro[i] ) )
            {
            s.LoadString(pMacro[i]);
            index = pComboBox->AddString(s);
            if(m_iMacro == pMacro[i+1])
                m_iMacroAction = index;
            }
        }

    // Boot
    pComboBox = (CComboBox*)GetDlgItem(IDC_BOOT_COMBO);
    for(i=0; pBoot[i]; i += 2)
        {
        s.LoadString(pBoot[i]);
        index = pComboBox->AddString(s);
        if(m_iBoot == pBoot[i+1])
            m_iBootAction = index;
        }

    // Transfer data to controls
    UpdateData(FALSE);    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomResponseDlg::OnOK()
{
    int *pFile;
    int *pMacro;
    int *pBoot;

	// Get Data from controls
	UpdateData();    
    
	// Retrieve and translate contents of list boxes.
    if(IS_NT())
        {
        pFile = m_bAP ? aAPFileActionsNT : aFileActionsNT;
        pMacro = m_bAP ? aAPMacroActionsNT : aMacroActions;
        pBoot = m_bAP ? aAPBootActionsNT : aBootActionsNT;
        }
    else
        {
        pFile = m_bAP ? aAPFileActions : aFileActions;
        pMacro = m_bAP ? aAPMacroActions : aMacroActions;
        pBoot = m_bAP ? aAPBootActions : aBootActions;
        }

    // File
    m_iFile = pFile[m_iFileAction * 2 + 1];

    // Macro
    m_iMacro = pMacro[m_iMacroAction * 2 + 1];

    // Boot
    m_iBoot = pBoot[m_iBootAction * 2 + 1];

	CHelperDialog::OnOK();
}
