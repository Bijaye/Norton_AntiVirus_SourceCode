// AVISTransaction.cpp: implementation of the CAVISTransaction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactions.h"
#include "AVISTransaction.h"
#include "AVISTransactionException.h"
#include "AttributeKeys.h"
#include "XError.h"
#include "Thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// macros
#define AVISTX_CRITICAL_ERROR	AVISTX_RC_BASE + 4

// initialize static members
CAVISTransaction::FirewallType CAVISTransaction::s_fwType = CAVISTransaction::FW_NONE;
CString CAVISTransaction::s_strSocksAddress;
CString CAVISTransaction::s_strProxyAddress;
CString CAVISTransaction::s_strProxyUsername;
CString CAVISTransaction::s_strProxyPassword;

int CAVISTransaction::s_iConnectTimeout = -1;
int CAVISTransaction::s_iConnectRetries = -1;

int CAVISTransaction::s_iSendRequestTimeout = -1;
int CAVISTransaction::s_iSendRequestDataTimeout = -1;

int CAVISTransaction::s_iReceiveResponseTimeout = -1;
int CAVISTransaction::s_iReceiveResponseDataTimeout = -1;

BOOL CAVISTransaction::s_boEnableWinInetLogging = FALSE;

BOOL CAVISTransaction::s_boIgnoreInvalidCertificateServerName = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*****/
CAVISTransaction::CAVISTransaction(
	LPCSTR pszAgentName,
	LPCSTR pszGatewayURL,
	int nVerb,
	UINT uiTimeoutTransactionConnected,
	UINT uiTimeoutTransactionCompleted,
	CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
	LPCTSTR pszTarget,
	BOOL boUseSSL)
 : m_nVerb(nVerb),
   m_strAgentName(pszAgentName),
   m_boUseSSL(boUseSSL),
   m_pInetSession(NULL),
   m_pHttpConnection(NULL),
   m_pHttpFile(NULL),
   m_uiResponseAnalysisCookie(0),
   m_dwResponseStatusCode(0),
   m_boIgnoreXErrorCrumbled(FALSE),
   m_uiTimeoutTransactionConnected(uiTimeoutTransactionConnected),
   m_uiTimeoutTransactionCompleted(uiTimeoutTransactionCompleted),
   m_pthreadTransactionTimeoutMonitor(pthreadTransactionTimeoutMonitor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CAVISTransactionsApp::s_pTc->enter("Entering CAVISTransaction constructor (agent = %s)...", pszAgentName);

	// create a manual-reset event that is initially signaled. The event should be
	// signaled (event is in the "set" state, which releases waiting threads) when the 
	// transaction is not running, and NOT signalled (event is in the "reset" state, 
	// which causes waiting threads to block) while the transaction IS running.
	m_hEventTxRunning = ::CreateEvent(NULL, TRUE, TRUE, "");

	if (pszTarget != NULL)
		m_strTarget = pszTarget;

	// break down the gateway URL into a gateway address and a gateway port
	DWORD dwServiceTypeDummy;
	CString strTargetDummy;

	::AfxParseURL(
		pszGatewayURL,
		dwServiceTypeDummy,				// dont care (should be "http")
		m_strGatewayAddress, 
		strTargetDummy,					// should be empty
		m_nGatewayPort);

	// make sure the transaction remembers the gateway address and port
	ConnectToGateway();

	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISTransaction constructor...");
}
/*****/
CAVISTransaction::~CAVISTransaction()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CAVISTransactionsApp::s_pTc->enter("Entering CAVISTransaction destructor...");

	// ensure that all dynamically allocated objects are deleted
	Close();
	WaitForTransactionToComplete();

	::CloseHandle(m_hEventTxRunning);

	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISTransaction destructor...");
}
/*****/
void CAVISTransaction::EnableWinInetLogging(BOOL boEnable)
// set a static flag to enable the callback for status
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CAVISTransactionsApp::s_pTc->debug("WinInet logging is enabled.");
	s_boEnableWinInetLogging = boEnable;
}
/*****/
/* The following method will analyze the last error and possible take action on it.
   pExcept represents a pointer to the caught exception. The caller wishes to
   determine if the exception can be handled automatically. If the error is
   determined to be "critical", then the return code AVISTX_CRITICAL_ERROR is returned. */

