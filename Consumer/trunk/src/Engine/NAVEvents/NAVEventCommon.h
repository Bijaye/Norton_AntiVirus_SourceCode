////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVEventBase.h: interface for the CNAVEventBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AllNAVEvents.h"
#include "EventData.h"
#include "ccEventId.h" // List of AV event ID's.

#include "ccSerializableEvent.h"

// {EFA77582-AC38-49ab-B6E0-8B508AFDF410}
SYM_DEFINE_INTERFACE_ID(IID_NAVEventCommon, 
						0xefa77582, 0xac38, 0x49ab, 0xb6, 0xe0, 0x8b, 0x50, 0x8a, 0xfd, 0xf4, 0x10);

class CNAVEventCommon : 
    public ccEvtMgr::CSerializableEvent
{
public:
	CNAVEventCommon();
	virtual ~CNAVEventCommon();

	SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ccEvtMgr::IID_SerializableEventEx, ccEvtMgr::CSerializableEventEx)
		SYM_INTERFACE_ENTRY(ccEvtMgr::IID_EventEx, ccEvtMgr::CEventEx)
        SYM_INTERFACE_ENTRY(IID_NAVEventCommon, CNAVEventCommon)
	SYM_INTERFACE_MAP_END()
    
	CNAVEventCommon(const CNAVEventCommon& Event)
    {
        // Call assignment operator
        *this = Event;
    }
    CNAVEventCommon& operator =(const CNAVEventCommon& Event);

    virtual long GetType() const;
    virtual void SetType ( long lTypeID );
    virtual bool IsTypeSupported(long nTypeId) const;

    virtual ccEvtMgr::CError::ErrorType Load(const LPVOID pData, 
                                   DWORD dwSize,
                                   DWORD& dwRead);
    virtual ccEvtMgr::CError::ErrorType Save(LPVOID pData, 
                                   DWORD dwSize, 
                                   DWORD& dwWritten) const;
    virtual ccEvtMgr::CError::ErrorType GetSizeMax(DWORD& dwSize) const;
    
    virtual void InitCommon ();

    CEventData props;

protected:
    long m_lTypeID;               // Unique type ID for the event (set by derived event)
};

typedef CSymPtr<CNAVEventCommon> CNAVEventCommonPtr;
typedef CSymQIPtr<CNAVEventCommon, &IID_NAVEventCommon> CNAVEventCommonQIPtr;

