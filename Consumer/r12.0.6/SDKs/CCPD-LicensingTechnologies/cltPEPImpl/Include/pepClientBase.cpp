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
// pepClientBase.cpp : 
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "hrx.h"
#include "cltTrace.h"
#include "pepClientBase.h"

AUTHORIZATION_MAP_BEGIN(EmptyV2Map)
AUTHORIZATION_MAP_END(EmptyV2Map)

// Constructor
CPEPClientBase::CPEPClientBase() :
    m_bEmptyV2Map(false)
{
}
/*-----------------------------------------------------------------------------------------------------------*/
// Initialization function
HRESULT CPEPClientBase::Init()
{
    return SIMON_COLLECTION::CreateInstance(&m_spDataCollection);
}
/*-----------------------------------------------------------------------------------------------------------*/
// Sets the component guid - this is the business rule DLL we will look for
void CPEPClientBase::SetComponentGuid(const TCHAR* szComponentGuid)
{
        m_sComponentGuid = szComponentGuid;
}

void CPEPClientBase::SetComponentGuid(const clt::lt_helper::tstring &sComponentGuid)
{
    m_sComponentGuid = sComponentGuid;
}
/*-----------------------------------------------------------------------------------------------------------*/
// Sets the owner seed - Which owner product you want to make the call to
void CPEPClientBase::SetOwnerSeed(clt::lt_helper::tstring &sOwnerSeed)
{
    m_sOwnerSeed = sOwnerSeed;
}
/*-----------------------------------------------------------------------------------------------------------*/
// Sets the context guid for the policy
void CPEPClientBase::SetContextGuid(const GUID &ownerGuid)
{
    m_guidContext = ownerGuid;
}
/*-----------------------------------------------------------------------------------------------------------*/
// Sets the policy ID
void CPEPClientBase::SetPolicyID(const DWORD dwPolicyID)
{
    m_dwPolicyID = dwPolicyID;
}
/*-----------------------------------------------------------------------------------------------------------*/
SIMON_COLLECTION::IDataCollection* CPEPClientBase::GetCollectionPtr() const
{
    return m_spDataCollection;
}
/*-----------------------------------------------------------------------------------------------------------*/
// Executes the policy – this will fill out the collection with answers for the policy
HRESULT CPEPClientBase::QueryPolicy()
{
    STAHLSOFT_HRX_TRY(hr)
    {
        hrx << SIMON_PEP::Initialize(m_spDataCollection, &m_guidContext, m_dwPolicyID);
        SIMON_COLLECTION::SetCollectionString(m_spDataCollection, SIMON_PEP::SZ_PROPERTY_OWNER_SEED, m_sOwnerSeed.c_str());

        clt::lt_helper::tstring sBRDLLName;
        hrx << getHandlerPath(sBRDLLName, m_sComponentGuid.c_str());

        CLT_TRACEI(_T("CPepClientBase::QueryPolicy - Loading business rule DLL: %s"), sBRDLLName.c_str());

        // Do a Digital Signature Validation on the Handler
        ///////////////////////////////////////////////////////////////////////
        clt::security::SecurityValidation(sBRDLLName.c_str());

        SIMON::CSimonPtr<SIMON_PEP::IPEPSingleton>   spSourceDriven;
        // Instantiate the Source object and hookup the handler.
        //////////////////////////////////////////////////////////////////////////
        hrx << createPEPSourceObjectByPath(sBRDLLName.c_str(),&spSourceDriven);
        hrx << spSourceDriven->QueryPolicy(m_spDataCollection);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN_LOG(_T("CPepClientBase::QueryPolicy - Exit: HR = 0x%08X"),hr);
}
/*-----------------------------------------------------------------------------------------------------------*/
// Collection methods
/*-----------------------------------------------------------------------------------------------------------*/
BOOL CPEPClientBase::ExistsCollectionData(LPCSTR lpcName)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::ExistsCollectionData(m_spDataCollection, lpcName);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::Reset()
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return m_spDataCollection->Reset();
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::GetCount(DWORD* pVal)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return m_spDataCollection->GetCount(pVal);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::DeleteCollectionData(LPCSTR lpcName)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::DeleteCollectionData(m_spDataCollection, lpcName);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::SetCollectionData(LPCSTR lpcName, DWORD type, CONST BYTE * lpData, DWORD dwSize)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::SetCollectionData(m_spDataCollection, lpcName, type, lpData, dwSize);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::SetCollectionDWORD(LPCSTR lpcName, DWORD dwData)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::SetCollectionDWORD(m_spDataCollection, lpcName, dwData);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::SetCollectionString(LPCSTR lpcName, LPCSTR lpcBuffer)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::SetCollectionString(m_spDataCollection, lpcName, lpcBuffer);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::GetCollectionData(LPCSTR lpcName, StahlSoft::CSmartDataPtr<BYTE>& spData, DWORD* pType)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::GetCollectionData(m_spDataCollection, lpcName, spData, pType);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::GetValueFromCollection(LPCSTR lpcName, DWORD& dwVal)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return SIMON_COLLECTION::GetValueFromCollection(m_spDataCollection, lpcName, dwVal);
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::GetValueFromCollection(LPCSTR name, DWORD& dwVal, DWORD dwDefParam)
{
	DWORD_PTR dwTemporaryCollectionDWORD = (DWORD_PTR) dwDefParam;
	HRESULT hr = GetValueFromCollection(name, dwTemporaryCollectionDWORD);
	
	if(FAILED(hr) && (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr))
		hr = S_OK;

	if(NULL == memcpy(&dwVal, &dwTemporaryCollectionDWORD, sizeof(DWORD_PTR)))
		hr = E_FAIL;

	return hr;
}
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::DeleteValue(LPCSTR lpcName)
{
    if(NULL == m_spDataCollection.p)
        return E_POINTER;

    return m_spDataCollection->DeleteValue(lpcName);
}
/*-----------------------------------------------------------------------------------------------------------*/
// Private functions
/*-----------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------*/
// Function GetHandlerPath
//
// Description:
// Function to get the DLL path to the business rule DLL
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::getHandlerPath(clt::lt_helper::tstring &sHandlerPath, LPCTSTR pszGUID)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        if( NULL == pszGUID )
            return E_POINTER;

        // Initialize csOwnerDLLPath to empty string.
        sHandlerPath.clear();

        CRegKey rk;
        CString regPath(::pep::registry::szSuiteOwnerKey);
        long rc = ERROR_SUCCESS;

        regPath += _T("\\");
        regPath += pszGUID;

        // Open the product suiteowner key in the registry
        rc = rk.Open(HKEY_LOCAL_MACHINE
            , regPath
            , KEY_READ);

        if(ERROR_SUCCESS == rc)
        {
            TCHAR szScratch[MAX_PATH] = {0};
            ULONG ulSize = MAX_PATH;

                // Read the name of the business rule handler
            rc = rk.QueryStringValue(::pep::registry::szOwnerDirectory
                    , szScratch
                    , &ulSize);

                hrx << HRESULT_FROM_WIN32(rc);

                ::PathAddBackslash(szScratch);
                sHandlerPath = szScratch;

                ZeroMemory(szScratch, sizeof(szScratch));
                ulSize = MAX_PATH;
                // Read the name of the business rule handler
                rc = rk.QueryStringValue(::pep::registry::szBRDllName
                    , szScratch
                    , &ulSize);
                hrx << HRESULT_FROM_WIN32(rc);

                sHandlerPath += szScratch;
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}
/*-----------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::createPEPSourceObject(SIMON_PEP::IPEPSingleton** ppObj)
{
    typedef V2Authorization::_AUTHORIZATION_GUID_ENTRY* V2MapPtrT;

    HRESULT hr = S_OK;
    if(m_bEmptyV2Map)
        hr = SIMON_PEP::CreatePEPSourceObject<(V2MapPtrT)&(EmptyV2Map)
        ,&SIMON_PEP::CLSID_CPolicyEnforcement>(ppObj);
    else
    hr = SIMON_PEP::CreatePEPSourceObject<(V2MapPtrT)&(DRMPEPSinkMustHaveThisMap)
        ,&SIMON_PEP::CLSID_CPolicyEnforcement>(ppObj);
    return hr;
}
/*-----------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
HRESULT CPEPClientBase::createPEPSourceObjectByPath(LPCTSTR pPolicyHandlerPath,SIMON_PEP::IPEPSingleton** ppObj)
{
    typedef V2Authorization::_AUTHORIZATION_GUID_ENTRY* V2MapPtrT;

    HRESULT hr = S_OK;

    if(m_bEmptyV2Map)
        hr = SIMON_PEP::CreatePEPSourceObjectByPath<(V2MapPtrT)&(EmptyV2Map)
        ,&SIMON_PEP::CLSID_CPolicyEnforcement>(pPolicyHandlerPath,ppObj);
    else
        hr = SIMON_PEP::CreatePEPSourceObjectByPath<(V2MapPtrT)&(DRMPEPSinkMustHaveThisMap)
        ,&SIMON_PEP::CLSID_CPolicyEnforcement>(pPolicyHandlerPath,ppObj);

    return hr;
}
