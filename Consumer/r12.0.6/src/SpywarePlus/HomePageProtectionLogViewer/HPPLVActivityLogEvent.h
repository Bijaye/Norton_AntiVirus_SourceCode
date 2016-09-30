#pragma once

#include "ccLogViewerInterface.h"
#include "HPPEventsInterface.h"


class CHPPLVActivityLogEvent : 
    public cc::ILogViewerEvent,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CHPPLVActivityLogEvent(void);
    virtual ~CHPPLVActivityLogEvent(void);

private:
    CHPPLVActivityLogEvent(const CHPPLVActivityLogEvent&);
    CHPPLVActivityLogEvent& operator =(const CHPPLVActivityLogEvent&);

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerEvent, cc::ILogViewerEvent)
    SYM_INTERFACE_MAP_END()                 

public:
    virtual HRESULT GetImageListIndex(int& iImageListIndex);
    virtual HRESULT GetColumnData(int iColumn, 
        DWORD dwFlags,
        VARIANT& varData);
    virtual HRESULT GetDetails(cc::LVEVENT_DETAILSTYPE Type, 
        DWORD dwFlags,
        LPTSTR szDetailText, 
        DWORD& dwSize);

public:
    HRESULT SetEventData(CHPPEventCommonInterface *pEvent)
    {
        if(pEvent)
        {
            m_spEvent = pEvent;
            return S_OK;
        }
        
        return E_FAIL;
    };

protected:
    HRESULT LoadHtmlResource(UINT iResourceId, CString &cszHtmlResource);
	bool UTCSystemTimeToLocalTime ( const SYSTEMTIME* psystimeUTC, SYSTEMTIME* psystimeLocal );
    bool LocalTimeToUTCSystemTime ( const SYSTEMTIME* psystimeLocal, SYSTEMTIME* psystimeUTC );
    
protected:
    CHPPEventCommonInterfacePtr m_spEvent;
	CString m_cszDetails;



};
