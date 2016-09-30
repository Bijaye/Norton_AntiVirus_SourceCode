// ScanService.cpp: implementation of the ScanService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>


#include <bldinfo.h>

#include "Scan.h"
#include "ScanService.h"
#include "NAVEngine.h"
#include "MonitorDBForSigChanges.h"
#include "ScanServiceGlobals.h"
#include "ScanException.h"

#include <WinRegistry.h>
#include <Logger.h>

#include <log\cfgprof.h>


using namespace std;

int ScanService::GetValue(const char* key, const int minV, const int maxV, const int defaultV)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::GetValue");
	WinRegistry	root(HKEY_LOCAL_MACHINE, NAVEngine::registryRoot);

	char		asChar[20];
	int			value	= defaultV;
	string		msg(key);
		
	if (root.ValidKey())
	{
		ulong	size	= 20;

		if (root.QueryValue(key, (uchar *) asChar, size))
		{
			sscanf(asChar, "%d", &value);

			msg	+= " found in registry (";
		}
		else
			msg += " not found in registry, default used (";
	}
	else
	{
		try
		{
			ConfigProfile	profile("AVISscan.prf");

			if (profile.validObject())
			{
				string	dct = profile[key];
				value	= atoi(dct.c_str());

				msg += " found in profile file (";
			}
			else
				msg += " not found in profile file, default used (";
		}
		catch (...)
		{
			value = defaultV;

			msg += " ConfigProfile threw an exception, default used (";
		}
	}

	if (value < minV)
		value = minV;
	else if (value > maxV)
		value = maxV;

	sprintf(asChar, "%d", value);
	msg += asChar;
	msg += " )";
	Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());

	return value;
}


SECURITY_ATTRIBUTES	ScanService::defaultInheritiable = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
ScanService::ServiceType	ScanService::serviceType	= ScanService::ServiceType::ScanLatest;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static ScanService*	currentScanService	= NULL;

const char*	ScanService::defChngTimeoutTag	= "definitionChangeTimeout";
const char* ScanService::maxThreadsTag		= "definitionConcurrencyMaximum";
const char* ScanService::minThreadsTag		= "definitionConcurrencyMinimum";

const uint	ScanService::maxThreads			= GetMaxThreadCount();
const uint	ScanService::minThreads			= GetMinThreadCount();
const uint	ScanService::defChangeTimeout	= GetDefChangeTimeout();

ScanService::ScanService() :
#ifdef LOG_TIME
//							 scanMonitor(monitorInterval),
#endif
							 threadPool(minThreads, maxThreads, threadTimeOut),
							 pipeName("\\\\.\\pipe\\ScanLatest"),
							 service(true),
							 usingDefs(maxThreads, maxThreads, NULL, &defaultInheritiable),
							 serviceClients(TRUE, TRUE, NULL, &defaultInheritiable),
							 changingDefs(TRUE, TRUE, NULL, &defaultInheritiable)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::ScanService");

	if (NULL != currentScanService)
		throw ScanException(ScanException::OnlyOneScanService, "Only one ScanService object is allowed");

	currentScanService = this;

	char	buffer[1024];

	sprintf(buffer, "%s version info, AVIS version = %s, build = %s, built on %s at %s",
							ShortName(), AVIS_VERSION, AVIS_BUILD, __DATE__, __TIME__);
	Logger::Log(Logger::LogInfo, Logger::LogEverything, buffer);

}

ScanService::~ScanService()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::~ScanService");

	if (NULL == currentScanService)
		throw ScanException(ScanException::ScanServiceDestructor,
							"Trying to destroy a non-existant ScanService.  Should not be possiable");

	currentScanService = NULL;
}

void	ScanService::Stop()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::Stop");

	ScanServiceGlobals::stopService	= true;
}

ScanService& ScanService::Current(void)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::Current");

	if (NULL == currentScanService)
		throw ScanException(ScanException::CurrentNull,
								"Asked for the current ScanService when non has been created yet");

	return *currentScanService;
}

DWORD ScanService::GetDefs(BOOL waitAll, DWORD milliseconds)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::GetDefs");

	if (NULL == currentScanService)
		throw ScanException(ScanException::CurrentNull,
								"Trying to use GetDefs before the ScanService is created");

	return currentScanService->usingDefs.WaitForTwo(currentScanService->changingDefs,
													waitAll, milliseconds);
}

DWORD ScanService::ReleaseDefs()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::ReleaseDefs");

	if (NULL == currentScanService)
		throw ScanException(ScanException::CurrentNull,
								"Trying to ReleaseDefs before the ScanService is created");

	return currentScanService->usingDefs.Release(1);
}

bool ScanService::InitFromProfiles()
{
	return true;
}

