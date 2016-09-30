// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AllNAVEvents.h"
#include "NAVEventFactoryLoader.h"
#include "NAVInfo.h"        // toolbox
#include "NAVEventCommon.h"

#include "StockFrameworkObjectsInterface.h"
#include "MCFWidgets.h"

#include "MessageImpl.h"

#include "StockCategory.h"
#include "StockLists.h"

#include "AVNamesMap.h"
#include "ThreatCatInfo.h"

#include "AvDataObjects.h"
#include "AvProdLoggingACP.h"

#include <AvInterfaceLoader.h>


class CAntivirusMessageBase : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public MCF::CMessageImpl
{
public:
    CAntivirusMessageBase(void){};
    virtual ~CAntivirusMessageBase(void){};

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(MCF::IID_Message, MCF::IMessage)
    SYM_INTERFACE_MAP_END()    

public:
    enum { eInvalid = 0, eAutoProtectResults, eManualScanResults, eAvModuleItem, eEmailScanResults, eOEHDetection };

protected:
    HRESULT CreateTimeData(const SYMGUID &guidDataId, const SYSTEMTIME& stTime);
    HRESULT CreateTimeData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData);
    HRESULT CreateTimeData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData);
    HRESULT CreateTimeData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData);
    
    HRESULT CreateStringData(const SYMGUID &guidDataId, LPCWSTR szString);
    HRESULT CreateStringData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData);
    HRESULT CreateStringData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData);
    HRESULT CreateStringData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData);
    
    HRESULT CreateNumberData(const SYMGUID &guidDataId, LONGLONG qdwNumber);
    HRESULT CreateNumberData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData);
    HRESULT CreateNumberData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData);
    HRESULT CreateNumberData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData);

public:
    static CAVFeatureNameMap m_Features; 
	static AvProdLogging::CAvProdLoggingACP m_AccessControl;
    static CThreatCatInfo m_ThreatInfo; // Cache object for performance. FreeLibrary in this obj dtor is expensive on high call volume.
};

class CThreatActivityMessage : 
    public CAntivirusMessageBase,
    public MCF::CSymObjectCreator<&MCF::IID_Message, MCF::IMessage, CThreatActivityMessage>
{
public:
    CThreatActivityMessage(void);
    virtual ~CThreatActivityMessage(void);

public:
    virtual HRESULT Initialize(REFGUID guidProviderId, CNAVEventCommon*& pNavEvent, const SYSTEMTIME& stTimestamp);

protected:
    HRESULT CreateCategoryList();
    HRESULT CreateShortDescription(const SYMGUID &guidDataId);
    HRESULT GetThreatData();

protected:
    CNAVEventCommonPtr m_spNavEventCommon;
    CManagedEventDataPtr m_spEventData;

};

class CScanResultsMessage : 
    public CAntivirusMessageBase,
    public MCF::CSymObjectCreator<&MCF::IID_Message, MCF::IMessage, CScanResultsMessage>
{
public:
    CScanResultsMessage(void);
    virtual ~CScanResultsMessage(void);

public:
    virtual HRESULT Initialize(REFGUID guidProviderId, CNAVEventCommon*& pNavEvent, const SYSTEMTIME& stTimestamp);

protected:
    HRESULT CreateCategoryList();
    HRESULT CreateShortDescription(const SYMGUID &guidDataId);
    HRESULT GetScanData();

protected:
    CNAVEventCommonPtr m_spNavEventCommon;
    CManagedEventDataPtr m_spEventData;
    
    CManagedEventDataPtr m_spResolvedByCategory;
    CManagedEventDataPtr m_spUnresolvedByCategory;
    CManagedEventDataPtr m_spEraserStats;
};

class CAvModuleItemMessage : 
    public CAntivirusMessageBase,
    public MCF::CSymObjectCreator<&MCF::IID_Message, MCF::IMessage, CAvModuleItemMessage>
{
public:
    CAvModuleItemMessage(void);
    virtual ~CAvModuleItemMessage(void);

public:
    virtual HRESULT Initialize(REFGUID guidProviderId, AVModule::IAVMapDwordData* pThreatData);

protected:
    HRESULT GetQuarantineData();
    

public:
    AVModule::IAVMapDwordDataPtr m_spThreatData;
};

