// Transaction.cpp: implementation of the Transaction class.
//
//////////////////////////////////////////////////////////////////////

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

#include "base.h"
#include "Transaction.h"
#include "StrUtil.h"
#include "StrVector.h"
#include "AttributeKeys.h"



//------ Transaction --------------------------------------------------

//
// constructor
//
Transaction::Transaction(	const std::string&	agentName,
							const std::string&	gatewayURL,
							int					verb,
							MyTraceClient&		tc,
							RawEvent&			eventStop,
							const std::string&	target,
							bool				useSSL	)
 : _verb				(verb),
   _agentName			(agentName),
   _eventStop			( eventStop ),
   _useSSL				(useSSL),
   _pInetSession		(),
   _pHttpConnection		(),
   _pHttpFile			(),
   _responseStatusCode	(0),
   _tc					(tc),
   _rc					( AVISTX_SUCCESS ),
   _target				(target)
{
	_tc.enter("Entering Transaction constructor (agent = %s)...", _agentName.c_str() );

	InternetUtil::ParseURL	(	gatewayURL.c_str(),
				  				_gatewayAddress, 
				  				_gatewayPort	);

	ConnectToGateway();

	_tc.exit("Leaving Transaction constructor...");
}


//
// destructor
//
Transaction::~Transaction()
{
	_tc.enter("Entering Transaction destructor...");

	// ensure that all dynamically allocated objects are deleted
	Close();

	_tc.exit("Leaving Transaction destructor...");
}


//
// ConnectToGateway
//
void Transaction::ConnectToGateway()
{
	DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
	std::string pszProxyName;
  std::string pszProxyUsername = "";
  std::string pszProxyPassword = "";

	switch(s_fwType)
	{
		case FW_PROXY:	_tc.debug("firewall type specified as \"proxy\"");				break;
		case FW_SOCKS:	_tc.debug("firewall type specified as \"socks\"");				break;
		case FW_DEFINED_IN_REGISTRY: _tc.debug("firewall/proxy specified in registry");	break;
		case FW_NONE:	_tc.debug("firewall type specified as \"none\"");				break;
	}

	// establish the access type and the proxy address
	switch(s_fwType)
	{
		case FW_PROXY:
		case FW_SOCKS:					dwAccessType = INTERNET_OPEN_TYPE_PROXY;
										pszProxyName = s_strProxyAddress;				break;
		case FW_DEFINED_IN_REGISTRY:	dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;	break;
		case FW_NONE:					dwAccessType = INTERNET_OPEN_TYPE_DIRECT;		break;
	}

  /*
   * Now, if there's a proxy configured, then get the username/password
   * sorted out.  Take care only to do this if there really is a proxy
   * configured
   * [inw 2000-08-15]
   */
  if (INTERNET_OPEN_TYPE_DIRECT != dwAccessType)
  {
    if (s_strProxyUsername.size())
      pszProxyUsername = s_strProxyUsername.c_str();
    if (s_strProxyPassword.size())
      pszProxyPassword = s_strProxyPassword.c_str();
  }

	// ensure that all instances of WinInet objects are closed and deleted before
	// continuing...
	Close();

	// create an internet session object, required in MFC internet apps, in order to
	// get an http connection object later.
	_tc.enter("creating new instance of InternetSession object...");
	_pInetSession = std::auto_ptr<InternetSession>( new InternetSession(
		_agentName.c_str(),			// use application name as agent name
		::GetCurrentThreadId(),		// context id
		dwAccessType,				// proxy or direct access or INTERNET_OPEN_TYPE_PRECONFIG
		pszProxyName,				// proxy name (if necessary)
		NULL,						// no proxy bypass
		_tc ) );
//			INTERNET_FLAG_DONT_CACHE);	// no caching or asynch stuff
	_tc.exit("created new instance of InternetSession object");

	// if logging flag is set (through calling EnableWinInetLogging())
	if (getEnableWinInetLogging())
	{
		// enable callbacks to record internet events (throws exception in the event
		// of an error).
		_pInetSession->EnableStatusCallback( getEnableWinInetLogging() );
	}

	SetSessionOptions( *_pInetSession );

	// create an HttpConnection object, from which an HttpFile object can be created
	// The HttpFile object is used to open and issue an Http request
	try
	{
		_tc.enter("creating new instance of HttpConnection object...");

		// throws exception in the event of an error
		_pHttpConnection = std::auto_ptr<HttpConnection>( _pInetSession->GetHttpConnection(
			_gatewayAddress.c_str(),	// gateway address
			_gatewayPort,			// gateway port
      pszProxyUsername.c_str(), // username
      pszProxyPassword.c_str())); // password
		_tc.exit("created new instance of HttpConnection object");

	}
	catch ( InternetException& except)
	{
		_tc.critical("exception occurred while calling GetHttpConnection().");
		Close();

		if (AVISTX_CRITICAL_ERROR == InternetUtil::ProcessInternetError( _tc, &except))
			throw; // let the next exception handler catch this exception
	}
}

