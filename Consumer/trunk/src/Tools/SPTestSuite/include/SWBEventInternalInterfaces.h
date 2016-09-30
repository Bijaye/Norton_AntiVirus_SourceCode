////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CSafeWebBrowserImpl.h"
#include "SafeWebBrowserEventsInterface.h"

namespace bbSafeWebBrowserEvt
{

///////////////////////////////////////////////////////////////////////////////////////////////////

class CSWBEventDataBase; // Forward Reference

///////////////////////////////////////////////////////////////////////////////////////////////////

class IBBSWBEventBaseInternal : public ISymBase
{
public:
    virtual void SetResult( bbSafeWebBrowser::SWBResult result ) throw() = 0;
    virtual bbSafeWebBrowser::SWBResult GetResult() const throw() = 0;
    virtual void SetOperation( SWB_OPERATION Operation ) throw() = 0;
    virtual SWB_OPERATION GetOperation() const throw() = 0;
};
// IID_BBSWBEventBaseInternal => {95F11795-1B34-4219-A049-8A076C9913B9}
SYM_DEFINE_INTERFACE_ID(IID_BBSWBEventBaseInternal,
0x95f11795, 0x1b34, 0x4219, 0xa0, 0x49, 0x8a, 0x07, 0x6c, 0x99, 0x13, 0xb9);

typedef CSymPtr<IBBSWBEventBaseInternal>   IBBSWBEventBaseInternalPtr;
typedef CSymQIPtr<IBBSWBEventBaseInternal, 
                  &IID_BBSWBEventBaseInternal> IBBSWBEventBaseInternalQIPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////

class IBBSWBControlEventInternal : public IBBSWBControlEvent
{
public:
    virtual CSWBEventDataBase& GetEventData() throw() = 0;
    virtual bbSafeWebBrowser::CSafeWebBrowserImpl& GetEventDataImpl() throw() = 0;
};
// IID_BBSWBControlEventInternal => {56E2E0C0-1377-4055-9E53-8B1572E68DD8}
SYM_DEFINE_INTERFACE_ID(IID_BBSWBControlEventInternal,
0x56e2e0c0, 0x1377, 0x4055, 0x9e, 0x53, 0x8b, 0x15, 0x72, 0xe6, 0x8d, 0xd8);

typedef CSymPtr<IBBSWBControlEventInternal>   IBBSWBControlEventInternalPtr;
typedef CSymQIPtr<IBBSWBControlEventInternal, 
                  &IID_BBSWBControlEventInternal> IBBSWBControlEventInternalQIPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////

class IBBSWBDataEventInternal : public IBBSWBDataEvent
{
public:
    virtual CSWBEventDataBase& GetEventData() throw() = 0;
    virtual bbSafeWebBrowser::CSafeWebBrowserImpl& GetEventDataImpl() throw() = 0;
};

// IID_BBSWBDataEventInternal => {D436DAEC-5E71-443B-ACC4-E8668E284443}
SYM_DEFINE_INTERFACE_ID(IID_BBSWBDataEventInternal,
0xd436daec, 0x5e71, 0x443b, 0xac, 0xc4, 0xe8, 0x66, 0x8e, 0x28, 0x44, 0x43);

typedef CSymPtr<IBBSWBDataEventInternal>   IBBSWBDataEventInternalPtr;
typedef CSymQIPtr<IBBSWBDataEventInternal, 
                  &IID_BBSWBDataEventInternal> IBBSWBDataEventInternalQIPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////

class CSWBNotificationEventData;

class IBBSWBNotificationEventInternal : public IBBSWBNotificationEvent
{
public:
    virtual CSWBNotificationEventData& GetEventData() const throw() = 0;
};

// IID_BBSWBNotificationEventInternal => {31ACA0A7-FD89-48B0-B231-58C61957116C}
SYM_DEFINE_INTERFACE_ID(IID_BBSWBNotificationEventInternal,
0x31aca0a7, 0xfd89, 0x48b0, 0xb2, 0x31, 0x58, 0xc6, 0x19, 0x57, 0x11, 0x6c);

typedef CSymPtr<IBBSWBNotificationEventInternal>   IBBSWBNotificationEventInternalPtr;
typedef CSymQIPtr<IBBSWBNotificationEventInternal, 
                  &IID_BBSWBNotificationEventInternal> IBBSWBNotificationEventInternalQIPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////

}  // bbSafeWebBrowserEvt

