////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//
// This class is to handle components that report error states in real-time,
// or need to be checked on login.
// We report the errors when the user logs in so that we know we have a
// UI desktop up. We also do the logging for the components here.
//
//
#include "AvInterfaces.h" // AntiVirus component
#include "AvInterfaceLoader.h"
#include "SyncQueue.h"

struct ERROR_DATA
{
    DWORD dwErrorID;
    DWORD dwModuleID;
    DWORD dwResourceID;
    HRESULT hr;
};

typedef std::queue<ERROR_DATA> QUEUE_ERROR_DATA;

class CErrorHandling :
    public CSyncQueue<ERROR_DATA>,  // thread and queue
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AVModule::IAVAutoProtectOptionsChange // does the updates
{
public:
    CErrorHandling(void);
    ~CErrorHandling(void);

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(AVModule::IID_AVAutoProtectOptionsChange, AVModule::IAVAutoProtectOptionsChange)
    SYM_INTERFACE_MAP_END()

// IAVAutoProtectOptionsChange
    // Auto-Protect state change notification
    void OnAPStateChange(AVModule::AutoProtect::FeatureStateAP eCurState) throw();

    void Notify(const wchar_t* pcwszOptName, DWORD dwNewValue) throw(){};

    // New ERROR_DATA to display
    void processQueue();

	// CThread override's
	BOOL InitInstance() throw();
	void ExitInstance() throw();


protected:
	AVModule::AVLoader_IAVAutoProtect m_APLoader;
    AVModule::IAVAutoProtectPtr m_spAP;
    AVModule::AutoProtect::FeatureStateAP m_eStateAP;

    void CheckAP();
    void CheckIDS();

    QUEUE_ERROR_DATA m_qErrors;
};
