////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccString.h"

class CDefHelper
{
public:
	CDefHelper(void);
	~CDefHelper(void);

	static HRESULT GetDefAge(DWORD& dwAge);
	static HRESULT GetDefDate(ccLib::CStringW& sDate);

private:
	static HRESULT init();

private:
	static bool m_bInit;
	static DWORD m_dwAge;
	static ccLib::CStringW m_sDate;
};
