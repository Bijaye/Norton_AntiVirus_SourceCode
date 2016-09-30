////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVEvent.h: interface for the CAVEvent class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "ccLogViewerInterface.h"
#include "NAVEventCommon.h"
#include "ccLogFactoryEx.h"
#include "resourcehelper.h"

#include "tchar.h"
#include <vector>
#include <string>
#include <map>

typedef ::std::map <long, _variant_t*> mapEventData;
typedef ::std::map <long, DWORD>   mapEventFormat;

class CAVEvent :
    public cc::ILogViewerEvent2,
	public ISymBaseImpl<CSymThreadSafeRefCount>  
{
public:
	void SetDetailsText ( LPCTSTR lpszDetails );
    void SetDetailsHTML ( LPCTSTR lpszDetails );

	bool AddData ( int iColumn, const VARIANT& vData, DWORD dwFormat = 0 );
	CAVEvent();
	virtual ~CAVEvent();

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerEvent, cc::ILogViewerEvent)
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerEvent2, cc::ILogViewerEvent2)
	SYM_INTERFACE_MAP_END() 

    HRESULT GetImageListIndex(int& iImageListIndex);
	HRESULT GetColumnData(int iColumn, DWORD dwFlags, VARIANT& varData);
    
    HRESULT IsDetailsHTML(BOOL& bIsHTML);

    HRESULT GetDetails( cc::LVEVENT_DETAILSTYPE Type, /*in*/
                        DWORD dwFlags, 
						LPSTR szDetailText,
                        DWORD& dwSize);         /* [in/out] */ 

    HRESULT GetDetailsW( cc::LVEVENT_DETAILSTYPE Type, /*in*/
        DWORD dwFlags, 
        LPWSTR szDetailText,
        DWORD& dwSize);         /* [in/out] */ 

    long lEventType;                                  // Event type

protected:
    mapEventData m_mapEventData;
    mapEventFormat m_mapEventFormat;

    tstring m_strEventDetailsHTML;
    tstring m_strEventDetailsText;

    bool UTCSystemTimeToLocalTime ( const SYSTEMTIME* psystimeUTC, SYSTEMTIME* psystimeLocal );
    bool LocalTimeToUTCSystemTime ( const SYSTEMTIME* psystimeLocal, SYSTEMTIME* psystimeUTC );
};
