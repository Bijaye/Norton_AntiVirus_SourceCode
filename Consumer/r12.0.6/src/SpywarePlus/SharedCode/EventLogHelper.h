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
#pragma once

#include <map>

#include "ccEventFactoryEx.h"
#include "ccLogFactoryEx.h"
#include "ccLogManagerEx.h"
#include "ccContextEventStore.h"

using namespace ccEvtMgr;

namespace HPP
{

class CEventLogHelper
{
public:
    CEventLogHelper() throw();
    virtual ~CEventLogHelper() throw();

private:
    CEventLogHelper(const CEventLogHelper&) throw();
    CEventLogHelper& operator =(const CEventLogHelper&) throw();

public:
    virtual BOOL Initialize(CEventFactoryEx* pEventFactory,
                  LPCTSTR szLogPath,
                  LPCTSTR szLogFileName,
                  BOOL bEncrypt = TRUE,
                  UINT uMaxSize = 65536) throw();
    virtual BOOL Destroy() throw();

// CLogFactoryEx
public:
    virtual CError::ErrorType SaveEvent(const CEventEx& Event, 
                                        ULONGLONG& uIndex,
                                        DWORD& dwIndexCheck) throw();
    virtual CError::ErrorType LoadEvent(ULONGLONG uIndex, 
                                        DWORD dwIndexCheck,
                                        long nContextId,
                                        CEventEx*& pEvent) throw();
    virtual CError::ErrorType DeleteEvent(ULONGLONG uIndex,
                                          DWORD dwIndexCheck) throw();
    virtual CError::ErrorType SearchEvents(const SYSTEMTIME& TimeStart,
                                           const SYSTEMTIME& TimeStop,
                                           bool bCreateContext,
                                           long nLimitCount,
                                           long& nContextId,
                                           ccEvtMgr::CLogFactoryEx::EventInfo*& pEventInfoArray,
                                           long& nEventCount) throw();
    virtual CError::ErrorType DeleteSearchData(ccEvtMgr::CLogFactoryEx::EventInfo* pEventInfoArray) throw();
    virtual CError::ErrorType DeleteSearchContext(long nContextId) throw();
    virtual CError::ErrorType ClearLog() throw();
    virtual CError::ErrorType SetMaxLogSize(ULONGLONG uSize) throw();
    virtual CError::ErrorType GetMaxLogSize(ULONGLONG& uSize) throw();
    virtual CError::ErrorType SetLogEnabled(bool bEnabled) throw();
    virtual CError::ErrorType GetLogEnabled(bool& bEnabled) throw();

// CLogFactoryEx2
public:
    virtual CError::ErrorType LoadEventBatch(const ULONGLONG* pIndexArray,
                                             const DWORD* pIndexCheckArray,
                                             long nContextId,
                                             CEventEx** pEventArray,
                                             long nEventCount) throw(); 
    virtual CError::ErrorType DeleteBatchData(CEventEx** pEventArray, 
                                              long nEventCount) throw();

protected:

// Pack with known order for binary saving to file
#pragma pack(push, 8)
    struct SettingsInfo
    {
        BOOL m_bEnabled;
        ULONGLONG m_uMaxSize;
    };
#pragma pack(pop)

protected:
    BOOL OpenEventStore(LPCTSTR szLogPath, LPCTSTR szLogFileName) throw();

protected:
    StahlSoft::CSmartPtr<CContextEventStore> m_spEventStore;
    CEventFactoryExPtr m_pEventFactory;
    SettingsInfo m_Settings;

    CString m_cszLogFilePath;
    CString m_cszLogFileName;
    BOOL m_bEncrypt;

protected:
    static const DWORD m_dwHeaderSignature;
    static const DWORD m_dwHeaderVersion;
    static const DWORD m_dwIndexSignature;
    static const DWORD m_dwIndexVersion;
};

template < class _TLoader >
class CEventLogHelperT : public CEventLogHelper
{
public:
    CEventLogHelperT()
    {
    };

    virtual ~CEventLogHelperT()
    {
        CCTRACEI(_T("CEventLogHelperT::~CEventLogHelperT - d'tor"));
        Destroy();
    };
    
    virtual BOOL Initialize(LPCTSTR szLogPath,
        LPCTSTR szLogFileName,
        BOOL bEncrypt = TRUE,
        UINT uMaxSize = 65536) throw()
    {
        SYMRESULT sr = m_Loader.Initialize();
        if(SYM_FAILED(sr))
            return FALSE;

        CEventFactoryExPtr spEventFactoryEx;
        sr = m_Loader.CreateObject(&spEventFactoryEx);
        if(SYM_FAILED(sr))
            return FALSE;

        return __super::Initialize(spEventFactoryEx, szLogPath, szLogFileName, bEncrypt, uMaxSize);
    }
    
protected:
    _TLoader m_Loader;
};

} // HPP

