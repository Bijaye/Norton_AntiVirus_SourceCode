// VQuarantineServer.cpp: implementation of the VQuarantineServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>

#include "VQuarantineServer.h"
#include <CMclCritSec.h>
#include <CMclAutoLock.h>

#include "Logger.h"
#include "Attention.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



VQuarantineServer::VQuarantineServer() : qs(NULL)
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQuarantineServer::VQuarantineServer");

	HRESULT	hr = CoCreateInstance(__uuidof(QuarantineServer),
									NULL, CLSCTX_LOCAL_SERVER,
									__uuidof(IQuarantineServer),
									reinterpret_cast<void **>(&qs));

	if (FAILED(hr))
	{
		// set the "quarantine" bit in the attention registry value
		Attention::Set(Attention::Quarantine);

		throw IcePackException(IcePackException::VQuarantineServer, 
						SampleBase::GenErrorMsg(hr, "VQuarantineServer::VQuarantineServer, CoCreateInstance",
						0));
	}

	// clear the "quarantine" bit in the attention registry value
	Attention::UnSet(Attention::Quarantine);
}

VQuarantineServer::~VQuarantineServer()
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQuarantineServer::~VQuarantineServer");

	if (qs)
	{
		HRESULT	hr = qs->Release();
		if (FAILED(hr))
		{
			Logger::Log(Logger::LogError, Logger::LogIcePackSample,
							"~VQuarantineServer, qs->Release() failed!");
		}
	}
}

bool VQuarantineServer::GetVQSEnum(VQSEnum& qsEnum)
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQuarantineServer::GetVQSEnum");

	CMclAutoLock				lock(inUse);
	IEnumQuarantineServerItems	*items;

	HRESULT hr = qs->EnumItems(&items);

	if (FAILED(hr))
	{
		throw IcePackException(IcePackException::VQuarantineServer,
									SampleBase::GenErrorMsg(hr, "VQuarantineServer::GetVQSEnum, qs->Release()",
									0));
	}

	qsEnum.Set(items);	// qsEnum is now responsiable for the items object

	return true;
}



bool VQuarantineServer::GetErrorMessage(std::string& errorMessage)
{
	return false;
}


