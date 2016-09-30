
#include "stdafx.h"

#include "Logger.h"

#include <system\Handle.h>
#include <DateTime.h>


Logger::LogSeverity Logger::filterSeverity	= LogError;
ulong				Logger::mask			= 0L;
const char*			Logger::appName			= NULL;
//const char*			Logger::logFileName		= NULL;
MTrace*				Logger::mTrace			= NULL;

static Handle		fileHandle = INVALID_HANDLE_VALUE;



const char* Logger::SevAsChar(LogSeverity severity)
{
	switch (severity)
	{
	case LogEntry:			return "Entry";
	case LogExit:			return "Exit";
	case LogDebug:			return "Debug";
	case LogInfo:			return "Info";
	case LogWarning:		return "Warning";
	case LogError:			return "Error";
	case LogCriticalError:	return "Critical";
	}

	return "unknown";
}

const char* Logger::SourceAsChar(LogSource source)
{
	switch (source)
	{
	case LogNone:					return "None";

	case LogAvisCommon:				return "AVIScommon";
	case LogAvisDB:					return "AVISdb";
	case LogAvisFile:				return "AVISfile";
	case LogAvisScanner:			return "AVISscanner";
	case LogAvisFilter:				return "AVISfilter";


	case LogIcePackSample:			return "IPSample";
	case LogIcePackSubmission:		return "IPSubmission";
	case LogIcePackQSMonitor:		return "IPQSMonitor";
	case LogIcePackRegMonitor:		return "IPRegMonitor";
	case LogIcePackStatusMonitor:	return "IPStatusMonitor";
	case LogIcePackBlessedMonitor:	return "IPBlessedMonitor";
	case LogIcePackSigDownload:		return "IPSigDownload";
	case LogIcePackScan:			return "IPScan";
	case LogIcePackDDS:				return "IPDDS";
	case LogIcePackDeliveryMonitor:	return "IPDeliveryMonitor";
	case LogIcePackOther:			return "IPOther";
	}

	return "unknown";
}

Logger::LogSeverity	Logger::SeverityFromChar(const char* traceSeverity)
{
	LogSeverity	severity	= Logger::LogWarning;

	char	leadChar = traceSeverity[0];

	if ('D' == leadChar || 'd' == leadChar)
	{
		if (!stricmp("Debug", traceSeverity))
			severity	= Logger::LogDebug;
	}
	else if ('E' == leadChar || 'e' == leadChar)
	{
		if (!stricmp("EntryExit", traceSeverity))
			severity	= Logger::LogEntry;
		else if (!stricmp("Error", traceSeverity))
			severity	= Logger::LogError;
	}
	else if ('I' == leadChar || 'i' == leadChar)
	{
		if (!stricmp("Info", traceSeverity))
			severity	= Logger::LogInfo;
	}
	else if ('W' == leadChar || 'w' == leadChar)
	{
		if (!stricmp("Warning", traceSeverity))
			severity	= Logger::LogWarning;
	}

	return severity;
}

void Logger::ApplicationName(const char* name)
{
	if (NULL != appName)
		delete (char *) appName;

	appName	= strcpy(new char[strlen(name)+1], name);
}

/*
void Logger::LogFileName(const char* name)
{
	if (NULL != logFileName)
		delete (char *) logFileName;

	logFileName	= strcpy(new char[strlen(name)+1], name);

	fileHandle.SetHandle(INVALID_HANDLE_VALUE);
}
*/

//void Logger::CloseLog()
//{
//	fileHandle.SetHandle(INVALID_HANDLE_VALUE);
//}

static MTrace::line_type SeverityConverter(Logger::LogSeverity lSeverity)
{
	switch (lSeverity)
	{
	case Logger::LogEntry:		return MTrace::lt_enter;
	case Logger::LogExit:		return MTrace::lt_exit;
	case Logger::LogDebug:		return MTrace::lt_debug;
	case Logger::LogInfo:		return MTrace::lt_info;
	case Logger::LogWarning:	return MTrace::lt_warning;
	case Logger::LogError:		return MTrace::lt_error;
	case Logger::LogCriticalError: return MTrace::lt_critical;
	default:					return MTrace::lt_message;
	}
}

void Logger::Log(LogSeverity mSeverity, LogSource mSource, const char* message)
{

	if (WillLog(mSeverity, mSource))	// source is being logged at this severity
	{
		mTrace->msg(SourceAsChar(mSource), message, SeverityConverter(mSeverity), mSource);
	}

}
