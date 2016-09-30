////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(LOGGING_H__INCLUDED_)
#define LOGGING_H__INCLUDED_

class CLog
{
public:
	CLog();
	~CLog();  // This will close the log file

	// This will create the log file and append to the existing one if it already exists
	bool CreateLog(LPTSTR);
	void CLog::Log(const TCHAR*, ...);

private:
	HANDLE m_hLog;
	TCHAR m_szLogFileName[MAX_PATH];
};

#endif // !defined(LOGGING_H__INCLUDED_)