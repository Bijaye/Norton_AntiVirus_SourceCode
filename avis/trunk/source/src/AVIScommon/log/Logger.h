
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include <AVIS.h>

#include "AVISCommon.h"

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )

#include "mtrace.h"

class AVISCOMMON_API Logger
{
public:
	//
	//	Messages can be filter based on two values, severity and source.
	//	Severity filtering excludes all messages below the current logging level.
	//		NOTE: in this implementation Errors and Critical errors are always logged.
	//	Source is a mask, items in the mask are logged, items outside the mask are
	//		ignored.  Source masking only applies LogInfo and below.
	enum LogSeverity {	LogEntry, LogExit, LogDebug, LogInfo, LogWarning,
						LogError, LogCriticalError };
	enum LogSource {
					LogNone			= 0,

					LogAvisCommon	= 0x00000001,
					LogAvisDB		= 0x00000002,
					LogAvisFile		= 0x00000004,
					LogAvisScanner	= 0x00000008,
					LogAvisFilter	= 0x00000010,
					LogComm			= 0x00000020,


					LogIcePackSample			= 0x00100000,
					LogIcePackSubmission		= 0x00200000,
					LogIcePackQSMonitor			= 0x00400000,
					LogIcePackRegMonitor		= 0x00800000,
					LogIcePackStatusMonitor		= 0x01000000,
					LogIcePackBlessedMonitor	= 0x02000000,
					LogIcePackSigDownload		= 0x04000000,
					LogIcePackScan				= 0x08000000,
					LogIcePackDDS				= 0x10000000,
					LogIcePackDeliveryMonitor	= 0x20000000,
					LogIcePackOther				= 0x40000000,

					LogEverything	= 0xFFFFFFFF
					};

	static void	Log(LogSeverity severity, LogSource source, const char* message);
	static void	ApplicationName(const char*name);
	static void SeverityFilter(LogSeverity severity) { filterSeverity = severity; };
	static void SourceFilter(ulong m)			 { mask	= m; };
//	static void	LogFileName(const char* logName);
	static void SendTo(MTrace* mT) { mTrace = mT; };
//	static void CloseLog(void);

	static bool	WillLog(LogSeverity severity, LogSource source)
				{ return NULL != mTrace && 
							(severity > LogWarning ||
							(severity >= filterSeverity && (mask & source))); }

	static const char* SevAsChar(LogSeverity severity);
	static const char* SourceAsChar(LogSource source);

	static LogSeverity	SeverityFromChar(const char* severity);

private:
	static	LogSeverity	filterSeverity;
	static	ulong		mask;
	static	const char	*appName;
//	static	const char	*logFileName;
	static	MTrace*		mTrace;
};


class EntryExit
{
public:
	EntryExit(Logger::LogSource s, const char* mName) : source(s), methodName(mName)
	{ 
		Logger::Log(Logger::LogEntry, source, methodName);
	}

	~EntryExit()
	{
		Logger::Log(Logger::LogExit, source, methodName);
	}

private:
	Logger::LogSource	source;
	const char*			methodName;
};

#endif
