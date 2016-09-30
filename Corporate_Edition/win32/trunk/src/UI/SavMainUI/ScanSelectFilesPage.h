#pragma once

#include "stdafx.h"
#include "ShelSel2.h"
#include "SavConfigureableScan.h"


// CScanSelectFilesPage dialog
class CScanSelectFilesPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanSelectFilesPage)

public:
	CScanSelectFilesPage();
	virtual ~CScanSelectFilesPage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_FILES };

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
    CShelsel2				selectorCtrl;
	CSavConfigureableScan*	scan;

	CStatic					typeTitleCtrl;
	CStatic					typeDescriptionCtrl;
};
