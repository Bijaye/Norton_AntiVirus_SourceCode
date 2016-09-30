///////////////////////////////////////////////////////////////////////////////
//
// CDebugLog
//
// To use this logging class do the following:
//
//		1. In your Stdafx.h add the following lines:
//
//				#include "DebugLog.h"
//				extern CDebugLog g_Log;
//
//		2. In your main .cpp file add the following line:
//
//				CDebugLog g_Log("MyModuleName.log");
//
// Now you can log from any file that includes Stdafx.h.
//
//
// To active CDebugLog, create the following DWORD registry key/value:
//
// [HKLM\Software\Symantec]
// "DebugLog"=0X0
//
// Replace 0x0 with the desired output flag(s).
// Flags may be combined (e.g. DebugLogOutput_File | DebugLogOutput_Debugger).
//
// Flags:
//
//		DebugLogOutput_File		= Logs to a file.
//		DebugLogOutput_FilePID	= Appends the process ID to the log file name.
//								  Only used if DebugLogOutput_File is also set.
//		DebugLogOutput_Debugger	= Logs to the debugger.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_DEBUGLOG_H__6F667569_AED4_4288_9F96_5C811E6D0C10__INCLUDED_)
#define _DEBUGLOG_H__6F667569_AED4_4288_9F96_5C811E6D0C10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CDebugLog
{
	enum DebugLogOutput
	{
		DebugLogOutput_None		= 0x0,
		DebugLogOutput_File		= 0x1,
		DebugLogOutput_FilePID	= 0x2,
		DebugLogOutput_Debugger	= 0x4
	};

public:
	// Constructor.
	CDebugLog(const char* szLogName);

	// Destructor.
	~CDebugLog();

	// Format and log a string. Adds a CRLF to the end of the output.
	void Log(const char* szFormat, ...);

	// Format and log an exception. Adds a CRLF to the end of the output.
	void LogEx(const char* szFormat, ...);

	// Format and log a String Appends with date & Time. Adds a CRLF to the end of the output.
	void LogT(const char* szFormat, ...);

	DWORD	getDebugFlags()
	{
		return GetOutputFlags();
	}
private:
	// Output type.
	DWORD m_dwOutputFlags;

	// Output file handle.
	HANDLE m_hFile;

	// Critical section object.
	CRITICAL_SECTION m_Critical;

private:
	// Returns the output flags (read from the registry).
	DWORD GetOutputFlags();

	// Helper function that retrieve a handle to the log file.
	HANDLE GetLogFileHandle(const char* szLogName);
};


#endif // !defined(_DEBUGLOG_H__6F667569_AED4_4288_9F96_5C811E6D0C10__INCLUDED_)
