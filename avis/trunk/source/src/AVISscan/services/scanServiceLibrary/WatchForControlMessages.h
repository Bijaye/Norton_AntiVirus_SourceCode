// WatchForControlMessages.h: interface for the WatchForControlMessages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHFORCONTROLMESSAGES_H__CADC3CA5_9115_11D2_AD01_00A0C9C71BBC__INCLUDED_)
#define AFX_WATCHFORCONTROLMESSAGES_H__CADC3CA5_9115_11D2_AD01_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <AVIS.h>
#include <CMclThread.h>
#include <Handle.h>

class WatchForControlMessages : public CMclThreadHandler  
{
public:
	WatchForControlMessages();
	virtual ~WatchForControlMessages();

	uint	ThreadHandlerProc(void);

private:
	SECURITY_ATTRIBUTES	defaultInheritiable;
	Handle				controlPipe;
	Handle				pipeEvents;
	OVERLAPPED			oLap;
	Handle				overlapEvent;
	OVERLAPPED			overlapped;

	uint				messageID, returnID;

	bool	SendResponse(std::string& typeSpecific);

	//	Handle the "stats" message
	bool	Stats(char* buffer, DWORD bytesRead);
	//	Handle the "stop" message
	bool	Stop();
	//	Handle the "ping" message
	bool	Ping();
	//	Handle the "pause" message
	bool	Pause();
	//	Handle the "continue" message
	bool	Continue();

	uint	HandleRequest(void);

	bool	DisconnectPipe(void);
};

class WatchForControlMessagesException
{
public:
	enum wfcmeType {
		CreateNamedPipe, CreateEvent, ConnectNamedPipe,
		ResetEvent, GetOverlappedResult, WriteFile, ReadFile

	};

	WatchForControlMessagesException(wfcmeType t) : type(t), errCode(GetLastError())
	{
	};
	~WatchForControlMessagesException() {};

	wfcmeType	type;
	DWORD		errCode;

	const char* TypeAsChar();
};

#endif // !defined(AFX_WATCHFORCONTROLMESSAGES_H__CADC3CA5_9115_11D2_AD01_00A0C9C71BBC__INCLUDED_)