DWORD CAVISTransaction::ProcessInternetError(CInternetException* pExcept)
{
	char stError[512];
	DWORD dwErr;

	if (pExcept != NULL)
	{
		pExcept->GetErrorMessage(stError, sizeof(stError));
		CAVISTransactionsApp::s_pTc->error("Internet exception occurred; err = %s", stError);
		dwErr = pExcept->m_dwError;
	}
	else
	{
		LPVOID lpMsgBuf;

		dwErr = GetLastError();

		::FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR )&lpMsgBuf,
			0,
			NULL);

		CAVISTransactionsApp::s_pTc->error("Internet exception occurred; err = %s", (LPCTSTR )lpMsgBuf);

		// Free the buffer.
		LocalFree(lpMsgBuf);
	}

	switch(dwErr)
	{
		// errors which either indicate that the current process is being shut down or
		// that the remote server has severed the connection
		case ERROR_INTERNET_CONNECTION_ABORTED:
		case ERROR_INTERNET_OPERATION_CANCELLED:
			if (CThread::ReceivedQuitMessage(FALSE))
			{
				CAVISTransactionsApp::s_pTc->debug("Received a terminate request; cancelling transaction...");
				return AVISTX_TERMINATE;
			}
			// else use following case to return AVISTX_NETWORK_FAILURE, forcing a retry
			
		// recoverable errors (these errors are due to problems in the network)
		case ERROR_INTERNET_CANNOT_CONNECT:
		case ERROR_INTERNET_CONNECTION_RESET:
		case ERROR_INTERNET_DISCONNECTED:
		case ERROR_INTERNET_FORCE_RETRY:
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
		case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
		case ERROR_INTERNET_SERVER_UNREACHABLE:
		case ERROR_INTERNET_TIMEOUT:
		case ERROR_HTTP_INVALID_SERVER_RESPONSE:
			CAVISTransactionsApp::s_pTc->debug("Error was due to a problem in the network, and is recoverable.");
			return AVISTX_NETWORK_FAILURE;

		// critical errors (expect that these errors can occur, but they 
		// are unrecoverable)
		case ERROR_HTTP_COOKIE_DECLINED:
		case ERROR_HTTP_COOKIE_NEEDS_CONFIRMATION:
		case ERROR_HTTP_DOWNLEVEL_SERVER:
		case ERROR_HTTP_HEADER_ALREADY_EXISTS:
		case ERROR_HTTP_HEADER_NOT_FOUND:
		case ERROR_HTTP_INVALID_HEADER:
		case ERROR_HTTP_INVALID_QUERY_REQUEST:
		case ERROR_HTTP_NOT_REDIRECTED:
		case ERROR_HTTP_REDIRECT_FAILED:
		case ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION:
		case ERROR_INTERNET_BAD_OPTION_LENGTH:
		case ERROR_INTERNET_BAD_AUTO_PROXY_SCRIPT:
		case ERROR_INTERNET_BAD_REGISTRY_PARAMETER:
		case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
		case ERROR_INTERNET_CLIENT_AUTH_NOT_SETUP:
		case ERROR_INTERNET_EXTENDED_ERROR:
		case ERROR_INTERNET_FAILED_DUETOSECURITYCHECK:
		case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
		case ERROR_INTERNET_INCORRECT_HANDLE_TYPE:
		case ERROR_INTERNET_INTERNAL_ERROR:
		case ERROR_INTERNET_INVALID_CA:
		case ERROR_INTERNET_INVALID_PROXY_REQUEST:
		case ERROR_INTERNET_INVALID_URL:
		case ERROR_INTERNET_MIXED_SECURITY:
		case ERROR_INTERNET_NO_DIRECT_ACCESS:
		case ERROR_INTERNET_NOT_PROXY_REQUEST:
		case ERROR_INTERNET_OUT_OF_HANDLES:
		case ERROR_INTERNET_POST_IS_NON_SECURE:
		case ERROR_INTERNET_REQUEST_PENDING:
		case ERROR_INTERNET_SEC_CERT_CN_INVALID:
		case ERROR_INTERNET_SEC_CERT_DATE_INVALID:
//		case ERROR_INTERNET_SEC_CERT_ERRORS:
		case ERROR_INTERNET_SEC_CERT_REVOKED:
//		case ERROR_INTERNET_SEC_CERT_NO_REV:
//		case ERROR_INTERNET_SEC_CERT_REV_FAILED:
		case ERROR_INTERNET_SEC_INVALID_CERT:
		case ERROR_INTERNET_SECURITY_CHANNEL_ERROR:
		case ERROR_INTERNET_SHUTDOWN:
		case ERROR_INTERNET_TCPIP_NOT_INSTALLED:
		case ERROR_INVALID_HANDLE:
			CAVISTransactionsApp::s_pTc->debug("Error was unrecoverable.");
			return AVISTX_CRITICAL_ERROR;

		// unexpected error codes (should never get these errors, and they 
		// are unrecoverable)
		case ERROR_FTP_DROPPED:
		case ERROR_FTP_TRANSFER_IN_PROGRESS:
		case ERROR_GOPHER_ATTRIBUTE_NOT_FOUND:
		case ERROR_GOPHER_DATA_ERROR:
		case ERROR_GOPHER_END_OF_DATA:
		case ERROR_GOPHER_INCORRECT_LOCATOR_TYPE:
		case ERROR_GOPHER_INVALID_LOCATOR:
		case ERROR_GOPHER_NOT_FILE:
		case ERROR_GOPHER_NOT_GOPHER_PLUS:
		case ERROR_GOPHER_PROTOCOL_ERROR:
		case ERROR_GOPHER_UNKNOWN_LOCATOR:
		case ERROR_INTERNET_ASYNC_THREAD_FAILED:
		case ERROR_INTERNET_CHG_POST_IS_NON_SECURE:
		case ERROR_INTERNET_DIALOG_PENDING:
//		case ERROR_INTERNET_FORTEZZA_LOGIN_NEEDED:
		case ERROR_INTERNET_HANDLE_EXISTS:
		case ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR:
		case ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR:
		case ERROR_INTERNET_HTTPS_HTTP_SUBMIT_REDIR:
		case ERROR_INTERNET_INCORRECT_FORMAT:
		case ERROR_INTERNET_INCORRECT_PASSWORD:
		case ERROR_INTERNET_INCORRECT_USER_NAME:
		case ERROR_INTERNET_INSERT_CDROM:
		case ERROR_INTERNET_INVALID_OPERATION:
		case ERROR_INTERNET_INVALID_OPTION:
		case ERROR_INTERNET_ITEM_NOT_FOUND:
		case ERROR_INTERNET_LOGIN_FAILURE:
//		case ERROR_INTERNET_LOGIN_FAILURE_DISPLAY_ENTITY_BODY:
//		case ERROR_INTERNET_NEED_MSN_SSPI_PKG:
		case ERROR_INTERNET_NEED_UI:
		case ERROR_INTERNET_NO_CALLBACK:
		case ERROR_INTERNET_NO_CONTEXT:
//		case ERROR_INTERNET_NOT_INITIALIZED:
		case ERROR_INTERNET_OPTION_NOT_SETTABLE:
		case ERROR_INTERNET_PROTOCOL_NOT_FOUND:
		case ERROR_INTERNET_REDIRECT_SCHEME_CHANGE:
		case ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND:
		case ERROR_INTERNET_RETRY_DIALOG:
		case ERROR_INTERNET_UNABLE_TO_CACHE_FILE:
		case ERROR_INTERNET_UNABLE_TO_DOWNLOAD_SCRIPT:
		case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
		case ERROR_NO_MORE_FILES:
		case ERROR_NO_MORE_ITEMS:
		default:
			CAVISTransactionsApp::s_pTc->debug("Error was severe and unexpected.");
			return AVISTX_CRITICAL_ERROR;
	}
}
/*****/
/* Static function used to define global internet data */
void CAVISTransaction::InitInternetCommunicationsData(
		CAVISTransaction::FirewallType fwType,
		LPCTSTR pszSocksAddress,
		LPCTSTR pszProxyAddress,
		LPCTSTR pszProxyUsername,
		LPCTSTR pszProxyPassword,
		int iConnectTimeout,
		int iConnectRetries,
		int iSendRequestTimeout,
		int iSendRequestDataTimeout,
		int iReceiveResponseTimeout,
		int iReceiveResponseDataTimeout,
		BOOL boIgnoreInvalidCertificateServerName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CAVISTransactionsApp::s_pTc->debug("Initializing internet communications parameters...");

	s_fwType = fwType;

	if (pszSocksAddress != NULL)
		s_strSocksAddress = pszSocksAddress;

	if (pszProxyAddress != NULL)
		s_strProxyAddress = pszProxyAddress;

	if (pszProxyUsername != NULL)
	{
		s_strProxyUsername = pszProxyUsername;

		if (pszProxyPassword != NULL)
			s_strProxyPassword = pszProxyPassword;
	}

	s_iConnectTimeout = iConnectTimeout;
	s_iConnectTimeout = iConnectTimeout;
	s_iConnectRetries = iConnectRetries;
	s_iSendRequestTimeout = iSendRequestTimeout;
	s_iSendRequestDataTimeout = iSendRequestDataTimeout;
	s_iReceiveResponseTimeout = iReceiveResponseTimeout;
	s_iReceiveResponseDataTimeout = iReceiveResponseDataTimeout;
	s_boIgnoreInvalidCertificateServerName = boIgnoreInvalidCertificateServerName;
}
/*****/
/* This method establishes an internet session (required for MFC Internet usage), 
   and establishes an HttpConnection for the requested server. 

   This method should now only be called from the constructor, negating the need to
   delete any existing WinInet objects first.*/
