// GeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "GeneralPage.h"
#include "util.h"


// CGeneralPage dialog
IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPage)
    ON_BN_CLICKED(IDC_GENERAL_ENABLESCHEDULEDLU, &CGeneralPage::OnBnClickedGeneralScheduleEnable)
    ON_BN_CLICKED(IDC_GENERAL_SCHEDULE, &CGeneralPage::OnBnClickedGeneralScheduleDetails)
END_MESSAGE_MAP()

CGeneralPage::CGeneralPage()
	: CPropertyPage(CGeneralPage::IDD)
    , purgeNumber(false)
    , purgeUnits(false)
    , enableScheduleControls(true)
    , enableScheduleLiveupdate(FALSE)
{
    // Nothing for now
}

CGeneralPage::~CGeneralPage()
{
    // Nothing for now
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_GENERAL_PURGETIME, purgeNumber);
    DDX_Control(pDX, IDC_GENERAL_PURGESPINNER, purgeSpinnerCtrl);
    DDX_Control(pDX, IDC_GENERAL_PURGETIMEUNIT, purgeUnitsCtrl);
    DDX_CBIndex(pDX, IDC_GENERAL_PURGETIMEUNIT, purgeUnits);
    DDX_Check(pDX, IDC_GENERAL_ENABLESCHEDULEDLU, enableScheduleLiveupdate);
    DDX_Control(pDX, IDC_GENERAL_SCHEDULE, scheduleCtrl);
    DDX_Control(pDX, IDC_GENERAL_SCHEDULEDESCRIPTION, scheduleDescriptionCtrl);
}

// CGeneralPage message handlers
BOOL CGeneralPage::OnInitDialog()
{
    HKEY        keyHandle                       = NULL;
    DWORD       returnValDW                     = ERROR_OUT_OF_PAPER;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CPropertyPage::OnInitDialog();

    // Configure spinner to work as expected (vs the default)
    purgeSpinnerCtrl.SetRange(0, 9999);

    // Purge:  configure units
    CString     unitText;
    unitText.LoadString(IDS_GENERAL_PURGE_DAYS);
    purgeUnitsCtrl.AddString(unitText);
    unitText.LoadString(IDS_GENERAL_PURGE_MONTHS);
    purgeUnitsCtrl.AddString(unitText);
    unitText.LoadString(IDS_GENERAL_PURGE_YEARS);
    purgeUnitsCtrl.AddString(unitText);
    purgeUnits = 0;

    // Read in the configured values
    // Purge history
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        // Purge number
        RegQueryInt(keyHandle, _T(szReg_Val_LogRollOverDays), &purgeNumber, 30);
        // Purge units
        RegQueryInt(keyHandle, _T(szReg_Val_LogFrequency), &purgeUnits, 30);

        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    purgeSpinnerCtrl.SetPos(purgeUnits);
    // Scheduled LiveUpdate
    LoadScheduledLiveupdate();

    // Save data to dialog
    UpdateData(FALSE);
    OnBnClickedGeneralScheduleEnable();

    return TRUE;
}

void CGeneralPage::OnOK()
{
    // Read in the configured values
    DWORD       returnValDW         = ERROR_OUT_OF_PAPER;
    HKEY        keyHandle           = NULL;
    
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), NULL, KEY_SET_VALUE, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        // Purge number (days, months, etc.)
        RegSetValueEx(keyHandle, _T(szReg_Val_LogRollOverDays), NULL, REG_DWORD, (LPBYTE) &purgeNumber, sizeof(purgeNumber));
        // Purge units
        RegSetValueEx(keyHandle, _T(szReg_Val_LogFrequency), NULL, REG_DWORD, (LPBYTE) &purgeUnits, sizeof(purgeUnits));

        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }

    // Scheduled LiveUpdate
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_PatternManager) _T("\\") _T(szReg_Key_ScheduleKey), NULL, KEY_SET_VALUE, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        RegSetValueEx(keyHandle, _T(szReg_Val_Enabled), NULL, REG_DWORD, (LPBYTE) &enableScheduleLiveupdate, sizeof(enableScheduleLiveupdate));
    }
}