class CCalculateThreatData
{
public:
	CCalculateThreatData(AVModule::IAVMapDwordData* pThreatInfo);
	~CCalculateThreatData();

protected:
	CCalculateThreatData();

public:
	bool AllowShowDeleteData();
	bool AllowShowDeleteItem();
	bool AllowShowRestoreItem();
	bool AllowShowSubmitItem();
	bool AllowShowRemoveNow();
	bool AllowShowExcludeItem();
	bool AllowShowAutoProtectOptions();
	bool AllowShowScanOptions();
	bool AllowShowEmailOptions();
	bool AllowShowExcludeOptions();
	bool AllowShowViewDetails();
	bool AllowShowQuickScan();
	bool AllowShowReviewRiskDetails();
    bool AllowShowOptionsSection();
    bool AllowShowFileList();

    GUID& GetThreatTrackId();
	const CString&  GetThreatTrackIdDisplayString();

	DWORD GetComponentId();
	const CString&  GetComponentDisplayName();
	
	DWORD GetThreatTypeId();
	const CString&  GetThreatTypeDisplayName();

	DWORD GetThreatStateId();
	const CString&  GetThreatStateDisplayString();

	DWORD GetRiskLevelOverallId();
	const CString&  GetRiskLevelOverallDisplayString();
	
	const CString&  GetThreatCatDisplayString();
	const CString&  GetShortDescription();

	const CString& GetThreatName();
	const CString& GetSIDString();

	DWORD GetMCFRiskLevel();
	DWORD GetMCFPriority();
	const CString& GetMCFStatusDisplayString();

	DWORD GetRecommendedActionId();
	const CString&  GetRecommendedActionDisplayString();
	
	bool HasDependencyFlag();

	bool ThreatHasBeenResolved();
	bool ThreatHasBeenIgnored();
	bool ThreatHasBeenRemoved();
	bool ThreatIsManualQuarantineItem();
    bool ThreatHasViralCategory();
	bool ThreatRequiresReboot();
	bool ThreatRequiresProcTerm();
    bool ThreatHasNoActions();
    bool ThreatHasBeenExcluded();

	bool CanRestoreItem();
	bool ManualRemovalRequired();

	bool IsMCFQuarantineItem();
	bool IsMCFSecurityRiskItem();
	bool IsMCFSuspiciousEmail();
	bool IsMCFUnresolvedRiskItem();
    bool IsMCFDoNotDisplay();

	
    bool ThreatIsAppHeuristic();
    
    DWORD GetHeuristicRankId();
    CString& GetHeuristicRankDisplayString();
    
protected:
	void CalcComponentId();
	void CalcThreatType();
	void CalcThreatState();
	void CalcRiskLevelOverall();
	void CalcThreatCat();
	void CalcShortDescription();
	void CalcThreatName();
	void CalcMCFRiskLevel();
	void CalcMCFPriority();
	void CalcCanRestoreItem();
	void CalcRecommendedAction();
	void CalcMCFStatus();
	void CalcDependency();
    void CalcTheatHasViralCategory();
    void CalcHeuristicRank();
	void CalcThreatRequiresReboot();
	void CalcThreatRequiresProcTerm();
    void CalcSIDString();
    
protected: // member vars
	AVModule::IAVMapDwordDataPtr m_spThreatInfo;

	GUID m_guidThreatTrackId;
	CString m_cszThreatTrackId;

	DWORD m_dwRiskLevelOverallId;
	CString m_cszRiskLevelOverallDisplayString;

	DWORD m_dwThreatStateId;
	CString m_cszThreatStateDisplayName;

	DWORD m_dwThreatTypeId;
	CString m_cszThreatTypeDisplayName;

	DWORD m_dwComponentId;
	CString m_cszComponentDisplayName;
	
	CString m_cszThreatCatDisplayString;
	CString m_cszSIDString;

	CString m_cszShortDescription;
	CString m_cszThreatName;
	
	DWORD m_dwMCFRiskLevel;
	DWORD m_dwMCFPriority;
	CString m_cszMCFStatusDisplayString;

	DWORD m_dwRecommendedActionId;
	CString m_cszRecommendedActionDisplayString;
    
    DWORD m_dwHeuristicRankId;
    CString m_cszHeuristicRankDisplayString;

	DWORD m_dwHasAtLeastOneUndo;
	DWORD m_dwHasViralCategory;
    DWORD m_dwDependencyFlag;
    DWORD m_dwRequiresReboot;
	DWORD m_dwRequiresProcTerm;

    bool m_bThreatIsAppHeuristic;
	bool m_bAllowShowAutoProtectOptions;
	bool m_bAllowShowScanOptions;
	bool m_bAllowShowEmailOptions;
	bool m_bAllowShowExcludeOptions;
	bool m_bAllowShowViewDetails;
};
