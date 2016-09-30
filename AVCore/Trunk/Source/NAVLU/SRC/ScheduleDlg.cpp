// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/scheduledlg.cpv   1.9   28 Aug 1998 16:22:36   JBRENNA  $
//
// Description:
//   Implementation file.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/scheduledlg.cpv  $
// 
//    Rev 1.9   28 Aug 1998 16:22:36   JBRENNA
// Port changes from QAKN branch to trunk.
// 
//    Rev 1.8.2.1   07 Jul 1998 12:52:42   DBuches
// Added picture string to call to GetTimeFormat.
// 
//    Rev 1.8.2.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.8   06 Aug 1997 10:31:20   JBRENNA
// If scheduler is not running prompt the user, ask them if they want it launched now.
// 
//    Rev 1.7   25 Jul 1997 20:53:22   JBRENNA
// Rework the random days and times. We now pre-compute the random days and
// times. When the user toggles between BiMonthly or overnight, we toggle
// between the pre-computed days / times.
// 
//    Rev 1.6   17 Jun 1997 11:43:08   JBRENNA
// 1. Changed the WinHelp call to specific HELP_CONTEXTPOPUP instead of just
//    HELP_CONTEXT. The new setting causes a popup window to be created. This
//    is more in sync with the normal context sensitive help.
// 2. Removed the help link on the dialog window. It was linked to the help
//    button's help. But, the help button's help will be too large for a popup.
// 
//    Rev 1.5   16 Jun 1997 14:28:48   JBRENNA
// Correct the displayed icon for the CScheduleDlg.
// 
//    Rev 1.4   16 Jun 1997 13:39:12   JBRENNA
// Add context sensitive help.
// 
//    Rev 1.3   10 Jun 1997 09:19:10   JBRENNA
// Put a "Prompt" checkbox on the ScheduleAdd dialog. When checked, the
// NPS Scheduled LiveUpdate command line includes the /PROMPT option.
// 
//    Rev 1.2   03 Jun 1997 14:00:04   JBRENNA
// Move IDS_NAVLU_OPTIONS_KEY and IDS_NAVLU_SHOW_SCHEDADD_DLG to string.h.
// 
//    Rev 1.1   01 Jun 1997 19:19:44   JBRENNA
// 1. Add links to the Norton Program Scheduler (NPS).
// 2. Move all of the random number generation into CMyUtils class.
// 
//    Rev 1.0   23 May 1997 21:26:34   JBRENNA
// Initial revision.
// 
//****************************************************************************

#include "stdafx.h"
#include "afxdisp.h"
#include "platform.h"
#include "navlu.h"
#include "MyUtils.h"
#include "resource.h"
#include "ScheduleDlg.h"
#include "strings.h"
#include "navluhlp.h"
#include "npsapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//************************************************************************
// CScheduleDlg dialog
//************************************************************************


CScheduleDlg::CScheduleDlg(CWnd* pParent /*=NULL*/) :
    CDialog(CScheduleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleDlg)
	m_bNotShowAgain = FALSE;
	m_bNightsOk = FALSE;
	m_bSchedBiMonthly = FALSE;
	m_strTime1 = _T("");
	m_strTime2 = _T("");
	m_nDay1 = -1;
	m_nDay2 = -1;
	m_bPrompt = FALSE;
	//}}AFX_DATA_INIT

    m_nDay1NBi = 0;
    m_nDay2NBi = 0;
    m_nDay1Bi  = 0;
    m_nDay2Bi  = 0;
}


void CScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleDlg)
	DDX_Check(pDX, IDC_NO_SHOW_AGAIN_CHK, m_bNotShowAgain);
	DDX_Check(pDX, IDC_NIGHTS_OK_CHK, m_bNightsOk);
	DDX_Check(pDX, IDC_BIMONTHLY, m_bSchedBiMonthly);
	DDX_Text(pDX, IDC_TIME1_EDT, m_strTime1);
	DDV_MaxChars(pDX, m_strTime1, 20);
	DDX_Text(pDX, IDC_TIME2_EDT, m_strTime2);
	DDV_MaxChars(pDX, m_strTime2, 20);
	DDX_CBIndex(pDX, IDC_DAY1_CMB, m_nDay1);
	DDX_CBIndex(pDX, IDC_DAY2_CMB, m_nDay2);
	DDX_Check(pDX, IDC_PROMPT, m_bPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleDlg, CDialog)
	//{{AFX_MSG_MAP(CScheduleDlg)
	ON_BN_CLICKED(IDC_NIGHTS_OK_CHK, OnNightsOkChk)
	ON_BN_CLICKED(IDC_BIMONTHLY, OnBimonthly)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//************************************************************************
// CScheduleDlg utilities functions
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   Calculate the Day1 and Day2. It is calculated for both BiMonthly and
//   non-BiMonthly. The results are stored in member variables.
//
//                       BiMonthly Off       BiMonthly On
//    Day1 is between    9th and 22nd        2nd and 8th
//    Day2 is between    23rd and 28th       15th and 21st
//
//////////////////////////////////////////////////////////////////////////
void CScheduleDlg::CalcRandomDays ()
{
    // These define the day range for when bBiMonthly is FALSE.

    const  int  n1Day1Begin      = 9;    // 1st  day for pDay1 control
    const  int  n1Day1End        = 22;   // Last day for pDay1 control
    const  int  n1Day2Begin      = 23;   // 1st  day for pDay2 control
    const  int  n1Day2End        = 28;   // Last day for pDay2 control

    // These define the day range for when bBiMonthly is TRUE.

    const  int  n2Day1Begin      =  2;   // 1st  day for pDay1 control
    const  int  n2Day1End        =  8;   // Last day for pDay1 control
    const  int  n2Day2Begin      = 15;   // 1st  day for pDay2 control
    const  int  n2Day2End        = 21;   // Last day for pDay2 control

    const  int       nIndexOffset = -1;  // 1st day in ctrl is index 0.
    auto   CMyUtils  MyUtils;

    // Pick the days for both non-BiMonthly and BiMonthly.
    m_nDay1NBi  = MyUtils.GetRandomNumRange (n1Day1Begin, n1Day1End);
    m_nDay1NBi += nIndexOffset;

    m_nDay2NBi  = MyUtils.GetRandomNumRange (n1Day2Begin, n1Day2End);
    m_nDay2NBi += nIndexOffset;

    m_nDay1Bi   = MyUtils.GetRandomNumRange (n2Day1Begin, n2Day1End);
    m_nDay1Bi  += nIndexOffset;

    m_nDay2Bi   = MyUtils.GetRandomNumRange (n2Day2Begin, n2Day2End);
    m_nDay2Bi  += nIndexOffset;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Precompute the random times for both day time and night time operation.
//////////////////////////////////////////////////////////////////////////
void CScheduleDlg::CalcRandomTimes ()
{
    const  CTimeSpan  DayStartTime   (0, 8, 0, 0); // 8am
    const  CTimeSpan  DayEndTime     (0,17, 0, 0); // 5pm
    const  CTimeSpan  DayRange       (DayEndTime - DayStartTime);

    const  CTimeSpan  NightStartTime (0,21, 0, 0); // 9pm
    const  CTimeSpan  NightEndTime   (1, 6, 0, 0); // 6am next day
    const  CTimeSpan  NightRange     (NightEndTime - NightStartTime);

    const  int        nLoopNum = 2;
    const  CTimeSpan  *pTimeStart[nLoopNum]      = { &DayStartTime, &NightStartTime };
    const  CTimeSpan  *pTimeRange[nLoopNum]      = { &DayRange,     &NightRange     };
    auto   CString    *pstrCalcedStart[nLoopNum] = { &m_strDayTime, &m_strNightTime };

    auto   long       CalcedTime;
    auto   CMyUtils   MyUtils;
    int    nCurLoop;

    for (nCurLoop = 0; nCurLoop < nLoopNum; ++nCurLoop)
    {
        // Pick the time.
        CalcedTime = (MyUtils.GetRandomNum() % pTimeRange[nCurLoop]->GetTotalMinutes());

        // Offset the calculated time to start at the specified start time.
        CalcedTime += pTimeStart[nCurLoop]->GetTotalMinutes();
    
        const int     nMinPerHour  = 60;
        const int     nHoursPerDay = 24;
        auto  int     nDays        = CalcedTime / nMinPerHour / nHoursPerDay;
        auto  int     nHours       = (CalcedTime - (nDays*nMinPerHour*nHoursPerDay)) / nMinPerHour;
        auto  int     nMins        = (CalcedTime - (nDays*nMinPerHour*nHoursPerDay)) - (nHours*nMinPerHour);
        auto  CTime   CalcedStart (1990, 1, 1, nHours, nMins, 0);
        
        *pstrCalcedStart[nCurLoop] = FormatTime (CalcedStart);
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set both day fields on the dialog to one of the two pre-computed
//   random days. The bBiMonthly flag determines which set of days is
//   actually shown.
//////////////////////////////////////////////////////////////////////////
void CScheduleDlg::SetInitialDays (BOOL bBiMonthly) const
{
    auto   CComboBox *pDay1       = (CComboBox*)GetDlgItem (IDC_DAY1_CMB);
    auto   CComboBox *pDay2       = (CComboBox*)GetDlgItem (IDC_DAY2_CMB);

    // We assume that these are non-NULL.
    ASSERT (pDay1 != NULL);
    ASSERT (pDay2 != NULL);

    if (!bBiMonthly)
    {
        pDay1->SetCurSel (m_nDay1NBi);
        pDay2->SetCurSel (m_nDay2NBi);
    }
    else
    {
        pDay1->SetCurSel (m_nDay1Bi);
        pDay2->SetCurSel (m_nDay2Bi);
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set the Day1 and Day2 controls to a random day. Day1 will be between
//   2 and 8. While Day2 will be between 15 and 21.
//////////////////////////////////////////////////////////////////////////
void CScheduleDlg::SetInitialTimes ()
{
    auto   CEdit      *pTime1  = (CEdit*)GetDlgItem (IDC_TIME1_EDT);
    auto   CEdit      *pTime2  = (CEdit*)GetDlgItem (IDC_TIME2_EDT);

    // We assume that these are non-NULL.
    ASSERT (pTime1 != NULL);
    ASSERT (pTime2 != NULL);

    // Transfer the data in the controls to the member variables.
    UpdateData ();

    if (m_bNightsOk)
    {
        pTime1->SetWindowText (m_strNightTime);
        pTime2->SetWindowText (m_strNightTime);
    }
    else
    {
        pTime1->SetWindowText (m_strDayTime);
        pTime2->SetWindowText (m_strDayTime);
    }
}

CString CScheduleDlg::FormatTime (const CTime &Time) const
{
    const int        nOutLen = 32;
    auto  TCHAR      szOut[nOutLen];
    auto  SYSTEMTIME stSystemTime;

    memset (&stSystemTime, 0, sizeof (stSystemTime));
    stSystemTime.wHour   = Time.GetHour();
    stSystemTime.wMinute = Time.GetMinute();
    stSystemTime.wSecond = Time.GetSecond();

    int nResult = GetTimeFormat (LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS,
                                 &stSystemTime, _T("h:mm tt"), szOut, nOutLen);

    return szOut;
}

BOOL CScheduleDlg::ShouldDisplay()
{
    auto  BOOL       bShow      = TRUE;
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen = sizeof (szValueBuf);
    auto  HKEY       hBaseKey;
    auto  DWORD      dwKeyType;
    auto  CMyUtils   MyUtils;

    // If NPS already has a Scheduled LiveUpdate listed, we
    // should not display this dialog.
    if (MyUtils.HasNpsSchedLuEvent ())
        return FALSE;

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, g_szOptKey, 0, KEY_READ|KEY_WRITE, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, g_szOptShowSchedAddDlgValue,
                                              0, &dwKeyType, (LPBYTE)szValueBuf,
                                              &dwValueBufLen))
        {
            if (dwKeyType == REG_SZ && dwValueBufLen)
                bShow = (*szValueBuf != _T('0'));
        }
    }
    RegCloseKey(hBaseKey);

    return bShow;
}

void CScheduleDlg::SaveOptions ()
{
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen = sizeof (szValueBuf);
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;

    // Transfer the data in the controls to the member variables.
    UpdateData ();

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, g_szOptKey, 0,
                                       KEY_READ|KEY_WRITE, &hBaseKey))
    {
        if (m_bNotShowAgain)
            _tcscpy (szValueBuf, "0");
        else
            _tcscpy (szValueBuf, "1");
        dwValueBufLen = _tcslen (szValueBuf);
        dwValueType   = REG_SZ;

        RegSetValueEx (hBaseKey, g_szOptShowSchedAddDlgValue, 0, dwValueType,
                       (LPBYTE)szValueBuf, dwValueBufLen);
    }

    RegCloseKey(hBaseKey);
}

