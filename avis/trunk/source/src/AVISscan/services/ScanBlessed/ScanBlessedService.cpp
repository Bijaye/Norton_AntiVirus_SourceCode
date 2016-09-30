// ScanBlessedService.cpp: implementation of the ScanBlessedService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include "ScanBlessedService.h"
#include "ScanException.h"
#include "Signature.h"


TCHAR*	ScanService::shortName	= "ScanBlessed";
TCHAR*	ScanService::displayName= "Scan with blessed signatures";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScanBlessedService::ScanBlessedService() : ScanService()
{

}

ScanBlessedService::~ScanBlessedService()
{

}

void ScanBlessedService::TypeSpecificInitialization()
{
	serviceType = ScanService::ScanBlessed;
	pipeName	= "\\\\.\\pipe\\ScanBlessed";

	Signature	blessed;

	Signature::LatestBlessed(blessed);
	if (!blessed.IsNull())
	{
		bool	changed = false;
		DWORD	rc;

		for (int i = 0; i < 5 && false == changed; i++)
		{
			if (NAVEngine::Okay == (rc = ChangeDefs(blessed.SequenceNumber())))
				changed = true;
			else
				Sleep(1000);
		}

		if (false == changed)
		{
			std::string		msg("Unable to change defs, NAV error = ");
			msg	+= ScanException::NAVErrorToChar(rc);

			throw ScanException(ScanException::LoadBlessedSig, msg.c_str());
		}
	}
	else
		throw ScanException(ScanException::LoadBlessedSig, "blessed.IsNull()");
}
