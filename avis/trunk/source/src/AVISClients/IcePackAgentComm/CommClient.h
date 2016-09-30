//
//	IBM AntiVirus Immune System
//
//	File Name:	CommClient.h
//	Author:		Milosz Muszynski
//
//	This class is based on the Thread class
//	and provides a thread for a specific communication function.
//  This is a pure abstract class, intended to be subclassed by
//  concrete sample or signature related communication classes.
//  This class provides methods to control behavior of a thread
//  performing communication functions.
//  This class also maintains data common for all communication tasks
//  like gateway URL, agent name, response headers, etc.
//
//	$Log: $
//


#if !defined(__COMMCLIENT__H__)
#define __COMMCLIENT__H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "thread.h"
#include "MyTraceClient.h"
#include "AutoEvent.h"
#include "IcePackAgentComm.h"
#include "HttpConnection.h"
#include "Config.h"

#pragma	warning( disable : 4251 )

class CommClient : public Thread  
{
private:
	std::string						_logFileName;
	std::string						_traceFileName;
	unsigned long					_systemExceptionCode;

public:
	CommClient(		const std::string&	logFileName,
					const std::string&	traceFileName,
					const std::string&	agentName,
					RawEvent&			eventStop,
					MyTraceClient&		traceClient,
          const std::string&  target,
          const HttpConnection::connectionVerb verb);
	virtual ~CommClient();

private:
	DWORD ThreadProc(LPVOID);
	virtual IcePackAgentComm::ErrorCode ClientMain() = 0;

public:
	void						Wait();
	void						Terminate();
	const char *				GetAgentName();
	virtual BOOL				End(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = FALSE) = 0;
	IcePackAgentComm::ErrorCode	Error()					const { return _errorCode; }
	unsigned long				SystemExceptionCode()	const { return _systemExceptionCode; }
	const std::string&			ResponseHeaders()       const { return _responseHeaders; }
protected:
	void						Go();
  IcePackAgentComm::ErrorCode Launch();
	bool						WaitForGo();// returns true if terminated
	void						Done();
private:
  std::string                         doDoubleLookup(std::string gateway);
  virtual IcePackAgentComm::ErrorCode _reallyLaunch(const std::string &gateway,
                                                    const std::string                    &target,
                                                    const HttpConnection::connectionVerb  verb) = 0;

private:
	AutoEvent						_eventGo;
	AutoEvent						_eventDone;
	AutoEvent						_eventTerminate;
protected:
	IcePackAgentComm::ErrorCode		_errorCode;
	std::string						_responseHeaders;
	std::string						_agentName;
	std::string						_gatewayURL;
	MyTraceClient&					_tc;
	bool							_useSSL;
	RawEvent&						_eventStop;

protected:
  std::string                    _defaultTarget;
  HttpConnection::connectionVerb _defaultVerb;
};

#endif // !defined(__COMMCLIENT__H__)
