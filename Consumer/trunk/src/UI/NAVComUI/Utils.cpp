////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// $Header: $
//
// AUTHOR(S)	 : Shaun Cooley
// COMPILED WITH : Microsoft Visual C++ (v6.0)
// MODEL         : Flat (32bit)
// ENVIRONMENT   : WIN32, C runtime, C++ runtime, MFC(4.2), COM, OLE, STL, ATL
//
// Copyright (c) 1999 by Symantec Corporation. All rights reserved.
//
//
//============================================================================
//                               HEADER FILES                                 
//============================================================================
// The order of the following #include sections minimizes any symbol clashing.
// It's been determined mainly by trial and error over time.
#include "stdafx.h"

// This module's header.
#include "Utils.h"

#ifdef USE_GDI_PLUS
#include <gdiplus.h>
#endif // USE_GDI_PLUS

// Private, product/project headers.
#define dprintf

// implementation-only includes
// <nothing>

#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#undef FUNCNAME
#define FUNCNAME FILE_LINE

#ifndef MAX_LOADSTRING
 #define MAX_LOADSTRING MAX_PATH * 2
#endif

// Font name that will revert to DEFAULT_GUI_FONT
const TCHAR _szDefaultFont[] = _T("MS Sans Serif");

// Windows installed app key
const TCHAR _szInstalledAppKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths");

// Symantec const's
const TCHAR _szSymInstalledAppKey[] = _T("Software\\Symantec\\InstalledApps");
const TCHAR _szSymRegKey[] = _T("Software\\Symantec\\");

