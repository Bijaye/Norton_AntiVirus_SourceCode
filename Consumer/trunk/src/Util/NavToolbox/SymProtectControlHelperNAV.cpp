////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// CSymProtectControlHelperNAV.cpp : SymProtect event provider implementation
//
// PROPRIETARY/CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.  All rights reserved. 
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ccProviderHelper.h"
#include "SymProtectControlHelperNAV.h"

using namespace ccEvtMgr;
using namespace SymProtectEvt;

CSymProtectControlHelperNAV::CSymProtectControlHelperNAV()
{
}

CSymProtectControlHelperNAV::~CSymProtectControlHelperNAV()
{
}

bool CSymProtectControlHelperNAV::AddNotificationFilter(const wchar_t* pszPath)
{
    CEventExPtr pEvent;
    m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent2::TypeId,
                                  pEvent.m_p);
    if( !pEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent) failed.\n"));
        return false;
    }

    IBBSymProtectSetConfigEvent2QIPtr pSetEvent(pEvent);
    if( !pSetEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - QI failed\n"));
        ASSERT(pSetEvent);
        return false;
    }

    // Add the filter
    pSetEvent->AddNotificationFilter(pszPath);

    CEventExPtr pReturnEvent;
    if( SendSPEvent(pSetEvent.m_p, &pReturnEvent) )
    {
        IBBSymProtectSetConfigEventQIPtr pReturnedSetEvent(pReturnEvent);
        ASSERT(pReturnedSetEvent);
        if( !pReturnedSetEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - QI on returned event failed\n"));
            return false;
        }

        ISymBBSettingsEvent::ErrorTypes eError;
        if( SYM_SUCCEEDED(pReturnedSetEvent->GetResult(eError)) &&
            eError != ISymBBSettingsEvent::UNRECOVERABLE_ERROR )
        {
            // Print a warning if there was any error recovery
            if( eError == ISymBBSettingsEvent::RECOVERED_FROM_ERROR )
            {
                CCTRACEW(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - Configuration error encountered and recovered"));
            }

            return true;
        }
        else
        {
            CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - Event configuration unsuccessfully applied\n"));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - SendSPEvent() failed\n"));
        return false;
    }

}
