#pragma once

#include <vector>
#include "ccAlertInterface.h"
#include "NAVHelpLauncher.h"
#include "ccAlertLoader.h"      // For loading the ccAlert.dll

class CAlertUI : public cc::IAlertCallback,
				 public cc::IAlertAcceleratorCallback, 
				 public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CAlertUI(void);
    ~CAlertUI(void);

// IAlertCallback methods
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(cc::IID_AlertCallback, IAlertCallback)
    SYM_INTERFACE_MAP_END()
    virtual bool Run (  HWND hWndParent, 
                        unsigned long nData,
                        cc::IAlert* pAlert,
                        cc::IAlertCallback::ALERTCALLBACK context);

// IAlertAcceleratorCallback methods
	virtual bool HandleAccelerators(HWND hWndParent, WORD keyCode, DWORD& comboSelection, bool& closeDialog);

private:
    CAlertUI& operator=(const CAlertUI&) throw();
    CAlertUI(const CAlertUI&) throw(); 

public:
    DWORD dwHelpID;           // NAV Help ID
    //DWORD dwExcludeProcID;    // Proc ID to exclude from attaching thead input to
                              // Used to insure the alert code does not deadlock by attaching to the
                              // Process generating the alert traffic

    virtual int DisplayAlert (HWND hParentWnd = NULL, HWND hwndInsertAfter = HWND_TOP);

    // Allow pass through access to the IAlert
    //
    cc::IAlert* GetAlert () { return m_pAlert; };

    // For action items (combo box) with accelerators - e.g. "&Permit"
    bool AddActionAccelerator ( UINT uResID );

protected:
    void initializeAlert();

	//Accelerators for the drop down menu. These are in the exact same order as the drop down
	//menu. If a menu item does not have an accelerator, a value of 0 is inserted for that menu.
	std::vector<WORD>	m_accelerators;

    // Make sure this factory is always declared before the interface
    cc::ccAlert_IAlert AlertFactory;
    cc::IAlertPtr m_pAlert;

    static CString csSARCFormat;
	static CString csProductName;
};
