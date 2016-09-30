// NAVScan.cpp: implementation of the NAVScan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVScan.h"

#include <time.h>

#include <Logger.h>
#include <CMclEvent.h>
#include <CMclAutoLock.h>

#include <WinRegistry.h>

#include <log\cfgprof.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
NAVScan::NAVScan()
{

}

NAVScan::~NAVScan()
{

}
*/

using namespace std;

string	NAVScan::blessedPipe	= "";
string	NAVScan::latestPipe		= "";
string	NAVScan::explicitPipe	= "";
bool	NAVScan::initialized	= NAVScan::Initialize();
uint	NAVScan::messageID		= 0;

uint	NAVScan::maxWaitForScannerResponse	= maxWaitDefault*1000;
uint	NAVScan::scanRetryInterval			= retryIntDefault*1000;
uint	NAVScan::scanRetryLimit				= SetValuesFromRegistry();

bool		NAVScan::Initialize()
{
	blessedPipe = "\\\\.\\pipe\\ScanBlessed";
	latestPipe	= "\\\\.\\pipe\\ScanLatest";
	explicitPipe= "\\\\.\\pipe\\ScanExplicit";

	return true;
}

//
//	This function replaces the WinAPI CallNamedPipe function and adds the
//	ability to abort the request if too much time passes.

enum RCodes {
	okay, unableToOpenPipe, unableToSetPipeHandleState, transactFailure, timedOut,
	waitError, overlapError, closeFailure, unknownError
};

const char*	RCodesToChar(RCodes code)
{
	switch (code)
	{
	case unableToOpenPipe:	return "unable to open pipe";
	case unableToSetPipeHandleState: return "unable to change the pipe's handle state";
	case transactFailure:	return "unable to open transaction with service";
	case waitError:			return "error waiting for io event to signal";
	case overlapError:		return "overlap error";
	case closeFailure:		return "operation succeeded, but failed on pipe closure";
	case unknownError:		return "unknown error";
	default:				return "really unknown error";
	}
}

void LogErrorMsg(const char* prefix)
{
	DWORD	lastError(GetLastError());
	string	msg(prefix);
	msg	+= " [ ";
	char	buffer[1024];
	if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
							0, buffer, 1024, NULL))
		sprintf(buffer, "unknown error %d", lastError);
	msg += buffer;
	msg += " ]";

	Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
}

