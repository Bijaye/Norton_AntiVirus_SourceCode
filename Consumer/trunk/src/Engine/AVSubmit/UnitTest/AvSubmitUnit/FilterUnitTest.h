////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccUnitTestCaseImpl.h"
#include "md5.h"

class CFilterUnitTest : public ccUnit::CTestCaseImpl
{
public:
	CFilterUnitTest(void);
	virtual ~CFilterUnitTest(void);

	bool Execute() throw();

	static bool BinResToFile(LPCTSTR pszFile, ccLib::CString& sPath, int res_id);

protected:
	bool TestWordMacro();
	bool TestExcelMacro();
	bool TestDLL();

	bool DoFilter(const ccLib::CString& sPath, ccLib::CString& sOutPath);
	bool FileMD5(const ccLib::CString& sPath, crypto::CMD5& md5);
};
