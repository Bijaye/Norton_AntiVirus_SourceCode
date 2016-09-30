// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScheduleEnableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wprotect32.h"
#include "ScheduleEnableDlg.h"
#include "ScheduleDialog.h"
#include "ClientReg.h"
#include "admininfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScheduleEnableDlg dialog


CScheduleEnableDlg::CScheduleEnableDlg(IConfig *pConfig, CWnd* pParent /*=NULL*/)
	: CDialog(CScheduleEnableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleEnableDlg)
	m_bScheduleEnable = FALSE;
	//}}AFX_DATA_INIT
	if( pConfig )
		(m_ptrConfig = pConfig)->AddRef();
	else
		m_ptrConfig	= NULL;
}

CScheduleEnableDlg::~CScheduleEnableDlg()
{
	if( m_ptrConfig  )
		m_ptrConfig->Release();
}

void CScheduleEnableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleEnableDlg)
	DDX_Control(pDX, IDC_SCHEDULE, m_ctlScheduleBtn);
	DDX_Control(pDX, IDC_SCHEDULE_DESCRIPTION, m_ctlDescription);
	DDX_Check(pDX, IDC_ENABLE_SCHEDULING, m_bScheduleEnable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleEnableDlg, CDialog)
	//{{AFX_MSG_MAP(CScheduleEnableDlg)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_BN_CLICKED(IDC_ENABLE_SCHEDULING, OnEnableScheduling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleEnableDlg message handlers

BOOL CScheduleEnableDlg::OnInitDialog() 
{
    CString sDescription;
    BOOL        bNormalUser = FALSE;
    CAdminInfo  AdminInfo;

    // See if we're a normal user (i.e. without reg write access)
    m_bNormalUser = !AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );

	CDialog::OnInitDialog();
	
	m_ConfigObject.SetConfig(m_ptrConfig);

    // Pull the enabled status out of the "Schedule" key
	m_ConfigObject.SetSubOption(szReg_Key_ScheduleKey);
	m_bScheduleEnable = m_ConfigObject.GetOption(szReg_Val_Enabled, 1);
	m_ConfigObject.SetSubOption("");
    
    GetScanDescription(sDescription);
    m_ctlDescription.SetWindowText(sDescription);

    
    UpdateData(FALSE);

    if (m_bNormalUser || m_bReadonly)
		ProtectScheduling();
	else
		OnEnableScheduling();

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScheduleEnableDlg::OnOK() 
{
	// don't try to save the values if we can't!
	if (!m_bNormalUser && !m_bReadonly)
	{
	time_t tCreate = time(NULL);

	m_ConfigObject.SetSubOption(szReg_Key_ScheduleKey);
    m_ConfigObject.SetOption(szReg_Val_Enabled, m_bScheduleEnable);

	// Stamp the create time of the scheduled event and reset
	// the last start value if its enabled.
	if( m_bScheduleEnable )
	{
		m_ConfigObject.SetOption(szReg_Val_Schedule_Created, (DWORD)tCreate);
		m_ConfigObject.SetOption(szReg_Val_Schedule_LastStart, (DWORD)0);
		m_ConfigObject.SetOption(szReg_Val_Schedule_SkipEvent, (DWORD)0);
	}

	m_ConfigObject.SetSubOption("");
	}

	CDialog::OnOK();
}

void CScheduleEnableDlg::OnSchedule() 
{
    CString sDescription;
    CScheduleDialog dlg(m_ptrConfig, NULL);

    dlg.DoModal();

    GetScanDescription(sDescription);
    m_ctlDescription.SetWindowText(sDescription);
}

void CScheduleEnableDlg::OnEnableScheduling() 
{
    {
        UpdateData(TRUE);
        m_ctlDescription.EnableWindow(m_bScheduleEnable);
        m_ctlScheduleBtn.EnableWindow(m_bScheduleEnable);
    }
}

void CScheduleEnableDlg::ProtectScheduling() 
{
    CWnd*   pWnd = NULL;

    UpdateData(FALSE);
    m_ctlDescription.EnableWindow(FALSE);
    m_ctlScheduleBtn.EnableWindow(FALSE);

    // Disable the check box too
    pWnd = GetDlgItem(IDC_ENABLE_SCHEDULING);
    pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDOK);
    pWnd->EnableWindow(FALSE);
}

void CScheduleEnableDlg::DayFromID( DWORD id, TCHAR* szBuf )
{
	TCHAR szTemp[20];

	switch(id)
	{
	case 1:
		LoadString(AfxGetInstanceHandle(), IDS_MONDAY, szTemp, 20 );
		break;
	case 2:
		LoadString(AfxGetInstanceHandle(), IDS_TUESDAY, szTemp, 20);
		break;
	case 3:
		LoadString(AfxGetInstanceHandle(), IDS_WEDNESDAY, szTemp, 20);
		break;
	case 4:
		LoadString(AfxGetInstanceHandle(), IDS_THURSDAY, szTemp, 20);
		break;
	case 5:
		LoadString(AfxGetInstanceHandle(), IDS_FRIDAY, szTemp, 20);
		break;
	case 6:
		LoadString(AfxGetInstanceHandle(), IDS_SATURDAY, szTemp, 20);
		break;
	case 0:
		LoadString(AfxGetInstanceHandle(), IDS_SUNDAY, szTemp, 20);
		break;
	default:
		break;
	}

	_tcscpy( szBuf, szTemp );

}

