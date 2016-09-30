////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccFirewallSettingsInterface.h"
#include "ccEventEx.h"

///////////////////////////////////////////////////////////////////////////////
class IAlertData : public ISymBase
{
public:
	enum ALERT_RESULT
	{
		NO_ACTION,
	    CREATE_ALE_RULES,
		CREATE_CUSTOM_RULE,
		CUSTOM_RULES_CREATED,
		PERMIT_ALL,
        PERMIT_ONCE,
		BLOCK_ALL,
        BLOCK_ONCE,
		ADD_NETSPEC_TO_LOCATION,
		FORWARD_ALERT_TO_TRAY,
        PERMIT_PROCESS,
        BLOCK_PROCESS
	};

	enum ALERT_TYPE
    {
	    ISALERTTYPE_ACTIVEX = 0,
	    ISALERTTYPE_CIB ,
	    ISALERTTYPE_COOKIE ,
	    ISALERTTYPE_IDS ,
	    ISALERTTYPE_IP ,
	    ISALERTTYPE_JAVA ,
	    ISALERTTYPE_LAUNCHER ,
	    ISALERTTYPE_LISTEN ,
	    ISALERTTYPE_LOCATION ,
		ISALERTTYPE_LOCATION_CHANGE , 
	    ISALERTTYPE_MODULE ,
	    ISALERTTYPE_SECURITY,
		ISALERTTYPE_SVCMON
    };

public:

	// This method returns a Ref counted Event object that it is the caller must release
	virtual bool GetEvent(ccEvtMgr::CEventEx*& pEvent) const throw() = 0;
	virtual bool GetAlertType(ALERT_TYPE& eAlertType) const throw() = 0;

	virtual bool GetAleInfo(bool& bAleExists, bool& bAleCoverTraffic, LPSTR szAppName, DWORD& dwByteSize) throw() = 0;
	virtual bool GetAleInfo(bool& bAleExists, bool& bAleCoverTraffic, LPWSTR wszAppName, DWORD& dwByteSize) throw() = 0;

	virtual bool SetAlertResult(ALERT_RESULT eResult) throw() = 0;
	virtual bool GetAlertResult(ALERT_RESULT& eResult) const throw() = 0;

	// This method returns a Ref counted Location object that the caller must release
	virtual bool GetLocationToSave(ccFirewallSettings::ILocation*& pLocation) const throw() = 0;
	virtual bool SetLocationToSave(const ccFirewallSettings::ILocation* pLocation) throw() = 0;
	
	// This method returns a Ref counted Rule object that it is the caller must release
	virtual bool GetCustomAppRule(ccFirewallSettings::IRuleList*& pRuleList) const throw() = 0;
	virtual bool SetCustomAppRule(const ccFirewallSettings::IRuleList* pRuleList) throw() = 0;

	// This method returns a Ref counted Rule object that it is the caller must release
	virtual bool GetCustomSystemRule(ccFirewallSettings::IRule*& pRule) const throw() = 0;
	virtual bool SetCustomSystemRule(const ccFirewallSettings::IRule* pRule) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AlertData, 
                        0x9f7a655, 0x264e, 0x4517, 0x83, 0xaa, 0xe2, 0xef, 0xeb, 0x28, 0xfd, 0x63);

typedef CSymPtr<IAlertData> IAlertDataPtr;
typedef CSymQIPtr<IAlertData, &IID_AlertData> IAlertDataQIPtr;


///////////////////////////////////////////////////////////////////////////////

// This interface is implemented by the product TrayIcon
// and is called back from SymFWAgt when tray icon updates are 
// needed
class ITrayIconSink : public ISymBase
{
public:
    virtual bool OnNewAlert(IAlertData* pAlertData) throw() = 0;
	virtual bool OnAleRulesSilentlyCreated(LPCSTR szShortAppName, LPCSTR szFilePath) throw() = 0;
    virtual bool OnAleRulesSilentlyCreated(LPCWSTR szShortAppName, LPCWSTR szFilePath) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_TrayIconSink, 
                        0xf070bd98, 0xffd3, 0x7b00, 0xba, 0x2c, 0x62, 0x27, 0x56, 0x7e, 0x2a, 0x37);

typedef CSymPtr<ITrayIconSink> ITrayIconSinkPtr;
typedef CSymQIPtr<ITrayIconSink, &IID_TrayIconSink> ITrayIconSinkQIPtr;

///////////////////////////////////////////////////////////////////////////////
class IAlertSink : public ISymBase
{
public:
	// The Alert Info Object is not additionaly ref counted for the sink, it should not be release by the sink 
    virtual bool OnDisplayAlert(IAlertData* pAlertInfo) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AlertSink, 
                        0xb301be38, 0xd040, 0x4a61, 0xa1, 0xcc, 0xc0, 0x98, 0xa2, 0x7b, 0xf9, 0x34);

typedef CSymPtr<IAlertSink> IAlertSinkPtr;
typedef CSymQIPtr<IAlertSink, &IID_AlertSink> IAlertSinkQIPtr;

///////////////////////////////////////////////////////////////////////////////
class ISymFWAgt : public ISymBase
{
public:
	virtual bool Initialize() throw() = 0;
	virtual void Destroy() throw() = 0;

	// StartSubscriber is used to start the Subscriber and auto Rule creation
	// The subscriber runs on the calling thread, so be sure to initialize COM in MTA
	// And pump messages as needed
	virtual bool StartSubscriber() throw() = 0;

	virtual bool RegisterTrayIcon(ITrayIconSink* pSink, DWORD& dwID) throw() = 0;
    virtual bool RegisterAlert(IAlertSink* pSink, DWORD& dwID) throw() = 0;

	virtual bool UnRegisterTrayIcon(DWORD dwID) throw() = 0;
    virtual bool UnRegisterAlert(DWORD dwID) throw() = 0;
};
SYM_DEFINE_INTERFACE_ID(IID_SymFWAgt, 
                        0x28c98dbb, 0xc47a, 0x4a90, 0xad, 0xf0, 0xcf, 0x21, 0xae, 0x7, 0xc7, 0x78);


typedef CSymPtr<ISymFWAgt> ISymFWAgtPtr;
typedef CSymQIPtr<ISymFWAgt, &IID_SymFWAgt> ISymFWAgtQIPtr;


