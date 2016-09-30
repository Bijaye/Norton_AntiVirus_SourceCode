#pragma once

#include "ccEventUtil.h"
#include "ccEventId.h"
#include "ccSerializableEventImpl.h"
#include "HPPEventsInterface.h"

#include "EventData.h"

// {3438AE38-A4B1-42c6-8183-31D29F435F9C}
SYM_DEFINE_INTERFACE_ID(IID_HPPEventCommonImpl, 
            0x3438ae38, 0xa4b1, 0x42c6, 0x81, 0x83, 0x31, 0xd2, 0x9f, 0x43, 0x5f, 0x9c);

class CHPPEventCommonImpl :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public ccEvtMgr::CSerializableEventImpl<CHPPEventCommonInterface>
{
public:
    CHPPEventCommonImpl(void);
    virtual ~CHPPEventCommonImpl(void);

public:
    typedef ccEvtMgr::CSerializableEventImpl<CHPPEventCommonInterface> CSerializableEventBase;

    // Copy/assignment operators.
    CHPPEventCommonImpl(const CHPPEventCommonImpl& otherEvent) throw();
    CHPPEventCommonImpl& operator =(const CHPPEventCommonImpl& otherEvent) throw();

    // Interface map.
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ccEvtMgr::IID_SerializableEventEx, ccEvtMgr::CSerializableEventEx)
        SYM_INTERFACE_ENTRY(ccEvtMgr::IID_EventEx, ccEvtMgr::CEventEx)
        SYM_INTERFACE_ENTRY(IID_HPPEventCommon, CHPPEventCommonInterface)
        SYM_INTERFACE_ENTRY(IID_HPPEventCommonImpl, CHPPEventCommonImpl)
    SYM_INTERFACE_MAP_END()                 


public:
    // CHPPEventCommonInterface interface methods
    virtual bool SetPropertyBSTR(long lPropIndex, BSTR pbszValue);
    virtual bool GetPropertyBSTR(long lPropIndex, LPBSTR pbszValue);

    virtual bool SetPropertyLONG(long lPropIndex, LONG lValue);
    virtual bool GetPropertyLONG(long lPropIndex, LPLONG plValue);

    virtual bool SetPropertySYSTEMTIME(long lPropIndex, SYSTEMTIME *pSystemTime);
    virtual bool GetPropertySYSTEMTIME(long lPropIndex, SYSTEMTIME *pSystemTime);

    // CSerializable event overrides.
    virtual long GetType() const throw();
    virtual bool IsTypeSupported(long nTypeId) const throw();


    virtual void SetType(long lType) throw();

public:
    // Enum type
    enum { TypeId = HPP::Event_ID_HPPNotifyHomePage };

    // Serialization methods.
    DECLARE_EVENT_SERIALIZE(); 

protected:
    long m_lTypeID;               // Unique type ID for the event (set by derived event)
	CEventData m_props;
    
};

typedef CSymPtr<CHPPEventCommonImpl> CHPPEventCommonImplPtr;
typedef CSymQIPtr<CHPPEventCommonImpl, &IID_HPPEventCommonImpl> CHPPEventCommonImplQIPtr;
