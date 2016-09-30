////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// AvEvents.h - Defines interfaces for AntiVirus event activities.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "syminterface.h"
#include "EventData.h"

///////////////////////////////////////////////////////////////////////////////
// IAvEvent	 
namespace AV
{
///////////////////////////////////////////////////////////////////////////////
// IAvEventFactory
//
// You *MUST* CoInitialize COM on the thread(s) that call these functions.
//
class IAvEventFactory : public ISymBase
{
public:
	virtual SYMRESULT BroadcastAvEvent( CEventData& event ) = 0;

    // ***********************************
    // Synchronous events only
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn ) = 0;
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime ) = 0;
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime, HANDLE hTerminate) = 0;
};

// {55103A2B-79D5-4469-8538-30867CDC7884}
SYM_DEFINE_INTERFACE_ID( IID_AvEventFactory,
                         0x55103a2b, 0x79d5, 0x4469, 0x85, 0x38, 0x30, 0x86, 0x7c, 0xdc, 0x78, 0x84);

typedef CSymPtr<IAvEventFactory> IAvEventFactoryPtr;
typedef CSymQIPtr<IAvEventFactory, &IID_AvEventFactory> IAvEventFactoryQIPtr;

///////////////////////////////////////////////////////////////////////////////
// Define error codes.

#define AVFACTORY_ERR_COM		SYM_MAKE_RESULT( SYM_ERROR, 0x0000 ) // Misc. COM error.
#define AVFACTORY_ERR_EVENT_MGR	SYM_MAKE_RESULT( SYM_ERROR, 0x0001 ) // Could not connect to local event manager.
}; // end namespace AV
