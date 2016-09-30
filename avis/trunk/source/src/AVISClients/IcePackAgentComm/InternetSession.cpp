/////////////////////////////////////////////////////////////////////////////
// InternetSession object

#include "base.h"
#include <winsock.h>
#include "InternetSession.h"
#include "HttpConnection.h"
//#include "Transactions.h"

//
// constructor
//
InternetSession::InternetSession(
		LPCTSTR			pstrAgent,
		DWORD			dwContext,
		DWORD			dwAccessType,
		std::string&	proxyName,
		LPCTSTR			pstrProxyBypass,
		MyTraceClient&	traceClient,
		DWORD			dwFlags)
		:		
		_tc( traceClient ),
		_callbackEnabled( false ),
		_context( dwContext ),
		_oldCallback( NULL )
{
	_internetSession = InternetOpen( pstrAgent, dwAccessType, proxyName.c_str(), pstrProxyBypass, dwFlags );

	if ( _internetSession == NULL )
		ThrowInternetException( dwContext );
}


//
// destructor
//
InternetSession::~InternetSession()
{
}


//
// GetHttpConnection
//
HttpConnection* InternetSession::GetHttpConnection	(	LPCTSTR pstrServer,
														INTERNET_PORT nPort		/*= INTERNET_INVALID_PORT_NUMBER*/,
														LPCTSTR pstrUserName	/*= NULL*/, 
														LPCTSTR pstrPassword	/*= NULL*/	)
{
	HttpConnection* p = new HttpConnection( *this, pstrServer, nPort, pstrUserName, pstrPassword, _context);
	return p;
}


//
// SetOption
//
bool InternetSession::SetOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength)
{
	bool b = InternetSetOption( _internetSession, dwOption, lpBuffer, dwBufferLength ) ? true : false;
	return b;
}


//
// SetOption
//
bool InternetSession::SetOption(DWORD dwOption, DWORD dwValue)
{ 
	return SetOption( dwOption, &dwValue, sizeof(dwValue) ); 
}


//
// Close
//
void InternetSession::Close()
{
	if ( _callbackEnabled )
		EnableStatusCallback( false );

	if ( _internetSession != NULL)
	{
		InternetCloseHandle( _internetSession);
		_internetSession = NULL;
	}
}


static InternetSession * pSession = NULL;

void __stdcall InternetStatusCallback(HINTERNET hInternet, DWORD dwContext,
	DWORD dwInternetStatus, LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
	if ( pSession != NULL )
	{
		pSession->OnStatusCallback(dwContext, dwInternetStatus,	lpvStatusInformation, dwStatusInformationLength );
	}
}


//
// EnableStatusCallback
//
bool InternetSession::EnableStatusCallback( bool bEnable /* = TRUE */)
{
	if ( _internetSession == NULL )
		return false;

	bool bResult = true;

	if (bEnable)
	{
		if (!_callbackEnabled)
		{
			INTERNET_STATUS_CALLBACK pRet =
				InternetSetStatusCallback(_internetSession, InternetStatusCallback);

			pSession = this;

			if (pRet != INTERNET_INVALID_STATUS_CALLBACK )
			{
				_oldCallback = pRet;
				_callbackEnabled = TRUE;
			}
			else
				ThrowInternetException( _context);
		}
	}
	else
	{
		if ( _callbackEnabled )
		{
			InternetSetStatusCallback( _internetSession, NULL);
			_callbackEnabled = false;
			pSession = NULL;
		}
	}

	return bResult;
}



void InternetSession::OnStatusCallback(
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLen)
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState( ) );

	switch(dwInternetStatus)
	{
		case INTERNET_STATUS_RESOLVING_NAME:
		case INTERNET_STATUS_NAME_RESOLVED:
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
		case INTERNET_STATUS_CONNECTED_TO_SERVER:
		{
			char szInfo[4*1024];
			::strncpy(szInfo, (LPCSTR )lpvStatusInformation, dwStatusInformationLen);
			szInfo[dwStatusInformationLen] = '\0';

			switch(dwInternetStatus)
			{
				case INTERNET_STATUS_RESOLVING_NAME:
					_tc.debug("WinInet resolving internet name \"%s\"...", szInfo);
					break;

				case INTERNET_STATUS_NAME_RESOLVED:
					_tc.debug("WinInet resolved internet name \"%s\".", szInfo);
					break;

				case INTERNET_STATUS_CONNECTING_TO_SERVER:
					_tc.debug("WinInet connecting to server with address \"%s\".", szInfo);
					break;

				case INTERNET_STATUS_CONNECTED_TO_SERVER:
					_tc.debug("WinInet connected to server with address \"%s\".", szInfo);
					break;
			}
			break;
		}

		case INTERNET_STATUS_SENDING_REQUEST:
			_tc.debug("WinInet sending request...");
			break;

		case INTERNET_STATUS_REQUEST_SENT:
			_tc.debug("WinInet sent request.");
			break;

		case INTERNET_STATUS_RECEIVING_RESPONSE:
			_tc.debug("WinInet receiving response...");
			break;

		case INTERNET_STATUS_RESPONSE_RECEIVED:
			_tc.debug("WinInet received response.");
			break;

		case INTERNET_STATUS_CLOSING_CONNECTION:
			_tc.debug("WinInet closing connection...");
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			_tc.debug("WinInet closed connection.");
			break;

		case INTERNET_STATUS_HANDLE_CREATED:
			_tc.debug("WinInet created a new handle.");
			break;

		case INTERNET_STATUS_HANDLE_CLOSING:
			_tc.debug("WinInet is closing a handle.");
			break;

		case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
			_tc.debug("WinInet received a control response.");
			break;

		case INTERNET_STATUS_PREFETCH:
			_tc.debug("WinInet \"prefetch\".");
			break;

		case INTERNET_STATUS_REQUEST_COMPLETE:
			_tc.debug("WinInet request complete.");
			break;

		case INTERNET_STATUS_REDIRECT:
			_tc.debug("WinInet redirect.");
			break;

		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
			_tc.debug("WinInet intermediate response.");
			break;

		case INTERNET_STATUS_STATE_CHANGE:
			_tc.debug("WinInet state change.");
			break;

		default:
			_tc.debug("WinInet unknown status.");
			break;
	}
}


