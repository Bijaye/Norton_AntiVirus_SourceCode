////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\contentfilter.h"
#include "ContentStripper.h"
#include "ccStringConvert.h"
#include "ccSymStringImpl.h"
#include "Utils.h"

namespace AVSubmit {

CContentFilter::CContentFilter(void)
{
}

CContentFilter::~CContentFilter(void)
{
}

HRESULT CContentFilter::FilterContent(const ISymBase* pIn, ISymBase*& pOut) throw()
{
	TRACE_SCOPE(s0);
	HRESULT hr = S_OK;
	for(;;)
	{
		cc::IStringQIPtr pSrcPath = pIn;
		if(pSrcPath == NULL)
		{
			CCTRCTXE0(_T("pSrcPath == NULL"));
			hr = E_NOINTERFACE;
			break;
		}

		TCHAR szTempDir[MAX_PATH] = {0};
		if(0 == GetTempPath(MAX_PATH, szTempDir))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			CCTRCTXE1(_T("GetTempPath failed: 0x%08X"), hr);
			break;
		}

		GUID guidName = GUID_NULL;
		hr = CoCreateGuid(&guidName);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to create GUID: 0x%08X"), hr);
			break;
		}

		ccLib::CString sPath;
		if(!ccLib::CStringConvert::FormatPath(sPath, szTempDir, SYMGUID_FORMAT_STR((&guidName))))
		{
			CCTRCTXE0(_T("FormatPath failed"));
			hr = E_FAIL;
			break;
		}

		CCTRCTXI2(_T("Input = %ls, Output = %s"), pSrcPath->GetStringW(), sPath);

		std::auto_ptr<filter::CContentStripper> pContentStripper(filter::CContentStripperFactory::CreateInstance());
		if(pContentStripper.get() == NULL)
		{
			hr = E_OUTOFMEMORY;
			CCTRCTXE0(_T("Failed to allocate content stripper"));
			break;
		}

		filter::CContentStripper::INIT_STATUS initStatus = pContentStripper->StartUp(szTempDir);
		if(initStatus != filter::CContentStripper::INIT_NO_ERROR)
		{
			hr = E_FAIL;
			CCTRCTXE1(_T("Init failed: %d"), initStatus);
			break;
		}

#ifdef UNICODE
		filter::CContentStripper::STRIP_STATUS stripStatus = pContentStripper->Strip(pSrcPath->GetStringW(), sPath);
#else
		filter::CContentStripper::STRIP_STATUS stripStatus = pContentStripper->Strip(pSrcPath->GetStringA(), sPath);
#endif
		if(stripStatus != filter::CContentStripper::STRIP_NO_ERROR)
		{
			switch(stripStatus)
			{
			case filter::CContentStripper::STRIP_MALLOC_ERROR: hr = E_OUTOFMEMORY; break;
			case filter::CContentStripper::STRIP_PASSWORD_ENCRYPTED: hr = E_ACCESSDENIED; break;
			case filter::CContentStripper::STRIP_FILE_NOT_FOUND_ERROR: hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND); break;
			default: hr = E_FAIL; break;
			}
			CCTRCTXE2(_T("strip failed (%d): 0x%08X"), stripStatus, hr);
			break;
		}

		utils::FreeObject(pOut);
		cc::IStringPtr pOutPath;
		pOutPath.Attach(ccSym::CStringImpl::CreateStringImpl(sPath));
		if(pOutPath ==  NULL)
		{
			hr = E_OUTOFMEMORY;
			CCTRCTXE1(_T("Failed to build output: 0x%08X"), hr);
			break;
		}
		pOut = pOutPath.Detach();

		CCTRCTXI0(_T("Success"));
		break;
	}
	return hr;
}

}