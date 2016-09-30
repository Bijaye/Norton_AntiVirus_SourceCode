////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CBBSettingsEventImpl.h"

namespace SymProtectEvt
{

// Public event to cause a settings validity check implementation
class CBBSettingsForceRefreshEvent
: public CBBSettingsEventImpl<ISymBBSettingsForceRefreshEvent>
//  public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CBBSettingsForceRefreshEvent() throw()
    {}
    virtual ~CBBSettingsForceRefreshEvent() throw()
    {}

public:
    CBBSettingsForceRefreshEvent( const CBBSettingsForceRefreshEvent& otherEvent ) throw();
    CBBSettingsForceRefreshEvent& operator=( const CBBSettingsForceRefreshEvent& otherEvent ) throw();

public:
    // Interface map.
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY( IID_BBSettingsForceRefreshEvent, ISymBBSettingsForceRefreshEvent )
    SYM_INTERFACE_MAP_CHAIN(CBBSettingsEventImpl<ISymBBSettingsForceRefreshEvent>)

public:
    // CSerializable event overrides.
    virtual long GetType() const throw();
    virtual bool IsTypeSupported( long nTypeId ) const throw();

    // Serialization methods.
    DECLARE_EVENT_SERIALIZE(); 
};

} // namespace SymProtectEvt
