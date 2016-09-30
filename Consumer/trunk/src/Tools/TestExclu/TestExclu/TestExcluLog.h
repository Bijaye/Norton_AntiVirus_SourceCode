////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include <vector>
#include "ccLibStd.h"

class TestExcluLog
{
public:
	TestExcluLog(void);
	~TestExcluLog(void);

	std:: vector <CString> m_strVector;
	std:: vector <std::vector <CString> > m_vVector;

	bool ReadFromFile(LPCTSTR strInputFile);
	bool Process();
};
