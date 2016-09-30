///////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////
// WinFW.h
///////////////////////////////////////////////////////////////////////
//	Created: JZ, 1/21/2005
///////////////////////////////////////////////////////////////////////
#pragma once

#include <atlbase.h>
#include <netfw.h>

///////////////////////////////////////////////////////////////////////
//	configure windows firewall
///////////////////////////////////////////////////////////////////////
class CWinFW : public ISymBaseImpl<CSymThreadSafeRefCount>,
               public IWinFW
{
public:
    CWinFW();
    virtual ~CWinFW();

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY (IID_WinFW, IWinFW)
    SYM_INTERFACE_MAP_END()
	//	====IWinFW interface====
	//
	//	is windows firewall installed
	//
	virtual bool IsWinFwInstalled() const;
	//
	//	is windows firewall on?
	//
	virtual HRESULT IsWinFwOn(OUT bool& bOn);
	//
	//	turn on/off windows firewall
	//
	virtual HRESULT TurnOnWinFw(IN bool bTurnOn);
	//
	//	is exception list allowed?
	//
	virtual HRESULT IsExceptionsAllowed(OUT bool& bAllow);
	//
	//	allow/disallow exception list
	//
	virtual HRESULT AllowExceptions(IN bool bAllow);
	//
	//	is a specific application authorized?
	//
	virtual HRESULT IsAppAuthorized(IN LPCTSTR szAppPath, OUT bool& bLocalSubnetOnly,OUT bool& bEnabled);
	//
	//	authorize a specific application
	//
	virtual HRESULT AuthorizeApp(IN LPCTSTR szAppPath,IN LPCTSTR szFriendlyName,IN bool bLocalSubnetOnly,IN bool bEnableIt);
	//
	//	is a specific port authorized?
	//
	virtual HRESULT IsPortAuthorized(IN long lPortNumber,IN NET_FW_IP_PROTOCOL ipProtocol,OUT bool& bSubnetOnly,OUT bool& bEnabled) ;
	//
	//	authorize a specific port
	//
	virtual HRESULT AuthorizePort( IN long portNumber, IN NET_FW_IP_PROTOCOL ipProtocol,IN LPCTSTR szFriendlyName,IN bool bLocalSubnetOnly,IN bool bEnableIt);
	//
	//	remove a specific authorized application
	//
	virtual HRESULT RemoveAuthorizedApp(LPCTSTR szAppPath) ;
	//
	//	remove specific port
	//
    virtual HRESULT RemoveAuthorizedPort(IN long lPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol) ;
private:
	//
	//	private function
	//
	bool IsInitOK() const {return m_bInitOK;}
	bool Init() ;
	HRESULT GetLastError() const;
	HRESULT CheckHr(HRESULT hr);
private:
	//
	//	private data
	//
	bool					m_bWinFWInstalled ;	//windows firewall installed?
	bool					m_bInitOK;			//initializatio ok?
	HRESULT					m_nLastError;		//last error
	CComPtr<INetFwMgr>		m_spFwMgr;			//windows firewall manager
	CComPtr<INetFwProfile>	m_spFwProfile;		//current profile
	CComPtr<INetFwPolicy>	m_spFwPolicy;		//local policy
private:
    // prevent from being used
    CWinFW (const CWinFW&);
    CWinFW& operator= (const CWinFW&);
};

