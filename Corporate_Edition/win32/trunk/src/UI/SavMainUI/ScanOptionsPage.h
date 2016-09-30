#pragma once

#include "stdafx.h"
#include "FileSystemUI.h"
#include "SavConfigureableScan.h"


// CScanOptionsPage dialog
class CScanOptionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanOptionsPage)

public:
	CScanOptionsPage();
	virtual ~CScanOptionsPage();

// Dialog Data
	enum { IDD = IDD_SCANS_CONFIGPAGE_OPTIONS };

    // Set the IConfig object for where the scan's data is stored
    void SetScan( CSavConfigureableScan* newScan );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	DECLARE_MESSAGE_MAP()
private:
	// NOTE:  This control uses the same CLSID as the FileSystem AutoProtect control, but is configured differently
	// via the DLGINIT to show scan options instead
    CFileSystemUI				optionsCtrl;
    CSavConfigureableScan*		scan;
};