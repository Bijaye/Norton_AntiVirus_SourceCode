////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ThreatTracker.h"

// CProcessAllThread
class CProcessAllThread :
    public ccLib::CThread
{
public:
    CProcessAllThread()
    {
    }

    virtual ~CProcessAllThread(void)
    {
        CCTRCTXI0(L"dtor");

        // Wait for our thread to exit
        WaitForExit(INFINITE);

        TRACE_REF_COUNT(m_spThreatTracker);
    }

    HRESULT ApplyAllThreatActions(HWND hParentWnd, CThreatTracker* pThreatTracker)
    {
        // If the thread is already running, return S_FALSE
        if(IsThread())
            return S_FALSE;

        // Save the input
        m_hParentWnd = hParentWnd;
        m_spThreatTracker = pThreatTracker;

        // Set thread options
        m_Options.m_bNoCRTThread = FALSE;
        m_Options.m_bPumpMessages = TRUE;
        m_Options.m_eCOMModel = ccLib::CCoInitialize::eAutoModel;
        m_Options.m_bWaitForInitInstance = FALSE;

        // Create the refresh thread
        if(!Create(NULL, 0, 0))
        {
            LOG_FAILURE_AND_RETURN(L"Failed to create apply thread...", E_FAIL);
        }

        return S_OK;
    }

protected:
    // CThread methods
    virtual int Run() throw()
    {
        CCTRCTXI0(L"--Enter");
        if(m_spThreatTracker)
            m_spThreatTracker->ApplyAllThreatActions(m_hParentWnd);

        CCTRCTXI0(L"--Exit");
        return 0;
    }

protected:
    CSymPtr<CThreatTracker> m_spThreatTracker;
    HWND m_hParentWnd;
};
