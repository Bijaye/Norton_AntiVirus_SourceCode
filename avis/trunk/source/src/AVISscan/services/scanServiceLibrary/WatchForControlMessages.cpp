// WatchForControlMessages.cpp: implementation of the WatchForControlMessages class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>

#include "WatchForControlMessages.h"
#include "ScanService.h"
#include "ScanServiceGlobals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WatchForControlMessages::WatchForControlMessages() :
					controlPipe((HANDLE) 0xFFFFFFFF),
					overlapEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
{
	std::string	pipeName;

	switch (ScanService::GetServiceType())
	{
		case ScanService::ScanLatest:
			pipeName	= "\\\\.\\pipe\\ScanLatest-Control";
			break;

		case ScanService::ScanBlessed:
			pipeName	= "\\\\.\\pipe\\ScanBlessed-Control";
			break;

		case ScanService::ScanExplicit:
			pipeName	= "\\\\.\\pipe\\ScanExplicit-Control";
			break;
	}

	SECURITY_ATTRIBUTES	secAttr;
	SECURITY_DESCRIPTOR	securityDesc;
	ScanService::CreateSecurityDesc(securityDesc);
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor	= &securityDesc;

	controlPipe.SetHandle(CreateNamedPipe(pipeName.c_str(),
									PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
									PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
									1, ScanService::maxMessageSize,
									ScanService::maxMessageSize,
									ScanService::maxGetPipeWait, NULL));
	if (INVALID_HANDLE_VALUE == (HANDLE) controlPipe)
		throw new WatchForControlMessagesException(WatchForControlMessagesException::CreateNamedPipe);

	pipeEvents.SetHandle(CreateEvent(NULL, TRUE, FALSE, NULL));
	if (INVALID_HANDLE_VALUE == (HANDLE) pipeEvents)
		throw new WatchForControlMessagesException(WatchForControlMessagesException::CreateEvent);

	//
	// Put each instance of pipe in listening mode to receive
	// requests from clients.  As named pipe is in overlapped mode,
	// ConnectNamedPipe() will return rightaway setting the event
	// in non-signalled state.

	oLap.hEvent= (HANDLE) pipeEvents;
	if (!ConnectNamedPipe((HANDLE) controlPipe, &oLap) &&
		ERROR_IO_PENDING != GetLastError())
		throw new WatchForControlMessagesException(WatchForControlMessagesException::ConnectNamedPipe);

	if (!ResetEvent(overlapEvent))
		throw new WatchForControlMessagesException(WatchForControlMessagesException::ResetEvent);

	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent	= overlapEvent;
}

WatchForControlMessages::~WatchForControlMessages()
{

}

uint WatchForControlMessages::ThreadHandlerProc()
{
	DWORD	rc;

	while (!ScanServiceGlobals::stopService)
	{
		rc = WaitForSingleObject(controlPipe, 1000);
		if (0xFFFFFFFF == rc)	// an error
		{
			ScanServiceGlobals::stopService = true;
		}
		else if (WAIT_TIMEOUT != rc)		// client connected
		{
			ResetEvent(controlPipe);		// set to non-signaled

			HandleRequest();
		}
	}


	return true;
}




//	Handle the "stats" message
bool WatchForControlMessages::Stats(char* buffer, DWORD bytesRead)
{
	return SendResponse(std::string("No stats being collected at this time"));
}

//	Handle the "stop" message
bool WatchForControlMessages::Stop()
{
	if (ScanServiceGlobals::stopService)
	{
		return SendResponse(std::string("Already stopping scan service"));
	}
	else
	{
		ScanServiceGlobals::stopService	= true;
		return SendResponse(std::string("Stopping scan service"));
	}
}

//	Handle the "ping" message
bool WatchForControlMessages::Ping()
{
	return SendResponse(std::string("Ping"));
}

//	Handle the "pause" message
bool WatchForControlMessages::Pause()
{
	ScanService::Pause();
	return SendResponse(std::string("Pausing"));
}

//	Handle the "continue" message
bool WatchForControlMessages::Continue()
{
	ScanService::Continue();
	return SendResponse(std::string("Continueing"));
}