void CGeneralPage::OnBnClickedGeneralScheduleDetails()
{
    UpdateData(TRUE);
    scheduleDlg.DoModal();
    LoadScheduledLiveupdate();
    // Technically unnecessary but good form
    OnBnClickedGeneralScheduleEnable();
}

void CGeneralPage::OnBnClickedGeneralScheduleEnable()
{
    UpdateData(TRUE);
    scheduleCtrl.EnableWindow(enableScheduleControls && enableScheduleLiveupdate);
    scheduleDescriptionCtrl.EnableWindow(enableScheduleControls && enableScheduleLiveupdate);
}

void CGeneralPage::LoadScheduledLiveupdate( void )
// Loads settings for scheduled LiveUpdate and updates the UI, but does not call UpdateData(FALSE)
{
    HKEY        keyHandle               = NULL;
    CString     scheduleDescription;
    bool        disableScheduledLiveupdateControls  = false;
    DWORD       returnValDW             = ERROR_OUT_OF_PAPER;

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_PatternManager) _T("\\") _T(szReg_Key_ScheduleKey), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        RegQueryBOOL(keyHandle, _T(szReg_Val_Enabled), &enableScheduleLiveupdate, FALSE);
        if (enableScheduleLiveupdate)
            scheduleCtrl.SetCheck(BST_CHECKED);
        else
            scheduleCtrl.SetCheck(BST_UNCHECKED);
        GetScheduleDescription(scheduleDescription);
        scheduleDescriptionCtrl.SetWindowTextW(scheduleDescription);
    }

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_PatternManager), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        RegQueryBool(keyHandle, _T(szReg_Val_DisallowClientScheduling), &disableScheduledLiveupdateControls, true);
        enableScheduleControls = !disableScheduledLiveupdateControls;
    }
}

void CGeneralPage::GetDayOfWeekName( DWORD dayID, LPTSTR nameBuffer, DWORD nameBufferSize )
// Sets *nameBuffer equal to the name of the specified day.  DayID is the number of a day within the week,
// starting with Sunday equal to 0.
{
    TCHAR tempNameBuffer[20] = _T("");

    switch (dayID)
    {
    case 1:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_MONDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 2:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_TUESDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 3:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_WEDNESDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 4:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_THURSDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 5:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_FRIDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 6:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_SATURDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    case 0:
        LoadString(AfxGetInstanceHandle(), IDS_DAYNAME_SUNDAY, tempNameBuffer, sizeof(tempNameBuffer)/sizeof(tempNameBuffer[0]));
        break;
    default:
        break;
    }

    wcsncpy_s(nameBuffer, nameBufferSize, tempNameBuffer, _TRUNCATE);
}

