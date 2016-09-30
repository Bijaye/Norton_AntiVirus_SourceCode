// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVOptionsObj.h"
#include "AutoLiveUpdate.h"
#include "resource.h"
#include "../navoptionsres/resource.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"
#include "ALUWrapper.h"
#include "isDataNoUIInterface.h"

/////////////////////////////////////////////////////////////////////////
// LiveUpdate

STDMETHODIMP CNAVOptions::get_LiveUpdate(BOOL *pbLiveUpdate)
{
    bool bOn;
    HRESULT hr;

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pbLiveUpdate )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if(FAILED(hr = m_ALUWrapper.GetALUUserOn(bOn)))
    {
        CCTRCTXE1(_T("Error getting ALU User Preference. HR = 0x%08x"), hr);
        return hr;
    }

    *pbLiveUpdate = bOn ? TRUE:FALSE;
    return S_OK;
}

STDMETHODIMP CNAVOptions::put_LiveUpdate(BOOL bLiveUpdate)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    return m_ALUWrapper.SetALUUserOn(bLiveUpdate==FALSE?false:true);
}

STDMETHODIMP CNAVOptions::DefaultLiveUpdate()
{
    DWORD dwValue;
    
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }
    
    // Initialize the defaults the first time
    if (!m_bNavDefsInitialized)
    {
        TCHAR szNAVDefs[MAX_PATH];

        // Path to the default options file
        ::_tcscat(::_tcscpy(szNAVDefs, g_NAVInfo.GetNAVDir()), _T("\\NAVOPTS.DEF"));
        // Load the list w/values from the file
        if( !m_pNavDefs->Init(szNAVDefs, FALSE) )
        {
            MakeError(IDS_Err_DefaultsFile, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }

        m_bNavDefsInitialized = true;
    }

    if (!forceError (IDS_Err_Default) && SUCCEEDED(m_pNavDefs->GetValue(L"DEFAULT:LiveUpdate", dwValue, 0)))
    {
        return m_ALUWrapper.SetALUUserOn(dwValue == 1 ? true:false);
    }
    else
    {
        //Error getting value from default file, return error.
        MakeError( IDS_Err_Default, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
        return E_FAIL;
    }

    return S_OK;
}


STDMETHODIMP CNAVOptions::get_ALUrunning(BOOL *pVal)
{
    bool bOn;
    HRESULT hr;

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if(FAILED(hr = m_ALUWrapper.GetALUState(bOn)))
    {
        CCTRCTXE1(_T("Error getting ALU state. HR = 0x%08x"), hr);
        return hr;
    }

    *pVal = bOn ? VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
}

// ALU Wrapper Implementation

CALUOptions::CALUOptions(): m_bInitialized(false), m_bOn(false), m_bUserWantsOn(false)
{}

CALUOptions::~CALUOptions()
{}

HRESULT CALUOptions::Load()
{
    SYMRESULT sr;
    HRESULT hr = E_FAIL;
    LONGLONG llState;
    ui::IDataPtr pData;
    ui::INumberDataQIPtr pDataNumber;

    if(m_bInitialized)
        return S_FALSE;

    // Initialize IElement
    ISShared::ISShared_IProvider providerFactory;

    if(SYM_FAILED(sr = providerFactory.CreateObject(GETMODULEMGR(), &m_pProvider)))
    {
        CCTRCTXE1 (_T("Failed to create IElement provider. SR = 0%x80"), sr);
        return E_FAIL;
    }
    else
    {
        if (FAILED(hr = m_pProvider->GetElement ( ISShared::CLSID_NIS_ALU, m_pElement)))
        { 
            CCTRCTXE1(_T("Failed to create IElement. HR = 0x%08x"), hr);
            return hr;
        }
    }

    if(FAILED(hr = m_pElement->GetData(pData)))
    {
        CCTRCTXE1(_T("Could not get IData from IElement. HR = 0x%08x"), hr);
        return hr;
    }

    pDataNumber = pData;
    if(!pDataNumber)
    {
        CCTRCTXE0(_T("Could not QI to INumberData."));
        return E_POINTER;
    }

    if(FAILED(hr = pDataNumber->GetNumber(llState)))
    {
        CCTRCTXE1(_T("Failed to get Number from IData. HR = 0x%08x"), hr);
        return hr;
    }

    m_bOn = m_bUserWantsOn = (llState == ISShared::i64StateOn);

    m_bInitialized = true;
    return S_OK;
}

HRESULT CALUOptions::GetALUState(bool& bOn)
{
    HRESULT hr;
    LONGLONG llState;
    ui::IDataPtr pData;
    ui::INumberDataQIPtr pDataNumber;

    if(FAILED(hr = this->Load()))
        return hr;

    if(FAILED(hr = m_pElement->GetData(pData)))
    {
        CCTRCTXE1(_T("Could not get IData from IElement. SymResult = 0x%08x"), hr);
        return E_FAIL;
    }

    pDataNumber = pData;
    if(!pDataNumber)
    {
        CCTRCTXE0(_T("Could not QI to INumberData."));
        return E_POINTER;
    }

    if(FAILED(hr = pDataNumber->GetNumber(llState)))
    {
        CCTRCTXE1(_T("Failed to get Number from IData. HR = 0x%08x"), hr);
        return hr;
    }

    bOn = m_bOn = llState == ISShared::i64StateOn;
    return S_OK;
}

HRESULT CALUOptions::SetALUUserOn(bool bOn)
{
    HRESULT hr;
    if(FAILED(hr = Load()))
        return hr;

    m_bUserWantsOn = bOn;
    return S_OK;
}

HRESULT CALUOptions::GetALUUserOn(bool &bOn)
{
    HRESULT hr;
    if(FAILED(hr = Load()))
        return hr;

    bOn = m_bUserWantsOn;
    return S_OK;
}

HRESULT CALUOptions::Dirty()
{
    if(!m_bInitialized)
        return S_FALSE;

    return m_bUserWantsOn == m_bOn ? S_FALSE:S_OK;
}

// POORLY NAMED! This actually is more along the lines of 'Save()'.
HRESULT CALUOptions::Enable()
{
    HRESULT hr;

    if(FAILED(hr = this->Load()))
        return hr;

    // Don't save if not dirty
    if(m_bUserWantsOn == m_bOn)
        return S_FALSE;

    ui::IDataPtr pData;
    if(FAILED(hr = m_pElement->GetData(pData)))
    {
        CCTRCTXE1(_T("Could not get IData from IElement. HR = 0x%08x"), hr);
        return hr;
    }

    ISShared::INumberDataNoUIQIPtr pDataNumber = pData;
    if(!pDataNumber)
    {
        CCTRCTXE0(_T("Could not QI to INumberDataNoUI."));
        return E_POINTER;
    }

    if(FAILED(hr = pDataNumber->SetNumberNoUI(m_bUserWantsOn?ISShared::i64StateOn:ISShared::i64StateOff)))
    {   
        CCTRCTXE1(_T("Failed to turn ALU on. HR = 0x%08x"), hr);
    }

    return hr;
}
