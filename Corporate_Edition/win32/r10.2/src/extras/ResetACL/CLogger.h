// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CLogger.h

#ifndef _CLOGGER_H_
#define _CLOGGER_H_

#include <windows.h>
#include <tchar.h>

#define NEWLINE _T("\r\n")
#define NEWLINE_CHAR _T('\n')

#define MAX_LOG_LINE 1024

class CLogger
	{
	public:
		CLogger(const TCHAR *filename);
		~CLogger();
		void LogStr(const TCHAR *string);
	private:
		CLogger();
		HANDLE m_fileHandle;
		TCHAR m_logLine[MAX_LOG_LINE];
	};

#endif

/* end header file */
