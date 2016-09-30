////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymProtectEventsInterface.h"

namespace SymProtectEvt
{

class CPrivatePolicyEventEx : public CPolicyEventEx
{
public:

    virtual SYMRESULT SetEvent( const SYSTEMTIME& TimeStamp,
                                LPCWSTR wszActorName,
                                DWORD dwActorPID,
                                SP_ACTION_TYPE eAction, 
                                LPCWSTR wszTargetName, 
                                DWORD dwTargetPID,  
                                DWORD dwSession,
                                bool bWasBlocked ) throw() = 0;

};
SYM_DEFINE_INTERFACE_ID(IID_PrivatePolicyEvent, 
                        0x9c1df848, 0xbbcb, 0x4319, 0xb5, 0xb6, 0x58, 0x82, 0x93, 0xf2, 0x8a, 0x9c);

typedef CSymPtr<CPrivatePolicyEventEx> CPrivatePolicyEventExPtr;
typedef CSymQIPtr<CPrivatePolicyEventEx, &IID_PrivatePolicyEvent> CPrivatePolicyEventExQIPtr;


// This interface is supported for legacy compatibility
// Newer clients should use ISymProtectSetConfigEvent
class CStartEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    enum { TypeId = CC_SYMPROTECT_START_EVENT }; 
};
SYM_DEFINE_INTERFACE_ID(IID_StartEvent, 
                        0x48bd3871, 0x357a, 0x4534, 0x90, 0x2c, 0x29, 0x7a, 0xf0, 0x25, 0xf7, 0x0c);

typedef CSymPtr<CStartEventEx> CStartEventExPtr;
typedef CSymQIPtr<CStartEventEx, &IID_StartEvent> CStartEventExQIPtr;


class CPrivateQueryStateEventEx : public CQueryStateEventEx
{
public:

    virtual void SetSymProtectState( SP_QUERY_STATE eState ) throw() = 0;

};
SYM_DEFINE_INTERFACE_ID(IID_PrivateQueryStateEvent, 
                        0x858d1096, 0xe222, 0x47d6, 0x90, 0xd2, 0x50, 0x1, 0x1f, 0x47, 0x6e, 0xfb);

typedef CSymPtr<CPrivateQueryStateEventEx> CPrivateQueryStateEventExPtr;
typedef CSymQIPtr<CPrivateQueryStateEventEx, &IID_PrivateQueryStateEvent> CPrivateQueryStateEventExQIPtr;

// Internal configuration update notification event interface
class ISymBBConfigUpdateEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_BB_CONFIGURATION_UPDATE_EVENT }; 
};
// {68BFF2A3-FED8-47d6-903A-372F551B3FA5}
SYM_DEFINE_INTERFACE_ID(IID_BBConfigUpdateEvent, 
0x68bff2a3, 0xfed8, 0x47d6, 0x90, 0x3a, 0x37, 0x2f, 0x55, 0x1b, 0x3f, 0xa5);

typedef CSymPtr<ISymBBConfigUpdateEvent>   ISymBBConfigUpdateEventPtr;
typedef CSymQIPtr<ISymBBConfigUpdateEvent, &IID_BBConfigUpdateEvent> ISymBBConfigUpdateEventQIPtr;

// Internal MSI data access interface
class IBBPrivateAuthorizedMSIQueryEvent : public IBBAuthorizedMSIQueryEvent
{
public:
    virtual void SetSecurityLevel(MSI_SECURITY_LEVEL eLevel) throw() = 0;
    virtual SYMRESULT SetAuthorizations(void* pAuthorizations, ULONG ulAuthorizationCount) throw() = 0;
};
// {7AE06103-0552-4daa-9213-91AAF256C450}
SYM_DEFINE_INTERFACE_ID(IID_BBPrivateAuthorizedMSIQueryEvent, 
0x7ae06103, 0x552, 0x4daa, 0x92, 0x13, 0x91, 0xaa, 0xf2, 0x56, 0xc4, 0x50);

typedef CSymPtr<IBBPrivateAuthorizedMSIQueryEvent> IBBPrivateAuthorizedMSIQueryEventPtr;
typedef CSymQIPtr<IBBPrivateAuthorizedMSIQueryEvent, 
                  &IID_BBPrivateAuthorizedMSIQueryEvent> IBBPrivateAuthorizedMSIQueryEventQIPtr;

class IBBPrivateAuthorizedMSISetEvent : public IBBAuthorizedMSISetEvent
{
public:
    virtual MSI_SECURITY_LEVEL GetSecurityLevel(void) const throw() = 0;
    virtual void GetAddedAuthorizations(const void*& pAuthorizations, ULONG& ulCount) throw() = 0;
    virtual void GetDeletedAuthorizations(const void*& pAuthorizations, ULONG& ulCount) throw() = 0;
};
// {AB3CAFDC-D1C1-48b6-89EC-79BC6E3CBA22}
SYM_DEFINE_INTERFACE_ID(IID_BBPrivateAuthorizedMSISetEvent, 
0xab3cafdc, 0xd1c1, 0x48b6, 0x89, 0xec, 0x79, 0xbc, 0x6e, 0x3c, 0xba, 0x22);

typedef CSymPtr<IBBPrivateAuthorizedMSISetEvent> IBBPrivateAuthorizedMSISetEventPtr;
typedef CSymQIPtr<IBBPrivateAuthorizedMSISetEvent, 
                  &IID_BBPrivateAuthorizedMSISetEvent> IBBPrivateAuthorizedMSISetEventQIPtr;

} // SymProtectEvt