// SessProc.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SessProc.h"

#define MAX_VALUE 255

// Section name in INSTOPTS.INI
static TCHAR s_InstoptsSection[]		= _T("DefAnnuity");

// Definitions for replacement tokens
static TCHAR s_szReplaceSKU[]			= _T("%SKU%");
static TCHAR s_szReplaceServiceID[]		= _T("%SID%");
static TCHAR s_szReplaceWarningLen[]	= _T("%WAR%");
static TCHAR s_szReplaceSubsLen[]		= _T("%SUB%");
static TCHAR s_szReplaceVeniceSubsSKU[] = _T("%VKU%");
static TCHAR s_szReplacePromoCode[]		= _T("%PRO%");
static TCHAR s_szReplacePartnumber[]	= _T("%PNU%");
static TCHAR s_szReplaceProductLang[]	= _T("%PLN%");
static TCHAR s_szReplaceSeviceLang[]	= _T("%SLN%");
static TCHAR s_szReplaceLiveAdvCancel[] = _T("%LAX%");

// Default values for parameters that can be overridden in INSTOPTS.INI
static TCHAR s_szDefaultSKU[]			= _T("07-00-02618");
static TCHAR s_szDefaultServiceID[]		= _T("61D460E0-07C0-11d3-A985-00A0244D507A");
static TCHAR s_szDefaultWarningLen[]	= _T("60");
static TCHAR s_szDefaultSubsLen[]		= _T("366");
static TCHAR s_szDefaultVeniceSubsSKU[] = _T("07-12-00024");
static TCHAR s_szDefaultPromoCode[]		= _T("V603");
static TCHAR s_szDefaultPartnumber[]	= _T("07-25-00335");
static TCHAR s_szDefaultProductLang[]	= _T("EN");
static TCHAR s_szDefaultSeviceLang[]	= _T("EN");
static TCHAR s_szDefaultLiveAdvCancel[] = _T("0x00000000");
static DWORD s_dwDefaultEnableInternetSubs	= 1;
static TCHAR s_szDefaultExtVeniceSubsSKU[]	= _T("07-12-00025");
static TCHAR s_szDefaultExtPromoCode[]		= _T("V604");

// Names of variables in INSTOPTS.INI
static TCHAR s_szInstoptSKU[]			= _T("SKU");
static TCHAR s_szInstoptServiceID[]		= _T("ServID");
static TCHAR s_szInstoptWarningLen[]	= _T("FreeEndWarning");
static TCHAR s_szInstoptSubsLen[]		= _T("FreeLength");
static TCHAR s_szInstoptVeniceSubsSKU[] = _T("VSKU");
static TCHAR s_szInstoptPromoCode[]		= _T("PRO");
static TCHAR s_szInstoptPartnumber[]	= _T("PNU");
static TCHAR s_szInstoptProductLang[]	= _T("PLN");
static TCHAR s_szInstoptSeviceLang[]	= _T("SLN");
static TCHAR s_szInstoptLiveAdvCancel[] = _T("LACANCEL");
static TCHAR s_szInstoptEnableInternetSubs[]	= _T("ENV");
static TCHAR s_szInstoptExtVeniceSubsSKU[]		= _T("XSKU");
static TCHAR s_szInstoptExtPromoCode[]			= _T("XPRO");

// Variables in which we store the values we read from INSTOPTS.INI
static TCHAR s_szValueSKU[MAX_VALUE];
static TCHAR s_szValueServiceID[MAX_VALUE];
static TCHAR s_szValueWarningLen[MAX_VALUE];
static TCHAR s_szValueSubsLen[MAX_VALUE];
static TCHAR s_szValueVeniceSubsSKU[MAX_VALUE];
static TCHAR s_szValuePromoCode[MAX_VALUE];
static TCHAR s_szValuePartnumber[MAX_VALUE];
static TCHAR s_szValueProductLang[MAX_VALUE];
static TCHAR s_szValueSeviceLang[MAX_VALUE];
static TCHAR s_szValueLiveAdvCancel[MAX_VALUE];
static DWORD s_dwValueEnableInternetSubs;
static TCHAR s_szValueExtVeniceSubsSKU[MAX_VALUE];
static TCHAR s_szValueExtPromoCode[MAX_VALUE];

typedef struct tagREPLACE_ITEM
{	LPCTSTR szKey;
	LPCTSTR szDefault;
	LPCTSTR szIntopts;
	LPTSTR  szValue;
} ReplaceItem;

