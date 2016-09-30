////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NAVEventFactoryLoader.h"
#include "AVInterfaces.h"
#include "AvProdLoggingInterface.h"

class CAutoProtectClientActivity : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProd::AvModuleLogging::IAutoProtectClientActivities
{
public:
    CAutoProtectClientActivity(void);
    virtual ~CAutoProtectClientActivity(void);

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(AvProd::AvModuleLogging::IID_AutoProtectClientActivities, AvProd::AvModuleLogging::IAutoProtectClientActivities)
    SYM_INTERFACE_MAP_END()    

public: // IAutoProtectClientActivities interface
    virtual HRESULT Initialize() throw();
    virtual HRESULT OnAutoProtectRemediationComplete(AVModule::IAVMapDwordData* pActivityData) throw();
    virtual HRESULT OnAutoProtectThreatBlocked(AVModule::IAVMapDwordData* pActivityData) throw();

protected:
    AV::IAvEventFactoryPtr  m_spLogger;

};


class CScanClientActivityBase 
{
public:
    CScanClientActivityBase(void);
    virtual ~CScanClientActivityBase(void);

protected: // methods
	virtual HRESULT Initialize() throw();
    virtual HRESULT LogThreatsFound(const REFGUID guidScanInstance);
    virtual HRESULT LogThreatFound(AVModule::IAVMapDwordData* pActivityData, AVModule::IAVArrayData* pArrDispInfo = NULL) throw();

protected:
    AV::IAvEventFactoryPtr  m_spLogger;
	
	long m_lFeature;
	
};

class CManualScanClientActivity : 
	CScanClientActivityBase,
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public AvProd::AvModuleLogging::IManualScanClientActivities
{
public:
	CManualScanClientActivity(void);
	virtual ~CManualScanClientActivity(void);

public:
	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(AvProd::AvModuleLogging::IID_ManualScanClientActivities, AvProd::AvModuleLogging::IManualScanClientActivities)
	SYM_INTERFACE_MAP_END()    

public: // IManualScanClientActivities interface
	virtual HRESULT Initialize() throw();
	virtual HRESULT SetScanResultByActivityData(AVModule::IAVMapStrData* pResults) throw();
	virtual HRESULT SetScanResultByProperty(DWORD dwPropertyId, DWORD dwValue) throw();
	virtual HRESULT SetScanResultByCategory(DWORD dwPropertyId, DWORD dwCategoryId, DWORD dwValue) throw();
	virtual HRESULT SetEraserScanResults(DWORD dwPropertyId, DWORD dwValue) throw();
	virtual HRESULT SetScanType(DWORD dwAvModuleScanType) throw(); // see AvDefines.h (eScanType)
	virtual HRESULT SetScanType(LPCWSTR wszScanType) throw();
	virtual HRESULT SetScanResult(DWORD dwResult) throw();
	virtual HRESULT SetScanInstanceId(REFGUID guidScanInstance) throw();
	virtual HRESULT Commit() throw();

	virtual HRESULT LogManualScanResults(REFGUID guidScanInstance) throw();

protected:
	CEventData m_AvActivity;

};

class CEmailScanClientActivity : 
	CScanClientActivityBase,
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProd::AvModuleLogging::IEmailScanClientActivities
{
public:
    CEmailScanClientActivity(void);
    virtual ~CEmailScanClientActivity(void);

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(AvProd::AvModuleLogging::IID_EmailScanClientActivities, AvProd::AvModuleLogging::IEmailScanClientActivities)
    SYM_INTERFACE_MAP_END()    

public: // IManualScanClientActivities interface
    virtual HRESULT Initialize() throw();
    virtual HRESULT OnEmailThreatsDetected(AVModule::IAVMapStrData* pActivityData) throw();
    virtual HRESULT OnOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD dwOEHAction) throw();
    virtual HRESULT OnEmailScanComplete(AVModule::IAVMapStrData* pResults) throw();
	virtual HRESULT LogEmailScanResults(REFGUID guidScanInstance) throw();

};

class CThreatDataEz : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProd::AvModuleLogging::IThreatDataEz
{
public:
    CThreatDataEz(void);
    virtual ~CThreatDataEz(void);

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(AvProd::AvModuleLogging::IID_ThreatDataEz, AvProd::AvModuleLogging::IThreatDataEz)
    SYM_INTERFACE_MAP_END()    

public: // IThreatDataEz interface
    virtual HRESULT GetCategoriesString(AVModule::IAVMapDwordData* pThreatTrackInfo, cc::IString*& pCategoriesString) throw();

protected:

};

