// MonitorDBForSigChanges.cpp: implementation of the MonitorDBForSigChanges class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#include <string>

#include <Globals.h>

#include "MonitorDBForSigChanges.h"
#include "ScanServiceGlobals.h"
#include "ScanService.h"
#include "NAVEngine.h"

//#include <Logger.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


uint MonitorDBForSigChanges::ThreadHandlerProc()
{
	if (ScanService::ScanExplicit == ScanService::GetServiceType())
		return 0;

	uint		newSeqNum;
	ushort		pauseCount	= 1;	// only look when this hits zero
	bool		blessed		= ScanService::ScanBlessed == ScanService::GetServiceType();
	bool		rc;
	Signature	sig;

std::cerr << "blessed = " << blessed << "\n";

	while (!ScanServiceGlobals::stopService)
	{
		Sleep(1000);
		if (0 >= --pauseCount)
		{
			try
			{
				if (blessed)
					rc = Globals::NewestBlessedSignature(sig);
				else
					rc = Globals::NewestSignature(sig);
			}
			catch (AVISDBException& dbExp)
			{
				string	msg("MonitorDBForSigChanges: Attempt to get newest(blessed?)signature threw an exception [");
				msg	+= dbExp.FullString();
				msg	+= "]";

//				Logger::Log(Logger::LogError, Logger::AVISdb, msg.c_str());
				cerr << msg << endl;

				continue;
			}
			catch (...)
			{
				cerr << "MonitorDBForSigChanges: Exception of unknown type thrown while attempting to get newest(blessed?) signature" << endl;
			}

			try
			{
				if (rc && !sig.IsNull())
				{
					newSeqNum = sig.SequenceNumber();

					if (sigSeqNum != newSeqNum)
					{
						ScanService::Current().ChangeDefs(newSeqNum);
						sigSeqNum	= NAVEngine::SeqNum();
						pauseCount	= 60;				// wait 60 seconds and look again
					}
					else
						pauseCount	= 5;				// wait 5 seconds and look again
				}
				else
					pauseCount	= 10;					// wait 10 seconds and try again

			}
			catch (AVISDBException& dbExp)
			{
				string	msg("MonitorDBForSigChanges: Attempt to get newest(blessed?)signature threw an exception [");
				msg	+= dbExp.FullString();
				msg	+= "]";

//				Logger::Log(Logger::LogError, Logger::AVISdb, msg.c_str());
				cerr << msg;

				continue;
			}
			catch (...)
			{
				cerr << "MonitorDBForSigChanges: Exception of unknown type thrown while trying to change sequence number" << endl;
			}
		}
	}

	return 0;
}