void CGeneralPage::GetScheduleDescription( CString &scheduleDescription  )
// Sets scanDescription equal to a textual description of the liveupdate schedule
{
    HKEY            keyHandle           = NULL;
    DWORD           scheduleType        = 0;
    DWORD           minutes             = 0;
    DWORD           dayOfMonth          = 0;
    DWORD           dayOfWeek           = 0;
    bool            randomDayEnabled    = false;
    bool            randomWeekEnabled   = false;
    bool            randomMonthEnabled  = false;
    DWORD           randomizeDayRange   = 0;
    DWORD           randomizeWeekStart  = 0;
    DWORD           randomizeWeekEnd    = 0;
    DWORD           randomizeMonthRange = 0;
    TCHAR           dayName[20]         = _T("");
    SYSTEMTIME      scanTime;
    COleDateTime    time;
    CString         timeString;
    TCHAR           startDayName[20]    = _T("");
    TCHAR           endDayName[20]      = _T("");
    DWORD           returnValDW         = ERROR_OUT_OF_PAPER;

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_PatternManager) _T("\\") _T(szReg_Key_ScheduleKey), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        RegQueryDword(keyHandle, _T(szReg_Val_HowOften), &scheduleType, SCHEDULE_DAILY);
        RegQueryDword(keyHandle, _T(szReg_Val_MinOfDay), &minutes, 720);
        RegQueryDword(keyHandle, _T(szReg_Val_DayOfWeek), &dayOfWeek, 0);
        RegQueryDword(keyHandle, _T(szReg_Val_DayOfMonth), &dayOfMonth, 1);

        RegQueryBool(keyHandle, _T(szReg_Val_RandomizeDay_Enabled), &randomDayEnabled, false);
        RegQueryBool(keyHandle, _T(szReg_Val_RandomizeWeek_Enabled), &randomWeekEnabled, false);
        RegQueryBool(keyHandle, _T(szReg_Val_RandomizeMonth_Enabled), &randomMonthEnabled, false);

        RegQueryDword(keyHandle, _T(szReg_Val_Randomize_Range), &randomizeDayRange, 60);
        RegQueryDword(keyHandle, _T(szReg_Val_Randomize_WeekStart), &randomizeWeekStart, 1);
        RegQueryDword(keyHandle, _T(szReg_Val_Randomize_WeekEnd), &randomizeWeekEnd, 5);
        RegQueryDword(keyHandle, _T(szReg_Val_RandomizeMonth_Range), &randomizeMonthRange, 60);

        // Format the time string
        GetDayOfWeekName(dayOfWeek, dayName, sizeof(dayName)/sizeof(dayName[0]));
        time.SetTime(minutes/60, minutes%60, 0);
        ZeroMemory(&scanTime, sizeof(scanTime));
        scanTime.wHour = time.GetHour();
        scanTime.wMinute = time.GetMinute(); 
        GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &scanTime, NULL, timeString.GetBuffer(12), 12);
        timeString.ReleaseBuffer();

        if (scheduleType >= 0)
        {
            if (!timeString.IsEmpty())
            {
                switch (scheduleType)
                {
                    case SCHEDULE_DAILY:
                        if (randomDayEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_DAILY_RANDOM, randomizeDayRange, timeString);
                        else
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_DAILY, timeString);
                        break;
                    case SCHEDULE_WEEKLY:
                        GetDayOfWeekName(randomizeWeekStart, startDayName, sizeof(startDayName)/sizeof(startDayName[0]));
                        GetDayOfWeekName(randomizeWeekEnd, endDayName, sizeof(endDayName)/sizeof(endDayName[0]));

                        if (randomDayEnabled && randomWeekEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_WEEKLY_RANDOM_DAYMINUTES, startDayName, endDayName, randomizeDayRange, timeString);
                        else if (randomWeekEnabled && !randomDayEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_WEEKLY_RANDOM_DAY, timeString, startDayName, endDayName );
                        else if (randomDayEnabled && !randomWeekEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_WEEKLY_RANDOM_MINUTES, dayName, randomizeDayRange, timeString);
                        else
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_WEEKLY, dayName, timeString);
                        break;
                    case SCHEDULE_MONTHLY:
                        if (randomDayEnabled && randomMonthEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_MONTHLY_RANDOM_DAYMINUTES, randomizeMonthRange, dayOfMonth, randomizeDayRange, timeString);
                        else if (randomMonthEnabled && !randomDayEnabled)
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_MONTHLY_RANDOM_DAY, randomizeMonthRange, dayOfMonth, timeString);
                        else if (randomDayEnabled && !randomMonthEnabled )
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_MONTHLY_RANDOM_MINUTES, dayOfMonth, randomizeDayRange, timeString);
                        else
                            scheduleDescription.FormatMessage(IDS_DEFUPDATEFORMAT_MONTHLY, dayOfMonth, timeString);
                        break;
                }
            }
        }

        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
}