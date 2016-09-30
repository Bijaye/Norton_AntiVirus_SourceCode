
#include "Stdafx.h"
#include <stdio.h>
#include <time.h>
#include "DebugLog.h"

const int BUFFER_SIZE				= 2048;
const char LOG_REGISTRY_KEY[]		= "Software\\Symantec";
const char LOG_REGISTRY_VALUE[]		= "DebugLog";

#ifdef _ENABLE_SYMREG_
    #pragma message("!!! WARNING: DebugLog.cpp :")
    #pragma message("!!! RegToSymReg.h has been detected in DebugLog.cpp")
    #pragma message("!!! CDebugLog is normally a global object and will be created with the module/process load.")
    #pragma message("!!! During the module load SymStore.dll may cause deadlocks while validating the digital signature!")
    #pragma message("!!! DO NOT #include RegToSymReg.h in header files, use it in Cxx files only when needed!")
#endif

CDebugLog::CDebugLog(const char* szLogName)
{
	m_hFile = INVALID_HANDLE_VALUE;

	// Initialize our critical section object.

	InitializeCriticalSection(&m_Critical);

	// Get the output flags.

	m_dwOutputFlags = GetOutputFlags();

	// Bail if logging is disabled.

	if (m_dwOutputFlags == 0)
		return;

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

	char szDate[32];
	_tstrdate(szDate);

	char szTime[32];
	_tstrtime(szTime);

	Log("Log started at %s on %s.", szTime, szDate);
}

CDebugLog::~CDebugLog()
{
	// Close the log file before quiting.

	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	// Delete our critical section object.

	DeleteCriticalSection(&m_Critical);
}

void CDebugLog::Log(const char* szFormat, ...)
{
	static DWORD dwBytes;
	static char szBuffer[BUFFER_SIZE];

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
		_tcscat(szBuffer, "\r\n");

	// Output the buffer.

	if (m_dwOutputFlags & DebugLogOutput_File)
	{
		WriteFile(m_hFile, szBuffer, (DWORD) _tcslen(szBuffer), &dwBytes, NULL);
	}
	
	if (m_dwOutputFlags & DebugLogOutput_Debugger)
	{
		OutputDebugString(szBuffer);
	}

	// Unlock
	LeaveCriticalSection(&m_Critical);
}

void CDebugLog::LogEx(const char* szFormat, ...)
{
	static DWORD dwBytes;
	static char szBuffer[BUFFER_SIZE];
	static const char szPrefix[] = "Exception: ";

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
		_tcscat(szBuffer, "\r\n");

	// Output the buffer.

	if (m_dwOutputFlags & DebugLogOutput_File)
	{
		WriteFile(m_hFile, szBuffer, (DWORD) _tcslen(szBuffer), &dwBytes, NULL);
	}
	
	if (m_dwOutputFlags & DebugLogOutput_Debugger)
	{
		OutputDebugString(szBuffer);
	}

	// Unlock
	LeaveCriticalSection(&m_Critical);
}

HANDLE CDebugLog::GetLogFileHandle(const char* szLogName)
{
	// Get the full path to the log file.
	// The log file will be created in the same directory as the process.

	char szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	char* pSlash = _tcsrchr(szPath, _T('\\'));

	if (pSlash == NULL)
		return INVALID_HANDLE_VALUE;

	_tcscpy(CharNext(pSlash), szLogName);

	// Append the PID to the log file name if necessary.

	if (m_dwOutputFlags & DebugLogOutput_FilePID)
	{
		char szPID[MAX_PATH];
		wsprintf(szPID, "#%d.log", GetCurrentProcessId());

		char* pExtension = _tcsrchr(szPath, _T('.'));

		if (pExtension == NULL)
		{
			_tcscat(szPath, szPID);
		}
		else
		{
			_tcscpy(pExtension, szPID);			
		}
	}

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