static bool TestServiceReady(const char* serviceName)
{
	bool	rc = false;

	DWORD	fileAttributes = -1;
	string	pipeName("\\\\.\\pipe\\");
	pipeName += serviceName;
	for (int i = 0; i < 20 && -1 == fileAttributes; i++)
	{
		Sleep(1000);
		fileAttributes = GetFileAttributes(pipeName.c_str());
	}
	if (-1 != fileAttributes)
		rc = true;

	return rc;
}
static bool StartAsApp(const char* serviceName)
{
	bool		rc = false;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset (&si, 0, sizeof (STARTUPINFO));
    si.cb = sizeof (STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;

    si.wShowWindow = SW_SHOWMINNOACTIVE;

    si.lpTitle = (char *) serviceName;
    memset (&pi, 0, sizeof (PROCESS_INFORMATION));
	string	msg("Starting ");
	msg	+= serviceName;
	msg += " as an application";
	Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
	string	cmndLine(serviceName);
	cmndLine += ".exe -run";
    if (CreateProcess (NULL,
        (char *) cmndLine.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
        NULL,
        NULL,
        &si,
        &pi) )
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

		rc = TestServiceReady(serviceName);
	}
	else
	{
		string	errMsg("Unable to start ");
		errMsg	+= serviceName;
		errMsg	+= ".exe as an application";
		LogErrorMsg(errMsg.c_str());
	}

	return rc;
}
bool StartScanService(const char* serviceName)
{
static	CMclCritSec	lock;

	CMclAutoLock	autoLock(lock);

	SC_HANDLE	scmHandle = OpenSCManager(NULL, NULL, GENERIC_READ);
	DWORD		lastError;
	bool		rc = false;

	if (NULL != scmHandle)
	{
		SC_HANDLE	serviceHandle = OpenService(scmHandle, serviceName,
												SERVICE_QUERY_STATUS | SERVICE_START);
		if (NULL != serviceHandle)
		{
			SERVICE_STATUS	sStatus;

			if (QueryServiceStatus(serviceHandle, &sStatus))
			{
				if (SERVICE_STOPPED == sStatus.dwCurrentState)
				{
					if (StartService(serviceHandle, 0, NULL))
					{
						rc = TestServiceReady(serviceName);
					}
					else
						LogErrorMsg("StartScanService, Restart Scan service ");
				}
				else if (SERVICE_PAUSED == sStatus.dwCurrentState)
				{
					SERVICE_STATUS	serviceStatus;
					if (ControlService(serviceHandle, SERVICE_CONTROL_CONTINUE, &serviceStatus))
					{
						if (SERVICE_RUNNING == serviceStatus.dwCurrentState)
							rc = true;
					}
					else
						LogErrorMsg("StartScanService, Unpause scan service ");
				}
				else
					LogErrorMsg("StartScanService, Querying scanning service status");
			}
			else
				LogErrorMsg("StartScanService, get service status");

			CloseServiceHandle(serviceHandle);
		}
		else	// service may not exist, launch as standard app
		{
			lastError = GetLastError();
			if (ERROR_INVALID_NAME == lastError ||			// not run as a service, launch as app
				ERROR_SERVICE_DOES_NOT_EXIST == lastError)
			{
				rc = StartAsApp(serviceName);
			}
			else
				LogErrorMsg("StartScanService, get service handle");
		}

		CloseServiceHandle(scmHandle);
	}
	else
		LogErrorMsg("StartScanService, get service control manager handle");

	return rc;
}

RCodes  ATKCallNamedPipe(const char* pipeName,
					   const void* writeBuffer, const ulong writeBuffSize,
					   char* readBuffer, const ulong readBuffSize, DWORD& bytesRead,
					   uint maxWaitTime)
{
	RCodes	rc = unknownError;
	HANDLE	pipeHandle = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
									OPEN_EXISTING,
									FILE_FLAG_OVERLAPPED,
//									FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
									NULL);

	DWORD	err = GetLastError();

	if (INVALID_HANDLE_VALUE == pipeHandle)
	{
		if (ERROR_PIPE_BUSY != err)
		{
			char*	serviceName = strrchr(pipeName, '\\');
			++serviceName;

			if (StartScanService(serviceName))
			{
				pipeHandle = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
									OPEN_EXISTING,
									FILE_FLAG_OVERLAPPED,
									NULL);
			}
		}
	}

	if (INVALID_HANDLE_VALUE != pipeHandle)
	{
		DWORD	flags = PIPE_READMODE_MESSAGE;
		if (SetNamedPipeHandleState(pipeHandle, &flags, NULL, NULL))
		{
			CMclEvent	event(true);	// manual reset event in unsignaled state
			OVERLAPPED	overlapped;
			memset(&overlapped, 0, sizeof(OVERLAPPED));
			overlapped.hEvent = event;

			if (TransactNamedPipe(pipeHandle, (void *) writeBuffer, writeBuffSize,
									readBuffer, readBuffSize, &bytesRead, &overlapped))
			{
				rc = okay;
			}
			else
			{
				DWORD	lastError = GetLastError();
				if (ERROR_IO_PENDING == lastError)
				{
					DWORD	eventRC = event.Wait(maxWaitTime);
					if (WAIT_OBJECT_0 == eventRC)
					{
						if (GetOverlappedResult(pipeHandle, &overlapped, &bytesRead, FALSE))
						{
							rc			= okay;
						}
						else
						{
							rc			= overlapError;
							bytesRead	= GetLastError();
						}
					}
					else if (WAIT_TIMEOUT == eventRC)
					{
						rc	= timedOut;
					}
					else
					{
						bytesRead	= GetLastError();
						rc			= waitError;
					}
				}
				else
					bytesRead	= lastError;
			}

			if (0 == CloseHandle(pipeHandle))
			{
				rc			= closeFailure;
				bytesRead	= GetLastError();
			}
		}
		else
		{
			rc = unableToSetPipeHandleState;
			bytesRead	= GetLastError();
		}
	}
	else
	{
		rc = unableToOpenPipe;

		bytesRead = err;
	}

	return rc;
}

