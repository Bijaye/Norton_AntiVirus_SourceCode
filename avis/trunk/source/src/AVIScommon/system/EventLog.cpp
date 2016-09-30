// RegistryLog.cpp: 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <string>

#include <AVIS.h>
#include "WinRegistry.h"

#include "EventLog.h"

//#include "IcePackEventMsgs.h"

using namespace EventLog;
using namespace std;

std::string EventSource::appName("");

bool EventLog::Register(const char* appName, const char* messageFileName,
						uint categoryCount)
{
	WinRegistry	top(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application");
	WinRegistry	myService(top.Key(), appName);

	if (!myService.ValidKey() && !myService.CreateKey(top.Key(), appName))
		return false;

    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

	string	directory(szFilePath);
	int	lastSlash	= directory.find_last_of("\\");
	string	fullMsgFileName = directory.substr(0, lastSlash+1);
	fullMsgFileName	+= messageFileName;

	myService.SetValue("CategoryMessageFile", (const uchar*) fullMsgFileName.c_str());
	myService.SetValue("CategoryCount", categoryCount);
	myService.SetValue("EventMessageFile", (const uchar*) fullMsgFileName.c_str());
	myService.SetValue("TypesSupported",
						EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE);
	
	return true;
}

void EventLog::UnRegister()
{
}

void EventLog::Log(DWORD severity, DWORD facility, DWORD code)
{
	EventSource	eventSource;

	ReportEvent(eventSource, severity, facility, code,
				NULL, 0, 0, NULL, NULL);
}

void EventLog::Log(DWORD severity, DWORD facility, DWORD code, const char* str1)
{
	EventSource	eventSource;
	const char * strs[1];
	strs[0]	= str1;

	ReportEvent(eventSource, severity, facility, code,
				NULL, 1, 0, strs, NULL);
}

void EventLog::Log(DWORD severity, DWORD facility, DWORD code, const char* str1, const char* str2)
{
	EventSource	eventSource;
	const char * strs[2];
	strs[0]	= str1;
	strs[1]	= str2;

	ReportEvent(eventSource, severity, facility, code,
				NULL, 2, 0, strs, NULL);
}
