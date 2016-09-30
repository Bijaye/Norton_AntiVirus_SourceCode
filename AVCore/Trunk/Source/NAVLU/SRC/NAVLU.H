// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navlu.h_v   1.17   28 Aug 1998 16:19:00   JBRENNA  $
//
// Description: Prototypes for NAVOR32.DLL's export functions
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navlu.h_v  $
// 
//    Rev 1.17   28 Aug 1998 16:19:00   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.16.1.2   20 Jul 1998 15:12:28   tcashin
//    Added code to see if MicroDef updates were delivered.
// 2. Rev 1.16.1.1   29 Jun 1998 14:28:24   tcashin
//    Process the /setup switch. If used, don't do the reboot dialogs or text.
// 
//    Rev 1.16   27 May 1998 15:02:10   TCASHIN
// Added microdef-related private data. Added parameter to SetSequenceNumber()
// to force the function to set the seq. number to zero.
// 
//    Rev 1.15   20 May 1998 15:57:10   tcashin
// Added support for a progress dialog to be called from PostMicroDefUpdate()
// when we're not in silent mode.
// 
//    Rev 1.14   27 Apr 1998 23:32:36   tcashin
// Changed the parameter to EditMicroDefVersion().
// 
//    Rev 1.13   27 Apr 1998 14:35:08   RStanev
// Added def annuity.
//
//    Rev 1.12   20 Apr 1998 23:24:06   tcashin
// Changed EditMicroDefLUType() to EditMicrDefVersion().
//
//    Rev 1.11   26 Mar 1998 15:24:02   tcashin
// Changed the parameter for EditMicroDefLUType.
//
//    Rev 1.10   15 Mar 1998 14:06:44   tcashin
// Added new private methods SetCmdlineN and EditMicroDefLUType for
// MicroDef support.
//
//    Rev 1.9   09 Mar 1998 20:48:32   tcashin
// Back out the changes for MicroDefs until we get
// AVCOMP build issues resolved.
//
//    Rev 1.7   01 Jul 1997 17:02:46   JBRENNA
// Now have the Shared Tech LiveUpdate parented off of our bogus main window.
//
//    Rev 1.6   03 Jun 1997 13:23:24   JBRENNA
// Remove m_strCmdLinesKey and m_strCmdLineNKey ... these were moved to
// the strings.h file.
//
//    Rev 1.5   02 Jun 1997 18:15:50   JBRENNA
// 1. Moved GetLuPathName to   NavLuUtils.cpp:NavLuGetLuPathName.
// 2. Moved MyFileExists to    NavLuUtils.cpp:NavLuFileExists.
// 3. Moved MyNameAppendFile to NavLuUtils.cpp:NavLuNameAppendFile.
//
//    Rev 1.4   01 Jun 1997 18:56:34   JBRENNA
// 1. Move class constructor to CPP file ... no longer inlined.
// 2. Misc function naming / parameter changes as required by changes to
//    NavLu.cpp.
//
//    Rev 1.3   27 May 1997 17:36:56   JBRENNA
// Only call LoadString for m_strCmdLinesKey and m_strCmdLineNKey when the
// caller gave us an instance handle.
//
//    Rev 1.2   23 May 1997 21:31:42   JBRENNA
// Associated changes from NavLu.cpp
//
//    Rev 1.1   16 May 1997 11:05:54   JBRENNA
// 1. Added function prototypes for several new functions.
// 2. Added m_szCmdLinesKey, m_szCmdLineNKey, and m_wLuFinishStatus.
// 3. Wrapped entire header file with #ifndef NAVLU_H_INCLUDED.
//
//    Rev 1.0   10 May 1997 01:21:10   JBRENNA
// Initial revision.
//
//    Rev 1.1   17 Jun 1996 16:21:32   MKEATIN
// Added BOOL bReboot parameter to CallOnramp() prototype
//
//    Rev 1.0   17 Jun 1996 15:39:42   MKEATIN
// Initial revision.
//****************************************************************************

#ifndef __NAVLU_H
#define __NAVLU_H

#include "DefUtils.h"
#include "navluprog.h"

typedef LONG (WINAPI *LPFUNC_SETSEQUENCENUMBER)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,DWORD);

class CNavLu
{
    friend BOOL WINAPI NavWelcomeOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData);
    friend BOOL WINAPI NavFinishOAM (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData);
	friend BOOL SYM_EXPORT NavIsLuInstalled (void);

public:
    DWORD m_DefAnnuityStatus;
    CNavLu (HINSTANCE hInstance, BOOL bReboot = FALSE, BOOL bSetup = FALSE);

    HINSTANCE GetInstance (void) const;
    BOOL      GetReboot   (void) const;

    // Start the LiveUpdate session.
    WORD DoLiveUpdate(HWND hParent, BOOL bScheduled, BOOL bFilterVirusDefType, BOOL bSetup);

// Private Functions
private:
    void CallLiveUpdate (HWND hParent, const CString &strCmdLine);
    BOOL WelcomeOAM (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData) const;
    BOOL FinishOAM  (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData);

    UINT GetNumVirusEntries () const;
    BOOL IsOlderThanOneMonth (void) const;
    void SetSequenceNumber (LPFUNC_SETSEQUENCENUMBER pfnSetSequenceNumber, BOOL bSetToZero = FALSE) const;
    BOOL GetVirusDefDate (LPDWORD lpdwDefYear    = NULL,
                          LPDWORD lpdwDefMonth   = NULL,
                          LPDWORD lpdwDefDay     = NULL,
                          LPDWORD lpdwDefVersion = NULL) const;
    BOOL BuildCmdLine (CString *pstrCmdLine, BOOL bSilent, BOOL bFilterVirusDefType) const;
    BOOL AppendCmdLineN (CString *pstrCmdLine, DWORD dwCmdLineNum, BOOL bFilterVirusDefType) const;
    BOOL EditMicroDefVersion(LPTSTR pszVersion) const;
    BOOL GetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches) const;
    BOOL SetCmdLineN (DWORD   dwCmdLineNum,
                      CString *pstrProductName,
                      CString *pstrProductType,
                      CString *pstrProductLanguage,
                      CString *pstrProductVersion,
                      CString *pstrOtherSwitches) const;

    BOOL CheckVirusDefsDelivered(LPMICRODEFINFO lpMicroDefInfo);


// Private Data
private:
    BOOL      m_bReboot;
    HINSTANCE m_hInstance;
    WORD      m_wLuFinishStatus;
    UINT      m_uStartNumViruses;
    CDefUtils *m_pDefUtils;
    BOOL      m_bDefUtilsPostDefUpdateCalled;
    BOOL      m_SetSeqNumToZero;
    BOOL      m_bSetup;

    MICRODEFINFO        m_MicroDefInfo;
    MICRODEFUPDATE      m_MicroDefUpdate;

    CLuPatchProgress    *m_pPAProgress;
};

// In-lined functions

inline HINSTANCE CNavLu::GetInstance (void) const
{
    return m_hInstance;
}

inline BOOL CNavLu::GetReboot (void) const
{
    return m_bReboot;
}

#endif // __NAVLU_H