bool NAVScan::SendMessageToScanService(string pipeName, string& message,
					   char* readBuffer, const ulong readBuffSize, DWORD& bytesReturned,
					   const char* whereFrom)
{
	bool	rc = false;

	for (int i = 0; i < scanRetryLimit; i++)	// try up to scanRetryLimit times to connect
	{
		RCodes	callRC = ATKCallNamedPipe(pipeName.c_str(), (void*)message.c_str(), message.size()+1,
											readBuffer, readBuffSize, bytesReturned,
											maxWaitForScannerResponse);
		if (okay != callRC)
		{
			// if the server is busy, we will simply call again
			if (unableToOpenPipe == callRC && ERROR_PIPE_BUSY == bytesReturned)
			{
				Logger::Log(Logger::LogWarning, Logger::LogAvisScanner, "scan service busy, waiting before trying again");
				Sleep(scanRetryInterval);
				continue;
			}
			else if (timedOut == callRC)
			{
				char	buffer[1024];

				sprintf(buffer, "Scanner service unable to respond in %d seconds for %s request",
									maxWaitForScannerResponse/1000, whereFrom);
				Logger::Log(Logger::LogError, Logger::LogAvisScanner, buffer);
				break;
			}
			else
			{
				DWORD	lastError = bytesReturned;
				string	msg("attempt to connect to scan service (");
				msg	+= whereFrom;
				msg	+= ") failed (";

				switch (callRC)
				{
				case unableToOpenPipe:
					msg	+= "unable to open pipe)[";
					break;

				case transactFailure:
					msg	+= "unable to open transaction with service)[";
					break;

				case waitError:
					msg	+= "error waiting for io event to signal)[";
					break;

				case overlapError:
					msg	+= "overlap error)[";
					break;

				case closeFailure:
					msg	+= "operation succeeded, but failed on pipe closure)[";
					break;

				case unknownError:
					msg	+= "unknown error)[";
					break;

				default:
					msg += "really unknown error)[";
					break;
				}

				char	buffer[1024];
				if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
										0, buffer, 1024, NULL))
					sprintf(buffer, "unknown error %d", lastError);
				msg += buffer;
				Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());

				break;		// error!!
			}
		}
		else 
			return true;
	}

	if (!rc)
		bytesReturned = 0;

	return rc;
}

	//	fileName, localCookie, and updateDB are input variables
	//	signatureSeq is output only and specifies the sig used to scan
	//	virusName is output only and, if the virus is found, contains the name
	//				of the virus found.
NAVScan::ScanRC NAVScan::ScanBlessed(const string& fileName,
								const uint localCookie,
								const bool updateDB,
								string& virusName,
								uint& signatureSeq,
								string& navVersion,
								uint& virusID)
{
	return ScanFile(fileName, localCookie, updateDB, virusName, signatureSeq, navVersion,
					NAVScan::blessedPipe, virusID);
}

NAVScan::ScanRC NAVScan::ScanNewest(const string& fileName,
								const uint localCookie,
								const bool updateDB,
								string& virusName,
								uint& signatureSeq,
								string& navVersion,
								uint& virusID)
{
	return ScanFile(fileName, localCookie, updateDB, virusName, signatureSeq, navVersion,
					NAVScan::latestPipe, virusID);
}

	//	In this case the signatureSeq number is input only and specifies
	//	which signature to use when scanning this file.
