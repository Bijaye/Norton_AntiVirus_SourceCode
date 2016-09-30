////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVEventBase.cpp: implementation of the CNAVEventBase class.
//  
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVEventCommon.h"
#include "time.h"
#include "NAVEventResource.h"
#include "lmcons.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVEventCommon::CNAVEventCommon() : m_lTypeID (AV::Event_ID_Base)
{
}

CNAVEventCommon::~CNAVEventCommon()
{
}

void CNAVEventCommon::InitCommon ()
{
    // Get the user name. If there isn't one use "N/A" from string table.
    //
    TCHAR szTempName [UNLEN] = _T("");
    DWORD dwTempNameSize = sizeof (szTempName);

    // If this fails don't log NULL.
    //
    if ( ::GetUserName (szTempName, &dwTempNameSize) > 0 && '\0' != szTempName[0])
    {
        props.SetData ( AV::Event_Base_propUserName, szTempName );
    }

    Init(); // CSerializableEvent
}

CNAVEventCommon& CNAVEventCommon::operator =(const CNAVEventCommon& Event)
{
    if (this != &Event)
    {
        // This is the CSerializableEvent::Init ()
        Init();

        // Copy all base members
        dynamic_cast<CSerializableEvent&>(*this).operator =(Event);
        m_lTypeID = Event.m_lTypeID;
        props = Event.props;     
    }

    return *this;
}

ccEvtMgr::CError::ErrorType CNAVEventCommon::Load( const LPVOID pData, 
                                         DWORD dwSize,
                                         DWORD& dwRead)
{
    LPBYTE pOffset = LPBYTE(pData);

    if( props.LoadDataFromStream(pOffset) == false )
	{
		return ccEvtMgr::CError::eFatalError;
	}
    ULONG ulSize = props.GetSize();
    pOffset+= (DWORD) ulSize;
    dwRead += (DWORD) ulSize;

    // Set our member Type ID equal to the serialized one
    //
    props.GetData ( AV::Event_Base_propType, m_lTypeID );

    // Call base
	return CSerializableEvent::Load(pOffset, dwSize, (DWORD) ulSize);
}

ccEvtMgr::CError::ErrorType CNAVEventCommon::Save ( LPVOID pData, 
                                        DWORD dwSize, 
                                        DWORD& dwWritten) const
{
    LPBYTE pOffset = LPBYTE(pData);

	if( props.SaveDataToStream(pOffset, (ULONG) dwSize, (ULONG) dwWritten) == false )
	{
		return ccEvtMgr::CError::eFatalError;
	}

    pOffset+= dwWritten;

    // Call base
    return CSerializableEvent::Save(pOffset, dwSize, dwWritten);
}

ccEvtMgr::CError::ErrorType CNAVEventCommon::GetSizeMax(DWORD& dwSize) const
{
    dwSize += (DWORD) props.GetSize();
    return CSerializableEvent::GetSizeMax (dwSize);
}

long CNAVEventCommon::GetType() const
{
    return m_lTypeID;
}

void CNAVEventCommon::SetType ( long lTypeID )
{
    m_lTypeID = lTypeID;
    props.SetData ( AV::Event_Base_propType, lTypeID );
}

bool CNAVEventCommon::IsTypeSupported(long nTypeId) const
{
    // Test against our id
    return  (nTypeId == m_lTypeID) ? true : false;
}
