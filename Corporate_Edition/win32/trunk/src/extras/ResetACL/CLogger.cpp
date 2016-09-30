// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CLogger.cpp

#include "stdafx.h"
#include "CLogger.h"

//===========================================================================

CLogger::CLogger(const TCHAR *filename)
	: m_fileHandle(INVALID_HANDLE_VALUE)
{
	TCHAR tempPath[MAX_PATH+1];

	if(!GetTempPath(MAX_PATH, tempPath))
		return;

	if (tempPath[_tcslen(tempPath)-1] != _T('\\'))
		_tcscat(tempPath, _T("\\"));
	_tcscat(tempPath, filename);

	m_fileHandle = CreateFile(tempPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_fileHandle != INVALID_HANDLE_VALUE)
		SetEndOfFile(m_fileHandle);
}

//===========================================================================

CLogger::~CLogger()
{
	if (m_fileHandle != INVALID_HANDLE_VALUE)
		CloseHandle(m_fileHandle);
}

//===========================================================================

void CLogger::LogStr(const TCHAR *string)
{
	DWORD bytes;
	DWORD len;

	_tcsncpy(m_logLine, string, MAX_LOG_LINE);
	m_logLine[MAX_LOG_LINE-3] = _T('\0');

	// if the string is empty, just do a newline/return sequence
	if(!(len = _tcslen(m_logLine)))
		_tcscpy(m_logLine, NEWLINE);
	// else check to see if the newline/return sequence is there
	else if(m_logLine[len-1] != NEWLINE_CHAR)
		_tcscat(m_logLine, NEWLINE);

	OutputDebugString(m_logLine);

	if (m_fileHandle == INVALID_HANDLE_VALUE)
		return;

	WriteFile(m_fileHandle, m_logLine, _tcslen(m_logLine)*sizeof(TCHAR), &bytes, NULL);
}

/* end source file */