NAVScan::ScanRC NAVScan::ScanExplicit(const string& fileName,
								const uint localCookie,
								const bool updateDB,
								string& virusName,
								const uint signatureSeq,
								string& navVersion,
								uint& virusID)
{
	uint sig = signatureSeq;

	return ScanFile(fileName, localCookie, updateDB, virusName, sig, navVersion,
					NAVScan::explicitPipe, virusID);
}

bool NAVScan::BlessedSeqNum(uint& sigSeqNum)
{
	return GetSeqNum(sigSeqNum, NAVScan::blessedPipe);
}

bool NAVScan::LatestSeqNum(uint& sigSeqNum)
{
	return GetSeqNum(sigSeqNum, NAVScan::latestPipe);
}

bool NAVScan::ExplicitSeqNum(uint& sigSeqNum)
{
	return GetSeqNum(sigSeqNum, NAVScan::explicitPipe);
}


bool NAVScan::GetSeqNum(uint& seqNum, string& pipeName)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "NAVScan::GetSeqNum");

	bool	rc = false;
	char	buffer[512];

	std::string	message("query, seqNum");

	DWORD	bytesReturned;
	if (SendMessageToScanService(pipeName, message, buffer, 512, bytesReturned, "GetSeqNum") &&
		bytesReturned > 0)
	{
		//	return message is of the form
		//	current seqNum = %u
		if (1 == sscanf(buffer, "current seqNum = %u", &seqNum))
		{
			rc = true;
		}
	}

	return rc;
}


bool NAVScan::BlessedSigPath(string& sigPath)
{
	return GetSigPath(sigPath, NAVScan::blessedPipe);
}

bool NAVScan::LatestSigPath(string& sigPath)
{
	return GetSigPath(sigPath, NAVScan::latestPipe);
}

bool NAVScan::ExplicitSigPath(string& sigPath)
{
	return GetSigPath(sigPath, NAVScan::explicitPipe);
}

bool NAVScan::GetSigPath(string& sigPath, std::string& pipeName)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "NAVScan::GetSigPath");

	bool	rc = false;
	char	buffer[512];

	std::string	message("query, sigPath");

	DWORD	bytesReturned;
	
	if (SendMessageToScanService(pipeName, message, buffer, 512, bytesReturned, "GetSigPath"))
	{
		const char*	prefix = "current sigPath = [";
		if (!strncmp(buffer, prefix, sizeof(prefix)))
		{
			string	tmp = buffer;
			int		start	= tmp.find("[");
			int		end		= tmp.find("]");
			sigPath = tmp.substr(start+1, end - start - 2);

			rc = true;
		}
	}

	return rc;
}

bool NAVScan::ExplicitDefVersion(string& defVersion, uint sigSeqNum)
{
	return GetDefVersion(defVersion, sigSeqNum, NAVScan::explicitPipe);
}

bool NAVScan::GetDefVersion(string& defVersion, uint sigSeqNum, std::string& pipeName)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "NAVScan::GetDefVersion");

	bool	rc = false;
	char	buffer[512];

	sprintf(buffer, "query, defVersion, sigSeqNum = %d", sigSeqNum);
	std::string	message(buffer);

	DWORD	bytesReturned;
	if (SendMessageToScanService(pipeName, message, buffer, 512, bytesReturned, "GetDefVersion"))
	{
		const char*	prefix = "current defVersion = [";
		if (!strncmp(buffer, prefix, sizeof(prefix)))
		{
			string	tmp = buffer;
			int		start	= tmp.find("[");
			int		end		= tmp.find("]");
			defVersion = tmp.substr(start+1, end - start - 2);

			rc = true;
		}		// else assume that it's "Invalid sequence number"
	}

	return rc;
}


