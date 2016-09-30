// IcePackException.cpp: implementation of the IcePack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IcePackException.h"
#include <iostream>

#include "Logger.h"

using namespace std;

char *IcePackException::typeAsChar[] = {
		"SampleID",
		"SamplePriority",
		"SampleSigPriority",
		"SampleForwardTime",
		"SetSampleStatus",
		"SampleGatewayCookie",
		"SampleStatus",
		"SampleFinalStatus",
		"SampleInfected",
		"SampleSigSeqNum",
		"SampleNeedeSeqNum",
		"SampleCompare",
		"SampleSetID",
		"SampleParseValue",
		"SampleScan",
		"SampleChanges",
		"ScannerSubmittor_ScanPendingSamples",
		"QuarantineMonitorPause",
		"VQuarantineServer",
		"VQSEnum",
		"SampleBase",
		"SampleStream",
		"MappedMemCallback",
		"RegistryMonitor"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IcePackException::IcePackException(TypeOfException t, std::string& di) :
					AVISException(), type(t), detailedInfo(di)
{
	string	msg("IcePackException [");
	msg	+= TypeAsString();
	msg	+= "] thrown, detailed info to follow.\n";
	msg	+= di;

	Logger::Log(Logger::LogCriticalError, Logger::LogIcePackOther, msg.c_str());

	cerr << msg << endl;
}

IcePackException::IcePackException(TypeOfException t, const char* di) :
					AVISException(), type(t), detailedInfo(di)
{
	string	msg("IcePackException [");
	msg	+= TypeAsString();
	msg	+= "] thrown, detailed info to follow.\n";
	msg	+= di;

	Logger::Log(Logger::LogCriticalError, Logger::LogIcePackOther, msg.c_str());

	cerr << msg << endl;
}