/////////////////////////////////////////////////////////////////////////////
// CScheduleDlg message handlers

BOOL CScheduleDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CComboBox *pDay1  = (CComboBox*)GetDlgItem (IDC_DAY1_CMB);
    CComboBox *pDay2  = (CComboBox*)GetDlgItem (IDC_DAY2_CMB);
    CWnd      *pTime1 = GetDlgItem (IDC_TIME1_EDT);
    CWnd      *pTime2 = GetDlgItem (IDC_TIME2_EDT);
    HICON     hIcon;

    // Initialize our random days and times once at the beginning of the
    // dialog.
    CalcRandomDays ();
    CalcRandomTimes ();

    SetInitialDays(m_bSchedBiMonthly);
    SetInitialTimes();

    // Initialize the help maps.
    InitHelpContextIds();

    // Set the titlebar icon.
    hIcon = (HICON)LoadImage(AfxGetInstanceHandle(),
                             MAKEINTRESOURCE(IDR_MAINFRAME),
                             IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

    SetIcon (hIcon, FALSE);     // Small Icon
    SetIcon (hIcon, TRUE);      // Large Icon

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CScheduleDlg::OnNightsOkChk() 
{
    SetInitialTimes();
}

void CScheduleDlg::OnOK() 
{
    UINT uErrStrId;
    uErrStrId = AddNpsEvents ();

    if (uErrStrId)
    {
        CString strErrTitle;
        CString strErr;

        strErrTitle.LoadString (IDS_CAPTION);
        strErr.LoadString      (uErrStrId);

        MessageBox (strErr, strErrTitle, MB_OK | MB_ICONSTOP);
        return;
    }

    SaveOptions ();
	CDialog::OnOK();
}

void CScheduleDlg::OnBimonthly() 
{
    auto  CWnd *pControls[] = {
                        GetDlgItem(IDC_DAY2_CMB),
                        GetDlgItem(IDC_TIME2_EDT),
                        GetDlgItem(IDC_SCHED2_TXT),
                        GetDlgItem(IDC_AT2_TXT) };

    const DWORD dwNumControls = sizeof(pControls)/sizeof(pControls[0]);
    auto  DWORD dwCurControl;
    auto  BOOL  bEnableWindow;

    // Transfer the data in the controls to the member variables.
    UpdateData ();

    bEnableWindow = m_bSchedBiMonthly;

    for (dwCurControl = 0; dwCurControl < dwNumControls; ++dwCurControl)
    {
        if (pControls[dwCurControl])
            pControls[dwCurControl]->EnableWindow (bEnableWindow);
    }

    SetInitialDays(m_bSchedBiMonthly);
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add some events to the scheduler (NPS). The actual settings depend
//   on user selections with this dialog.
//////////////////////////////////////////////////////////////////////////
UINT CScheduleDlg::AddNpsEvents ()
{
    auto  CMyUtils   MyUtils;
    auto  int        nResult;

    // Transfer the data in the controls to the member variables.
    UpdateData ();

    if (m_bSchedBiMonthly)
        nResult = MyUtils.AddNpsSchedLuEvents (m_bPrompt,
                                               m_nDay1+1, m_strTime1,
                                               m_nDay2+1, m_strTime2);
    else
        nResult = MyUtils.AddNpsSchedLuEvents (m_bPrompt, m_nDay1+1, m_strTime1);

    if (nResult == -1)
    {
        CWnd *pTime = GetDlgItem (IDC_TIME1_EDT);
        pTime->SetFocus ();

        return IDS_ERR_SCHED_TIME1_INVALID;
    }
    else if (nResult == -2)
    {
        CWnd *pTime = GetDlgItem (IDC_TIME2_EDT);
        pTime->SetFocus ();

        return IDS_ERR_SCHED_TIME2_INVALID;
    }

    // If the Scheduler is not running, ask the user if they want
    // it started now.
    if (!IsSchedulerRunning ())
    {
        CString strMsg;
        CString strCaption;

        strMsg.LoadString     (IDS_LAUNCH_NPS);
        strCaption.LoadString (IDS_CAPTION);

        if (IDYES == MessageBox (strMsg, strCaption, 
                                 MB_YESNO|MB_ICONQUESTION))
        {
            CString strNpsStartUpName;

            strNpsStartUpName.LoadString(IDS_NPS_STARTUP_NAME);
            StartScheduler (strNpsStartUpName);
        }
    }

    return 0;
}

BOOL CScheduleDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
    ASSERT (pHelpInfo->iContextType == HELPINFO_WINDOW);

    // Execute the help link in a popup window.
    WinHelp (pHelpInfo->dwContextId, HELP_CONTEXTPOPUP);

    // Help already processed ... do not propagate.
    return FALSE;
}

//------------------------------------------------------------------------
// Walk through the controls for this dialog and assign the help context
// Ids.
//------------------------------------------------------------------------
void CScheduleDlg::InitHelpContextIds()
{
    static DWORD pdwHelpMap[] = {
        IDC_SCHEDULE_GRPBOX,    IDH_NAVLU_SCHEDULING_GRPBX,
        IDC_DAY1_CMB,           IDH_NAVLU_LIVEUPDATE_WILL_LAUNCH_DATE_LSTBX,
        IDC_DAY2_CMB,           IDH_NAVLU_LIVEUPDATE_WILL_LAUNCH_DATE_LSTBX,
        IDC_TIME1_EDT,          IDH_NAVLU_START_DATE_TXTBX,
        IDC_TIME2_EDT,          IDH_NAVLU_START_DATE_TXTBX,
        IDC_NIGHTS_OK_CHK,      IDH_NAVLU_MACHINE_IS_LEFT_ON_OVERNIGHT_CHKBX,
        IDC_BIMONTHLY,          IDH_NAVLU_SCHEDULE_BI_MONTHLY_CHKBX,
        IDC_PROMPT,             IDH_NAVLU_PROMPT_BEFORE_LAUNCHING_CHKBX,
        IDC_NO_SHOW_AGAIN_CHK,  IDH_NAVLU_DO_NOT_SHOW_CHKBX,
        0, 0 };
    auto   int   nCur;
    auto   CWnd  *pWnd;

    for (nCur = 0; pdwHelpMap[nCur] != 0; nCur += 2)
    {
        pWnd = GetDlgItem(pdwHelpMap[nCur]);
        
        // If this asserts, that means that pdwHelpMap refers to a nonexistent
        // control.
        ASSERT (pWnd);
        
        pWnd->SetWindowContextHelpId (pdwHelpMap[nCur+1]);
    }
}

void CScheduleDlg::OnHelpBtn() 
{
	WinHelp (IDH_NAVLU_DLG_HLP_BTN);
}
