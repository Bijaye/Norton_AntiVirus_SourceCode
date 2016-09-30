// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#include "StdAfx.h"
#include ".\regkeyformat.h"
#include "Utils.h"
#include "ccSymStringImpl.h"

namespace AVSubmit {
namespace utils {

CRegKeyFormat::CRegKeyFormat(void)
{
}

CRegKeyFormat::~CRegKeyFormat(void)
{
}

HRESULT CRegKeyFormat::AddObject(ccEraser::IRemediationAction* pAction, cc::IStream* pUndo) throw()
{
	BoolX brx(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
		TRACE_SCOPE(s0);

		hrx << (pAction == NULL ? E_INVALIDARG : S_OK);
		cc::IKeyValueCollectionPtr pProps;
		pAction->GetProperties(pProps.m_p);
		hrx <<  (pProps == NULL ? E_POINTER : S_OK);
		cc::IStringPtr pKeyName;
		brx << utils::GetString(ccEraser::IRemediationAction::KeyName, pProps, pKeyName.m_p);
		if(pKeyName->GetLength() > 0)
		{
			CCTRCTXI1(_T("Key = %ls"), pKeyName->GetStringW());
			m_oss << "[" << pKeyName->GetStringA() << "]\r\n";
		}
		else
		{
			CCTRCTXE0(_T("KeyName is empty"));
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CRegKeyFormat::ToString(cc::IString*& pString) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		std::string str = m_oss.str();
		if(str.length() < 1)
			hrx << HRESULT_FROM_WIN32(ERROR_NO_DATA);

		utils::FreeObject(pString);

		pString = ccSym::CStringImpl::CreateStringImpl(str.c_str());
		CCTHROW_BAD_ALLOC(pString);
	}
	CATCH_RETURN(hr);
}

}
}
