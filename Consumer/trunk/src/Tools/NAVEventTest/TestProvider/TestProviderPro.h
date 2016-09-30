////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestProviderPro.h: interface for the CTestProviderPro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTPROVIDERPRO_H__7D4ADB6D_433B_498D_973E_C4CA30870318__INCLUDED_)
#define AFX_TESTPROVIDERPRO_H__7D4ADB6D_433B_498D_973E_C4CA30870318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SymInterface.h"
#include "ccProviderEx.h"

using namespace ccEvtMgr;

#include "NAVEvents.h"
#include "NAVEventUtils.h"
#include "ccModuleID.h"

#define UM_EVENT WM_USER + 1
#define UM_SHUTDOWN WM_USER + 2

class CTestProviderPro : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public CProviderEx  
{
public:
	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(IID_ProviderEx, CProviderEx)
	SYM_INTERFACE_MAP_END() 

    CTestProviderPro();
    virtual ~CTestProviderPro();

private:
    CTestProviderPro(const CTestProviderPro&);
    CTestProviderPro& operator =(const CTestProviderPro&);

public:
    BOOL Connect();
    BOOL Disconnect();
    BOOL Register(HWND hWnd, 
                  long& nProviderId);
    BOOL Unregister();
    void DeleteEvent(CEventEx* pEvent);
    BOOL CreateTestEvent(BOOL bBroadcast, 
                         long nValue,
                         long& nEventId,
                         long lEventType, 
						 CString strName);

public:
    virtual CError::ErrorType OnEvent(long nProviderId, 
                                      const CEventEx& Event);
    virtual CError::ErrorType OnShutdown();

protected:
    long m_nProviderId;
    CEventFactoryEx* m_pEventFactory;
    IProviderEx* m_piProvider;
    HWND m_hWnd;
    EVENTMANAGERLib::IEventManagerPtr m_piEventManager;
};

#endif // !defined(AFX_TESTPROVIDERPRO_H__7D4ADB6D_433B_498D_973E_C4CA30870318__INCLUDED_)
