// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#include "StdAfx.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "EventLogHelper.h"
#include "ccCatch.h"

using namespace ccLib;
using namespace ccEvtMgr;
using namespace HPP;

const DWORD CEventLogHelper::m_dwHeaderSignature = 0xCEEECEE0;
const DWORD CEventLogHelper::m_dwHeaderVersion = 1;
const DWORD CEventLogHelper::m_dwIndexSignature = 0xCEBECEB0;
const DWORD CEventLogHelper::m_dwIndexVersion = 1;

CEventLogHelper::CEventLogHelper() 
{
    ZeroMemory(&m_Settings, sizeof(m_Settings));
}

CEventLogHelper::~CEventLogHelper()
{
    CCTRACEI(_T("CEventLogHelper::~CEventLogHelper - d'tor"));
    Destroy();
}

BOOL CEventLogHelper::Initialize(CEventFactoryEx* pEventFactory,
                                 LPCTSTR szLogPath,
                                 LPCTSTR szLogFileName,
                                 BOOL bEncrypt,
                                 UINT uMaxSize)
{
    Destroy();

    // Smart pointer will AddRef()
    m_pEventFactory = pEventFactory;

    BOOL bRet = TRUE;
    CExceptionInfo exceptionInfo(_T("CEventLogHelper::Initialize()"));
    try
    {
        m_spEventStore = new CContextEventStore(m_dwHeaderSignature,
                                                                m_dwHeaderVersion,
                                                                m_dwIndexSignature,
                                                                m_dwIndexVersion);
        
        // Set the encryption from the default
        m_spEventStore->SetEncryption(bEncrypt);
        
        // Set settings
        m_Settings.m_bEnabled = TRUE;
        m_Settings.m_uMaxSize = uMaxSize;

        // Open the eventstore and read the settings
        if (OpenEventStore(szLogPath, szLogFileName) == FALSE)
        {
            CCTRACEE(_T("CEventLogHelper::Initialize() : OpenEventStore() == FALSE\n"));
            bRet = FALSE;
        }

    }
    CCCATCHMEM(exceptionInfo);
    if (exceptionInfo != FALSE ||
        bRet == FALSE)
    {
        Destroy();
        return FALSE;
    }
    return TRUE;
}    

BOOL CEventLogHelper::Destroy()
{
    // Cleanup
    m_pEventFactory.Release();
    
    if(m_spEventStore)
    {
        m_spEventStore->Close();
        m_spEventStore.Release();
    }

    return TRUE;
}

CError::ErrorType CEventLogHelper::SaveEvent(const CEventEx& Event,
                                               ULONGLONG& uIndex,
                                               DWORD& dwIndexCheck)
{
    uIndex = 0;

    // Does the event support CSerializableEventEx
    CSerializableEventExQIPtr pSerializableEvent(&Event);
    if (pSerializableEvent == NULL)
    {
        CCTRACEE(_T("CEventLogHelper::SaveEvent() : pSerializableEvent == NULL\n"));
        return CError::eFatalError;
    }

    // Save the event
    CError::ErrorType eError = CError::eNoError;
    eError = m_spEventStore->Add(*pSerializableEvent, 
                                uIndex, 
                                dwIndexCheck);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CEventLogHelper::SaveEvent() : m_spEventStore->Add() != CError::eNoError\n"));
    }
    return eError;
}

CError::ErrorType CEventLogHelper::LoadEvent(ULONGLONG uIndex, 
                                               DWORD dwIndexCheck,
                                               long nContextId,
                                               CEventEx*& pEvent)
{
    pEvent = NULL;

    // Load event
    CSerializableEventEx* pSerializableEvent = NULL;
    CError::ErrorType eError = CError::eNoError;
    eError = m_spEventStore->Read(uIndex, 
                                dwIndexCheck,
                                nContextId,
                                pSerializableEvent);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CEventLogHelper::LoadEvent() : m_spEventStore->Read() != CError::eNoError\n"));
    }
    else
    {
        pEvent = pSerializableEvent;
    }
    return eError;
}

CError::ErrorType CEventLogHelper::DeleteEvent(ULONGLONG uIndex,
                                                 DWORD dwIndexCheck)
{
    return CError::eNotImplementedError;
}

CError::ErrorType CEventLogHelper::SearchEvents(const SYSTEMTIME& TimeStart,
                                                  const SYSTEMTIME& TimeStop,
                                                  bool bCreateContext,
                                                  long nLimitCount,
                                                  long& nContextId,
                                                  ccEvtMgr::CLogFactoryEx::EventInfo*& pEventInfoArray,
                                                  long& nEventCount)
{
    // Set the event id to 0 in order to load all events that are derived from the requested type
    pEventInfoArray = NULL;
    nEventCount = 0;
    CError::ErrorType eError = CError::eNoError;
    eError = m_spEventStore->Search(0, 
                                    TimeStart, 
                                    TimeStop, 
                                    bCreateContext,
                                    nLimitCount,
                                    nContextId,
                                    pEventInfoArray,
                                    nEventCount);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CEventLogHelper::SearchEvents() : m_spEventStore->Read() != CError::eNoError\n"));
    }
    return eError;
}

CError::ErrorType CEventLogHelper::DeleteSearchData(ccEvtMgr::CLogFactoryEx::EventInfo* pEventInfoArray)
{
    // Delete
    CError::ErrorType eError = CError::eNoError;
    eError = m_spEventStore->DeleteSearchData(pEventInfoArray);
    pEventInfoArray = NULL;
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CEventLogHelper::DeleteSearchData() : m_spEventStore->DeleteSearchData() != CError::eNoError\n"));
    }
    return eError;
}

