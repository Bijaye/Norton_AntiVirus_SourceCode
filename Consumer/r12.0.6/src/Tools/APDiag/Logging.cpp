// Logging.cpp - This file contains the logging class
//

#include "stdafx.h"
#include "Logging.h"
#include <stdio.h>

const int BUFFER_SIZE = 2048;

CLog::CLog() : m_hLog(NULL)
{
	m_szLogFileName[0] = _TCHAR('\0');
}

CLog::~CLog()
{
	if( m_hLog )
	{
		CloseHandle(m_hLog);
		m_hLog = NULL;
	}
}

bool CLog::CreateLog(LPTSTR szLogFileName)
{
	// Save the log file name
	_tcscpy(m_szLogFileName, szLogFileName);

	// Create the log file
    m_hLog = CreateFile(m_szLogFileName, 
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ, // allow users to read the file during use
                            NULL,
                            OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if( m_hLog == INVALID_HANDLE_VALUE)
    {
        m_hLog = NULL;
		ShowErrorMsg(IDS_ERR_CREATE_LOG);
        m_szLogFileName[0] = _TCHAR('\0');

		return false;
    }

	// Move the file pointer to the end of the file
	SetFilePointer(m_hLog, 0, NULL, FILE_END);

	return true;
}

void CLog::Log(const char* szFormat, ...)
{
	if( m_hLog )
	{
		static DWORD dwBytes;
		static char szBuffer[BUFFER_SIZE];

		// Format the output.
		// According to KB Q77255, output greater than 1K will be truncated by wvsprintf.

		va_list args;
		va_start(args, szFormat);
		wvsprintf(szBuffer, szFormat, args);
		va_end(args);

		// Add a CRLF terminator to the output.

		if (szBuffer[_tcslen(szBuffer) - 1] != '\n')
			_tcscat(szBuffer, "\r\n");

		// Output the buffer.
		WriteFile(m_hLog, szBuffer, _tcslen(szBuffer), &dwBytes, NULL);
	}
}