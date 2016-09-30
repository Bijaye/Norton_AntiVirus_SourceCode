// EventLog.h: interface for the EventLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOG_H__FBAEEAEB_705F_11D3_AE60_00A0C9C71BBC__INCLUDED_)
#define AFX_EVENTLOG_H__FBAEEAEB_705F_11D3_AE60_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISCommon.h"

	// values that must exist in all event msg dlls for this to work properly
#define REG_BLANK_INFO                   ((DWORD)0x61010001L)
#define REG_BLANK_ERROR                  ((DWORD)0xE1010002L)
#define REG_SERVICE_START                ((DWORD)0x61010003L)
#define REG_SERVICE_STOP                 ((DWORD)0x61010004L)
#define REG_EXCEPTION_CAUGHT             ((DWORD)0xE1010005L)
#define REG_EXCEPTION_CAUGHT_STOP        ((DWORD)0xE1010006L)
#define REG_UNKNOWN_EXCEPTION_CAUGHT     ((DWORD)0xE1010007L)
#define REG_UNKNOWN_EXCEPTION_CAUGHT_STOP ((DWORD)0xE1010008L)

#define FACILITY_GENERAL                 0x101

namespace EventLog  
{
	class AVISCOMMON_API EventSource
	{
	public:
		EventSource() : eventSource(RegisterEventSource(NULL, appName.c_str())) {};
		~EventSource()	{ DeregisterEventSource(eventSource); eventSource = NULL; };

		operator HANDLE() { return eventSource; };

		static Init(const char* aName) { appName = aName; };
	private:
		static std::string	appName;
		HANDLE				eventSource;
	};

	inline void Init(const char* appName) { EventSource::Init(appName); }
	bool AVISCOMMON_API Register(const char* appName, const char* messageFileName, unsigned int categoryCount);
	void AVISCOMMON_API UnRegister(void);

	void AVISCOMMON_API Log(DWORD severity, DWORD facility, DWORD code);
	void AVISCOMMON_API Log(DWORD severity, DWORD facility, DWORD code, const char* str1);
	void AVISCOMMON_API Log(DWORD severity, DWORD facility, DWORD code, const char* str1, const char* str2);

								// Error log entries
	inline void Exception(const char* where, const char* details)
			{ Log(EVENTLOG_ERROR_TYPE, FACILITY_GENERAL, REG_EXCEPTION_CAUGHT, where, details); }

	inline void ExceptionStop(const char* where, const char* details)
			{ Log(EVENTLOG_ERROR_TYPE, FACILITY_GENERAL, REG_EXCEPTION_CAUGHT_STOP, where, details); }

	inline void UnknownException(const char* where)
			{ Log(EVENTLOG_ERROR_TYPE, FACILITY_GENERAL, REG_UNKNOWN_EXCEPTION_CAUGHT, where); }

	inline void UnknownExceptionStop(const char* where)
			{ Log(EVENTLOG_ERROR_TYPE, FACILITY_GENERAL, REG_UNKNOWN_EXCEPTION_CAUGHT_STOP, where); }



	inline void Error(DWORD facility, DWORD code)
			{ Log(EVENTLOG_ERROR_TYPE, facility, code); }

	inline void Error(DWORD facility, DWORD code, const char* str1)
			{ Log(EVENTLOG_ERROR_TYPE, facility, code, str1); }

	inline void Error(DWORD facility, DWORD code, const char* str1, const char* str2)
			{ Log(EVENTLOG_ERROR_TYPE, facility, code, str1, str2); }

								// Info log entries
	inline void StartMsg()
			{ Log(EVENTLOG_INFORMATION_TYPE, FACILITY_GENERAL, REG_SERVICE_START); }
	inline void StopMsg()
			{ Log(EVENTLOG_INFORMATION_TYPE, FACILITY_GENERAL, REG_SERVICE_STOP); }
	inline void Info(DWORD facility, DWORD code)
			{ Log(EVENTLOG_INFORMATION_TYPE, facility, code); }
	inline void Info(DWORD facility, DWORD code, const char* str1)
			{ Log(EVENTLOG_INFORMATION_TYPE, facility, code, str1); }
	inline void Info(DWORD facility, DWORD code, const char* str1, const char* str2)
			{ Log(EVENTLOG_INFORMATION_TYPE, facility, code, str1, str2); }
};

#endif // !defined(AFX_EVENTLOG_H__FBAEEAEB_705F_11D3_AE60_00A0C9C71BBC__INCLUDED_)
