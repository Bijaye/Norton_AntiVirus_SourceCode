// ScanException.h: interface for the ScanException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANEXCEPTION_H__E3B0CEA7_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANEXCEPTION_H__E3B0CEA7_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <NAVScan.h>
#include <string.h>

#include <AVISException.h>

class ScanException : public AVISException
{
public:
	enum seType { CreateSecurityDesc, LatestBlessed, LoadBlessedSig,
					LoadLatestSig, Latest, CreateNamedPipe, CreateEvent,
					ConnectNamedPipe, NavEngineConstructor,
					ioDelete, OnlyOneScanService,
					ScanServiceDestructor, CurrentNull,
					ioGetDateTime, ioSetDateTime,
					RepairFileReadToCopy,
					ioWrite, ioRead, ioSeek, ioOpen, ioClose,
					RepairFile_OpenFileMapping,
					RepairFile_MapViewOfFile,
					NAVScanError, WaitToScan_DefLoad, WaitToScan_TimedOut,
					NAVGetVirusInfo, NAVIsInfectable, NAVFreeVirusHandle
	};


	ScanException(seType t, NAVScan::ScanRC r, const char* message);
	ScanException(seType t, const char* message);
	ScanException(seType t, const char* message, DWORD lastErrorCode);

	virtual ~ScanException()		{};

	std::string	ClassAsString(void)		{ return std::string("ScanException"); }
	std::string DetailedInfo(void)		{ return detailedInfo; }
	std::string	TypeAsString(void)		{ return std::string(TypeAsChar(type)); }

	seType		Type(void)			{ return type; }


	static const char* TypeAsChar(seType type);
	static const char*	NAVErrorToChar(uint type);

private:
	seType			type;
	std::string		detailedInfo;
};

#endif // !defined(AFX_SCANEXCEPTION_H__E3B0CEA7_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