void CAVISTransaction::ConnectToGateway()
{
	DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
	LPCSTR pszProxyName = NULL;
	CString strProxyName;

	switch(s_fwType)
	{
		case FW_PROXY:
			CAVISTransactionsApp::s_pTc->debug("Firewall type specified as a \"proxy\"firewall.");
			break;

		case FW_SOCKS:
			CAVISTransactionsApp::s_pTc->debug("Firewall type specified as a \"socks\".");
			break;

		case FW_DEFINED_IN_REGISTRY:
			CAVISTransactionsApp::s_pTc->debug("Firewall/proxy specified in registry.");
			break;

		case FW_NONE:
			CAVISTransactionsApp::s_pTc->debug("Firewall type specified as \"none\".");
			break;
	}

	switch(s_fwType)
	{
		case FW_PROXY:
		case FW_SOCKS:
			dwAccessType = INTERNET_OPEN_TYPE_PROXY;
			//	   pszProxyName = strFireWall;

			// define proxy in "address:port" format
			//	   strProxyName.Format("%s:%d", strFireWall, iFireWallPort);
			pszProxyName = s_strProxyAddress;

			break;

		case FW_DEFINED_IN_REGISTRY:
			dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
			break;

		case FW_NONE:
			dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
			break;
	}

	// ensure that all instances of WinInet objects are closed and deleted before
	// continuing...
//	Close();

	// create an internet session object, required in MFC internet apps, in order to
	// get an http connection object later.

	CAVISTransactionsApp::s_pTc->debug("Creating new instance of InternetSession object...");
	m_pInetSession = new CAVISInternetSession(
		m_strAgentName,	// use application name as agent name
		::GetCurrentThreadId(),	// context id
		dwAccessType,	// proxy or direct access or INTERNET_OPEN_TYPE_PRECONFIG,
		pszProxyName,	// proxy name (if necessary)
		NULL);			// no proxy bypass
//			INTERNET_FLAG_DONT_CACHE);	// no caching or asynch stuff
	CAVISTransactionsApp::s_pTc->debug("Created new instance of InternetSession object.");

	// if logging flag is set (through calling EnableWinInetLogging())
	if (s_boEnableWinInetLogging)
	{
		// enable callbacks to record internet events (throws exception in the event
		// of an error).
		m_pInetSession->EnableStatusCallback(s_boEnableWinInetLogging);
	}

	UINT uiThreadId = ::GetCurrentThreadId();

	if (s_iConnectRetries != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, s_iConnectRetries))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iConnectTimeout != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, s_iConnectTimeout))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iSendRequestTimeout != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, s_iSendRequestTimeout))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iSendRequestDataTimeout != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, s_iSendRequestDataTimeout))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iReceiveResponseTimeout != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, s_iReceiveResponseTimeout))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iReceiveResponseDataTimeout != -1)
		if (!m_pInetSession->SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, s_iReceiveResponseDataTimeout))
			::AfxThrowInternetException(uiThreadId, ::GetLastError());

	if (!m_pInetSession->SetOption(INTERNET_OPTION_SETTINGS_CHANGED, TRUE))
		::AfxThrowInternetException(uiThreadId, ::GetLastError());

	// create an HttpConnection object, from which an HttpFile object can be created
	// The HttpFile object is used to open and issue an Http request
	TRY
	{
		CAVISTransactionsApp::s_pTc->debug("Creating new instance of HttpConnection object...");

		// throws exception in the event of an error
		m_pHttpConnection = m_pInetSession->GetHttpConnection(
			m_strGatewayAddress,	// gateway address
			m_nGatewayPort);		// gateway port
	//		pszUserName,	// user id
	//		pszPassword);	// password
		CAVISTransactionsApp::s_pTc->debug("Created new instance of HttpConnection object.");
	}
	CATCH (CInternetException, pExcept)
	{
		CAVISTransactionsApp::s_pTc->critical("Exception occurred while calling GetHttpConnection().");
		Close();

		if (AVISTX_CRITICAL_ERROR == ProcessInternetError(pExcept))
			// let the next exception handler catch this exception
			THROW_LAST();

		// The exception object should be deleted automatically by the framework
//		pExcept->Delete();
	}
	END_CATCH
}
/*****/
// close/delete all internet objects
void CAVISTransaction::Close(BOOL boCloseHttpFileOnly)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// close and delete the request handle
	EndRequest();

	if (boCloseHttpFileOnly)
		return;

	if (m_pHttpConnection != NULL)
	{
		CAVISTransactionsApp::s_pTc->debug("Closing HttpConnection object...");
		m_pHttpConnection->Close();
		CAVISTransactionsApp::s_pTc->debug("Closed HttpConnection object.");
		CAVISTransactionsApp::s_pTc->debug("Deleting HttpConnection object...");
		delete m_pHttpConnection;
		m_pHttpConnection = NULL;
		CAVISTransactionsApp::s_pTc->debug("Deleted HttpConnection object.");
	}

	if (m_pInetSession != NULL)
	{
		CAVISTransactionsApp::s_pTc->debug("Closing InternetSession object...");
		m_pInetSession->Close();
		CAVISTransactionsApp::s_pTc->debug("Closed InternetSession object.");
		CAVISTransactionsApp::s_pTc->debug("Deleting InternetSession object...");
		delete m_pInetSession;
		m_pInetSession = NULL;
		CAVISTransactionsApp::s_pTc->debug("Deleted InternetSession object.");
	}
}
/*****/
/*****/
/*****/
/* The following is a helper class to get around the Microsoft problem.  Just trying
   to provide access to the protected m_hFile. */
