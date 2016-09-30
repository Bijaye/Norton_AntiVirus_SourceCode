////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccValueCollectionInterface.h"
#include "ccStringInterface.h"
#include "ccFile.h"
#include "ccSymInstalledApps.h"

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include <hrx.h>

#define TRACE_SCOPE(s0) ::AVSubmit::utils::TraceScope s0(_T(__FUNCTION__));

#define SYMGUID_FORMAT_STR(xGUID) _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), \
								  xGUID->Data1, xGUID->Data2, xGUID->Data3, \
								  xGUID->Data4[0], xGUID->Data4[1], xGUID->Data4[2], xGUID->Data4[3], \
								  xGUID->Data4[4], xGUID->Data4[5], xGUID->Data4[6], xGUID->Data4[7] 

#define CATCH_NO_RETURN(hr) \
	catch(std::bad_alloc&) {hr = E_OUTOFMEMORY; CCTRCTXE0(_T("caught std::_bad_alloc"));} \
	catch(ATL::CAtlException& e) {hr = e; CCTRCTXE1(_T("atl ex: 0x%08X"), hr);} \
	catch(_com_error& e) {hr = e.Error(); CCTRCTXE1(_T("error: 0x%08X"), hr);} 

#define CATCH_RETURN(hr) \
	CATCH_NO_RETURN(hr) \
	return hr;

namespace AVSubmit {
namespace utils {

struct BoolX
{
	HRESULT hrDefault;
	BoolX(HRESULT hrd = E_FAIL) : hrDefault(hrd) {}
	const BoolX& operator << (bool b) throw(_com_error)
	{
		if(!b) throw _com_error(hrDefault);
		return *this;
	}
};

template <typename T>
void FreeObject(const T*& t)
{
	if(t != NULL)
	{
		t->Release();
		t = NULL;
	}
}

struct TraceScope
{
	const TCHAR* pszFunction;
	TraceScope(const TCHAR* pszFunc) : pszFunction(pszFunc)
	{
		CCTRACEI(_T("+++ entering %s"), pszFunction);
	}
	~TraceScope()
	{
		CCTRACEI(_T("--- leaving %s"), pszFunction);
	}
};

static bool GetString(size_t key, const cc::IValueCollection* pCol, cc::IString*& pString)
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

static HRESULT LastError()
{
	DWORD dwErr = GetLastError();
	return dwErr == ERROR_SUCCESS ? E_FAIL : HRESULT_FROM_WIN32(dwErr);
}

enum { GUID_MAX_CHARS = 39 };
static bool GUIDToString(const GUID& guid, TCHAR* szGUID) throw()
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

static bool StringToGUID(const WCHAR* buf, GUID& id)
{
	return SUCCEEDED(::IIDFromString((LPOLESTR)buf, &id));
}


static bool GetTempFile(ccLib::CString& sName, ccLib::CString& sTempDir, ccLib::CString& sPath)
{
	BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		TCHAR szGuid[utils::GUID_MAX_CHARS];
		GUID guid;
		CoCreateGuid(&guid);

		brx << utils::GUIDToString(guid, szGuid);
		brx << ccSym::CInstalledApps::GetInstAppsDirectory(_T("SubmissionEngineData"), sTempDir);

		PathAppend(sTempDir.GetBuffer(MAX_PATH), _T("Temp"));
		sTempDir.ReleaseBuffer();

		if(!ccLib::CFile::MakeSureDirectoryExists(sTempDir))
		{
			CCTRCTXE2(_T("Failed to create tempdir: %s, %d"), sTempDir, GetLastError());
			brx << false;
		}

		sPath = sTempDir;
		PathAppend(sPath.GetBuffer(MAX_PATH), szGuid);
		sPath.ReleaseBuffer();
		sName = szGuid;
	}
	CATCH_NO_RETURN(hr);
	return SUCCEEDED(hr);
}


}
}