// Speed start const's
const TCHAR _szSpeedStartRunRegLoc[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\SpdStart");
const TCHAR _szSpeedStartTrayRegLoc[] = _T("Software\\Symantec\\Norton Utilities\\Norton Speed Start\\TrayIcon");
const TCHAR _szSpeedStartVXDRegLoc[] = _T("System\\CurrentControlSet\\Services\\VxD\\SpdStart\\StaticVxD");
const TCHAR _szSpeedStartRegLoc[] = _T("Software\\Symantec\\Norton Utilities\\Norton Speed Start\\1.0\\Enable");
const TCHAR _szSpeedStartProdName[] = _T("Norton Utilities");
const TCHAR _szSpeedStartExe[] = _T("NSS\\SPDSTART.EXE");
const TCHAR _szSpeedStartVxD[] = _T("NSS\\SPDSTART.VXD");

// CrashGuard const's
const TCHAR _szNCGName[] = _T("NortonCrashGuard");
const TCHAR _szCrashGuardRegLoc[] = _T("Software\\Symantec\\Norton CrashGuard\\Statcap\\CaptureName");
const LPTSTR _szCGRunLoc = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Norton CrashGuard Monitor");
const LPTSTR _szCGExe = _T("CGMENU.EXE");
const LPTSTR _szCGClassName = _T("NortonCrashGuard_MonitorWnd");
const LPTSTR _szCrashGuardSettings[] =
{
	_T("Software\\Symantec\\Norton CrashGuard\\16-bit EH"),
	_T("Software\\Symantec\\Norton CrashGuard\\32-bit JIT"),
	_T("Software\\Symantec\\Norton CrashGuard\\EnableBlueScreenProtection")
};
const DWORD _dwCrashGuardOn = 1;
const DWORD _dwCrashGuardOff = 0;

// NAV AP const's
const TCHAR _szNavVxDRegLoc[] = _T("System\\CurrentControlSet\\Services\\VxD\\NAVAP\\StaticVxD");
const TCHAR _szNavVxDName[] = _T("\\NAVAP.VXD");
const TCHAR _szNavAPRegLoc[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Norton Auto-Protect");
const TCHAR _szNavAPName[] = _T("\\NAVAPW32.EXE /LOADQUIET");
const TCHAR _szNavAPNameNT[] = _T("\\NAVAPW32.EXE");
const TCHAR _szNavAPLnkNT[] = _T("Norton AntiVirus AutoProtect");

// Norton Protected Recycle Bin const's
const LPTSTR _szNUName = _T("Norton Utilities");
const LPTSTR _szNPRBExe = _T("nprotect.exe");
const LPTSTR _szNPRBSettings = _T("Software\\Symantec\\Norton Utilities\\Norton Protection");
const LPTSTR _szNPRBSwitches = _T("Switches");
const LPTSTR _szNPRBLoc[2] = 
{
	_T("Software\\Microsoft\\Windows\\CurrentVersion\\RunServices\\NPROTECT"),
	_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\NPROTECT")
};


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsSystemHCMode:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/7/99 - 6:08:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsSystemHCMode
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsSystemHCMode()
{
	OSVERSIONINFO osInfo;
	memset(&osInfo, 0, sizeof(OSVERSIONINFO));
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osInfo) && 
		((osInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
		|| (osInfo.dwPlatformId & VER_PLATFORM_WIN32_NT && osInfo.dwMajorVersion >= 5)))
	{
		HIGHCONTRAST hcInfo;
		memset(&hcInfo, 0, sizeof(HIGHCONTRAST));
		hcInfo.cbSize = sizeof(HIGHCONTRAST);

		SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hcInfo, 0);

		if(hcInfo.dwFlags & HCF_HIGHCONTRASTON)
			return TRUE;
	}	
	return FALSE;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsLowColorMode:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/6/02 - 6:08:48 PM (Chris Brown) /////////////////////
// 
// Function name	: CUtils::IsLowColorMode
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsLowColorMode( HDC hDC )
{
	const int nBits = GetDeviceCaps( hDC, BITSPIXEL );
    const int nPlanes = GetDeviceCaps( hDC, PLANES );
    const int nColorQuality = nBits*nPlanes;

    return ( nColorQuality<16 );
}


/////////////////////////////////////////////////////////////////////////////
// Misc Utils
/////////////////////////////////////////////////////////////////////////////


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/3/99 - 6:29:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : Reads a resource string that is formated like this:
//                    ";Arial;18;0"   or    "/Arial/18/0"
//                    The first character is the delimiter, followed by
//                    the font name, then the size, then the attributes
//                    to set attributes, OR together the values in the 
//                    utils.h header
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     UINT nFaceNameID: 
//     HINSTANCE hInst:
//     DWORD dwAddAttrib:
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, UINT nFaceNameID, DWORD dwAddAttrib, HINSTANCE hInst)
{
	TCHAR szData[MAX_LOADSTRING] = {0};
	LPTSTR pszData = szData;
	TCHAR szDelim[2] = {0, 0};
	TCHAR szFont[33] = {0};
	UINT uSize = 0;
	UINT uAttributes = 0;

	if(hInst == INVALID_HANDLE_VALUE)
		hInst = GetModuleHandle(NULL);
	
	// Load the data string
	if(!LoadString(hInst, nFaceNameID, szData, MAX_LOADSTRING))
		return NULL;

	// Get the delimiter
	_tcsncpy(szDelim, pszData, 1);
	pszData = _tcsninc(pszData, 1);

	// Get the font name	
	LPTSTR pszTemp = _tcstok(pszData, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		_tcsncpy(szFont, pszTemp, 32);

	// Get the point size
	pszTemp = _tcstok(NULL, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		uSize = _ttoi(pszTemp);

	// Get the font attributes
	pszTemp = _tcstok(NULL, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		uAttributes = _ttoi(pszTemp);

	return CreateFontEz(hdc, szFont, uSize, uAttributes | dwAddAttrib);
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/5/99 - 12:55:36 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : 
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     UINT nFaceNameID: 
//     HINSTANCE hInst: 
//     UINT uPtSize : 
//     DWORD dwAttributes: 
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, UINT nFaceNameID, HINSTANCE hInst, UINT uPtSize, DWORD dwAttributes)
{
	TCHAR szFont[32];
	LoadString(hInst, nFaceNameID, szFont, 32);
	return CreateFontEz(hdc, szFont, uPtSize, dwAttributes);
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/16/99 - 1:34:20 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : 
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     LPTSTR szFaceName: 
//     UINT uPtSize : 
//     DWORD dwAttributes: 
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, LPTSTR szFaceName, UINT uPtSize, DWORD dwAttributes)
{
	if(_tcsicmp(szFaceName, _szDefaultFont) == 0)
		return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	
	int nLogPxY = GetDeviceCaps(hdc, LOGPIXELSY);
	if(nLogPxY != 96)
	{
		dprintf("CreateFontEz: LOGPIXELSY is not 96, it is %d, using 96 anyway...", nLogPxY);
		nLogPxY = 96;
	}

	LOGFONT lf;
    ZeroMemory( &lf, sizeof(lf) );

    HFONT hFontRet = 0;

#ifdef USE_GDI_PLUS
#pragma message("** USING GDI+ **")

	if (!IsOS9x)
	{
		FontStyle style = FontStyleRegular;

		if( dwAttributes & CFEZ_ATTR_BOLD )
		{
			style = FontStyleBold;
		}

		if( dwAttributes & CFEZ_ATTR_UNDERLINE )
		{
			style = static_cast<FontStyle>( style | FontStyleUnderline );
		}

		if( dwAttributes & CFEZ_ATTR_ITALIC )
		{
			style = static_cast<FontStyle>( style | FontStyleItalic );
		}

		if( dwAttributes & CFEZ_ATTR_STRIKEOUT )
		{
			style = static_cast<FontStyle>( style | FontStyleStrikeout );
		}

		USES_CONVERSION;
		LPCWSTR lpwFaceName = NULL;
		lpwFaceName = T2CW( szFaceName );

		FontFamily fontFamily( lpwFaceName );
		Font fontTemp( &fontFamily, uPtSize, style, UnitPixel );

		Graphics graphics( hdc );

	#ifdef UNICODE
		fontTemp.GetLogFontW( &graphics, &lf );
	#else
		fontTemp.GetLogFontA( &graphics, &lf );
	#endif

		hFontRet = CreateFontIndirect( &lf );
	}

#endif //USE_GDI_PLUS
    
    if( !hFontRet )
    {
	    lf.lfHeight         = -MulDiv(uPtSize, nLogPxY, 72);
	    lf.lfWidth          = 0;
	    lf.lfEscapement     = lf.lfOrientation = 0;
	    lf.lfWeight         = dwAttributes & CFEZ_ATTR_BOLD      ?  700 : 0;
	    lf.lfItalic         = dwAttributes & CFEZ_ATTR_ITALIC    ? TRUE : FALSE;
	    lf.lfUnderline      = dwAttributes & CFEZ_ATTR_UNDERLINE ? TRUE : FALSE;
	    lf.lfStrikeOut      = dwAttributes & CFEZ_ATTR_STRIKEOUT ? TRUE : FALSE;
	    lf.lfCharSet        = GetDBCSCharSet();
	    lf.lfOutPrecision   = OUT_TT_PRECIS;		// Try to get a truetype font
	    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	    lf.lfQuality        = PROOF_QUALITY;
	    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	    _tcscpy(lf.lfFaceName, szFaceName);

	    hFontRet = CreateFontIndirect(&lf);
	    if(hFontRet == NULL)
	    {
		    // CreateFont failed ... let the system pick a similar font
		    _tcscpy(lf.lfFaceName, _T(""));
		    hFontRet = CreateFontIndirect(&lf);
	    }
    }
	
	return hFontRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetDBCSCharSet:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/15/99 - 4:19:33 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetDBCSCharSet
// Description	    : 
// Return type		: BYTE 
// 
/////////////////////////////////////////////////////////////////////////

BYTE CUtils::GetDBCSCharSet()
{
	switch(GetACP())
	{
		case 932:	// Japanese
			return SHIFTJIS_CHARSET;

		case 936:	// Chinese Simplified
			return GB2312_CHARSET;

		case 950:	// Chinese Traditional
			return CHINESEBIG5_CHARSET;

		case 949:	// Hangul
		case 1361:	// Johab
			return HANGEUL_CHARSET;

		case 874:	// Thai
			return THAI_CHARSET;

		case 1250:
			return EASTEUROPE_CHARSET;

		case 1251:	// Russian
			return RUSSIAN_CHARSET;

		case 1253:	// Greek
			return GREEK_CHARSET;

		case 1254:	// Turkish
			return TURKISH_CHARSET;

		case 1255:	// Hebrew
			return HEBREW_CHARSET;

		case 1256:	// Arabic
			return ARABIC_CHARSET;

		case 1257:	// Baltic
			return BALTIC_CHARSET;
	}

	return ANSI_CHARSET;
}

/////////////////////////////////////////////////////////////////////////////
// Color functions
/////////////////////////////////////////////////////////////////////////////

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::RGBtoPALRGB:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:03:10 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::RGBtoPALRGB
// Description	    : 
// Return type		: inline COLORREF 
// Arguments
//     COLORREF cr  : 
// 
/////////////////////////////////////////////////////////////////////////

inline COLORREF CUtils::RGBtoPALRGB(COLORREF cr)
{
	return cr | 0x02000000;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::HLStoRGB:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:20:13 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::HLStoRGB
// Description	    : 
// Return type		: inline COLORREF 
// Arguments
//     DOUBLE dHue  : 
//     DOUBLE dLuminance: 
//     DOUBLE dSaturation: 
// 
/////////////////////////////////////////////////////////////////////////

inline COLORREF CUtils::HLStoRGB(DOUBLE dHue, DOUBLE dLuminance, DOUBLE dSaturation)
{
	DOUBLE dRed   = dLuminance + 1.402 * (dSaturation - 128);
	DOUBLE dGreen = dLuminance - 0.34414 * (dHue - 128) - 0.71414 * (dSaturation - 128);
	DOUBLE dBlue  = dLuminance + 1.772 * (dHue - 128);
	return RGB(dRed, dGreen, dBlue);
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::RGBtoHLS:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:03:04 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::RGBtoHLS
// Description	    : 
// Return type		: inline VOID 
// Arguments
//     COLORREF clrRGB: 
//     DOUBLE *pdHue: 
//     DOUBLE *pdLuminance: 
//     DOUBLE *pdSaturation: 
// 
/////////////////////////////////////////////////////////////////////////

inline VOID CUtils::RGBtoHLS(COLORREF clrRGB, DOUBLE *pdHue, DOUBLE *pdLuminance, DOUBLE *pdSaturation)
{
	WORD wRed = GetRValue(clrRGB);
	WORD wGreen = GetGValue(clrRGB);
	WORD wBlue = GetBValue(clrRGB);

	*pdLuminance = 0.299 * wRed + 0.587 * wGreen + 0.114 * wBlue;
	*pdHue =  -0.1687 * wRed - 0.3313 * wGreen + 0.5 * wBlue + 128;
	*pdSaturation = 0.5 * wRed - 0.4187 * wGreen - 0.0813 * wBlue + 128;

}

