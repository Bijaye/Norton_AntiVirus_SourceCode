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

#include "DefUtils.h"
#include "navluprog.h"

typedef LONG (_cdecl *LPFUNC_SETSEQUENCENUMBER)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,DWORD);

#define VERSION_BUFFER_SIZE     16

class CNavLu  
{
public:
    DWORD GetDefAnnuityStatus();
	WORD GetStatusCodes();

    CNavLu (HINSTANCE hInstance, BOOL bReboot = FALSE, BOOL bSetup = FALSE);
	virtual ~CNavLu();


	BOOL PreProcess( );
	WORD PostProcess( WORD wLuFinishStatus );

	BOOL GetStatusText(
		CString& sStatusText,
		CString& sTitle,
		LPBOOL lpbReboot	// This function sets this to 1 if advising reboot
		);

    static BOOL BuildCmdLine (CString *pstrCmdLine, BOOL bSilent, BOOL bFilterVirusDefType);
	static HRESULT RegisterCommandLines( DWORD DefAnnuityStatus );
    static HRESULT DisableCommandLines( DWORD DefAnnuityStatus );
	static HRESULT UnregisterCommandLines( void );


// Private Functions
private:
    UINT GetNumVirusEntries () const;
    BOOL IsOlderThanOneMonth (void) const;
    void SetSequenceNumber (LPFUNC_SETSEQUENCENUMBER pfnSetSequenceNumber, BOOL bSetToZero = FALSE) const;
    BOOL GetVirusDefDate (LPDWORD lpdwDefYear    = NULL,
                          LPDWORD lpdwDefMonth   = NULL,
                          LPDWORD lpdwDefDay     = NULL,
                          LPDWORD lpdwDefVersion = NULL) const;
    static BOOL AppendCmdLineN (CString *pstrCmdLine, DWORD dwCmdLineNum, BOOL bFilterVirusDefType);
    BOOL EditMicroDefVersion(LPTSTR pszVersion) const;
    static BOOL GetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches,
					  CString *pstrProductMoniker,
					  CString *pstrCallbacksHere,
					  CString *pstrGroup );
    static BOOL SetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches,
					  CString *pstrProductMoniker );

    BOOL CheckVirusDefsDelivered(LPMICRODEFINFO lpMicroDefInfo);

// Private Data
private:
    BOOL      m_bMicroDefsDelivered;
    BOOL      m_bMicroDefUpdateSuccess;
    BOOL      m_bReboot;
    HINSTANCE m_hInstance;
    WORD      m_wLuFinishStatus;
    WORD      m_wLuPatchStatus;
    UINT      m_uStartNumViruses;
    CDefUtils *m_pDefUtils;
    BOOL      m_bDefUtilsPostDefUpdateCalled;
    BOOL      m_SetSeqNumToZero;
    BOOL      m_bSetup;
    BOOL      m_bLuRetry;
    
    DWORD     m_DefAnnuityStatus;
    
    MICRODEFINFO        m_MicroDefInfo;
    MICRODEFUPDATE      m_MicroDefUpdate;

    CLuPatchProgress    *m_pPAProgress;

    TCHAR           m_szBinaryPatchDir[_MAX_PATH];
    TCHAR           m_szBinaryPatchVersion[VERSION_BUFFER_SIZE];
    TCHAR           m_szTextPatchDir[_MAX_PATH];
    TCHAR           m_szTextPatchVersion[VERSION_BUFFER_SIZE];

    BYTE            m_abyDABuffer[64];
};

#if 0
// In-lined functions
inline HINSTANCE CNavLu::GetInstance (void) const
{
    return m_hInstance;
}

inline BOOL CNavLu::GetReboot (void) const
{
    return m_bReboot;
}
#endif

#endif // !defined(AFX_NAVLU_H__3C2E74AD_887E_11D2_B40A_00600831DD76__INCLUDED_)
