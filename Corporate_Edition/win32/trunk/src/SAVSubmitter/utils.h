
#pragma once

//#include "ServiceInterface.h"
#include "ccMemory.h"
#include "ccString.h"
#include "ccStringInterface.h"
#include "ccValueCollectionInterface.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymMemoryStreamImpl.h"

#include <comutil.h>
#include <srx.h>
#include <hrx.h>

//#define _PROFILE_MEM
//#define _PROFILE_TIME
//#include "ProfileMem.h"

namespace clfs {
	namespace utils {

		struct BoolX
		{
			HRESULT hrDefault;
			BoolX(HRESULT hrd = E_FAIL) : hrDefault(hrd) {}
			const BoolX& operator << (bool b)
			{
				if(!b) throw _com_error(hrDefault);
				return *this;
			}
		};

		template <typename T>
		void FreeObject(T*& pObj)
		{
			if(pObj)
			{
				pObj->Release();
				pObj = NULL;
			}
		}

		enum {GUID_MAX_CHARS = 39};
		bool GUIDToString(REFGUID id, TCHAR* buf);
		bool StringToGUID(const WCHAR* buf, GUID& id);
		HRESULT LastError();
		bool GetString(size_t key, const cc::IValueCollection* pCol, cc::IString*&);

		HRESULT SymresultToHresult(SYMRESULT);
		SYMRESULT HresultToSymresult(HRESULT);
		bool ConvertUTCTimeToLocalTime(SYSTEMTIME& stUTCTime, SYSTEMTIME& stLocalTime);

//		static const TCHAR* const g_szInstAppsKey = _T("SubmissionEngine");
//		static const TCHAR* const g_szDataKey = _T("SubmissionEngineData");
//		static const TCHAR* const g_szControlDataFile = _T("SCD.xml");

	} //namespace utils
} //namespace clfs

#define TRACE_SCOPE(var)			TRACE_PROFI_FUNC(var)
#define TRACE_SCOPE1(var, msg)		TRACE_PROFI(var, msg)

#define CATCH_NO_RETURN(hr) \
	catch(std::bad_alloc&) {hr = E_OUTOFMEMORY; CCTRCTXE0(_T("caught std::bad_alloc"));} \
	catch(ATL::CAtlException& e) {hr = e; CCTRCTXE1(_T("atl ex: 0x%08X"), hr);} \
	catch(_com_error& e) {hr = e.Error(); CCTRCTXE1(_T("error: 0x%08X"), hr);} \
	catch(srx_exception& e) {hr = ::clfs::utils::SymresultToHresult(e.Error()); CCTRCTXE2(_T("srx: 0x%08X (hr 0x%08X)"), e.Error(), hr);}

#define CATCH_RETURN(hr) \
	CATCH_NO_RETURN(hr) \
	return hr;

#define SYMGUID_FORMAT_STR_W(xGUID) L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", \
								  (xGUID)->Data1, (xGUID)->Data2, (xGUID)->Data3, \
								  (xGUID)->Data4[0], (xGUID)->Data4[1], (xGUID)->Data4[2], (xGUID)->Data4[3], \
								  (xGUID)->Data4[4], (xGUID)->Data4[5], (xGUID)->Data4[6], (xGUID)->Data4[7] \


#define SYMGUID_FORMAT_STR(xGUID) _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), \
								  (xGUID)->Data1, (xGUID)->Data2, (xGUID)->Data3, \
								  (xGUID)->Data4[0], (xGUID)->Data4[1], (xGUID)->Data4[2], (xGUID)->Data4[3], \
								  (xGUID)->Data4[4], (xGUID)->Data4[5], (xGUID)->Data4[6], (xGUID)->Data4[7] \


#define STRIZE(x) #x
#define PPSTRIZE(x) STRIZE(x)
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#define INFO(msg) message(FILE_LINE "information : " msg)
