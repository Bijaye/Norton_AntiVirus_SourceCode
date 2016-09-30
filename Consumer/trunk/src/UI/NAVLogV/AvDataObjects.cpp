////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AvDataObjects.h"

#include "ccInstanceFactory.h"

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CManagedEventData::CManagedEventData(void)
{
}

CManagedEventData::~CManagedEventData(void)
{
}

CManagedEventData::CManagedEventData(CEventData* pEventData) : CEventData(*pEventData)
{
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvItemBagDataBase::CAvItemBagDataBase(void)
{
}

CAvItemBagDataBase::~CAvItemBagDataBase(void)
{
}

//****************************************************************************
//****************************************************************************
void CAvItemBagDataBase::SetDataSource(const SYMGUID& guidDataId, CManagedEventData* pItemBag, LONG lIndex)
{
    m_spItemBag = pItemBag;
    m_lItemBagIndex = lIndex;
    memcpy(&m_guidObject, &guidDataId, sizeof(SYMGUID));
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvItemBagStringData::CAvItemBagStringData(void)
{
    m_DataType = ui::IData::eString;
}

CAvItemBagStringData::~CAvItemBagStringData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagStringData::GetString(cc::IString*& pString) const
{
    if(!m_spItemBag)
        return E_UNEXPECTED;
    
    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        std::wstring strString;
        bool bRet = m_spItemBag->GetData(m_lItemBagIndex, strString);
        if(bRet && !strString.empty())
        {
            cc::IStringQIPtr spString = ccLib::CInstanceFactory::CreateStringImpl();
            if(!spString)
                throw _com_error(E_UNEXPECTED);

            spString->SetString(strString.c_str());
            SYMRESULT sr = spString->QueryInterface(cc::IID_String, (void**)&pString);
            hrReturn = AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
    
    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagStringData::SetString(LPCWSTR wszString)
{
    if(!m_spItemBag)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_spItemBag->SetData(m_lItemBagIndex, wszString);
        hrReturn = S_OK;
    } 
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvItemBagNumberData::CAvItemBagNumberData(void)
{
    m_DataType = ui::IData::eNumber;
}

CAvItemBagNumberData::~CAvItemBagNumberData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagNumberData::GetNumber(LONGLONG& qdwNumber) const
{
    if(!m_spItemBag)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        long lNumber = NULL;
        bool bRet = m_spItemBag->GetData(m_lItemBagIndex, lNumber);
        if(bRet)
        {
            qdwNumber = (LONGLONG)lNumber;
            hrReturn = S_OK;
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagNumberData::SetNumber(LONGLONG qdwNumber)
{
    if(!m_spItemBag)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_spItemBag->SetData(m_lItemBagIndex, (LONG)qdwNumber);
        hrReturn = S_OK;
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvItemBagDateData::CAvItemBagDateData(void)
{
    m_DataType = ui::IData::eDate;
}

CAvItemBagDateData::~CAvItemBagDateData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagDateData::GetDate(SYSTEMTIME& stDate) const
{
    if(!m_spItemBag)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        long lLength = sizeof(SYSTEMTIME);
        bool bRet = m_spItemBag->GetData(m_lItemBagIndex, (LPBYTE)&stDate, lLength);
        if(!bRet)
        {
			long lTime = NULL;
			bool bRet = m_spItemBag->GetData(m_lItemBagIndex, lTime);
        if(bRet)
        {
				CTime ctTime = CTime((DWORD)lTime);
				ctTime.GetAsSystemTime(stDate);
				hrReturn = S_OK;
			}
        }
		else
		{
            hrReturn = S_OK;
        }
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvItemBagDateData::SetDate(const SYSTEMTIME& stDate)
{
    if(!m_spItemBag)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        long lLength = sizeof(SYSTEMTIME);
        m_spItemBag->SetData(m_lItemBagIndex, (const LPBYTE)&stDate, lLength);
        hrReturn = S_OK;
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVDwordMapDataBase::CAVDwordMapDataBase(void)
{
}

CAVDwordMapDataBase::~CAVDwordMapDataBase(void)
{
}

//****************************************************************************
//****************************************************************************
void CAVDwordMapDataBase::SetDataSource(const SYMGUID& guidDataId, AVModule::IAVMapDwordData* pAVDwordMap, DWORD dwIndex)
{
    m_spAVDwordMap = pAVDwordMap;
    m_dwAVDwordMapIndex = dwIndex;
    memcpy(&m_guidObject, &guidDataId, sizeof(SYMGUID));
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVDwordMapStringData::CAVDwordMapStringData(void)
{
    m_DataType = ui::IData::eString;
}

CAVDwordMapStringData::~CAVDwordMapStringData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapStringData::GetString(cc::IString*& pString) const
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVDwordMap->GetValue(m_dwAVDwordMapIndex, pString);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapStringData::SetString(LPCWSTR wszString)
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVDwordMap->SetValue(m_dwAVDwordMapIndex, wszString);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVDwordMapNumberData::CAVDwordMapNumberData(void)
{
    m_DataType = ui::IData::eNumber;
}

CAVDwordMapNumberData::~CAVDwordMapNumberData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapNumberData::GetNumber(LONGLONG& qdwNumber) const
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVDwordMap->GetValue(m_dwAVDwordMapIndex, qdwNumber);
        if(FAILED(hrReturn) && 0x8007065d == hrReturn)
        {
            DWORD dwNumberData = NULL;
            hrReturn = m_spAVDwordMap->GetValue(m_dwAVDwordMapIndex, dwNumberData);
            if(SUCCEEDED(hrReturn))
                qdwNumber = dwNumberData;
        }

    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapNumberData::SetNumber(LONGLONG qdwNumber)
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVDwordMap->SetValue(m_dwAVDwordMapIndex, qdwNumber);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVDwordMapDateData::CAVDwordMapDateData(void)
{
    m_DataType = ui::IData::eDate;
}

CAVDwordMapDateData::~CAVDwordMapDateData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapDateData::GetDate(SYSTEMTIME& stDate) const
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        DWORD dwTime;
        hrReturn = m_spAVDwordMap->GetValue(m_dwAVDwordMapIndex, dwTime);
        if(SUCCEEDED(hrReturn))
        {
            CTime ctTime = CTime(dwTime);
            ctTime.GetAsSystemTime(stDate);
        }
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVDwordMapDateData::SetDate(const SYSTEMTIME& stDate)
{
    if(!m_spAVDwordMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CTime ctTime = CTime(stDate);
        DWORD dwTime = (DWORD)ctTime.GetTime();

        hrReturn = m_spAVDwordMap->SetValue(m_dwAVDwordMapIndex, dwTime);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}



//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVStrMapDataBase::CAVStrMapDataBase(void)
{
}

CAVStrMapDataBase::~CAVStrMapDataBase(void)
{
}

//****************************************************************************
//****************************************************************************
void CAVStrMapDataBase::SetDataSource(const SYMGUID& guidDataId, AVModule::IAVMapStrData* pAVStrMap, LPCWSTR wszIndex)
{
    m_spAVStrMap = pAVStrMap;
    m_cwszAVStrMapIndex = wszIndex;
    memcpy(&m_guidObject, &guidDataId, sizeof(SYMGUID));
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVStrMapStringData::CAVStrMapStringData(void)
{
    m_DataType = ui::IData::eString;
}

CAVStrMapStringData::~CAVStrMapStringData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapStringData::GetString(cc::IString*& pString) const
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVStrMap->GetValue(m_cwszAVStrMapIndex, pString);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapStringData::SetString(LPCWSTR wszString)
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVStrMap->SetValue(m_cwszAVStrMapIndex, wszString);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVStrMapNumberData::CAVStrMapNumberData(void)
{
    m_DataType = ui::IData::eNumber;
}

CAVStrMapNumberData::~CAVStrMapNumberData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapNumberData::GetNumber(LONGLONG& qdwNumber) const
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVStrMap->GetValue(m_cwszAVStrMapIndex, qdwNumber);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapNumberData::SetNumber(LONGLONG qdwNumber)
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        hrReturn = m_spAVStrMap->SetValue(m_cwszAVStrMapIndex, qdwNumber);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAVStrMapDateData::CAVStrMapDateData(void)
{
    m_DataType = ui::IData::eDate;
}

CAVStrMapDateData::~CAVStrMapDateData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapDateData::GetDate(SYSTEMTIME& stDate) const
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        DWORD dwTime;
        hrReturn = m_spAVStrMap->GetValue(m_cwszAVStrMapIndex, dwTime);
        if(SUCCEEDED(hrReturn))
        {
            CTime ctTime = CTime(dwTime);
            ctTime.GetAsSystemTime(stDate);
        }
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAVStrMapDateData::SetDate(const SYSTEMTIME& stDate)
{
    if(!m_spAVStrMap)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CTime ctTime = CTime(stDate);
        DWORD dwTime = (DWORD)ctTime.GetTime();

        hrReturn = m_spAVStrMap->SetValue(m_cwszAVStrMapIndex, dwTime);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        hrReturn = E_FAIL;

    return hrReturn;
}

