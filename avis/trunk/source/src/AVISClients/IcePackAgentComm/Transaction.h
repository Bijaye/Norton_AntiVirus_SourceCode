// Transaction.h: interface for the Transaction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_)
#define TRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxinet.h>
#include <string>
#include <memory>
#include "MyTraceClient.h"
#include "InternetSession.h"
#include "InternetUtil.h"
#include "TransactionSettings.h"
#include "AutoEvent.h"

#pragma	warning( disable : 4251 )

class Transaction : public TransactionSettings
{
private:
	std::string							_agentName;			// name the transaction owner
	std::string							_requestHeaders;	// the headers to send along with the HTTP request
	int									_verb;				// verb identifier (i.e. HTTP_VERB_POST)
	std::string							_gatewayAddress;	// parsed URL parameters
	std::string							_target;
	INTERNET_PORT						_gatewayPort;
	bool								_useSSL;				// security
	DWORD								_responseStatusCode;
	std::auto_ptr<InternetSession>		_pInetSession;
	std::auto_ptr<HttpConnection>		_pHttpConnection;
	std::auto_ptr<ZHttpFile>			_pHttpFile;

protected:
	MyTraceClient&						_tc;
	int									_rc;
	std::string							_responseHeaders;
	RawEvent&							_eventStop;

public:
	Transaction(		const std::string&	agentName,
						const std::string&	gatewayURL,
						int					verb,
						MyTraceClient&		tc,
						RawEvent&			eventStop,
						const std::string&	target = "",
						bool				useSSL = false	);

	virtual ~Transaction();

	
	void				AddRequestHeader( const std::string& header );								// Add an individual request header before the request has been issued
	void				EmptyRequestHeaders();														// reset the request headers
	const std::string&	ResponseHeaders() const { return _responseHeaders; }

protected: // helper functions for derived classes
	DWORD	OpenRequest();
	int		SendRequest(DWORD cbRequestContent = 0, PVOID pvRequestContent = NULL);	// send request with specified content to be written all at once
	int		GetResponseContentLength();												// retrieve the length of the response content
	int		ReadNextResponseContentBlock(PVOID pvResponseContentBlock, DWORD cbToRead);
	BOOL	SetReadBufferSize(DWORD dwBufSize);										// set the buffer size for reading response content
	void	RetrieveResponseHeaders();												// retrieve the response headers into _responseHeaders
	void	AddRequestHeaders();
	bool	GetErrorString( std::string& errorString );
	DWORD 	GetResponseStatusCode() const { return _responseStatusCode; }

private:
	void	ConnectToGateway();	// ***call before calling execute
	void	Close( bool closeHttpFileOnly = false );// call after finishing transaction
};




#endif // !defined(AFX_AVISTRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_)
