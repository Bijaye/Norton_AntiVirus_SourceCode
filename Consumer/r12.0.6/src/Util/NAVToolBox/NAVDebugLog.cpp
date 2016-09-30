#include "Stdafx.h"
#include <stdio.h>
#include <time.h>
#include "NAVDebugLog.h"
#include "NAVInfo.h"

namespace NAVToolbox
{
const int BUFFER_SIZE				= 2048;
const TCHAR LOG_REGISTRY_KEY[]		= _T("Software\\Symantec");
const TCHAR LOG_REGISTRY_VALUE[]	= _T("DebugLog");
const TCHAR LOG_EXTENSION[]         = _T("LOG");

#ifdef _ENABLE_SYMREG_
    #pragma message("!!! WARNING: DebugLog.cpp :")
    #pragma message("!!! RegToSymReg.h has been detected in DebugLog.cpp")
    #pragma message("!!! CDebugLog is normally a global object and will be created with the module/process load.")
    #pragma message("!!! During the module load SymStore.dll may cause deadlocks while validating the digital signature!")
    #pragma message("!!! DO NOT #include RegToSymReg.h in header files, use it in Cxx files only when needed!")
#endif

CDebugLog::CDebugLog(const TCHAR* szLogName)
{
    m_hFile = INVALID_HANDLE_VALUE;

    // Get the output flags.
    //
	m_dwOutputFlags = GetOutputFlags();

	// Bail if logging is disabled.

	if (m_dwOutputFlags == 0)
		return;

	// Initialize our critical section object.
    //
	InitializeCriticalSection(&m_Critical);

	// Open the log file if we're logging to file.

	if (m_dwOutputFlags & DebugLogOutput_File)
	{
		m_hFile = GetLogFileHandle(szLogName);

		// Log to the debugger if we can't log to file.

		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwOutputFlags = DebugLogOutput_Debugger;
		}
	}

	// Log the current time/date.

	TCHAR szDate[32];
	_tstrdate(szDate);

	TCHAR szTime[32];
	_tstrtime(szTime);

	Log(_T("Log started at %s on %s."), szTime, szDate);
}

CDebugLog::~CDebugLog()
{
	// Close the log file before quiting.

	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	// Delete our critical section object.

	DeleteCriticalSection(&m_Critical);
}

void CDebugLog::Log(const TCHAR* szFormat, ...)
{
	static DWORD dwBytes;
	static TCHAR szBuffer[BUFFER_SIZE];

	// Make sure logging is enabled.

	if (m_dwOutputFlags == 0)
		return;

	// This is a static buffer, we need to lock before copying to this buffer
	EnterCriticalSection(&m_Critical);

	// Format the output.
	// According to KB Q77255, output greater than 1K will be truncated by wvsprintf.

	va_list args;
	va_start(args, szFormat);
	wvsprintf(szBuffer, szFormat, args);
	va_end(args);

	// Add a CRLF terminator to the output.

	if (szBuffer[_tcslen(szBuffer) - 1] != '\n')
		_tcscat(szBuffer, _T("\r\n"));

	// Output the buffer.

	if (m_dwOutputFlags & DebugLogOutput_File)
	{
		WriteFile(m_hFile, szBuffer, _tcslen(szBuffer)*sizeof(TCHAR), &dwBytes, NULL);
	}
	
	if (m_dwOutputFlags & DebugLogOutput_Debugger)
	{
		OutputDebugString(szBuffer);
	}

	// Unlock
	LeaveCriticalSection(&m_Critical);
}

void CDebugLog::LogEx(const TCHAR* szFormat, ...)
{
	static DWORD dwBytes;
	static TCHAR szBuffer[BUFFER_SIZE];
	static const TCHAR szPrefix[] = _T("Exception: ");

	// Make sure logging is enabled.

	if (m_dwOutputFlags == 0)
		return;

	// This is a static buffer, we need to lock before copying to this buffer
	EnterCriticalSection(&m_Critical);

	// Start with the "Exception:" prefix.

	_tcscpy(szBuffer, szPrefix);

	// Format the output.
	// According to KB Q77255, output greater than 1K will be truncated by wvsprintf.

	va_list args;
	va_start(args, szFormat);
	wvsprintf(szBuffer + _tcslen(szPrefix), szFormat, args);
	va_end(args);
	
	// Add a CRLF terminator to the output.

	if (szBuffer[_tcslen(szBuffer) - 1] != '\n')
		_tcscat(szBuffer, _T("\r\n"));

	// Output the buffer.

	if (m_dwOutputFlags & DebugLogOutput_File)
	{
		WriteFile(m_hFile, szBuffer, _tcslen(szBuffer)*sizeof(TCHAR), &dwBytes, NULL);
	}
	
	if (m_dwOutputFlags & DebugLogOutput_Debugger)
	{
		OutputDebugString(szBuffer);
	}

	// Unlock
	LeaveCriticalSection(&m_Critical);
}

HANDLE CDebugLog::GetLogFileHandle(const TCHAR* szLogName)
{
	if ( !szLogName )
        return INVALID_HANDLE_VALUE;

    // Get the full path to the log file.
	// The log file will be created in the same directory as the NAV install.

    CNAVInfo NAVInfo;

    TCHAR szPath[MAX_PATH] = {0};
    TCHAR szPID[MAX_PATH] = {0};

	// Append the PID to the log file name if necessary.

	if (m_dwOutputFlags & DebugLogOutput_FilePID)
	{
		wsprintf(szPID, _T("#%d"), GetCurrentProcessId());
    }

    int i = _tcslen ( NAVInfo.GetNAVDir () );
    _tcsncpy ( szPath, NAVInfo.GetNAVDir (), i );
    _tcscat ( szPath, _T("\\"));
    _tcscat ( szPath, szLogName );
    _tcscat ( szPath, szPID );
    _tcscat ( szPath, _T("."));
    _tcscat ( szPath, LOG_EXTENSION );

    //wsprintf ( "%s\\%s%s.%s", szPath, NAVInfo.GetNAVDir (), szLogName, szPID, LOG_EXTENSION);

#ifdef _UNICODE
    // Unicode text files need a tag in the beginning to signify that they are Unicode.
    // See MSDN http://msdn.microsoft.com/library/default.asp?url=/library/en-us/intl/unicode_42jv.asp
    // Or search for Byte-Order Mark
    //

    // If the file doesn't exist, create it and write out the tag.
    //
    if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes ( szPath ))
    {
	    HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                  OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if ( hFile ) 
        {
            wchar_t cBOM = (wchar_t)0xFEFF;
            DWORD dwBytesWritten = 0;
            WriteFile ( hFile, &cBOM, sizeof(wchar_t), &dwBytesWritten, NULL ); // Don't write the NULL byte out
            CloseHandle ( hFile );
        }
    }
#endif

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		SetFilePointer(hFile, 0, 0, FILE_END);

	return hFile;
}

DWORD CDebugLog::GetOutputFlags()
{
	// Read the logger state from the registry.

	HKEY hKey;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, LOG_REGISTRY_KEY, 0,
		KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
		return 0;

	DWORD dwValue = 0;
	DWORD dwSize = sizeof(dwValue);

	lResult = RegQueryValueEx(hKey, LOG_REGISTRY_VALUE, NULL, NULL,
		(BYTE*) &dwValue, &dwSize);

	RegCloseKey(hKey);

	if (lResult != ERROR_SUCCESS)
		return 0;

	return dwValue;
}

bool CDebugLog::IsEnabled ()
{
	if (m_dwOutputFlags == 0)
		return false;
    else
        return true;
}

}