ReplaceItem array[] =
{	{	s_szReplaceSKU, s_szDefaultSKU, s_szInstoptSKU, s_szValueSKU },
	{	s_szReplaceServiceID, s_szDefaultServiceID, s_szInstoptServiceID, s_szValueServiceID },
	{	s_szReplaceWarningLen, s_szDefaultWarningLen, s_szInstoptWarningLen, s_szValueWarningLen },
	{	s_szReplaceSubsLen, s_szDefaultSubsLen, s_szInstoptSubsLen, s_szValueSubsLen },
	{	s_szReplaceVeniceSubsSKU, s_szDefaultVeniceSubsSKU, s_szInstoptVeniceSubsSKU, s_szValueVeniceSubsSKU },
	{	s_szReplacePromoCode, s_szDefaultPromoCode, s_szInstoptPromoCode, s_szValuePromoCode },
	{	s_szReplacePartnumber, s_szDefaultPartnumber, s_szInstoptPartnumber, s_szValuePartnumber },
	{	s_szReplaceProductLang, s_szDefaultProductLang, s_szInstoptProductLang, s_szValueProductLang },
	{	s_szReplaceSeviceLang, s_szDefaultSeviceLang, s_szInstoptSeviceLang, s_szValueSeviceLang },
	{	s_szReplaceLiveAdvCancel, s_szDefaultLiveAdvCancel, s_szInstoptLiveAdvCancel, s_szValueLiveAdvCancel },
	{	NULL, NULL, NULL, NULL }
};

BOOL GetMyDirectory(CString &csPath)
{	TCHAR szPath[MAX_PATH];
	HMODULE hModule;
	int i;

	szPath[0] = '\0';
	hModule = GetModuleHandle(_T("defannty.dll"));
	if(hModule == NULL)
		return FALSE;
	GetModuleFileName(hModule, szPath, MAX_PATH);
	csPath = szPath;
	i = csPath.ReverseFind('\\');
	ASSERT(i != -1);
	if(i == -1)
	{	csPath.Empty();
		return FALSE;
	}
	csPath = csPath.Left(i);
	return TRUE;
}

void ReadInstoptsValues(LPCTSTR szInstoptsFile)
{	int i;

	for(i = 0; array[i].szKey; ++i)
		GetPrivateProfileString(s_InstoptsSection, array[i].szIntopts,
			array[i].szDefault, array[i].szValue, MAX_VALUE, szInstoptsFile);
	DefAnnuitySetSubsID ( s_szValueSKU );
	GetPrivateProfileString(s_InstoptsSection, s_szInstoptExtVeniceSubsSKU,
		s_szDefaultExtVeniceSubsSKU, s_szValueExtVeniceSubsSKU, MAX_VALUE, szInstoptsFile);
	DefAnnuitySetSubsXSKU( s_szValueExtVeniceSubsSKU );
	GetPrivateProfileString(s_InstoptsSection, s_szInstoptExtPromoCode,
		s_szDefaultExtPromoCode, s_szValueExtPromoCode, MAX_VALUE, szInstoptsFile);
	DefAnnuitySetSubsXPRO( s_szValueExtPromoCode );
	s_dwValueEnableInternetSubs = GetPrivateProfileInt(s_InstoptsSection, 
		s_szInstoptEnableInternetSubs, s_dwDefaultEnableInternetSubs, szInstoptsFile);
	DefAnnuitySetInetEnable( s_dwValueEnableInternetSubs );
}

LPCTSTR GetReplacement(LPCTSTR szKey)
{	int i;
	CString csKey = szKey;
	LPCTSTR pReplacement = NULL;

	for(i = 0; array[i].szKey; ++i)
	{	if(!csKey.Compare(array[i].szKey))
		{	pReplacement = array[i].szValue;
			break;
		}
	}
	return pReplacement;
}

BOOL ProcessSessionTemplate(LPCTSTR szInstoptsFile)
{	CStdioFile	template_file;
	CFile		session_file;
	CString		csIn, csOut, csKeyword, csInFile, csOutFile;
	LPCTSTR		pszReplacement;
	BOOL		bRes = TRUE;
	int			i;

	ReadInstoptsValues(szInstoptsFile);

	GetMyDirectory(csInFile);
	csInFile += '\\';
	csOutFile = csInFile;
	csInFile += TEMPLATE_FILE;
	csOutFile += SESSION_FILE;

	if(!template_file.Open(csInFile, CFile::modeRead))
		return FALSE;
	if(!session_file.Open(csOutFile, CFile::modeCreate | CFile::modeWrite))
	{	template_file.Close();
		return FALSE;
	}
	while(template_file.ReadString(csIn))
	{	csOut.Empty();
		while((i = csIn.Find('%')) != -1)
		{	csOut += csIn.Left(i);
			csKeyword = csIn.Mid(i, 5);
			csIn = csIn.Mid(i + 5);
			pszReplacement = GetReplacement(csKeyword);
			if(!pszReplacement)
				bRes = FALSE;
			else
				csOut += pszReplacement;
		}
		csOut += csIn;
		csOut += _T("\r\n");
		session_file.Write((LPCTSTR) csOut, csOut.GetLength());
	}
	template_file.Close();
	session_file.Close();
	return bRes;
}