//
// Close
//
void Transaction::Close( bool closeHttpFileOnly )
{
// close/delete all internet objects

	if (_pHttpFile.get() != NULL)
	{
		_tc.enter("closing HttpFile object...");
		_pHttpFile->Close();
		_tc.exit("closed HttpFile object");
		_tc.enter("deleting HttpFile object...");
		_pHttpFile = std::auto_ptr<ZHttpFile>( 0 );
		_tc.exit("deleted HttpFile object");
	}

	if (closeHttpFileOnly)
		return;

	if (_pHttpConnection.get() != NULL)
	{
		_tc.enter("closing HttpConnection object...");
		_pHttpConnection->Close();
		_tc.exit("closed HttpConnection object");
		_tc.enter("deleting HttpConnection object...");
		_pHttpConnection = std::auto_ptr<HttpConnection>( 0 );
		_tc.exit("deleted HttpConnection object");
	}

	if (_pInetSession.get() != NULL)
	{
		_tc.enter("closing InternetSession object...");
		_pInetSession->Close();
		_tc.exit("closed InternetSession object");
		_tc.enter("deleting InternetSession object...");
		_pInetSession = std::auto_ptr<InternetSession>( 0 );
		_tc.exit("deleted InternetSession object");
	}
}


//
// OpenRequest
//
DWORD Transaction::OpenRequest()
{
	// flags for OpenRequest
	DWORD dwFlags = INTERNET_FLAG_DONT_CACHE |
				    INTERNET_FLAG_NO_AUTO_REDIRECT |
				    INTERNET_FLAG_NO_COOKIES |
				    INTERNET_FLAG_NO_UI |
				    INTERNET_FLAG_PRAGMA_NOCACHE |
				    INTERNET_FLAG_RELOAD;

  if (_useSSL)
	{
		_tc.debug("enabled flags for using SSL in Http request");
		dwFlags |= INTERNET_FLAG_SECURE;
		if ( getIgnoreSSLNameMismatch() )
			dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
	}

	// ensure that if this transaction is being reused, that the ZHttpFile object
	// was closed before recreating it in the OpenRequest() call.
	// only close and delete the HttpFile object.
	Close( true );

	// Create an HttpFile object, from which Http requests can be executed
	_tc.enter("creating new HttpFile object...");
	_pHttpFile = std::auto_ptr<ZHttpFile>( _pHttpConnection->OpenRequest(
		_verb,		// verb
		_target.c_str(),	// target and query
		NULL,			// referrer
		::GetCurrentThreadId(),	// context id
		NULL,			// accept all types
		NULL,			// use "HTTP/1.0"
		dwFlags) );		// internet flags

	if (_pHttpFile.get() == NULL)
	{
		_tc.critical("failed in HttpConnection::OpenRequest");
		::ThrowMemoryException();
	}

	if (!_pHttpFile->IsHFileValid())
	{
		_tc.critical("error occuring during OpenRequest()");
		// throws exception for us
		InternetUtil::ProcessInternetError( _tc );
	}

	_tc.exit("created new HttpFile object");

  /*
   * JD.
   */
  return AVISTX_SUCCESS;
}


