#pragma once

#include "stdafx.h"
#include "SavConfigureableScan.h"


// CScanWhenToScanPage dialog
class CScanWhenToScanPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanWhenToScanPage)

public:
	CScanWhenToScanPage();
	virtual ~CScanWhenToScanPage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_WHENRUN };

	// Sets the scan to configure
	void SetScan( CSavConfigureableScan* newScan );
	void SetReadOnly( bool newIsReadOnly );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardNext();
	BOOL OnSetActive();
	DECLARE_MESSAGE_MAP()
private:
	CSavConfigureableScan*		scan;
	bool						isReadOnly;
};