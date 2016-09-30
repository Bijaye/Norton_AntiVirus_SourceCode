///////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// pepClientBase.h : 
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(_PEPCLIENTBASE_H__8C353CA1_A7B3_4ca9_8C43_95378D7EE091__INCLUDED_)
#define _PEPCLIENTBASE_H__8C353CA1_A7B3_4ca9_8C43_95378D7EE091__INCLUDED_
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <stahlsoft.h>
#include <simon.h>
#include <SimonCollection.h>
#include <SimonCollectionImpl.h>
#include <SimonCollectionToolBox.h>

#include "cltSecurityValidation.h"
#include "cltHelper.h"

#ifdef _PEP_IMPL_
#define INIT_V2AUTHORIZATION_IMPL
#endif

#define _SIMON_PEP_
#define _V2AUTHORIZATION_SIMON
#include <PolicyEnforcement_Simon.h>


AUTHORIZATION_MAP_EXTERN(DRMPEPSinkMustHaveThisMap)
AUTHORIZATION_MAP_EXTERN(EmptyV2Map)

typedef V2Authorization::_AUTHORIZATION_GUID_ENTRY* V2MapPtrT;

namespace pep{
    namespace registry{
static const TCHAR* szSuiteOwnerKey           = _T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners");
static const TCHAR* szOwnerDirectory          = _T("OwnerDirectory");
static const TCHAR* szBRDllName               = _T("BRDLLName");
} // namespace registry
} // namespace pep

class CPEPClientBase
{
protected:
    // Path to BR dll 
    CString m_csLibPath;
    // Smart handle for suiteowner
    StahlSoft::CSmartModuleHandle m_shDLLPath;
    // Internal data collection
    SIMON::CSimonPtr<SIMON_COLLECTION::IDataCollection> m_spDataCollection;

    DWORD m_dwPolicyID;
    GUID m_guidContext;
    clt::lt_helper::tstring m_sOwnerSeed;
    clt::lt_helper::tstring m_sComponentGuid;
    bool m_bEmptyV2Map;

public:

    // Constructor
    CPEPClientBase();

    // Initialization function
    HRESULT Init();

    // Sets the component guid
    void SetEmptyV2Mode(bool bEmptyV2Map){m_bEmptyV2Map = bEmptyV2Map;};

    // Sets the component guid
    void SetComponentGuid(const TCHAR* szComponentGuid);
    void SetComponentGuid(const clt::lt_helper::tstring &sComponentGuid);
    // Sets the owner seed
    void SetOwnerSeed(clt::lt_helper::tstring &sOwnerSeed);
    // Sets the context guid for the policy
    void SetContextGuid(const GUID &guidContext);
    // Sets the policy ID
    void SetPolicyID(const DWORD dwPolicyID);
    // Executes the policy – this will fill out the collection with answers for the policy
    HRESULT QueryPolicy();

    // Gets the pointer to the colelction
    SIMON_COLLECTION::IDataCollection* GetCollectionPtr() const;

protected:
    // Gets the path to the business rule DLL
    HRESULT getHandlerPath(clt::lt_helper::tstring &sHandlerPath, LPCTSTR pszGUID);
    HRESULT createPEPSourceObject(SIMON_PEP::IPEPSingleton** ppObj);
    HRESULT createPEPSourceObjectByPath(LPCTSTR pPolicyHandlerPath,SIMON_PEP::IPEPSingleton** ppObj);
   
public:

    // Exposed data collection functions
    // FYI:  These functions should not be called prior to the Init otherwise they 
    // will return an E_POINTER since the datacollection will not have been created by then

    BOOL    ExistsCollectionData(LPCSTR name);
    HRESULT Reset();
    HRESULT GetCount(DWORD* pVal);
    HRESULT DeleteCollectionData(LPCSTR name);
    HRESULT SetCollectionData(LPCSTR name, DWORD type, CONST BYTE * lpData,DWORD dwSize);
    HRESULT SetCollectionDWORD(LPCSTR lpcName,DWORD dwData);
    HRESULT SetCollectionString(LPCSTR lpcName,LPCSTR lpcBuffer);
    HRESULT GetCollectionData(LPCSTR name, StahlSoft::CSmartDataPtr<BYTE>& spData, DWORD* pType = NULL);
	HRESULT GetValueFromCollection(LPCSTR name, DWORD& dwVal);
	HRESULT GetValueFromCollection(LPCSTR name, DWORD& dwVal, DWORD dwDefParam);
    HRESULT DeleteValue(LPCSTR name);

private:
    // Disallowed
    CPEPClientBase(CPEPClientBase&);
    CPEPClientBase &operator=(CPEPClientBase&);
};
#endif //!defined(_PEPCLIENTBASE_H__8C353CA1_A7B3_4ca9_8C43_95378D7EE091__INCLUDED_)
