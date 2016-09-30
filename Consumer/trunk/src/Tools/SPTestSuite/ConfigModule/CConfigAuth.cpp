////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "cconfigauth.h"

using namespace ccEvtMgr;
using namespace SymProtectEvt;

CConfigAuth::CConfigAuth(void)
{
}

CConfigAuth::~CConfigAuth(void)
{
}

bool CConfigAuth::Initialize()
{
    return m_EventProvider.Initialize();
}

bool CConfigAuth::AddUser(const wchar_t* szAccountName)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedUsersSetEvent::TypeId, pEvent) )
        return false;

    if( SYM_FAILED(IBBAuthorizedUsersSetEventQIPtr(pEvent)->
        AddUser(szAccountName)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddMD5Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedHashesSetEvent::TypeId, pEvent) )
        return false;

    if( SYM_FAILED(IBBAuthorizedHashesSetEventQIPtr(pEvent)->
        AddHash(szHashName, BB_HASH_MD5, pHash, 16, ulFileLength)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddSHA1Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedHashesSetEvent::TypeId, pEvent) )
        return false;

    if( SYM_FAILED(IBBAuthorizedHashesSetEventQIPtr(pEvent)->
        AddHash(szHashName, BB_HASH_SHA1, pHash, 20, ulFileLength)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddLocation(const wchar_t* szLocation, bool bFileOnly)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedLocationsSetEvent::TypeId, pEvent) )
    return false;

    if( SYM_FAILED(IBBAuthorizedLocationsSetEventQIPtr(pEvent)->
        AddLocation(szLocation, bFileOnly)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddInstallLocation(const wchar_t* szInstallLocation)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedLocationsSetEvent::TypeId, pEvent) )
    return false;

    if( SYM_FAILED(IBBAuthorizedLocationsSetEventQIPtr(pEvent)->
        AddMSILocation(szInstallLocation)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddDevice(unsigned long ulDeviceType)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedDevicesSetEvent::TypeId, pEvent) )
    return false;

    if( SYM_FAILED(IBBAuthorizedDevicesSetEventQIPtr(pEvent)->
        SetDeviceTypeAuthorization(ulDeviceType, true)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}

bool CConfigAuth::AddInstallGuid(const GUID& iid)
{
    CEventExPtr pEvent;
    if( !m_EventProvider.GetNewEvent(IBBAuthorizedInstallGuidsSetEvent::TypeId, pEvent) )
    return false;

    if( SYM_FAILED(IBBAuthorizedInstallGuidsSetEventQIPtr(pEvent)->
        AddGuid(iid)) )
        return false;

    return m_EventProvider.SendConfigEvent(pEvent);
}
