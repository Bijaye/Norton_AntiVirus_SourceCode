////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atltime.h>
#include "DefHelper.h"
#include "uiElementInterface.h"
#include "uiDateDataInterface.h"
#include "uiProviderInterface.h"
#include "uiNISDataElementGuids.h"
#include "ISDataClientLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"

using namespace ui;
using namespace ISShared;

bool CDefHelper::m_bInit = false;
DWORD CDefHelper::m_dwAge = 0;
ccLib::CStringW CDefHelper::m_sDate;

CDefHelper::CDefHelper(void)
{
}

CDefHelper::~CDefHelper(void)
{
}

HRESULT CDefHelper::init()
{
	if(m_bInit)
		return S_OK;

	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Create the data provider object
		ISShared_IProvider loader;
		IProviderPtr spDataProvider;
		hrx << (SYM_SUCCEEDED(loader.CreateObject(GETMODULEMGR(), &spDataProvider))? S_OK : E_FAIL);

		// Get definitions element object.
		IElementPtr spDefsElement;
		hrx << spDataProvider->GetElement(CLSID_NIS_VirusDefs, spDefsElement);

		// Get data object
		IDataPtr spData;
		hrx << spDefsElement->GetData(spData);

		// QI for definitions date 
		IDateDataQIPtr spDefsDate(spData);

		// Get definitions date.
		SYSTEMTIME st = {0};
		hrx << spDefsDate->GetDate(st);
		CCTRCTXI3(L"DefsDate(y,m,d): %d,%d,%d", st.wYear, st.wMonth, st.wDay);

		// Validate virus defs date before doing conversion
		if( st.wYear > 2000 && st.wYear < 2199 && 
			st.wMonth > 0 && st.wMonth < 13 && 
			st.wDay > 0 && st.wDay < 32 )
		{
			DWORD dwSize = 24;
			GetDateFormat(LOCALE_USER_DEFAULT,	DATE_SHORTDATE, 
					&st, NULL, m_sDate.GetBufferSetLength(dwSize), dwSize);
			m_sDate.ReleaseBuffer();
			CCTRCTXI1(L"DefsDate: %s", m_sDate);

			//
			// Computer definitions age from definitions date.
			// 
			ATL::CTime timeDefTime(st.wYear, st.wMonth, st.wDay, 0, 0, 0);

			// Compute age of defs
			//  Note: We experienced that difftime return the difference of
			//   two timers, not necessary (timer1-timer2).
			//   Therefore, the check for (timeCurrentTime > timeDefTime)
			//   is necessary.
			if(CTime::GetCurrentTime() > timeDefTime)
			{
				// how many days old are the defs?
				m_dwAge = (CTime::GetCurrentTime() - timeDefTime).GetDays();   
				CCTRCTXI1(L"Age: %d", m_dwAge);
			}
		}
		else
		{
			CCTRCTXE0(L"Bogus definitions date.");
		}

		m_bInit = true;
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRCTXE1(L"_com_error thrown. Error: 0x%08X", hr);
	}
	catch(CAtlException& e)
	{
		hr = e.m_hr;
		CCTRCTXE1(L"AtlExpception thrown. Error: 0x%08X", e.m_hr);
	}

	return hr;
}

HRESULT CDefHelper::GetDefAge(DWORD& dwAge)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << init();
		dwAge = m_dwAge;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

HRESULT CDefHelper::GetDefDate(ccLib::CStringW& sDate)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << init();
		sDate = m_sDate;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}
