#pragma once

#include "stdafx.h"
#include "afxwin.h"
#include "VirusDefUpdateScheduleDlg.h"

// CGeneralPage dialog
class CGeneralPage : public CPropertyPage
{
    DECLARE_DYNAMIC(CGeneralPage)

public:
	CGeneralPage();
	virtual ~CGeneralPage();

// Dialog Data
	enum { IDD = IDD_CONFIGPAGE_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedGeneralScheduleDetails();
    afx_msg void OnBnClickedGeneralScheduleEnable();

    DECLARE_MESSAGE_MAP()

private:
    // ** FUNCTION MEMBERS **
    // Loads settings for scheduled LiveUpdate and updates the UI, but does not call UpdateData(FALSE)
    void LoadScheduledLiveupdate( void );
    // Sets scanDescription equal to a textual description of the liveupdate schedule
    void GetScheduleDescription( CString &scheduleDescription );
    // Sets *nameBuffer equal to the name of the specified day.  DayID is the number of a day within the week, starting with
    // Sunday equal to 0.
    void GetDayOfWeekName( DWORD dayID, LPTSTR nameBuffer, DWORD nameBufferSize );

    // ** CONSTANTS **
    static const int SCHEDULE_NONE = 0;
    static const int SCHEDULE_DAILY = 1;
    static const int SCHEDULE_WEEKLY = 2;
    static const int SCHEDULE_MONTHLY = 3;

    // ** DATA MEMBERS **
    // Purge history
    int                 purgeNumber;
    int                 purgeUnits;
    CSpinButtonCtrl     purgeSpinnerCtrl;
    CComboBox           purgeUnitsCtrl;

    // Scheduled LiveUpdate
    bool                            enableScheduleControls;
    BOOL                            enableScheduleLiveupdate;
    CButton                         scheduleCtrl;
    CStatic                         scheduleDescriptionCtrl;
    CVirusDefUpdateScheduleDlg      scheduleDlg;
};