void CScheduleEnableDlg::GetScanDescription(CString &sScanDescription)
{
	CString			sTime;
	CString			sType;
	CString			sFmt;
	CString			sDay;
	COleDateTime	time;
	DWORD			dwScheduleType;
	DWORD			dwMinutes;
	DWORD			dwDayOfMonth;
	DWORD			dwDayOfWeek;
    BOOL            bRandomDayEnabled;
	BOOL			bRandomWeekEnabled;
	BOOL			bRandomMonthEnabled;
    DWORD           dwRandomDayRange;
	DWORD			dwRandomWeekStart;
	DWORD			dwRandomWeekEnd;
	DWORD			dwRandomMonthRange;
	SYSTEMTIME		sysTime;

	m_ConfigObject.SetSubOption(szReg_Key_ScheduleKey);

	//Read in the type, time, day of week and day of month
	dwScheduleType = m_ConfigObject.GetOption(szReg_Val_HowOften, SCHEDULE_DAILY);
	dwMinutes = m_ConfigObject.GetOption(szReg_Val_MinOfDay, 720);
	dwDayOfWeek = m_ConfigObject.GetOption(szReg_Val_DayOfWeek, 0);
	dwDayOfMonth = m_ConfigObject.GetOption(szReg_Val_DayOfMonth, 1);
	sDay = GetDayName(dwDayOfWeek);

    // Read in randomization options
    bRandomDayEnabled = m_ConfigObject.GetOption(szReg_Val_RandomizeDay_Enabled, FALSE);
	bRandomWeekEnabled = m_ConfigObject.GetOption(szReg_Val_RandomizeWeek_Enabled, FALSE);
	bRandomMonthEnabled = m_ConfigObject.GetOption(szReg_Val_RandomizeMonth_Enabled, FALSE);

    dwRandomDayRange = m_ConfigObject.GetOption(szReg_Val_Randomize_Range, 60);
	dwRandomWeekStart = m_ConfigObject.GetOption(szReg_Val_Randomize_WeekStart, 1);
	dwRandomWeekEnd = m_ConfigObject.GetOption(szReg_Val_Randomize_WeekEnd, 5);
	dwRandomMonthRange = m_ConfigObject.GetOption(szReg_Val_RandomizeMonth_Range, 60);

	//Convert the minutes into a COleDateTime
	time.SetTime( dwMinutes / 60, dwMinutes % 60, 0 );

	// Format the time string
	ZeroMemory(&sysTime, sizeof(SYSTEMTIME));
	sysTime.wHour = time.GetHour();
	sysTime.wMinute = time.GetMinute(); 
    GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL,//"hh':'mm tt",
					sTime.GetBuffer(12), 12);
	sTime.ReleaseBuffer();
 
	if ( dwScheduleType >= 0 )
	{
		if(!sTime.IsEmpty())
        {
			switch( dwScheduleType )
			{
				case SCHEDULE_DAILY:
					if(bRandomDayEnabled)
					{
						sScanDescription.Format(IDS_UPDATE_DAILY_RANDOM, dwRandomDayRange, sTime);
					}
					else
					{
						sScanDescription.Format(IDS_UPDATE_DAILY, sTime);
					}
					break;
				case SCHEDULE_WEEKLY:
					// Get the day of week
					TCHAR sStart[20];
					TCHAR sEnd[20];

					DayFromID(dwRandomWeekStart, sStart);
					DayFromID(dwRandomWeekEnd, sEnd);

					if(bRandomDayEnabled && bRandomWeekEnabled)
					{
						sScanDescription.Format(IDS_PATTERN_PROMPT_WEEK_AND_DAY_RANDOM, sStart, sEnd, dwRandomDayRange, sTime);
					}
					else if(bRandomWeekEnabled && !bRandomDayEnabled)
					{
						sScanDescription.Format(IDS_PATTERN_PROMPT_RANDOM_WEEKDAY, sTime, sStart, sEnd );
					}
					else if(bRandomDayEnabled && !bRandomWeekEnabled)
					{
						sScanDescription.Format(IDS_UPDATE_WEEKLY_RANDOM, sDay, dwRandomDayRange, sTime);
					}
					else
					{
						sScanDescription.Format(IDS_UPDATE_WEEKLY, sDay, sTime);
					}
					break;
				case SCHEDULE_MONTHLY:
					if(bRandomDayEnabled && bRandomMonthEnabled)
					{
						// random daily and day of week
						sScanDescription.Format(IDS_PATTERN_PROMPT_MONTH_AND_DAY_RANDOM, dwRandomMonthRange, dwDayOfMonth, dwRandomDayRange, sTime);
					}
					else if(bRandomMonthEnabled && !bRandomDayEnabled)
					{
						sScanDescription.Format(IDS_PATTERN_PROMPT_RANDOM_MONTHDAY, dwRandomMonthRange, dwDayOfMonth, sTime);
					}
					else if( bRandomDayEnabled && !bRandomMonthEnabled )
					{
						sScanDescription.Format(IDS_UPDATE_MONTHLY_RANDOM, dwDayOfMonth, dwRandomDayRange, sTime);
					}
					else
					{
						sScanDescription.Format(IDS_UPDATE_MONTHLY, dwDayOfMonth, sTime);
					}
					break;
			}
        }
	}
	m_ConfigObject.SetSubOption("");
}
