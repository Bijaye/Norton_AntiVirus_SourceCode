// Code that translates a date and a duration to and from the 
// SmellyCat II authorization string

#include "stdafx.h"
#include "SCValid.h"

#define _SC_ENCRYPT 1

static TCHAR s_EncryptString[] = _T("anon");
static TCHAR s_DatePrefix[] = _T("ND");
static TCHAR s_DatePostfix[] = _T("6");
static TCHAR s_szMasterKey[] = _T("NQ022919566");

int g_MonthLengths[] =
{	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// szKey must be 12 characters long, minimum
BOOL ConvertKeyDWORDToString(DWORD dwKey, LPTSTR szKey)
{	_stprintf(szKey, _T("%s%08lx%s"), s_DatePrefix, dwKey, s_DatePostfix);
	return TRUE;
}

// String must be upper case
BOOL ConvertKeyStringToDWORD(LPCTSTR szKey, DWORD *pKey)
{	LPTSTR pEnd;
	TCHAR  szData[9];
	DWORD   dwKey;

	// validate
	if(_tcslen(szKey) != 11)
		return FALSE;
	if(_tcsncmp(s_DatePrefix, szKey, _tcslen(s_DatePrefix)))
		return FALSE;
	if(szKey[10] != s_DatePostfix[0])
		return FALSE;
	_tcsncpy(szData, szKey + _tcslen(s_DatePrefix), 8);
	szData[8] = '\0';
	dwKey = _tcstoul(szData, &pEnd, 16);
	if(pEnd != szData + 8)
		return FALSE;
	*pKey = dwKey;
	return TRUE;
}

BOOL EncodeDate(int iDay, int iMonth, int iYear, int iDuration, DWORD *pData)
{	DWORD b1, b2, b3, b4;
	int iOffsetYear;

	// Check validity
	if(iMonth < 1 || iMonth > 12)
		return FALSE;
	if(iDay < 1 || iDay > g_MonthLengths[iMonth - 1])
		return FALSE;
	if(iYear < 1999 || iYear > 2100)
		return FALSE;
	if(iDuration < 1 || iDuration > 1023)
		return FALSE;
	if(!pData)
		return FALSE;

	iOffsetYear = iYear - 1999;

	b1 = LOBYTE((iDay << 2) | ((iDuration >> 8) & 0x03));
	b2 = LOBYTE(iDuration);
	b3 = LOBYTE((iOffsetYear << 1) | 0x01);
	b4 = LOBYTE((iMonth << 2) | 0xC3);

#ifdef _SC_ENCRYPT
	b1 ^= s_EncryptString[0];
	b2 ^= s_EncryptString[1];
	b3 ^= s_EncryptString[2];
	b4 ^= s_EncryptString[3];
#endif

	*pData = (((((b1 * 256) + b2) * 256) + b3) * 256) + b4;
	return TRUE;
}

BOOL DecodeDate(DWORD Data, int *pDay, int *pMonth, int *pYear, int *pDuration)
{	BYTE b1, b2, b3, b4;
	int iDay, iMonth, iYear, iDuration;

	b1 = HIBYTE(HIWORD(Data));
	b2 = LOBYTE(HIWORD(Data));
	b3 = HIBYTE(LOWORD(Data));
	b4 = LOBYTE(LOWORD(Data));

#ifdef _SC_ENCRYPT
	b1 ^= s_EncryptString[0];
	b2 ^= s_EncryptString[1];
	b3 ^= s_EncryptString[2];
	b4 ^= s_EncryptString[3];
#endif

	iDay = b1 >> 2;
	iDuration = b2 + ((b1 & 0x03) * 256);
	iYear = (b3 >> 1) + 1999;
	iMonth = (b4 >> 2) & 0x0F;

	// Check validity
	if((b3 & 0x01) != 0x01)
		return FALSE;
	if((b4 & 0xC3) != 0xC3)
		return FALSE;
	if(iMonth < 1 || iMonth > 12)
		return FALSE;
	if(iDay < 1 || iDay > g_MonthLengths[iMonth - 1])
		return FALSE;
	if(iYear < 1999 || iYear > 2100)
		return FALSE;
	if(iDuration < 1 || iDuration > 1023)
		return FALSE;
	if(!pDay)
		return FALSE;
	if(!pMonth)
		return FALSE;
	if(!pYear)
		return FALSE;
	if(!pDuration)
		return FALSE;

	*pDay = iDay;
	*pMonth = iMonth;
	*pYear = iYear;
	*pDuration = iDuration;
	return TRUE;
}

BOOL SCShouldExtend(LPCTSTR szKey)
{	CString csKey = szKey;
	DWORD dwKey;
	int iDay, iMonth, iYear, iDuration;
	SYSTEMTIME time;

	GetSystemTime(&time);
	COleDateTime odtToday(time);

	csKey.MakeUpper();
	if(!csKey.Compare(s_szMasterKey))
		return FALSE;
	if(!ConvertKeyStringToDWORD(csKey, &dwKey))
		return FALSE;
	if(!DecodeDate(dwKey, &iDay, &iMonth, &iYear, &iDuration))
		return FALSE;
	COleDateTime odtStart(iYear, iMonth, iDay, 0, 0, 0);
	if(odtStart > odtToday)
		return FALSE;
	COleDateTimeSpan span(iDuration, 0, 0, 0);
	COleDateTime odtEnd(iYear, iMonth, iDay, 0, 0, 0);
	odtEnd += span;
	if(odtEnd < odtToday)
		return FALSE;
	return TRUE;
}