#ifndef __INTERNETSESSION__H__
#define __INTERNETSESSION__H__

#include <string>
#include <wininet.h>
#include "InternetException.h"
#include "HttpConnection.h"
#include "MyTraceClient.h"

class InternetSession// : public CInternetSession
{
public:
	InternetSession(
		LPCTSTR			pstrAgent,
		DWORD			dwContext,
		DWORD			dwAccessType,
		std::string&	proxyName,
		LPCTSTR			pstrProxyBypass,
		MyTraceClient&	traceClient,
		DWORD			dwFlags = 0);

	virtual ~InternetSession();

	HttpConnection*		GetHttpConnection	(	LPCTSTR pstrServer,
												INTERNET_PORT nPort = INTERNET_INVALID_PORT_NUMBER,
												LPCTSTR pstrUserName = NULL, LPCTSTR pstrPassword = NULL	);

 	bool				SetOption			(	DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
	bool				SetOption			(	DWORD dwOption, DWORD dwValue);

	bool				EnableStatusCallback(	bool bEnable = true );

	virtual	void		Close();

	HINTERNET			GetHandle()			{ return _internetSession; }

	// virtual override to receive callbacks about internet events
	virtual void OnStatusCallback(
		DWORD dwContext,
		DWORD dwInternetStatus,
		LPVOID lpvStatusInformation,
		DWORD dwStatusInformationLen);
private:
	MyTraceClient&				_tc;
	HINTERNET					_internetSession;
	bool						_callbackEnabled;
	DWORD						_context;
	INTERNET_STATUS_CALLBACK	_oldCallback;
};

#endif __INTERNETSESSION__H__
