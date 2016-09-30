////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptRefresh.h
//
//////////////////////////////////////////////////////////////////////

#if defined(_NAVOPTREFRESH_CONSTANTS)
        const TCHAR szNAVOptionRefreshEngine_OnDemandRefreshEvent[]    = _T("NAVOptionRefreshEngine:OnDemandRefreshEvent_{0DE1A1FB-F13D-4d16-A287-51EF23F28913}");
        const TCHAR szNAVOptionRefreshEngine_DefAlertLicenseChange[]    = _T("NAVOptionRefreshEngine:DefAlertLicenseChange_{C36CC3BE-FD77-4ac6-982A-47078FDA0B97}");
#else
        extern const TCHAR szNAVOptionRefreshEngine_OnDemandRefreshEvent;
        extern const TCHAR szNAVOptionRefreshEngine_DefAlertLicenseChange;
#endif

#if !defined(_NAVOPTREFRESH_NOINTERFACES)
#if !defined(_NAVOPTREFRESH_H__B4D43162_6F7B_4dfc_91A1_74D1FC52DFA9__INCLUDED_)
#define _NAVOPTREFRESH_H__B4D43162_6F7B_4dfc_91A1_74D1FC52DFA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//====================================================================================================
//
// IID's of Interfaces
//
//====================================================================================================

/////////////////////////////////////////////////////////
// ILicensingGateway
class INAVOptionRefreshEngine: public SIMON::IInterfaceManagement
{
public:
    SIMON_STDMETHOD(Init)() = 0;
    SIMON_STDMETHOD(Start)() = 0;
    SIMON_STDMETHOD(Stop)() = 0;
};

// {9AFA4667-6B5F-44c8-9445-CA1256B1B01E}
DEFINE_SIMON_GUID(IID_INAVOptionRefreshEngine, 0x9afa4667, 0x6b5f, 0x44c8, 0x94, 0x45, 0xca, 0x12, 0x56, 0xb1, 0xb0, 0x1e);

//====================================================================================================
//
// CLSID's of Objects
//
//====================================================================================================

// {FD4B39E0-F19C-487f-AB88-5DC7C455FDD9}
DEFINE_SIMON_GUID(CLSID_CNAVOptionRefreshEngine, 0xfd4b39e0, 0xf19c, 0x487f, 0xab, 0x88, 0x5d, 0xc7, 0xc4, 0x55, 0xfd, 0xd9);

//=============================================================================
//
// Authentication of shipping (post-beta) NAVOPTRF.DLL
//
//=============================================================================

// {812D44B1-51FE-464c-AE79-F2F6890FE8EF}
DEFINE_GUID(GUID_NAVOPTRF_AUTH,
	0x812d44b1, 0x51fe, 0x464c, 0xae, 0x79, 0xf2, 0xf6, 0x89, 0xf, 0xe8, 0xef);

#if defined NAVOPTREFRESH_EXPORTS
  #define NAVOPTRF_IMPL __declspec(dllexport)
#else
  #define NAVOPTRF_IMPL __declspec(dllimport)
#endif

extern "C"
{
	NAVOPTRF_IMPL void GetAuthGUID(GUID& guid);
	typedef void (*GETAUTHGUID)(GUID&);
};

#endif // !defined(_NAVOPTREFRESH_H__B4D43162_6F7B_4dfc_91A1_74D1FC52DFA9__INCLUDED_)
#endif// !defined(_NAVOPTREFRESH_NOINTERFACES)