//
// SendRequest
//
/* This function sends the request content in one fell-swoop. class
   must have all of the data to write before this call is made.  Will be useful
   in situations when content size is small. */
int Transaction::SendRequest(
	DWORD cbRequestContent,
	PVOID pvRequestContent)
{
	try
	{
		_tc.debug("sending request, %d bytes of request content...", cbRequestContent);
		_pHttpFile->SendRequest(NULL, 0, pvRequestContent, cbRequestContent);
		_tc.debug("sent request");

		// get the response status code
		_tc.debug("retrieving response status code...");
		_pHttpFile->QueryInfoStatusCode(_responseStatusCode);
		_tc.debug("retrieved response status code");

		return AVISTX_SUCCESS;
	}
	catch ( InternetException& except )
	{
		_tc.critical("failed sending request");

		DWORD dwErr = InternetUtil::ProcessInternetError( _tc, &except);
		return dwErr;
	}
}

//
// GetResponseContentLength
//
int Transaction::GetResponseContentLength()
{
	int cbResponseContentLength;
	DWORD cbSize = sizeof(cbResponseContentLength);

	// retrieve content-length header value
	if (_pHttpFile->QueryInfo(	HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, 
								&cbResponseContentLength,
								&cbSize) )
		return cbResponseContentLength;
	else
		return -1;
}


//
// ReadNextResponseContentBlock
//
int Transaction::ReadNextResponseContentBlock(PVOID pvResponseContentBlock, DWORD cbToRead)
{
	UINT uiBytesRead;

	try
	{
		//_tc.debug("Reading response content...");
		uiBytesRead = _pHttpFile->Read(pvResponseContentBlock, cbToRead);
		_tc.debug("requested %d bytes, read %d bytes", cbToRead, uiBytesRead);
	}
	catch ( InternetException& except)
	{
		_tc.critical("failed reading response content");

		DWORD dwErr = InternetUtil::ProcessInternetError( _tc, &except );

		if (AVISTX_CRITICAL_ERROR == dwErr)
			throw;

		return -1;
	}

	return uiBytesRead;
}


//
// AddRequestHeader
//
void Transaction::AddRequestHeader( const std::string& header )
{
//	if (!_requestHeaders.empty())
//		_requestHeaders += "\n";
//
//	_requestHeaders += header;
	if (_requestHeaders.empty())
		_requestHeaders = header;
	else
		_requestHeaders = header + "\n" + _requestHeaders;
}


//
// SetReadBufferSize
//
BOOL Transaction::SetReadBufferSize(DWORD dwBufSize)
{
	return _pHttpFile->SetReadBufferSize(dwBufSize);
}


//
// EmptyRequestHeaders
//
void Transaction::EmptyRequestHeaders()
{
	_requestHeaders = "";
}


//
// RetrieveResponseHeaders
//

void Transaction::RetrieveResponseHeaders()
{
	std::string s;

	_pHttpFile->GetResponseHeaders( s );

	_responseHeaders = s;

	StrUtil::replaceAll( s, "\r", "" );
	StrVector v( s, "\n" );
	for ( int i = 0; i < v.size(); i++ )
	{
		if ( !v[ i ].empty() )
			_tc.debug( "response header = %s", v[ i ].c_str() );
	}
}


//
// AddRequestHeaders
//

void Transaction::AddRequestHeaders()
{
  /*
   * It turns out that we always need to add this header
   * to the other headers.  This works correctly when
   * there are no headers currently assigned.
   */
  _requestHeaders = "Pragma: no-cache\n" + _requestHeaders;

	_pHttpFile->AddRequestHeaders(_requestHeaders.c_str() );

	StrVector v( _requestHeaders, "\n" );

	for ( int i = 0; i < v.size(); i++ )
	{
		if ( !v[ i ].empty() )
			_tc.debug("added request header = %s", v[ i ].c_str() );
	}
}


//
// GetErrorString
//

bool Transaction::GetErrorString( std::string& _errorString )
{
	bool b = _pHttpFile->GetCustomHeaderValue2( AttributeKeys::Error(), _errorString );
	return b;
}