bool WatchForControlMessages::SendResponse(std::string& messageSpecific)
{
	char		tmpBuffer[ScanService::maxMessageSize];
	sprintf(tmpBuffer, "%d, %d, ", messageID, returnID);

	std::string	returnMsg = tmpBuffer;
	returnMsg	+= messageSpecific;

	DWORD	bytesWritten = 0;
	if (WriteFile((HANDLE)controlPipe, returnMsg.c_str(), returnMsg.size()+1,
					&bytesWritten, &overlapped) == FALSE)
	{
		// If the pipe broke, we will disconnect
		if (ERROR_BROKEN_PIPE == GetLastError())
		{
		}
		else if (ERROR_IO_PENDING == GetLastError())
		{
			// waiting for write to complete
			if (FALSE == GetOverlappedResult(controlPipe, &overlapped,
									&bytesWritten, TRUE))	// block
			{
				throw new WatchForControlMessagesException(WatchForControlMessagesException::GetOverlappedResult);
			}
		}
	}
	else
		throw new WatchForControlMessagesException(WatchForControlMessagesException::WriteFile);

	return true;
}


uint WatchForControlMessages::HandleRequest()
{
	try
	{
		if (0 == ResetEvent(overlapEvent))
			throw new WatchForControlMessagesException(WatchForControlMessagesException::ResetEvent);
		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = overlapEvent;

		DWORD	bytesRead	= 0;
		char	readBuffer[ScanService::maxMessageSize];

		if (FALSE == ReadFile((HANDLE)controlPipe, &readBuffer, ScanService::maxMessageSize,
								&bytesRead, &overlapped))
		{
			DWORD	lastError = GetLastError();
			if (ERROR_BROKEN_PIPE == lastError)
			{
				bytesRead = 0;
			}
			else if (ERROR_IO_PENDING == lastError)	// this is what we expect
			{
				if (FALSE == GetOverlappedResult((HANDLE)controlPipe, &overlapped, &bytesRead, TRUE))
				{
					bytesRead = 0;
				}
			}
			else									// unexpected error
			{
				bytesRead = 0;
				throw new WatchForControlMessagesException(WatchForControlMessagesException::ReadFile);
			}
		}

		if (bytesRead)
		{
			if ('c' == readBuffer[0])
			{
				if (!strncmp("continue", readBuffer, 8))
				{
					Continue();
				}
			}
			else if ('p' == readBuffer[0])
			{
				if (!strncmp("pause", readBuffer, 5))
				{
						Pause();
				}
				else if (!strncmp("ping", readBuffer, 4))
					{
					Ping();
				}
			}
			else if ('s' == readBuffer[0])
			{
					if (!strncmp("stats", readBuffer, 5))
				{
					Stats(readBuffer, bytesRead);
				}
					else if (!strncmp("stop", readBuffer, 4))
				{
					Stop();
				}
			}
		}
	}
	catch (WatchForControlMessagesException e)
	{
		std::cerr << "ScanService::WatchForControlMessage::Exception, from = " << e.TypeAsChar();
		std::cerr << ", system error code = " << e.errCode << std::endl;
	}

	if (!DisconnectPipe())
	{
		std::cerr << "ScanService::WatchForControlMessage::DisconnectPipe failed, system error code = " << GetLastError() << std::endl;
	}

	return true;
}


bool WatchForControlMessages::DisconnectPipe()
{
	if (DisconnectNamedPipe(controlPipe) &&			// disconnect the client
		ConnectNamedPipe(controlPipe, &oLap))		// set up so serice is notified on
	{												// next connection.
		return true;
	}

	return false;
}


const char* WatchForControlMessagesException::TypeAsChar()
{
	switch (type)
	{
	case CreateNamedPipe:	return "CreateNamedPipe";
	case CreateEvent:		return "CreateEvent";
	case ConnectNamedPipe:	return "ConnectNamedPipe";
	case ResetEvent:		return "ResetEvent";
	case GetOverlappedResult: return "GetOverlappedResult";
	case WriteFile:			return "WriteFile";
	case ReadFile:			return "ReadFile";
	}

	return "Unknown Exception";
}