NAVScan::ScanRC NAVScan::ScanFile(const string& fileName, const uint localCookie,
							 const bool updateDB, string& virusName,
							 uint& signatureSeq, string& navVersion,
							 string& pipeName, uint& virusID)
{
	EntryExit	entryExit(Logger::LogAvisScanner, "NAVScan::ScanFile");

	if (Logger::WillLog(Logger::LogInfo, Logger::LogAvisScanner))
	{
		string	msg;
		if (blessedPipe == pipeName)
			msg	= "ScanBlessed ";
		else if (latestPipe == pipeName)
			msg = "ScanLatest ";
		else
			msg = "ScanExplicit ";
		msg	+= "fileName = \"";
		msg	+= fileName;
		msg	+= "\"";
		if (explicitPipe == pipeName)
		{
			char	tmp[32];
			sprintf(tmp, ", signatureSeq = %d", signatureSeq);
			msg	+= tmp;
		}
		Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
	}

	NAVScan::ScanRC	rc = WaitForScanTimedOut;

	char	buffer[512];
	uint	id	= ++messageID;

	sprintf(buffer, "scan, %d, %d, %d, %c, %d, ", id, id, localCookie,
												(updateDB ? 'T' : 'F'), signatureSeq);
	std::string	message(buffer);
	message += fileName;

	DWORD	bytesReturned;

#ifdef LOG_TIME
	clock_t	tmpClock		= clock();
#endif

	if (SendMessageToScanService(pipeName, message, buffer, 512, bytesReturned, "ScanFile"))
	{
#ifdef LOG_TIME
		float	elapsedTime = ((float)(clock() - tmpClock))/((float) CLOCKS_PER_SEC);
		if (elapsedTime > 16.0)
		{
			string	msg("scan request took ");
			char	tmpBuffer[128];
			sprintf(tmpBuffer, "%f", elapsedTime);
			msg	+= tmpBuffer;
			msg += " seconds to scan ";
			msg += fileName;
			msg += " with signature";
			sprintf(tmpBuffer, "%d", signatureSeq);
			msg += tmpBuffer;

			Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
		}
#endif

		rc = BadResponceFromScanner;

		//	return message is of the form
		//	messageID, returnID, sigSeqNum, returnState [, virusName]
		string	tmp;
		message = buffer;
		int		begining	= 0;
		int		end			= message.find(",", 0);
		if (std::string::npos != end)
		{
			if (id == atoi(message.c_str()))
			{
				begining = message.find(",", end+1);	// skip redundant return id
				begining = message.find_first_not_of(" ", begining+1);
				end		 = message.find(",", begining+1);
				tmp		 = message.substr(begining, end);
				signatureSeq = atoi(tmp.c_str());

				begining = message.find_first_not_of(" ", end+1);	// get the result
				end		 = message.find(",", begining);
				tmp		 = message.substr(begining, end - begining);
				rc	= static_cast<ScanRC>(atoi(tmp.c_str()));

				begining = message.find_first_not_of(" ", end+1);
				end		 = message.find(",", begining);
				navVersion=message.substr(begining, end - begining);

				switch (rc)
				{
				case Infected:
				case NoRepairInstructions:
				case RepairTriedAndError:
				case RepairTriedAndFailed:
				case Repaired:
					begining = message.find_first_not_of(" ", end+1);
					end		 = message.find(",", begining);
					tmp		 = message.substr(begining, end - begining);
					virusID	 = atoi(tmp.c_str());

					begining = message.find_first_not_of(" ", end+1);
					virusName= message.substr(begining, message.size());
					break;

				case NotInfectable:
				case BadScan:
				case NotInfected:
				case UnSubmittable:
				case Heuristic:
//				case InvalidFile:
//				case NavHung:
				case NavCrashed:
				case NavVcdInitError:
				case NavDefinitionFileError:
//				case NavUnknownError:
//				case NavUnknownScanError:
				case NavLoadDefFileError:
				case SigDefMissing:
				case WaitForScanTimedOut:
				case CommWithScannerOffline:
				case ScannerServiceError:
				case BadResponceFromScanner:
					break;

				default:
					rc = UnknownError;
				}
			}
		}
	}

	if (Logger::WillLog(Logger::LogInfo, Logger::LogAvisScanner))
	{
		char	tmp[64];
		string	msg("Returning from scan call, returning ");
		msg	+= RCtoChar(rc);
		msg	+= " [ virusName = \"";
		msg	+= virusName;
		sprintf(tmp, "\", signatureSeq = %d, navVersion = ", signatureSeq);
		msg	+= tmp;
		msg += navVersion;
		msg += " ]";

		Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
	}

	return rc;
}

