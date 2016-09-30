///////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////
// IWinFW.h
///////////////////////////////////////////////////////////////////////
//	Created: JZ, 1/21/2005
///////////////////////////////////////////////////////////////////////

#ifndef __SYM_WINFOWS_FIREWALL_
#define __SYM_WINFOWS_FIREWALL_
#include <icftypes.h>
#include "SymInterface.h"
#include "SymInterfaceloader.h"
#include <tchar.h>
#include <string>
#include <list>

///////////////////////////////////////////////////////////////////////
//	define tstring
///////////////////////////////////////////////////////////////////////

#if defined(UNICODE) || defined (_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

typedef struct
{

	tstring		strPath;
	tstring		strFriendlyName;
	bool		bTCP;
	long		lPort;
	bool		bSubnetOnly;

}SYMAPP,*LPSYMAPP;

typedef std::list<SYMAPP> SYMAPP_LIST;


///////////////////////////////////////////////////////////////////////
//	Interface for wrappering Windows Firewall Api.
///////////////////////////////////////////////////////////////////////

class IWinFW : public ISymBase
{
public:


	//	====IWinFW interface====
	//
	//	is windows firewall installed
	//
	virtual bool IsWinFwInstalled() const =0;
	//
	//	is windows firewall on?
	//
	virtual HRESULT IsWinFwOn(OUT bool& bOn) =0;
	//
	//	turn on/off windows firewall
	//
	virtual HRESULT TurnOnWinFw(IN bool bTurnOn) =0;
	//
	//	is exception list allowed?
	//
	virtual HRESULT IsExceptionsAllowed(OUT bool& bAllow) =0;
	//
	//	allow/disallow exception list
	//
	virtual HRESULT AllowExceptions(IN bool bAllow) =0;
	//
	//	is a specific application authorized?
	//
	virtual HRESULT IsAppAuthorized(IN LPCTSTR szAppPath, OUT bool& bLocalSubnetOnly,OUT bool& bEnabled) =0;
	//
	//	authorize a specific application
	//
	virtual HRESULT AuthorizeApp(IN LPCTSTR szAppPath,IN LPCTSTR szFriendlyName,IN bool bLocalSubnetOnly,IN bool bEnableIt) =0;
	//
	//	is a specific port authorized?
	//
	virtual HRESULT IsPortAuthorized(IN long lPortNumber,IN NET_FW_IP_PROTOCOL ipProtocol,OUT bool& bSubnetOnly,OUT bool& bEnabled)  =0;
	//
	//	authorize a specific port
	//
	virtual HRESULT AuthorizePort( IN long portNumber, IN NET_FW_IP_PROTOCOL ipProtocol,IN LPCTSTR szFriendlyName,IN bool bLocalSubnetOnly,IN bool bEnableIt) =0;
	//
	//	remove a specific authorized application
	//
	virtual HRESULT RemoveAuthorizedApp(LPCTSTR szAppPath) =0;
	//
	//	remove specific port
	//
    virtual HRESULT RemoveAuthorizedPort(IN long lPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol)=0 ;
    
};


SYM_DEFINE_INTERFACE_ID(IID_WinFW,
                        0xf8079333, 0x80a6, 0x41fb, 0xb9, 0xf7, 0xc0, 0x37, 0xcf, 0xdc, 0x70, 0x4c);

typedef CSymPtr  <IWinFW>            IWinFWPtr;
typedef CSymQIPtr<IWinFW, &IID_WinFW> IWinFWQIPtr;



// Implements IWinFW 
SYM_DEFINE_OBJECT_ID (SYMOBJECT_WinFW,
					  0xa781b699, 0x925f, 0x49c0, 0x97, 0xb9, 0x52, 0xd8, 0xee, 0xd7, 0xcf, 0xe2);

// typedefs for sym secure communication
typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             IWinFW,
                             &SYMOBJECT_WinFW,
                             &IID_WinFW > CWinFWLoader;
static const TCHAR s_szWinFW_Dll[]= _T("WinFW.Dll");



HRESULT ConfigWinFw4Apps	  (	LPCTSTR szWinFwDll, SYMAPP_LIST& apps);
HRESULT UnConfigWinFw4Apps    (	LPCTSTR szWinFwDll, SYMAPP_LIST& apps);



#endif // !__SYM_WINFOWS_FIREWALL_