void ScanService::CreateSecurityDesc(SECURITY_DESCRIPTOR& securityDesc)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::CreateSecurityDesc");

	static SID_IDENTIFIER_AUTHORITY	NtAuthority = SECURITY_NT_AUTHORITY;
	static SID_IDENTIFIER_AUTHORITY LocalAuthority = SECURITY_LOCAL_SID_AUTHORITY;
	static PSID	systemSid	= NULL;

	if (InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION))
	{
		//
		//	We now have the basic struture ready.
		//	Add an owner of named pipe to the descriptor
		if (!systemSid)
		{
			if (!AllocateAndInitializeSid(&NtAuthority, 1, SECURITY_LOCAL_SYSTEM_RID,
									0, 0, 0, 0, 0, 0, 0, &systemSid))
				systemSid = NULL;
		}
		if (systemSid && SetSecurityDescriptorOwner(&securityDesc, systemSid, FALSE))
		{
			//
			//	Set the primary group information (LocalSystem)
			if (SetSecurityDescriptorGroup(&securityDesc, systemSid, FALSE))
			{
				//
				// Set a NULL Dacl, which is supposed to indicate all access granted
				// as opposed to an empty ACL which is no access:
				if (SetSecurityDescriptorDacl(&securityDesc, TRUE, NULL, TRUE))
					return;
			}
		}
	}
	
	throw ScanException(ScanException::CreateSecurityDesc,
							"Failed trying to create a security descriptor", GetLastError());
}

bool ScanService::Initialize(SERVICE_STATUS_HANDLE sHandle, SERVICE_STATUS& serviceStatus)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::Initialize");

	NAVEngine::Status	navStatus = NAVEngine::GetStatus();
	if (!(NAVEngine::Okay == navStatus || NAVEngine::NotInitialized == navStatus))
	{
		std::string	msg("NAVEngine failed to initialize properly, rc = ");
		msg	+= NAVEngine::StatusToChar();
		Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, msg.c_str());
		return false;
	}

	try
	{
		if (service)
		{
			serviceStatus.dwCheckPoint++;
		
			SetServiceStatus(sHandle, &serviceStatus);
		}

		serviceType = ServiceType::ScanLatest;

		TypeSpecificInitialization();

		InitFromProfiles();

		SECURITY_ATTRIBUTES	secAttr;
		SECURITY_DESCRIPTOR	securityDesc;
		CreateSecurityDesc(securityDesc);
		secAttr.nLength = sizeof(secAttr);
		secAttr.lpSecurityDescriptor	= &securityDesc;

		if (service)
		{
			serviceStatus.dwCheckPoint++;
			SetServiceStatus(sHandle, &serviceStatus);
		}

	// Create pipes and put them in listen mode
		for (int i = 0; i < maxThreads; i++)
		{
		// create overlapped mode named pipe instances
			pipeArray[i].SetHandle(CreateNamedPipe(pipeName.c_str(),
												PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
												PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
												maxThreads,
												maxMessageSize, maxMessageSize,
												maxGetPipeWait, NULL /*&secAttr*/));
			if (INVALID_HANDLE_VALUE == (HANDLE) pipeArray[i])
				throw ScanException(ScanException::CreateNamedPipe,
										"SetHandle/CreateNamedPipe failed", GetLastError());

		// Create an event object to associate with each named pipe.
		// These objects remain unsignaled until a client does something
		// like connect.
			pipeEvents[i].SetHandle(CreateEvent(NULL, TRUE, FALSE, NULL));
			if (INVALID_HANDLE_VALUE == (HANDLE) pipeEvents[i])
				throw ScanException(ScanException::CreateEvent,
										"SetHandle/CreateEvent failed", GetLastError());

		//
		// Put each instance of pipe in listening mode to receive
		// requests from clients.  As named pipe is in overlapped mode,
		// ConnectNamedPipe() will return rightaway setting the event
		// in non-signalled state.

//			memset(&oLapArray[i], 0, sizeof(OVERLAPPED));
			oLapArray[i].Internal		= 0;
			oLapArray[i].InternalHigh	= 0;
			oLapArray[i].Offset			= 0;
			oLapArray[i].OffsetHigh		= 0;
			oLapArray[i].hEvent= (HANDLE) pipeEvents[i];

			if (!ConnectNamedPipe((HANDLE) pipeArray[i], &oLapArray[i]))
			{
				DWORD	err = GetLastError();

				if (0 != err && ERROR_IO_PENDING != err)
					throw ScanException(ScanException::ConnectNamedPipe,
											"ConnectNamedPipe failed", err);
			}
		}

		if (service)
		{
			serviceStatus.dwCheckPoint++;
			SetServiceStatus(sHandle, &serviceStatus);
		}

		for (i = 0; i < maxThreads; i++)
			scanJobs[i].SetPipeData((HANDLE)pipeArray[i], &oLapArray[i]);
	}

	catch (ScanException& se)
	{
		ScanServiceGlobals::stopService = true;
		string	msg("Exception thrown in ScanService::Initialize, [");
		msg	+= se.FullString();
		msg	+= "]";

		Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, msg.c_str());

		std::cerr << msg << "\n";

		return false;
	}

	NAVEngine::SetStatus(NAVEngine::Okay);

	return true;
}

#pragma warning ( disable : 4101 )