class CAVISHttpFile : public CHttpFile  
{
public:
	BOOL GetCustomHeaderValue(const CString& strHeaderName, CString& strHeaderValue);
	BOOL IsHFileValid() { return m_hFile != NULL; };
	BOOL IsValidRequestHandle();
	BOOL CancelRequest() { return ::InternetCloseHandle(m_hFile); }
};
/*****/
/*****/
/*****/
void CAVISTransaction::OpenRequest()
{
	// flags for OpenRequest
	DWORD dwFlags = INTERNET_FLAG_DONT_CACHE |
				    INTERNET_FLAG_NO_AUTO_REDIRECT |
				    INTERNET_FLAG_NO_COOKIES |
				    INTERNET_FLAG_NO_UI |
				    INTERNET_FLAG_PRAGMA_NOCACHE |
				    INTERNET_FLAG_RELOAD;

	if (m_boUseSSL)
	{
		CAVISTransactionsApp::s_pTc->debug("Enabled flags for using SSL in Http request.");
		dwFlags |= INTERNET_FLAG_SECURE;

		if (s_boIgnoreInvalidCertificateServerName)
		{
			CAVISTransactionsApp::s_pTc->debug("Ignoring invalid certificate server names.");
			dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
		}
	}
	else
	{
		CAVISTransactionsApp::s_pTc->debug("Disabled flags for using SSL in Http request.");
	}

	// ensure that if this transaction is being reused, that the CHttpFile object
	// was closed before recreating it in the OpenRequest() call.
	// only close and delete the HttpFile object.
	Close(TRUE);

	// Create an HttpFile object, from which Http requests can be executed
	CAVISTransactionsApp::s_pTc->debug("Creating new HttpFile object...");
	m_pHttpFile = m_pHttpConnection->OpenRequest(
		m_nVerb,		// verb
		m_strTarget,	// target and query
		NULL,			// referrer
		0,				// context id (must be 0 for synchronous-mode calls)
		NULL,			// accept all types
		NULL,			// use "HTTP/1.0"
		dwFlags);		// internet flags

	// handle error
	if (m_pHttpFile == NULL)
	{
		CAVISTransactionsApp::s_pTc->critical("Failed to OpenRequest.");

		// *** throw an "out of memory" exception
		::AfxThrowMemoryException();
	}

	// Microsoft's MFC OpenRequest() method doesn't check to see that the WinInet 
	// HttpOpenRequest() call returns a valid handle, so we must do it here.
	if (!((CAVISHttpFile* )m_pHttpFile)->IsHFileValid())
	{
		CAVISTransactionsApp::s_pTc->critical("Error occuring during OpenRequest().");

		// throw exception
		::AfxThrowInternetException(m_pHttpConnection->GetContext(), ::GetLastError());
	}

	CAVISTransactionsApp::s_pTc->debug("Created new HttpFile object.");
}
/*****/
DWORD CAVISTransaction::InitRequest()
{
	// does nothing
	return AVISTX_SUCCESS;
}
/*****/
/* Execute the request. Content may be specified to be sent. Will open the Http request,
   then add any headers specified in the constructor, then call the StartRequest()
   method. StartRequest() is a virtual method which can be overriden if the default
   does not suffice. The default will simply perform a default SendRequest(), sending
   no content and receiving no content.

*/
int CAVISTransaction::Execute(
	int cbSendContent,		// how much data to send
	PVOID pvSendContent)	// data to be sent
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_strGatewayError = "";

	// create a new request (an HttpFile object).  This must be done to ensure that
	// headers added to a previous request do not carry over when reusing that
	// request (the HttpFile object). Since there is no automatic way to clear the
	// header from the request, we must simply delete the old and create a new one.
	// i.e. a new request/HttpFile object must be created each time the transaction
	// is executed (each time this method is called).
	OpenRequest();

	// allow derived classes to do any initialization before the request is executed
	CAVISTransactionsApp::s_pTc->debug("Initializing request...");
	int rc = InitRequest();
	CAVISTransactionsApp::s_pTc->debug("Initialized request.");
	if (AVISTX_SUCCESS != rc)
	{
		CAVISTransactionsApp::s_pTc->error("Error occurred while initializing request.");
		EndRequest();
		return rc;
	}

	CAVISTransactionsApp::s_pTc->debug("Executing request...");

	// add the headers
	if (!m_strRequestHeaders.IsEmpty())
	{
		// valid handle?
		if (!IsValidRequestHandle())
		{
			EndRequest();

			if (CThread::ReceivedQuitMessage(FALSE))
			{
				return AVISTX_TERMINATE;
			}
			else
			{
				throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
			}
		}

		// add the header if it doesn't exist; otherwise, replace it
		m_pHttpFile->AddRequestHeaders(m_strRequestHeaders, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
		CAVISTransactionsApp::s_pTc->debug("Added request headers as passed into the transaction constructor (hdrs = %s).", m_strRequestHeaders);
	}

	// virtual call; should be overriden ONLY if need to send request content
	// data in blocks; default sends the request with the specified request
	// content, if any
	CAVISTransactionsApp::s_pTc->debug("Starting request...");
	rc = StartRequest(cbSendContent, pvSendContent);
	CAVISTransactionsApp::s_pTc->debug("Started request.");

	// don't need to read response content if error during request
	if (rc != AVISTX_SUCCESS)
	{
		CAVISTransactionsApp::s_pTc->error("Error occurred while starting request.");
		EndRequest();
		return rc;
	}

	// check for the "X-Error" header
	if (FindAVISGatewayErrorHeaderValue())
	{
		// Only throw the exception if we are NOT ignoring when X-error is "crumbled"
		// *OR* when we ARE ignoring it, and "X-error" has a value other than "crumbled.
		if (!m_boIgnoreXErrorCrumbled || 
			(m_boIgnoreXErrorCrumbled && (m_strGatewayError != XError::Crumbled()) ))
		{
			CString strMsg;
			strMsg.Format("Gateway returned error: %s", m_strGatewayError);
			EndRequest();

			throw new CAVISTransactionException(CAVISTransactionException::gatewayError, strMsg);
		}

		// If we are ignoring the case when X-error == "crumbled", then we must
		// continue as if there were no error.
		CAVISTransactionsApp::s_pTc->debug("\"X-Error\" header is \"crumbled\"; ignoring...");
	}

	// Only need to read response content if http return code says 'ok' and there
	// is a 'content-length' header present.
	if ((HTTP_STATUS_OK == m_dwResponseStatusCode) &&
		(GetResponseContentLength() != -1))
	{
		// virtual call; should be overriden ONLY if need to read response content
		// default does nothing
		CAVISTransactionsApp::s_pTc->debug("Reading response content...");
		rc = ReadResponseContent();
		CAVISTransactionsApp::s_pTc->debug("Finished reading response content.");
	}
	else
	{
		CAVISTransactionsApp::s_pTc->debug("No content in response.");
	}

	EndRequest();
	return rc;
}
/*****/
BOOL CAVISHttpFile::GetCustomHeaderValue(const CString& strHeaderName, CString& strHeaderValue)
{
	// *** Thanks, Microsoft, for blowing an assert on QueryInfo() when querying for
	// *** custom headers! Thus, must insert MFC QueryInfo() code, without
	// *** Microsoft's ASSERT.

//	ASSERT(dwInfoLevel <= HTTP_QUERY_MAX && dwInfoLevel >= 0);
	ASSERT(m_hFile != NULL);

	BOOL bRet;

	// ask for nothing to see how long the return really is

	// *** Microsoft obviously didn't plan for custom header queries, since they
	// *** blank out the header here!

	// tell HttpQueryInfo() how long the custom header is; upon return, dwLen
	// will contain the length of the custom header value.

	char buf[4*1024]; // make a large buffer, so that it can hold any-sized header name and value
	DWORD cbBuf = sizeof(buf);
	::ZeroMemory(buf, cbBuf);
	::strcpy(buf, (LPCSTR )strHeaderName);

	if (::HttpQueryInfo(m_hFile, HTTP_QUERY_CUSTOM, buf, &cbBuf, 0))
	{
		// ensure the header is null-terminated
		strHeaderValue = buf;

		bRet = TRUE;
	}
	else // couldn't find the custom header
	{
		bRet = FALSE;
	}

	return bRet;
}
/*****/
BOOL CAVISTransaction::GetCustomHeaderValue(const CString& strHeaderName, CString& strHeaderValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// cast to a custom helper class which has access to protected members of CHttpFile
	CAVISHttpFile* pFile = (CAVISHttpFile* )m_pHttpFile;

	BOOL boRc = pFile->GetCustomHeaderValue(strHeaderName, strHeaderValue);

	if (boRc)
		return TRUE;

	// if didn't find header, try again, with '_' instead of '-' character
	CString strNewHeaderName = strHeaderName;
	strNewHeaderName.Replace("-", "_");

	boRc = pFile->GetCustomHeaderValue(strNewHeaderName, strHeaderValue);

	return boRc;
}
/*****/
BOOL CAVISTransaction::FindAVISGatewayErrorHeaderValue()
{
	return GetCustomHeaderValue(AttributeKeys::Error().c_str(), m_strGatewayError);
}
/*****/
BOOL CAVISTransaction::FindAnalysisCookieHeaderValue()
{
	CString strResponseAnalysisCookie;
	if (GetCustomHeaderValue(AttributeKeys::AnalysisCookie().c_str(), strResponseAnalysisCookie))
	{
		m_uiResponseAnalysisCookie = ::atoi((LPCTSTR )strResponseAnalysisCookie);
		return TRUE;
	}

	return FALSE;
}
/*****/
/* This function performs a WinInet SendRequest(), which will send any content 
   specified, in one large block write.  Override this function in a derived class 
   in order to send content in blocks. */
