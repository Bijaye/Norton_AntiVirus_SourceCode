#pragma once

#include "stdafx.h"
#include "SavConfigureableScan.h"


// CScanWhatToScanPage dialog
class CScanWhatToScanPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanWhatToScanPage)

public:
	CScanWhatToScanPage();
	virtual ~CScanWhatToScanPage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_WHATTOSCAN };

	// Sets the scan to configure
	void SetScan( CSavConfigureableScan* newScan );
	void SetReadOnly( bool newIsReadOnly );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	DECLARE_MESSAGE_MAP()
private:
	CSavConfigureableScan*		scan;
	bool						isReadOnly;

	CStatic						quickScanDescriptionCtrl;
	CButton						quickScanTitle;
	CButton						customScanTitle;
	CButton						fullScanTitle;
	CFont						titleFont;
	DWORD						whatToScanDW;
};