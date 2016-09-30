// ScanLatestService.cpp: implementation of the ScanLatestService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <string>

#include "ScanLatestService.h"
#include <ScanException.h>

#include <Signature.h>


TCHAR*	ScanService::shortName	= "ScanLatest";
TCHAR*	ScanService::displayName= "Scan with latest signatures";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScanLatestService::ScanLatestService() : ScanService()
{

}

ScanLatestService::~ScanLatestService()
{

}

void ScanLatestService::TypeSpecificInitialization()
{
	Signature	latest;
	Signature::Latest(latest);
	if (!latest.IsNull())
	{
		bool	changed = false;
		DWORD	rc;

		for (int i = 0; i < 5 && false == changed; i++)
		{
			if (NAVEngine::Okay == (rc = ChangeDefs(latest.SequenceNumber())))
				changed = true;
			else
				Sleep(1000);
		}

		if (false == changed)
		{
			std::string		msg("Unable to change defs, NAV error = ");
			msg	+= ScanException::NAVErrorToChar(rc);

			throw ScanException(ScanException::LoadLatestSig, msg.c_str());
		}
	}
	else
		throw ScanException(ScanException::Latest, "latest.IsNull()");
}
