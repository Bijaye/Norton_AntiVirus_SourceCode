#pragma once

#include "stdafx.h"
#include "SavConfigureableScan.h"


// CScanNamePage dialog
class CScanNamePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanNamePage)

public:
	CScanNamePage();
	virtual ~CScanNamePage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_NAME };

    // Set the scan object we're changing
    void SetScan( CSavConfigureableScan* newScanConfig );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
	virtual BOOL OnWizardFinish();
    virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	void ValidateName();
	DECLARE_MESSAGE_MAP()
private:
	CSavConfigureableScan*		scan;
	CString						name;
	CString						description;
	CString						whenRun;
};