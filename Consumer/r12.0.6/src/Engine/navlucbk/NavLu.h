// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLUCBK/VCS/NavLu.h_v   1.0   08 Jan 1999 19:29:02   CEATON  $
//
// Description: Prototypes for NAVOR32.DLL's export functions
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLUCBK/VCS/NavLu.h_v  $
// 
//    Rev 1.0   08 Jan 1999 19:29:02   CEATON
// Initial revision.
// 
//****************************************************************************

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
#include "NAVDefutilsLoader.h"
#include "defutilsinterface.h"

enum {
    DefAnnuitySuccess = 0,
    DefAnnuityError,
    DefAnnuityUpdateAll,
    DefAnnuityUpdatePatches,
    DefAnnuityCancelUpdate,
    DefAnnuityDownloadAllowed,
    DefAnnuityDownloadNotAllowed
};

struct DefsDateStruct
{
    WORD wYear;
    WORD wMonth;
    WORD wDay;
    DWORD dwRev;
};

#define VERSION_BUFFER_SIZE     256

class CNavLu  
{
public:
    BOOL Initialize();
	BOOL GetVirusDefsDelivered();
    DWORD GetDefAnnuityStatus();
    BOOL LaunchSideEffectScan();

    CNavLu ();
	virtual ~CNavLu();


	BOOL PreProcess( );
	HRESULT PostProcess( );

	HRESULT RegisterCommandLines( DWORD DefAnnuityStatus );
	HRESULT UnregisterCommandLines( void );

    // This will abort the virus defs comamandlines
    BOOL AbortDefsCommandlines(unsigned int unAbortTextResID);


// Private Functions
private:
    void SetSequenceNumber (BOOL bSetToZero = FALSE);
    BOOL EditMicroDefVersion(LPTSTR pszVersion);
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

	// This function will return TRUE if LiveUpdate is run in silent mode.  This function 
	// can only determine this for LiveUpdate 1.6 and above.  In LiveUpdate 1.6, this function
	// will only work if LuAll has already started.
	//		This function will return FALSE if there is an error.
	BOOL IsLiveUpdateInSilentMode();

    static HRESULT SetEnvProp();

// Private Data
private:
    DefUtilsLoader3 m_DefUtilsLoader;
    IDefUtilsPtr3 m_pDefUtils;
    CComPtr<IluProductReg> m_ptrProductReg;

	TCHAR	  m_szCurrentDirectory[ MAX_PATH ];
    DWORD     m_DefAnnuityStatus;

    TCHAR           m_szBinaryPatchDir[MAX_PATH];
    TCHAR           m_szPatchDirectDir[MAX_PATH];
    TCHAR           m_szPatch25DllPath[MAX_PATH];
    TCHAR           m_szBinaryPatchVersion[VERSION_BUFFER_SIZE];

	CString m_strBrandingProductName;

    DefsDateStruct initialDefs;
    DefsDateStruct postDefs;
};

#endif // !defined(AFX_NAVLU_H__3C2E74AD_887E_11D2_B40A_00600831DD76__INCLUDED_)
