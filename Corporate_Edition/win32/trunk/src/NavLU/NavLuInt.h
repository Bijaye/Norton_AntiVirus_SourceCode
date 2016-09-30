// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// NAVLUINT.H : main internal header file for the NAVLU DLL
//              external interface can be found in NAVLU.H
//

#if !defined(AFX_NAVLU_H__A9C5A105_093E_11D3_B509_00104B252EE6__INCLUDED_)
#define AFX_NAVLU_H__A9C5A105_093E_11D3_B509_00104B252EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "platform.h"
#include "liveupdate.h"
#include "navluprog.h"
#include "resource.h"		// main symbols
#include "clientreg.h"

// MicroDefs LiveUpdate product version

#define PRODUCT_VERSION_MICRODEFS _T("MicroDefs")

/////////////////////////////////////////////////////////////////////////////
// CNAVLUApp
// See NAVLU.cpp for the implementation of this class
//

class CNAVLUApp : public CWinApp
{
public:
	CNAVLUApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNAVLUApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNAVLUApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// Temporary minimalist def annuity support. 
// I've left most of the definitions annuity code, but it has been disabled.
// See AVCORE NAVLU for full functionality.  -DALLEE
//

enum {
    DefAnnuitySuccess = 0,
    DefAnnuityError,
    DefAnnuityUpdateAll,
    DefAnnuityUpdatePatches,
    DefAnnuityCancelUpdate,
    DefAnnuityDownloadAllowed,
    DefAnnuityDownloadNotAllowed
};

/////////////////////////////////////////////////////////////////////////////
// CNavLU class for LiveUpdate functionality.
//

#define VERSION_BUFFER_SIZE     32

class CNavLu
{
    friend BOOL WINAPI NavFinishOAM (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData);

public:
    CNavLu( HINSTANCE hInstance,
            LPCTSTR   szzPackageTypeFilters,
            bool      bSilent );

    DWORD     m_dwDefAnnuityStatus;

    HINSTANCE GetInstance (void) const;

    // Start the LiveUpdate session.
    WORD DoLiveUpdate();

	bool CheckContentLicense( bool bSilent, bool& bAbort, LICENSECALLBACKFN fnLicCalback );

// Protected Functions
protected:
    void CallLiveUpdate (const CString &strCmdLine) throw();
    bool FinishOAM  (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData);

    void SetSequenceNumberMicrodefs (LPFSETSEQUENCENUMBEREX pfnSetSequenceNumber, bool bSetToZero = false) const;
    void SetSequenceNumberXdb       (LPFSETSEQUENCENUMBEREX pfnSetSequenceNumber) const;

    bool GetVirusDefDate (LPDWORD pdwDefYear    = NULL,
                          LPDWORD pdwDefMonth   = NULL,
                          LPDWORD pdwDefDay     = NULL,
                          LPDWORD pdwDefVersion = NULL) const;
    bool GetHubDate (LPDWORD pdwDefYear    = NULL,
                     LPDWORD pdwDefMonth   = NULL,
                     LPDWORD pdwDefDay     = NULL,
                     LPDWORD pdwDefVersion = NULL) const;

    bool GetXdbDate (LPDWORD pdwDefYear, 
                     LPDWORD pdwDefMonth,
                     LPDWORD pdwDefDay,
                     LPDWORD pdwDefVersion) const;

    DWORD BuildCmdLine  (CString *pstrCmdLine, bool bSilent) const;
	bool  AppendCmdLine (LPCTSTR szKeyBuf, CString *pstrCmdLine, bool* pbSkipped) const;

    bool  UpdateHubVersion (const CString &szNewVersion) const;

    bool GetCmdLineN (DWORD    dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches) const;
    bool SetCmdLineN (DWORD          dwCmdLineNum,
                      const CString *pstrProductName,
                      const CString *pstrProductType,
                      const CString *pstrProductLanguage,
                      const CString *pstrProductVersion,
                      const CString *pstrOtherSwitches) const;

    bool DoesDirHaveFiles (LPCTSTR szDir) const;

    bool GetPatchDllFullPath (TCHAR *szPatchDll, size_t nNumPatchDllBytes);

// Private Data
protected:
    bool                  m_bSilent;
	bool                  m_bWantMicroDefs;
    bool                  m_bWantDefUpdates;
    HINSTANCE             m_hInstance;
    WORD                  m_wLuFinishStatus;
    CDefUtilsLoaderHelper m_objDefUtilsLdr;
    IDefUtils4Ptr         m_ptrDefUtils;
    bool                  m_bSetSeqNumToZero;
    TCHAR                 m_szHubDeltaDir   [MAX_PATH+1];
    TCHAR                 m_szDirectDeltaDir[MAX_PATH+1];
    bool                  m_bRunLuAgain;
    unsigned short        m_nNumLuRuns;
	CStringList           m_PackageTypeFilters;

	bool IsPackageTypeFiltered( LPCTSTR type ) const;
	void AddPackageTypeFiltered( LPCTSTR type );

    bool WantMicroDefs(void);

private: // don't implement to prevent use
	CNavLu();
	CNavLu( const CNavLu& );
	CNavLu& operator = ( const CNavLu& );
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVLU_H__A9C5A105_093E_11D3_B509_00104B252EE6__INCLUDED_)
