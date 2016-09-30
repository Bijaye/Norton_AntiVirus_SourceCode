#include "StdAfx.h"
#include "HPPEventCommonImpl.h"

using namespace ccEvtMgr;
using namespace ccLib;

CHPPEventCommonImpl::CHPPEventCommonImpl(void) : 
m_lTypeID (HPP::Event_ID_HPPNotifyHomePage)
{
}

CHPPEventCommonImpl::~CHPPEventCommonImpl(void)
{
}

CHPPEventCommonImpl::CHPPEventCommonImpl(const CHPPEventCommonImpl& otherEvent)
{
    // Call assignment operator
    *this = otherEvent;
}


CHPPEventCommonImpl& CHPPEventCommonImpl::operator =(const CHPPEventCommonImpl& otherevent)
{
    if (this != &otherevent)
    {
        // This is the CSerializableEvent::Init ()
        Init();

        // Copy all base members
        dynamic_cast<CSerializableEventBase&>(*this).operator =(otherevent);
        m_lTypeID = otherevent.m_lTypeID;
        m_props = otherevent.m_props;     
    }

    return *this;
}

void CHPPEventCommonImpl::Destroy()
{
    return;
}

ccEvtMgr::CError::ErrorType CHPPEventCommonImpl::Load( const LPVOID pData, 
                                                  DWORD dwSize,
                                                  DWORD& dwRead)
{
    LPBYTE pOffset = LPBYTE(pData);

    if( m_props.LoadDataFromStream(pOffset) == false )
    {
        return ccEvtMgr::CError::eFatalError;
    }
    ULONG ulSize = m_props.GetSize();
    pOffset+= (DWORD) ulSize;
    dwRead += (DWORD) ulSize;

    // Set our member Type ID equal to the serialized one
    //
    m_props.GetData ( HPP::Event_Base_propType, m_lTypeID );

    // Call base
    return CSerializableEventBase::Load(pOffset, dwSize, (DWORD) ulSize);
}

ccEvtMgr::CError::ErrorType CHPPEventCommonImpl::Save ( LPVOID pData, 
                                                   DWORD dwSize, 
                                                   DWORD& dwWritten) const
{
    LPBYTE pOffset = LPBYTE(pData);

    if( m_props.SaveDataToStream(pOffset, (ULONG) dwSize, (ULONG) dwWritten) == false )
    {
        return ccEvtMgr::CError::eFatalError;
    }

    pOffset+= dwWritten;

    // Call base
    return CSerializableEventBase::Save(pOffset, dwSize, dwWritten);
}

ccEvtMgr::CError::ErrorType CHPPEventCommonImpl::GetSizeMax(DWORD& dwSize) const
{
    dwSize += (DWORD) m_props.GetSize();
    return CSerializableEventBase::GetSizeMax (dwSize);
}

long CHPPEventCommonImpl::GetType() const
{
    return m_lTypeID;
}

void CHPPEventCommonImpl::SetType ( long lTypeID )
{
    m_lTypeID = lTypeID;
    m_props.SetData ( HPP::Event_Base_propType, lTypeID );
}

bool CHPPEventCommonImpl::IsTypeSupported(long nTypeId) const
{
    return (nTypeId == m_lTypeID);
}


bool CHPPEventCommonImpl::SetPropertyBSTR(long lPropIndex, BSTR bszValue)
{
#pragma message(AUTO_FUNCNAME "TODO: Code Review Item")
// 
// TODO: Code Review Item
//   Use the common client ccCatch for try/catch
//   handling.
	CExceptionInfo exceptionInfo;
    if(!bszValue)
        return false;

    try
    {
        _bstr_t bstrTemp ( bszValue );
        m_props.SetData ( lPropIndex, (wchar_t*) bstrTemp );

        return true;
    }
	CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException() != false)
    {
        return false;
    }
    return true;
}

bool CHPPEventCommonImpl::GetPropertyBSTR(long lPropIndex, LPBSTR pbszValue)
{
#pragma message(AUTO_FUNCNAME "TODO: Code Review Item")
// 
// TODO: Code Review Item
//   put a try/catch handler around _bstr_t usage
//   it can throw _com_error's if allocation fails

	CExceptionInfo exceptionInfo;
    if(!pbszValue)
        return false;

    std::wstring strTemp; 

    if ( !m_props.GetData ( lPropIndex, strTemp ))
        return false;
	try
	{
		_bstr_t bstrTemp ( strTemp.c_str() );
		*pbszValue = bstrTemp.copy ();
		return true;
	}
	CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException() != false)
    {
        return false;
    }

    return true;
}

bool CHPPEventCommonImpl::SetPropertyLONG(long lPropIndex, LONG lValue)
{
    m_props.SetData ( lPropIndex, lValue );
    return true;
}

bool CHPPEventCommonImpl::GetPropertyLONG(long lPropIndex, LPLONG plValue)
{
    if(!plValue)
        return false;
	
    return m_props.GetData ( lPropIndex, *plValue );
}

bool CHPPEventCommonImpl::SetPropertySYSTEMTIME(long lPropIndex, SYSTEMTIME *pSystemTime)
{
    m_props.SetData ( lPropIndex, (LPBYTE)pSystemTime, sizeof(SYSTEMTIME));
    return true;
}

bool CHPPEventCommonImpl::GetPropertySYSTEMTIME(long lPropIndex, SYSTEMTIME *pSystemTime)
{
    if(!pSystemTime)
        return false;

    long lSize = sizeof(SYSTEMTIME);

    return m_props.GetData ( lPropIndex, (LPBYTE)pSystemTime, lSize );
}



