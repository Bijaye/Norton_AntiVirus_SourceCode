#pragma once

#include <vector>
#include <set>
#include "Resource.h"
#include "NAVOptions.h"
#include <ObjSafe.h>
#include <atlctl.h>
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "ccSymInterfaceLoader.h"
#include "AVCommonUILoader.h"
#include "ccScanLoader.h"
#include "ccEraserInterface.h"
#include "ScanProperties.h"
#include "EraserScanSink.h"

class CThreatsByVID : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CThreatsByVID, &CLSID_ThreatsByVID>,
    public IDispatchImpl<IThreatsByVID, &IID_IThreatsByVID>,
    public CScriptSafe<CThreatsByVID>,						  // For SafeScript
    public IObjectSafetyImpl<CThreatsByVID, INTERFACESAFE_FOR_UNTRUSTED_DATA | INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{

class CThreatItem
{
public:
    CThreatItem();
    ~CThreatItem();

    bool InFilter(long lCategory);

    CComBSTR m_bstrThreatName;
    DWORD m_dwVID;
    std::set<long> m_setCategories;
};

static bool Compare_ByThreatName(CThreatItem Item1, CThreatItem Item2);

public:
    CThreatsByVID(void);
    ~CThreatsByVID(void);

    DECLARE_REGISTRY_RESOURCEID(IDR_THREATSBYVID)

    BEGIN_COM_MAP(CThreatsByVID)
        COM_INTERFACE_ENTRY(IThreatsByVID)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IObjectSafety)
        COM_INTERFACE_ENTRY(IObjectWithSite)  // For SafeScript
        COM_INTERFACE_ENTRY(ISymScriptSafe)   // Allow programmatic enabling/disabling of safety check
    END_COM_MAP()

    BEGIN_CATEGORY_MAP(CThreatsByVID)
        IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
        IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
    END_CATEGORY_MAP()

    HRESULT FinalConstruct();
    void FinalRelease();

    // IThreatsByVID interface
    STDMETHOD (get_Count)(/*[out,retval]*/ unsigned long* pulCount);
    STDMETHOD (get_ThreatName)(/*[in]*/ unsigned long ulIndex, /*[out,retval]*/ BSTR* bstrThreatName);
    STDMETHOD (get_ThreatVID)(/*[in]*/ unsigned long ulIndex, /*[out,retval]*/ unsigned long* pulVID);
    STDMETHOD (get_Filter)(/*[out,retval]*/ ENonViralCategories* peFilterCategory);
    STDMETHOD (put_Filter)(/*[in]*/ long lFilterCategory);
    STDMETHOD (put_NAVERROR)(/*[in]*/  INAVCOMError* pNavError);
    STDMETHOD (get_NAVERROR)(/*[out,retval]*/  INAVCOMError** ppNavError);
    STDMETHOD (DisplayDetails)(/*[in]*/ unsigned long ulVID, /*[in]*/ long hwnd);

protected:
    bool Initialize();
    void MakeError (long lErrorID, long lHResult, long lNAVErrorResID);

    std::vector<CThreatItem> m_vItems;
    std::vector<CThreatItem*> m_vCurFilterItems;

    // Category filter
    ENonViralCategories m_CurFilter;

    // CED support
    CComPtr <INAVCOMError> m_spError;

    // CommonUI for displaying details about a VID
    AV::AVCommonUI_ICommonUIFactory m_CommonUILoader;
    CSymPtr<ICommonUI> m_spCommonUI;

    // ccScan and ccEraer for getting the Anomaly to pass into the CommonUI
    bool LoadccEraser();
    bool GenerateAnomaly(unsigned long ulVid, ccEraser::IAnomaly*& pAnomaly);
    cc::ccScan_IScanner4 m_ccScanLoader;
    cc::CSymInterfaceTrustedLoader m_ccEraserLoader;
    IScanner4Ptr m_spScanner4;
    ccEraser::IEraserPtr m_spEraser;

    // Sinks required by eraser
    CEraserScanSink* m_pEraserScanSink;
    CScanProperties* m_pScanProps;
};