int CAVISTransaction::StartRequest(
	DWORD cbSendContent,	// how much data to send
	PVOID pvSendContent)	// data to send
{
	return SendRequest(cbSendContent, pvSendContent);
}
/*****/
void CAVISTransaction::EndRequest()
{
	// delete the request object if it exists
	if (m_pHttpFile != NULL)
	{
		// first close the request handle
		CAVISTransactionsApp::s_pTc->debug("Closing HttpFile object...");

		// valid handle?
		if (IsValidRequestHandle())
		{
			m_pHttpFile->Close();
			CAVISTransactionsApp::s_pTc->debug("Closed HttpFile object.");
		}

		// now delete the request object and references to it
		CAVISTransactionsApp::s_pTc->debug("Deleting HttpFile object...");
		delete m_pHttpFile;
		m_pHttpFile = NULL;
		CAVISTransactionsApp::s_pTc->debug("Deleted HttpFile object.");
	}

	// Set the event to signaled, so waiting threads can continue (transaction is
	// not running)
	::SetEvent(m_hEventTxRunning);
}
/*****/
/* This function sends the request content in one fell-swoop. class
   must have all of the data to write before this call is made.  Will be useful
   in situations when content size is small. */
int CAVISTransaction::SendRequest(
	DWORD cbRequestContent,
	PVOID pvRequestContent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD dwErr;

	TRY
	{
		// valid handle?
		if (!IsValidRequestHandle())
		{
			if (CThread::ReceivedQuitMessage(FALSE))
			{
				return AVISTX_TERMINATE;
			}
			else
			{
				EndRequest();

				throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
			}
		}

		CAVISTransactionsApp::s_pTc->debug("Sending request, %d bytes of request content...", cbRequestContent);

		// tell the timeout monitor thread that we are starting a transaction
		m_pthreadTransactionTimeoutMonitor->PostTransactionStartingMessage(
			m_uiTimeoutTransactionConnected);

		// force waiting threads to block
		::ResetEvent(m_hEventTxRunning);

		m_pHttpFile->SendRequest(NULL, 0, pvRequestContent, cbRequestContent);
		CAVISTransactionsApp::s_pTc->debug("Sent request.");

		// get the analysis cookie from the response headers
		FindAnalysisCookieHeaderValue();

		// get the response status code
		CAVISTransactionsApp::s_pTc->debug("Retrieving response status code...");
		m_pHttpFile->QueryInfoStatusCode(m_dwResponseStatusCode);
		CAVISTransactionsApp::s_pTc->debug("Retrieved response status code...");

		dwErr = AVISTX_SUCCESS;
	}
	CATCH(CInternetException, pExcept)
	{
		CAVISTransactionsApp::s_pTc->critical("Failed sending request.");

		dwErr = ProcessInternetError(pExcept);

		if (AVISTX_CRITICAL_ERROR == dwErr)
		{
			// tell the timeout monitor thread that we have completed a transaction
			m_pthreadTransactionTimeoutMonitor->PostTransactionCompletedMessage();

			CAVISTransactionsApp::s_pTc->debug("Rethrowing critical exception...");

			// let the next exception handler catch this exception
			THROW_LAST();
		}

		// The exception object should be deleted automatically by the framework
//		pExcept->Delete();

	}
	END_CATCH

	// tell the timeout monitor thread that we have connected and completed a transaction
	m_pthreadTransactionTimeoutMonitor->PostTransactionConnectedMessage(m_uiTimeoutTransactionCompleted);
	m_pthreadTransactionTimeoutMonitor->PostTransactionCompletedMessage();

	return dwErr;
}
/*****/
/* This function allows the calling derived class to send request content in chunks.
   This is beneficial when the amount of data to send is large or is not all available
   at the time when the sending needs to begin. The derived class must override the
   GetContentBlock() virtual function in order to provide the consecutive blocks of
   content data to write. */