inline int CheckLimits(uint value, uint min, uint max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	return value;
}

uint NAVScan::SetValuesFromRegistry()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "NAVScan::SetValuesFromRegistry");

	static const char*	maxWaitTag	= "scanCompletionTimeout";
	static const char*	retryIntTag	= "scanRetryInterval";
	static const char*	retryLimitTag="scanRetryLimit";

	maxWaitForScannerResponse	= maxWaitDefault*1000;
	scanRetryInterval			= retryIntDefault*1000;
	scanRetryLimit				= retryLimitDefault;

	WinRegistry	root(HKEY_LOCAL_MACHINE, "Software\\Symantec\\Quarantine\\Server\\Avis\\current");
	
	DWORD	value	= maxWaitDefault;
	char	asChar[20];
	ulong	size	= 20;

	if (root.ValidKey())
	{
		if (root.QueryValue(maxWaitTag, (uchar *) asChar, size))
		{
			sscanf(asChar, "%d", &value);
			maxWaitForScannerResponse = CheckLimits(value, maxWaitMin, maxWaitMax)*1000;
		}

		value	= retryIntDefault;
		size	= 20;

		if (root.QueryValue(retryIntTag, (uchar *) asChar, size))
		{
			sscanf(asChar, "%d", &value);
			scanRetryInterval = CheckLimits(value, retryIntMin, retryIntMax)*1000;
		}

		value	= retryLimitDefault;
		size	= 20;

		if (root.QueryValue(retryLimitTag, (uchar *) asChar, size))
		{
			sscanf(asChar, "%d", &value);
			scanRetryLimit = CheckLimits(value, retryLimitMin, retryLimitMax);
		}
	}
	else
	{
		try
		{
			ConfigProfile	profile("AVISscan.prf");

			if (profile.validObject())
			{
				try
				{
					string	tmp = profile[maxWaitTag];
					value	= atoi(tmp.c_str());

					value = CheckLimits(value, maxWaitMin, maxWaitMax);
					maxWaitForScannerResponse = value*1000;
				}
				catch (...)
				{
					maxWaitForScannerResponse = maxWaitDefault*1000;
				}
				try
				{
					string	tmp = profile[retryIntTag];
					value	= atoi(tmp.c_str());

					value = CheckLimits(value, retryIntMin, retryIntMax);
					scanRetryInterval = value*1000;
				}
				catch (...)
				{
					scanRetryInterval = retryIntDefault*1000;
				}
				try
				{
					string	tmp = profile[retryLimitTag];
					value	= atoi(tmp.c_str());

					value = CheckLimits(value, retryLimitMin, retryLimitMax);
					scanRetryLimit = value;
				}
				catch (...)
				{
					scanRetryLimit = retryLimitDefault*1000;
				}
			}
		}
		catch (...)
		{
			maxWaitForScannerResponse	= maxWaitDefault*1000;
			scanRetryInterval			= retryIntDefault*1000;
			scanRetryLimit				= retryLimitDefault;
		}
	}

	char	buffer[512];
	sprintf(buffer, "max wait for scanner = %d sec, retry interval = %d sec, retry limit = %d",
						maxWaitForScannerResponse/1000, scanRetryInterval/1000,
						scanRetryLimit);

	Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, buffer);

	return scanRetryLimit;
}
