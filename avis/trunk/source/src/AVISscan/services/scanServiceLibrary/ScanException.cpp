// ScanException.cpp: implementation of the ScanException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanException.h"
#include "NAVAPI.h"

#include "Logger.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



ScanException::ScanException(seType t, NAVScan::ScanRC r, const char* message) :
					AVISException(), type(NAVScanError), detailedInfo(NAVScan::RCtoChar(r))
{
	Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, this->FullString().c_str());
}

ScanException::ScanException(seType t, const char* message) :
					AVISException(), type(t), detailedInfo(message)
{
	Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, this->FullString().c_str());
}

ScanException::ScanException(seType t, const char* message, DWORD lastErrorCode) :
					AVISException(), type(t), detailedInfo(message)
{
	detailedInfo	+= ", last system error [";

	char	msg[512];

	if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
											lastErrorCode, 0, msg, 512, NULL))
		strcpy(msg, "No message from system");

	detailedInfo	+= msg;
	detailedInfo	+= "]";

	Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, this->FullString().c_str());
}


const char* ScanException::TypeAsChar(seType type)
{
	switch (type)
	{
	case CreateSecurityDesc:	return "CreateSecurityDesc";
	case LatestBlessed:			return "LatestBlessed";
	case LoadBlessedSig:		return "LoadBlessedSig";
	case LoadLatestSig:			return "LoadLatestsig";
	case Latest:				return "Latest";
	case CreateNamedPipe:		return "CreateNamedPipe";
	case CreateEvent:			return "CreateEvent";
	case ConnectNamedPipe:		return "ConnectNamedPipe";
	case NavEngineConstructor:	return "NavEngineConstructor";
	case ioDelete:				return "ioDelete";
	case OnlyOneScanService:	return "OnlyOneScanService";
	case ScanServiceDestructor:	return "ScanServiceDestructor";
	case CurrentNull:			return "CurrentNull";
	case ioGetDateTime:			return "ioGetDateTime";
	case ioSetDateTime:			return "ioSetDateTime";
	case RepairFileReadToCopy:	return "RepairFileReadToCopy";
	case ioWrite:				return "ioWrite";
	case ioRead:				return "ioRead";
	case ioSeek:				return "ioSeek";
	case ioClose:				return "ioClose";
	case ioOpen:				return "ioOpen";
	case RepairFile_OpenFileMapping:	return "RepairFile, OpenFileMapping";
	case RepairFile_MapViewOfFile:		return "RepairFile, MapViewOfFile";
	case NAVScanError:			return	"NAVScanError";
	case WaitToScan_DefLoad:	return	"WaitToScan, LoadDefinition file";
	case WaitToScan_TimedOut:	return	"WaitToScan, timed out";
	case NAVGetVirusInfo:		return	"NAVGetVirusInfo";
	case NAVIsInfectable:		return	"NAVIsInfectable";
	case NAVFreeVirusHandle:	return	"NAVFreeVirusHandle";

	default:					return	"Unknown type";
	}

	return "Unknown location";
}

const char* ScanException::NAVErrorToChar(uint rc)
{
	switch (rc)
	{
	case NAV_OK:					return "NAV okay";
	case NAV_ERROR:					return "NAV error";
	case NAV_INSUFFICIENT_BUFFER:	return "NAV insufficient buffer";
	case NAV_INVALID_ARG:			return "NAV invalid arg";
	case NAV_MEMORY_ERROR:			return "NAV memory error";
	case NAV_NO_ACCESS:				return "NAV no access";
	case NAV_CANT_REPAIR:			return "NAV cant repair";
	case NAV_CANT_DELETE:			return "NAV cant delete";
	case NAV_ENGINE_IN_USE:			return "NAV engine in use";
	case NAV_VXD_INIT_FAILURE:		return "NAV vxd init failure";
	case NAV_DEFINITIONS_ERROR:		return "NAV definitions error";

	default:						return "NAV unknown error code";
	}
}
