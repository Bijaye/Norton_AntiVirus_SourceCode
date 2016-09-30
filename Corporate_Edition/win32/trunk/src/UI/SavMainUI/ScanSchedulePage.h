#pragma once

#include "stdafx.h"
#include "ldtime.h"
#include "ldvpschedule.h"
#include "ldvpschedule2.h"
#include "SavConfigureableScan.h"


// CScanSchedulePage dialog
class CScanSchedulePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanSchedulePage)

public:
	CScanSchedulePage();
	virtual ~CScanSchedulePage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_SCHEDULE };

    // Set the IConfig object for where the scan's data is stored
    void SetScan( CSavConfigureableScan* newScan );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	DECLARE_MESSAGE_MAP()

private:
	CSavConfigureableScan*		scan;

	bool						useResumableScan;
	bool						scheduleControlInitialized;
    CLDVPSchedule*				scheduleCtrl;
	CLDVPSchedule2*				scheduleResumeableScanCtrl;
};