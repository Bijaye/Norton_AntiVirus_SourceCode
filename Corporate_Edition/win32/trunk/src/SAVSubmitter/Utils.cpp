
#include <stdafx.h>
#include "Utils.h"
#include "ccTrace.h"
#include "ccLib.h"

#include <strsafe.h>

namespace clfs {
namespace utils {
namespace {
	static const BYTE g_tempKey[] = {0x18, 0x12, 0x6e, 0x7b, 0xd3, 0xf8, 0x4b, 0x3f, 0x3e, 0x4d, 0xf0, 0x94, 0xde, 0xf5, 0xb7, 0xde};
	static struct hr_sr_map
	{
		HRESULT hr;
		SYMRESULT sr;
	} g_resmap[] = {

		//general
		{S_OK, SYM_OK},
		{E_FAIL, SYMERR_UNKNOWN},
		{E_INVALIDARG, SYMERR_INVALIDARG}, //SR(invalidarg) ==> HR(invalidarg), not HR(pointer)
		{E_NOINTERFACE, SYMERR_NOINTERFACE},
		{E_OUTOFMEMORY, SYMERR_OUTOFMEMORY},
		{E_NOTIMPL, SYMERR_NOTIMPLEMENTED},
		{E_ACCESSDENIED, SYMERR_ACCESSDENIED},
		{HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), SYMERR_PATH_NOT_FOUND},
		{HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND), SYMERR_MISSING_EXPORTS},
		{HRESULT_FROM_WIN32(ERROR_BAD_FORMAT), SYMERR_INVALID_FILE},
		{S_FALSE, SYM_FALSE},

		/*
		//ccSettings
		{HRESULT_FROM_WIN32(ERROR_NOT_FOUND), SYM_SETTINGS_NOT_FOUND},
		{HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER), SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL},
		{HRESULT_FROM_WIN32(ERROR_INTERNAL_ERROR), SYM_SETTINGS_ERROR_INTERNAL_ERROR},
		{CO_E_NOTINITIALIZED, SYM_SETTINGS_ERROR_NO_COINIT},
		{RPC_E_VERSION_MISMATCH, SYM_SETTINGS_ERROR_BAD_VERSION},
		{HRESULT_FROM_WIN32(ERROR_INVALID_USER_BUFFER), SYM_SETTINGS_BAD_BUFFER},
		{CO_E_INIT_SCM_EXEC_FAILURE, SYM_SETTINGS_SVC_NOT_RUNNING},
		{TYPE_E_TYPEMISMATCH, SYM_SETTINGS_TYPE_MISMATCH}, 
		*/

		//duplicated SYMRESULTS/HRESULT here
		{E_POINTER, SYMERR_INVALIDARG},
		//{E_UNEXPECTED, SYM_SETTINGS_COM_ERROR},
		//{TYPE_E_TYPEMISMATCH, SYM_SETTINGS_ERROR_CANT_CHANGE_TYPE},
	};
}

HRESULT SymresultToHresult(SYMRESULT sr)
{
	if(SYM_FAILED(sr))
		CCTRCTXE1(_T("sr = 0x%08X"), sr);

	for(short i = 0; i<CCDIMOF(g_resmap); ++i)
	{
		if(sr == g_resmap[i].sr)
			return g_resmap[i].hr;
	}
	return E_FAIL;
}

SYMRESULT HresultToSymresult(HRESULT hr)
{
	if(FAILED(hr))
		CCTRCTXE1(_T("hr = 0x%08X"), hr);

	for(short i = 0; i<CCDIMOF(g_resmap); ++i)
	{
		if(hr == g_resmap[i].hr)
			return g_resmap[i].sr;
	}
	return SYMERR_UNKNOWN;
}

HRESULT LastError()
{
	DWORD dwErr = GetLastError();
	return dwErr == ERROR_SUCCESS ? E_FAIL : HRESULT_FROM_WIN32(dwErr);
}

bool GUIDToString(const GUID& guid, TCHAR* szGUID) throw()
{
    return SUCCEEDED(
        StringCchPrintf(szGUID, GUID_MAX_CHARS, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
        guid.Data1, 
        long(guid.Data2), 
        long(guid.Data3),
        long(guid.Data4[0]), long(guid.Data4[1]),
        long(guid.Data4[2]), long(guid.Data4[3]), long(guid.Data4[4]), long(guid.Data4[5]), long(guid.Data4[6]), long(guid.Data4[7])
        ));
}

bool StringToGUID(const WCHAR* buf, GUID& id)
{
	return SUCCEEDED(::IIDFromString((LPOLESTR)buf, &id));
}

bool GetString(size_t key, const cc::IValueCollection* pCol, cc::IString*& pString)
{
	FreeObject(pString);

	if(pCol == NULL)
		return false;

	ISymBasePtr p;

	if(!pCol->GetValue(key, p.m_p))
		return false;

	cc::IStringQIPtr pQI = p;
	if(pQI == NULL)
		return false;

	pString = pQI.Detach();
	return true;
}

bool ConvertUTCTimeToLocalTime(SYSTEMTIME& stUTCTime, SYSTEMTIME& stLocalTime)
{
	bool bRetVal = true;

	TIME_ZONE_INFORMATION tz;
	DWORD dwRetVal = ::GetTimeZoneInformation(&tz);
	if (dwRetVal == TIME_ZONE_ID_INVALID)
	{
		// Cannot get timezone information. Information will be shown in UTC Time
		stLocalTime = stUTCTime;
		bRetVal = false;
	}
	else
	{
		BOOL bRv = ::SystemTimeToTzSpecificLocalTime(&tz, &stUTCTime, &stLocalTime);
		if (bRv == FALSE)
		{
			// cannot convert from UTC Time to local time. Information will be shown in UTC Time
			stLocalTime = stUTCTime;
			bRetVal = false;
		}
	}

	return bRetVal;
}

}
}
