// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavEmailFilter.h
//
// SAV Corporate Edition ccEmailProxy filter plug-in.
//***************************************************************************

#ifndef __SAV_EMAIL_FILTER_H
#define __SAV_EMAIL_FILTER_H

#include "EmailProxyInterface.h"
#include "MailComm.h"

class CSavEmailFilter :
    public IEmailFilter,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(IID_EmailFilter, IEmailFilter)
    SYM_INTERFACE_MAP_END()                 

    // Constructor.
    CSavEmailFilter();

    // Destructor.
    ~CSavEmailFilter();

    // IEmailFilter override.
    bool Initialize(int iFilterID, IEmailProxy* pEmailProxy);
    
    // IEmailFilter override.
    void Notify(DWORD dwSessionID, EPXYEVENT Event);

    // IEmailFilter override.
    EPXYRESULT Filter(const EPXYMESSAGEINFO& MessageInfo);

private:
    // Filter ID.
    int m_iFilterID;

    // ccEmailProxy communication

    void GetOptionsInterface();
    void FreeOptionsInterface();

    IEmailProxy*    m_pEmailProxy;
    HINSTANCE       m_hEmailProxyDll;
    IEmailOptions*  m_pEmailProxyOptions;

    // Updates settings.
    void UpdateSettings();

    // Options watch thread
    HANDLE  m_hOptionsWatchThread;
    HANDLE  m_hOptionsWatchStopEvent;

    unsigned OptionsWatch();
    static unsigned __stdcall OptionsWatchProc(void* pArgument);

    // Settings internal to filter
    
    bool m_bScanOutgoing;               // Current state of outgoing.
    bool m_bScanIncoming;               // Current state of incoming.

    // Communication with RTVScan Internet Mail Storage Extension
    CMailCommBuffer m_commBuffer;
};

#endif // __SAV_EMAIL_FILTER_H
