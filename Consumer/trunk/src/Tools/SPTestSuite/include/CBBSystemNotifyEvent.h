////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "bbNotifyEventsInterface.h"
#include "ccEventUtil.h"
#include "ccSerializableEventImpl.h"

using ccEvtMgr::CError;

namespace bbEvt
{
/////////////////////////////////////////////////////////////////////////////
// Return values
#define SYM_BBNOTIFY_ERROR_BUFFER_TOO_SMALL     SYM_MAKE_RESULT( SYM_ERROR, 0x200 )

// Public event to cause a settings validity check implementation
class CBBSystemNotifyEvent
    : public ISymBaseImpl<CSymThreadSafeRefCount>,
      public ccEvtMgr::CSerializableEventImpl<ISymBBSystemNotifyEvent>
{
public:
    CBBSystemNotifyEvent() throw();
    virtual ~CBBSystemNotifyEvent() throw();

public:
    CBBSystemNotifyEvent( const CBBSystemNotifyEvent& otherEvent ) throw();
    CBBSystemNotifyEvent& operator=( const CBBSystemNotifyEvent& otherEvent ) throw();

public:
    // Interface map.
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY( IID_BBSystemNotifyEvent, ISymBBSystemNotifyEvent )
        SYM_INTERFACE_ENTRY( ccEvtMgr::IID_SerializableEventEx, ccEvtMgr::CSerializableEventEx )
        SYM_INTERFACE_ENTRY( ccEvtMgr::IID_EventEx, ccEvtMgr::CEventEx )
    SYM_INTERFACE_MAP_END()

public:
    // CSerializable event overrides.
    virtual long GetType() const throw();
    virtual bool IsTypeSupported( long nTypeId ) const throw();

public:
    // ISymBBSystemNotifyEvent implementation
    virtual SYMRESULT GetTimeStamp( PSYSTEMTIME stEventTime ) const throw();
    virtual SYMRESULT GetEventCode( BBNotifyEventCode* pulEventCode ) const throw();
    virtual SYMRESULT GetEventData( PWSTR szOutput, DWORD& dwSize ) const throw();
    virtual BOOL      IsEventDataPresent( ) const throw();

    virtual SYMRESULT SetTimeStamp( const SYSTEMTIME* pstEventTime ) throw();
    virtual SYMRESULT SetEventCode( BBNotifyEventCode ulEventCode ) throw();
    virtual SYMRESULT SetEventData( PCWSTR szEventData ) throw();

private:
    // Data members
    BBNotifyEventCode m_NotifyCode;
    SYSTEMTIME        m_EventTime;
    PWSTR             m_pszEventData;

    // Serialization methods.
    DECLARE_EVENT_SERIALIZE(); 
};

} // namespace
