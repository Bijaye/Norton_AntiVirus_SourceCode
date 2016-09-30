////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(DRIVERCHECK_H__INCLUDED_)
#define DRIVERCHECK_H__INCLUDED_

class CDriverCheck
{
public:
	CDriverCheck(LPTSTR szDriverName);
	virtual ~CDriverCheck();

	bool Verify();

	// Checks registry key and file path
	virtual bool CheckFile();

	// Verifies the driver is set to the correct startup mode
	virtual bool VerifyStartType();

	// Performs extra driver specific checks
	virtual bool ExtraChecks() = 0;

	// Installs the driver
	bool Install(LPCTSTR cszImagePath, DWORD dwStart, DWORD dwType);

	CRegKey m_ServiceKey;
	TCHAR m_szPath[MAX_PATH];

private:

	TCHAR m_szDriverName[MAX_PATH];
};

#endif // !defined(DRIVERCHECK_H__INCLUDED_)