CError::ErrorType CEventLogHelper::DeleteSearchContext(long nContextId)
{
    // Delete
    CError::ErrorType eError = CError::eNoError;
    eError = m_spEventStore->DeleteSearchContext(nContextId);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CEventLogHelper::DeleteSearchContext() : m_spEventStore->DeleteSearchContext() != CError::eNoError\n"));
    }
    return eError;
}

CError::ErrorType CEventLogHelper::ClearLog()
{
    // Clear
    if (m_spEventStore->Clear() == FALSE)
    {
        CCTRACEE(_T("CEventLogHelper::ClearLog() : m_spEventStore->Clear() == FALSE\n"));
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CEventLogHelper::SetMaxLogSize(ULONGLONG uSize)
{
    // Apply the log settings to the opened file
    m_spEventStore->SetMaxSize(uSize);

    // Save the size
    m_Settings.m_uMaxSize = uSize;
    if (m_spEventStore->WriteHeader(0, &m_Settings, sizeof(m_Settings)) == FALSE)
    {
        CCTRACEE(_T("CEventLogHelper::SetMaxLogSize() : m_spEventStore->WriteHeader() == FALSE\n"));
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CEventLogHelper::GetMaxLogSize(ULONGLONG& uSize)
{
    uSize = 0;

    // Get size
    uSize = m_spEventStore->GetMaxSize();
    
    return CError::eNoError;
}

CError::ErrorType CEventLogHelper::SetLogEnabled(bool bEnabled)
{
    // Set state
    m_Settings.m_bEnabled = (bEnabled != false);

    // Save the size
    if (m_spEventStore->WriteHeader(0, &m_Settings, sizeof(m_Settings)) == FALSE)
    {
        CCTRACEE(_T("CEventLogHelper::SetLogEnabled() : m_spEventStore->WriteHeader() == FALSE\n"));
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CEventLogHelper::GetLogEnabled(bool& bEnabled)
{
    bEnabled = false;

    // Get state
    bEnabled = (m_Settings.m_bEnabled != FALSE);
    
    return CError::eNoError;
}

CError::ErrorType CEventLogHelper::LoadEventBatch(const ULONGLONG* pIndexArray,
                                                    const DWORD* pIndexCheckArray,
                                                    long nContextId,
                                                    CEventEx** pEventArray,
                                                    long nEventCount)
{
    // Set all event pointers to NULL
    // The input array is allocated by the caller
    ZeroMemory(pEventArray, sizeof(CEventEx*) * nEventCount);

    // Load all events
    CSerializableEventEx* pSerializableEvent = NULL;
    CError::ErrorType eError = CError::eNoError;
    long i = 0;
    for (i = 0; i < nEventCount; i ++)
    {
        eError = m_spEventStore->Read(pIndexArray[i], 
                                    pIndexCheckArray[i], 
                                    nContextId, 
                                    pSerializableEvent);
        if (eError != CError::eNoError)
        {
            CCTRACEE(_T("CEventLogHelper::LoadEventBatch() : m_spEventStore->Read() != CError::eNoError\n"));
            break;
        }
        pEventArray[i] = pSerializableEvent;
    }

    // If there was a failure unload
    if (eError != CError::eNoError)
    {
        DeleteBatchData(pEventArray, nEventCount);
    }
    return eError;
}

CError::ErrorType CEventLogHelper::DeleteBatchData(CEventEx** pEventArray, 
                                                     long nEventCount)
{
    // Delete the same way we allocated
    long i = 0;
    for (i = 0; i < nEventCount; i ++)
    {
        // Delete the events
        if (pEventArray[i] != NULL)
        {
            m_pEventFactory->DeleteEvent(pEventArray[i]);
            pEventArray[i] = NULL;
        }
    }
    return CError::eNoError;
}

BOOL CEventLogHelper::OpenEventStore(LPCTSTR szLogPath, LPCTSTR szLogFileName)
{
    m_cszLogFilePath = szLogPath;
    m_cszLogFileName = szLogFileName;
    
    // Make sure the last path char is a '\\'	
    TCHAR szStorePath[_MAX_PATH];
    lstrcpy(szStorePath, szLogPath);

    // Build file name
    PathAppend(szStorePath, szLogFileName);

    // Open the existing file
    if (m_spEventStore->Open(szStorePath, 
                            FALSE, 
                            sizeof(SettingsInfo), 
                            0,
                            m_pEventFactory) == FALSE)
    {
        CCTRACEW(_T("CEventLogHelper::OpenEventStore() m_spEventStore->Open(FALSE) == FALSE\n"));

        // Create a new file
        if (m_spEventStore->Open(szStorePath, 
                                TRUE, 
                                sizeof(SettingsInfo), 
                                0,
                                m_pEventFactory) == FALSE)
        {
            CCTRACEE(_T("CEventLogHelper::OpenEventStore() m_spEventStore->Open(TRUE) == FALSE\n"));
            return FALSE;
        }

        // Save the log settings for the newly created file
        if (m_spEventStore->WriteHeader(0, &m_Settings, sizeof(m_Settings)) == FALSE)
        {
            CCTRACEE(_T("CEventLogHelper::OpenEventStore() m_spEventStore->WriteHeader() == FALSE\n"));
            m_spEventStore->Close();
            return FALSE;
        }
    }
    else
    {
        // Extract the log settings from the opened file
        if (m_spEventStore->ReadHeader(0, &m_Settings, sizeof(m_Settings)) == FALSE)
        {
            CCTRACEE(_T("CEventLogHelper::OpenEventStore() m_spEventStore->ReadHeader() == FALSE\n"));
            m_spEventStore->Close();
            return FALSE;
        }
    }

    // Apply the log settings to the opened file
    // The enabled state is used locally
    m_spEventStore->SetMaxSize(m_Settings.m_uMaxSize);

    return TRUE;
}