int CAVISTransaction::SendRequestEx(
	DWORD cbRequestContent,
	DWORD nBlockSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL done = FALSE;
	DWORD cbTotalWritten = 0;
	int cbToWrite;

	CAVISTransactionsApp::s_pTc->debug("Sending request (content sent in blocks)...");

	// allocate memory for the content block
	PVOID pvRequestContentBlock = (PVOID )new char[nBlockSize];

	if (pvRequestContentBlock == NULL)
	{
		CAVISTransactionsApp::s_pTc->critical("Unable to allocate memory for request block.");
		AfxThrowMemoryException();
	}

	// start the request

	TRY
	{
		// valid handle?
		if (!IsValidRequestHandle())
		{
			if (CThread::ReceivedQuitMessage(FALSE))
			{
				return AVISTX_TERMINATE;
			}
			else
			{
				EndRequest();

				throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
			}
		}

		CAVISTransactionsApp::s_pTc->debug("Calling SendRequestEx()...");

		// tell the timeout monitor thread that we are starting a transaction
		m_pthreadTransactionTimeoutMonitor->PostTransactionStartingMessage(
			m_uiTimeoutTransactionConnected);

		// force waiting threads to block
		::ResetEvent(m_hEventTxRunning);
		m_pHttpFile->SendRequestEx(cbRequestContent);
		CAVISTransactionsApp::s_pTc->debug("Called SendRequestEx()...");
	}
	CATCH(CInternetException, pExcept)
	{
		CAVISTransactionsApp::s_pTc->critical("Failed calling SendRequestEx()");

		DWORD dwErr = ProcessInternetError(pExcept);

		// we are finished with this transaction; no need to monitor it anymore
		m_pthreadTransactionTimeoutMonitor->PostTransactionCompletedMessage();

		if (AVISTX_CRITICAL_ERROR == dwErr)
			// let the next exception handler catch this exception
			THROW_LAST();

		// The exception object should be deleted automatically by the framework
//		pExcept->Delete();

		// de-allocate the memory for the content block
		delete [] pvRequestContentBlock;

		return dwErr;
	}
	END_CATCH

	// tell the timeout monitor thread that we have connected a transaction
	m_pthreadTransactionTimeoutMonitor->PostTransactionConnectedMessage(m_uiTimeoutTransactionCompleted);

	while (cbTotalWritten < cbRequestContent)
	{
		// GetContentBlock() returns the number of bytes to write
		CAVISTransactionsApp::s_pTc->debug("Getting next request content block...");
		cbToWrite = GetNextRequestContentBlock(pvRequestContentBlock);
		CAVISTransactionsApp::s_pTc->debug("Got next request content block.");

		// write the content bytes out as request content
		TRY
		{
			// valid handle?
			if (!IsValidRequestHandle())
			{
				if (CThread::ReceivedQuitMessage(FALSE))
				{
					return AVISTX_TERMINATE;
				}
				else
				{
					EndRequest();

					throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
				}
			}

			CAVISTransactionsApp::s_pTc->debug("Writing content block as request content, %d bytes...", cbToWrite);
			m_pHttpFile->Write(pvRequestContentBlock, cbToWrite);
			CAVISTransactionsApp::s_pTc->debug("Wrote content block as request content.");
		}
		CATCH(CInternetException, pExcept)
		{
			// we are finished with this transaction; no need to monitor it anymore
			m_pthreadTransactionTimeoutMonitor->PostTransactionCompletedMessage();

			CAVISTransactionsApp::s_pTc->error("Failed writing request content.");

			DWORD dwErr = ProcessInternetError(pExcept);

			if (AVISTX_CRITICAL_ERROR == dwErr)
				// let the next exception handler catch this exception
				THROW_LAST();

			// The exception object should be deleted automatically by the framework
//			pExcept->Delete();

			// de-allocate the memory for the content block
			delete [] pvRequestContentBlock;

			return dwErr;
		}
		END_CATCH

		// accumulate the total number of bytes written
		cbTotalWritten += cbToWrite;
	}

	// de-allocate the memory for the content block
	delete [] pvRequestContentBlock;

	// tell the timeout monitor thread that we have completed the transaction
	m_pthreadTransactionTimeoutMonitor->PostTransactionCompletedMessage();

	// done sending data
	TRY
	{
		CAVISTransactionsApp::s_pTc->debug("Ending request...");
		m_pHttpFile->EndRequest();
		CAVISTransactionsApp::s_pTc->debug("Ended request...");
	}
	CATCH(CInternetException, pExcept)
	{
		CAVISTransactionsApp::s_pTc->error("Failed ending request.");

		DWORD dwErr = ProcessInternetError(pExcept);

		if (AVISTX_CRITICAL_ERROR == dwErr)
			// let the next exception handler catch this exception
			THROW_LAST();

		// The exception object should be deleted automatically by the framework
//		pExcept->Delete();

		return dwErr;
	}
	END_CATCH

	FindAnalysisCookieHeaderValue();

	// get the response status code
	CAVISTransactionsApp::s_pTc->debug("Retrieving response status code...");
	m_pHttpFile->QueryInfoStatusCode(m_dwResponseStatusCode);
	CAVISTransactionsApp::s_pTc->debug("Retrieved response status code...");

	return AVISTX_SUCCESS;
}
/*****/
/* This function MUST be overriden when the SendContentEx() */
int CAVISTransaction::GetNextRequestContentBlock(PVOID pvRequestContentBlock)
{
	// return the number of bytes read
	return 0;
}
/*****/
int CAVISTransaction::ReadResponseContent()
{
	return AVISTX_SUCCESS;
}
/*****/
int CAVISTransaction::GetResponseContentLength()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// valid handle?
	if (!IsValidRequestHandle())
	{
		if (CThread::ReceivedQuitMessage(FALSE))
		{
			return -1;
		}
		else
		{
			EndRequest();

			throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
		}
	}

	int cbResponseContentLength;
	DWORD cbSize = sizeof(cbResponseContentLength);

	// retrieve content-length header value
	if (m_pHttpFile->QueryInfo(
		HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, 
		&cbResponseContentLength,
		&cbSize))
		return cbResponseContentLength;

	// *** check return value ***
	ProcessInternetError();

	return -1;
}
/*****/
int CAVISTransaction::ReadNextResponseContentBlock(PVOID pvResponseContentBlock, DWORD cbToRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	UINT uiBytesRead;

	TRY
	{
		CAVISTransactionsApp::s_pTc->debug("Reading response content...");

		// valid handle?
		if (!IsValidRequestHandle())
		{
			if (!CThread::ReceivedQuitMessage(FALSE))
			{
				EndRequest();

				throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
			}
			else
				return RNRCB_TERMINATE_TX;
		}

		uiBytesRead = m_pHttpFile->Read(pvResponseContentBlock, cbToRead);
		CAVISTransactionsApp::s_pTc->debug("Read response content.");
	}
	CATCH (CInternetException, pExcept)
	{
		CAVISTransactionsApp::s_pTc->critical("Failed reading response content.");

		DWORD dwErr = ProcessInternetError(pExcept);

		if (AVISTX_CRITICAL_ERROR == dwErr)
			// let the next exception handler catch this exception
			THROW_LAST();

		// The exception object should be deleted automatically by the framework
//		pExcept->Delete();

		if (AVISTX_NETWORK_FAILURE == dwErr)
			return RNRCB_NETWORK_ERROR;
		else
			return RNRCB_TERMINATE_TX;			
	}
	END_CATCH

	return uiBytesRead;
}
/*****/
int CAVISTransaction::GetResponseHeaders(PVOID pvResponseHeaders, PDWORD pdwBufLength)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// valid handle?
	if (!IsValidRequestHandle())
	{
		if (CThread::ReceivedQuitMessage(FALSE))
		{
			return AVISTX_TERMINATE;
		}
		else
		{
			EndRequest();

			throw new CAVISTransactionException(CAVISTransactionException::invalidRequestHandle, "Invalid request handle");
		}
	}

	int rv = m_pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, pvResponseHeaders, pdwBufLength, NULL);

	PCHAR pHeaders = (PCHAR )pvResponseHeaders;
	for (DWORD i = 0; i < *pdwBufLength; i++)
	{
		// the following block is here to fix a bug in Mike's code
		// convert the "_" to "-"
		if (pHeaders[i] == '_')
			pHeaders[i] = '-';
	}

	return rv;
}
/*****/
void CAVISTransaction::AddRequestHeader(LPCSTR pszNewHeader)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_strRequestHeaders.IsEmpty())
		m_strRequestHeaders += "\n";

	m_strRequestHeaders += pszNewHeader;
}
/*****/
void CAVISTransaction::AddRequestHeader(CString& strNewHeader)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_strRequestHeaders.IsEmpty())
		m_strRequestHeaders += "\n";

	m_strRequestHeaders += strNewHeader;
}
/*****/
void CAVISTransaction::AddRequestHeader(std::string strHeaderName, std::string strHeaderValue)
{
	std::string strHeader = strHeaderName + ": " + strHeaderValue;

	AddRequestHeader(strHeader.c_str());
}
/*****/
void CAVISTransaction::AddRequestHeader(std::string strHeaderName, UINT uiHeaderValue)
{
	CString strHeader;
	strHeader.Format("%s: %u", strHeaderName.c_str(), uiHeaderValue);

	AddRequestHeader(strHeader);
}
/*****/
DWORD CAVISTransaction::GetResponseStatusCode()
{
	return m_dwResponseStatusCode;
}
/*****/
BOOL CAVISTransaction::SetReadBufferSize(DWORD dwBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_pHttpFile->SetReadBufferSize(dwBufSize);
}
/*****/
UINT CAVISTransaction::GetResponseAnalysisCookie()
{
	return m_uiResponseAnalysisCookie;
}
/*****/
void CAVISTransaction::EmptyRequestHeaders()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_strRequestHeaders.Empty();
}
/*****/
CString CAVISTransaction::GetGatewayAddress()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_strGatewayAddress;
}
/*****/
BOOL CAVISTransaction::WaitForTransactionToComplete()
{
	// should wait for some timeout interval
	CAVISTransactionsApp::s_pTc->debug("Waiting for transaction to complete...");

	if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hEventTxRunning, 3000))
		CAVISTransactionsApp::s_pTc->debug("Timed out waiting for transaction to complete.");

	CAVISTransactionsApp::s_pTc->debug("Transaction has completed.");

	return TRUE;
}
/*****/
void CAVISTransaction::CancelTransaction()
/* This call is only made from a thread other than the one in which the transaction is
   running. Because of this, we should not delete the request object, since it may
   still be referenced by the transaction thread even after the transaction terminates.
   The transaction thread will delete the request object after the transaction
   terminates. */
{
	// ensure that the request object exists
	if (m_pHttpFile != NULL)
	{
		CAVISTransactionsApp::s_pTc->debug("Issuing \"Cancel Transaction\" request to running transaction...");
//		m_pHttpFile->Close();
		((CAVISHttpFile* )m_pHttpFile)->CancelRequest();
		CAVISTransactionsApp::s_pTc->debug("\"Cancel Transaction\" request was sent.");

		// check if the transaction is running
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hEventTxRunning, 0))
		{
			WaitForTransactionToComplete();
			CAVISTransactionsApp::s_pTc->debug("Transaction has completed.");
		}
	}
}
/*****/
void CAVISTransaction::SetIgnoreXErrorCrumbled()
{
	CAVISTransactionsApp::s_pTc->debug("Enabled flag to ignore the presence of the \"X-Error\" header when its value is \"crumbled\".");
	m_boIgnoreXErrorCrumbled = TRUE;
}
/*****/
BOOL CAVISHttpFile::IsValidRequestHandle()
{
	if (m_hFile == NULL)
		return FALSE;

	if (::AfxGetInternetHandleType(m_hFile) == INTERNET_HANDLE_TYPE_HTTP_REQUEST)
		return TRUE;
	else
		return FALSE;
}
/*****/
BOOL CAVISTransaction::IsValidRequestHandle()
{
	return ((CAVISHttpFile* )m_pHttpFile)->IsValidRequestHandle();
}
