// WaitToScan.h: interface for the WaitToScan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAITTOSCAN_H__E3B0CEA8_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
#define AFX_WAITTOSCAN_H__E3B0CEA8_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CMclAutoLock.h>

#include "ScanException.h"
#include "ScanService.h"
#include "NAVEngine.h"

class WaitToScan  
{
public:
	WaitToScan(uint& sigSeqNum)
	{
		CMclAutoLock	autoLock(waitToScanCreate);

		if (ScanService::ScanExplicit == ScanService::GetServiceType())
		{
			NAVEngine::Status	rc	= ScanService::Current().ChangeDefs(sigSeqNum);
			if (NAVEngine::Status::Okay != rc)
			{
				std::string	msg("WaitToScan, NAVEngine::Status = ");
				msg	+= ScanException::NAVErrorToChar(rc);
				throw ScanException::ScanException(ScanException::WaitToScan_DefLoad,
													NAVScan::NavLoadDefFileError, msg.c_str());
			}
		}

		DWORD rc = ScanService::GetDefs(TRUE, 3000);
		if (WAIT_TIMEOUT == rc)
			throw ScanException::ScanException(ScanException::WaitToScan_TimedOut,
									NAVScan::WaitForScanTimedOut, "WaitToScan, wait for scan timed out");

		if (ScanService::ScanExplicit != ScanService::GetServiceType())
			sigSeqNum = NAVEngine::SeqNum();
	}
	virtual ~WaitToScan()
	{
		ScanService::ReleaseDefs();
	}

private:
	static CMclCritSec	waitToScanCreate;
};

#endif // !defined(AFX_WAITTOSCAN_H__E3B0CEA8_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