bool ScanService::Run()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::Run");
	bool		rc = true;

	if (NAVEngine::Okay != NAVEngine::GetStatus())
		return false;

	try
	{
		MonitorDBForSigChanges	monitorDB(NAVEngine::SeqNum());
		CMclThread				monitorDBThread(&monitorDB);	// note: in explicit mode this thread exits immedately
//		WatchForControlMessages	monitorControl;					// waits on control pipe for
//		CMclThread				monitorControlPipe(&monitorControl);	// pause/continue/stop/ping
																	// or stats messages
#ifdef LOG_TIME
//		CMclThread				monitorScanThread(&scanMonitor);
#endif

		HANDLE	waitObjects[absMaximumNumberOfThreads];

		for (int i = 0; i < maxThreads; i++)
			waitObjects[i] = (HANDLE) pipeEvents[i];

		DWORD	rc;

		while (!ScanServiceGlobals::stopService)
		{
			rc = WaitForMultipleObjects(maxThreads, (const HANDLE*)&waitObjects, FALSE, 5000);
			if (0xFFFFFFFF == rc)	// an error
			{
				ScanServiceGlobals::stopService = true;
				rc = false;
				char	sysErr[512];
				if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
										0, sysErr, 512, NULL))
					strcpy(sysErr, "No explaination from system why");

				string	msg("WaitForMultipleObjects failed in ScanService::Run, system error message = [");
				msg	+= sysErr;
				msg	+= "]";
				Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, msg.c_str());
			}
			else if (WAIT_TIMEOUT != rc)		// client connected
			{
				if (0 == ResetEvent(waitObjects[rc]))	// set to non-signaled
				{
					ScanServiceGlobals::stopService = true;
					rc = false;
					Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner,
								"ResetEvent failed in ScanService::Run");
				}
				else
					threadPool.DispatchThread(&scanJobs[rc]);	// assign a thread to handle the job
			}
		}
	}

	catch (ScanException& se)
	{
		rc = false;
	}

	NAVEngine::Close();

	return rc;
}
#pragma warning ( default : 4101 )

bool ScanService::DeInitialize(SERVICE_STATUS_HANDLE sHandle, SERVICE_STATUS& sStatus)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::DeInitialize");
	return true;
}


NAVEngine::Status ScanService::ChangeDefs(uint newSigSeqNum)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanService::ChangeDefs");

	NAVEngine::Status	rc = NAVEngine::Status::Okay;

	static CMclCritSec	changDef;

	if (newSigSeqNum != NAVEngine::SeqNum())
	{
		try
		{
			int				semaphores = 0;
			DWORD			wfsoRC;
			CMclAutoLock	autoLock(changDef);

			ResetEvent((HANDLE) changingDefs);		// stop scanning threads from grabbing usingDefs semaphone
			for (; semaphores < maxThreads; semaphores++)// grab all of the usingDefs semaphore values, ensuring
			{										// that nobody is using the def file
				wfsoRC = WaitForSingleObject((HANDLE) usingDefs, defChangeTimeout);
				if (wfsoRC == WAIT_TIMEOUT)
				{
					Logger::Log(Logger::LogError, Logger::LogAvisScanner,
									"WaitForSingleObject(usingDefs, defChangeTimeout) timed out!!");
					break;
				}
			}
			
			if (semaphores == maxThreads)
			{
				try 
				{
					if (NAVEngine::Open(newSigSeqNum))				// change the def file
					{
						rc = NAVEngine::Status::Okay;
					}
					else
					{
						rc = NAVEngine::GetStatus();
						Logger::Log(Logger::LogError, Logger::LogAvisScanner,
									"Unable to open signature");
					}
				}
				catch (...)
				{
				  // Terminate the process immediately if any otherwise-uncaught
				  // exceptions are thrown from NAVENgine::OpenNAVEngineInit.
				  Logger::Log(Logger::LogError, Logger::LogAvisScanner,
					      "NAVEngine::Open threw an exception");
				  Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
					      "exiting after catching NAVEngine::Open exception");
				  ScanServiceGlobals::stopService = true;
				  exit(1);
				}
			}
			else
			{
				Logger::Log(Logger::LogError, Logger::LogAvisScanner,
								"Unable to change definition set because active scan did not finish in time");
				rc = NAVEngine::Status::CouldntChangeDef;
			}
			
			if (semaphores > 0 &&
				0 == ReleaseSemaphore((HANDLE) usingDefs, semaphores, NULL))	// release them all
			{
				Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner,
								"Unable to release semaphore in ScanService::ChangeDefs.");

				rc = NAVEngine::Status::SystemError;
				ScanServiceGlobals::stopService = true;
			}
			if (0 == SetEvent((HANDLE) changingDefs))
			{
				Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner,
								"Unable to set event changingDefs in ScanService::ChangeDefs.");
				rc = NAVEngine::Status::SystemError;
				ScanServiceGlobals::stopService = true;
			}
		}
		catch (...)
		{
			Logger::Log(Logger::LogError, Logger::LogAvisScanner,
						"caught outer exception while change defs");
			rc = NAVEngine::Status::UnknownError;
		}
	}

	return rc;
}


