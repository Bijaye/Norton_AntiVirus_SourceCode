//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// Global.h - contains global functions for the Scan and Deliver dll.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/Global.h_v  $
// 
//    Rev 1.7   18 Jun 1998 21:28:10   SEDWARD
// Added 'DecodeString'.
//
//    Rev 1.6   11 Jun 1998 03:22:24   jtaylor
// reenabled the trialware.
//
//    Rev 1.5   11 Jun 1998 02:02:48   jtaylor
// Added support for dissabling Scan and Deliver when a trial copy of NAV expires.
//
//    Rev 1.4   08 Jun 1998 16:19:38   SEDWARD
// Added GetMainNavDir, GetClickedItemIndex and ToggleListItemCheckbox.
//
//    Rev 1.3   05 Jun 1998 14:57:40   jtaylor
// Added more registry key/value defines
//
//    Rev 1.2   01 Jun 1998 17:04:34   SEDWARD
// Added 'InitCountryListbox'.
//
//    Rev 1.1   26 May 1998 12:59:50   SEDWARD
// Added 'MyCharNext' and InitDbcsVars
//
//    Rev 1.0   16 Apr 1998 23:37:24   JTaylor
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include <map>

#define REGKEY_NAV_MAIN                 _T("SOFTWARE\\Symantec\\Norton AntiVirus")
#define REGVAL_NAV_LANGUAGE             _T("Language")
#define REGKEY_QUARANTINE_TEMP_PATH     _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Quarantine")
#define REGVAL_QUARANTINE_TEMP_PATH     _T("TempPath")
#define REGKEY_NAV_INSTALL_PATH         _T("SOFTWARE\\Symantec\\InstalledApps")
#define REGVAL_NAV_INSTALL_PATH_VALUE   _T("NAV")
#define REGKEY_NAV_DEFINITIONS_PATH     _T("SOFTWARE\\Symantec\\SharedDefs")
#define REGVAL_NAV_DEFINITIONS_PATH     _T("NAV_90_QUAR")

#define COUNTRY_DAT_FILENAME            _T("COUNTRY.DAT")
#define PLATFORM_DAT_FILENAME           _T("PLATFORM.DAT")
#define INVALID_COUNTRY_DATA_CODE       0xFFFFFFFF

extern BOOL   g_bIsDBCS;
extern BYTE*  g_arLeadByteRange;
extern CPINFO g_codePageInfo;

typedef std::map<DWORD, CString> MAP_CODE_TO_COUNTRY;

// Use this function to retrieve paths from the registry.
BOOL GetMainNavDir(TCHAR* szDirBuf);
BOOL GetPathFromRegKey(HKEY   hInputKey,
                       LPTSTR lpszKeyName,
                       LPTSTR lpszValueName,
                       LPTSTR lpszOutBuf,
                       DWORD  nBufSize);

void  InitDbcsVars(void);
char* MyCharNext(char*  pszStr);
int   InitCountryListbox(CComboBox& countryComboBox);
BOOL  GetClickedItemIndex(CListCtrl& listCtrl, int& itemIndex);
BOOL  ToggleListItemCheckbox(CListCtrl& listCtrl, int itemIndex);
DWORD DecodeString(char* szStrData);
UINT  ReturnExtension(LPCTSTR lpszFileSpec, LPTSTR lpszExtension, UINT cbExtBuffSize);
UINT  FileZeroAndDelete(LPCTSTR lpFilename, UINT uTimesToWipe);
