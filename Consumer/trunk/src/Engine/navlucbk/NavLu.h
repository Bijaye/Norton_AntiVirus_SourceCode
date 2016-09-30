////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavLu.h: interface for the CNavLu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVLU_H__3C2E74AD_887E_11D2_B40A_00600831DD76__INCLUDED_)
#define AFX_NAVLU_H__3C2E74AD_887E_11D2_B40A_00600831DD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingsNames.h"  // Settings Names

#include "navlucbk.h"
#include "LuCOM.h"          // The LiveUpdate COM interface definitions.
#include "ProductRegCOM.h"
//#include "NAVDefutilsLoader.h"
//#include "defutilsinterface.h"
#include <AVInterfaceLoader.h>
#include <defutils-types.h>
#include <ccStringInterface.h>

struct DefsDateStruct
{
    DWORD dwYear;
    DWORD dwMonth;
    DWORD dwDay;
    DWORD dwRev;
};

class CNavLu  
{
public:
    BOOL Initialize();
	BOOL GetVirusDefsDelivered();

    CNavLu ();
	virtual ~CNavLu();

    //=================================================================================================
    // PreProcess() checks licensing for paid content (virus definitions) subscription and
    // calls into the IAVDefInfo object to pre-process microdefs provided the subscription
    // is valid.
    // Return results:
    //  true: Paid content subscription is valid (Pre-processing for microdefs has been called)
    //  false: Paid content subscription is expired (Pre-processing for microdefs has not been called)
    //=================================================================================================
	bool PreProcess( );

    //=================================================================================================
    // If you call PreProcess() call PostProcess() regardless of what it returned so defutils can
    // clean up the mess it made in PreProcess().
    //=================================================================================================
	HRESULT PostProcess( );

	HRESULT RegisterCommandLines();
	HRESULT UnregisterCommandLines( void );

    // This will abort the virus defs comamandlines
    BOOL AbortDefsCommandlines(unsigned int unAbortTextResID);

// Private Functions
private:
    void SetSequenceNumber (bool bIsHubOK);
    BOOL UpdateMicroDefHubVersion();
    BOOL GetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches,
					  CString *pstrProductMoniker,
					  CString *pstrCallbacksHere,
					  CString *pstrGroup,
					  CString *pstrSetSKUProperty,
					  CString *pstrSetEvironment);
    BOOL SetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches,
					  CString *pstrProductMoniker );

    static HRESULT SetEnvProp();

// Private Data
private:
    AVModule::AVLoader_IAVDefInfo m_AVDefInfoLoader;
    AVModule::IAVDefInfoPtr m_spAVDefInfo;

    CComPtr<IluProductReg> m_ptrProductReg;

    cc::IStringPtr m_spBinaryPatchDir;
    cc::IStringPtr m_spPatchDirectDir;
    cc::IStringPtr m_spBinaryPatchVersion;

    // Path to Patch25.dll
    CString m_strPatch25DllPath;

	CString m_strBrandingProductName;

    DefsDateStruct initialDefs;
    DefsDateStruct postDefs;
};

#endif // !defined(AFX_NAVLU_H__3C2E74AD_887E_11D2_B40A_00600831DD76__INCLUDED_)
