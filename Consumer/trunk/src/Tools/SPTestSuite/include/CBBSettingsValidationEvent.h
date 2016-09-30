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
class CBBSettingsValidationEvent
: public CBBSettingsEventImpl<ISymBBSettingsValidationEvent>
//  public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CBBSettingsValidationEvent() throw()
    {}
    virtual ~CBBSettingsValidationEvent() throw()
    {}

public:
    CBBSettingsValidationEvent( const CBBSettingsValidationEvent& otherEvent ) throw();
    CBBSettingsValidationEvent& operator=( const CBBSettingsValidationEvent& otherEvent ) throw();

public:
    // Interface map.
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY( IID_BBSettingsValidationEvent, ISymBBSettingsValidationEvent )
    SYM_INTERFACE_MAP_CHAIN(CBBSettingsEventImpl<ISymBBSettingsValidationEvent>)

public:
    // CSerializable event overrides.
    virtual long GetType() const throw();
    virtual bool IsTypeSupported( long nTypeId ) const throw();

    // Serialization methods.
    DECLARE_EVENT_SERIALIZE(); 
};

} // namespace SymProtectEvt
