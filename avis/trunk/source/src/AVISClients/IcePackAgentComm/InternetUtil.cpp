//------ standard headers ---------------------------------------------
#include "base.h"

#include "InternetUtil.h"
#include "StrUtil.h"

//------ local definitions --------------------------------------------

//#define AFX_INET_SERVICE_FTP        INTERNET_SERVICE_FTP
//#define AFX_INET_SERVICE_HTTP       INTERNET_SERVICE_HTTP
//#define AFX_INET_SERVICE_GOPHER     INTERNET_SERVICE_GOPHER

// these are types that MFC parsing functions understand

//#define AFX_INET_SERVICE_UNK        0x1000
//#define AFX_INET_SERVICE_FILE       (AFX_INET_SERVICE_UNK+1)
//#define AFX_INET_SERVICE_MAILTO     (AFX_INET_SERVICE_UNK+2)
//#define AFX_INET_SERVICE_MID        (AFX_INET_SERVICE_UNK+3)
//#define AFX_INET_SERVICE_CID        (AFX_INET_SERVICE_UNK+4)
//#define AFX_INET_SERVICE_NEWS       (AFX_INET_SERVICE_UNK+5)
//#define AFX_INET_SERVICE_NNTP       (AFX_INET_SERVICE_UNK+6)
//#define AFX_INET_SERVICE_PROSPERO   (AFX_INET_SERVICE_UNK+7)
//#define AFX_INET_SERVICE_TELNET     (AFX_INET_SERVICE_UNK+8)
//#define AFX_INET_SERVICE_WAIS       (AFX_INET_SERVICE_UNK+9)
//#define AFX_INET_SERVICE_AFS        (AFX_INET_SERVICE_UNK+10)
//#define AFX_INET_SERVICE_HTTPS      (AFX_INET_SERVICE_UNK+11)


//------ static functions ---------------------------------------------


static _ParseURLWorker(	LPCTSTR				pstrURL,
						LPURL_COMPONENTS	lpComponents, 
						INTERNET_PORT&		nPort, 
						DWORD				dwFlags)
{
	// this function will return bogus stuff if lpComponents
	// isn't set up to copy the components

//	ASSERT(lpComponents != NULL && pstrURL != NULL);
	if (lpComponents == NULL || pstrURL == NULL)
		return FALSE;
//	ASSERT(lpComponents->dwHostNameLength == 0 ||
//			lpComponents->lpszHostName != NULL);
//	ASSERT(lpComponents->dwUrlPathLength == 0 ||
//			lpComponents->lpszUrlPath != NULL);
//	ASSERT(lpComponents->dwUserNameLength == 0 ||
//			lpComponents->lpszUserName != NULL);
//	ASSERT(lpComponents->dwPasswordLength == 0 ||
//			lpComponents->lpszPassword != NULL);

//	ASSERT(AfxIsValidAddress(lpComponents, sizeof(URL_COMPONENTS), TRUE));

	LPTSTR pstrCanonicalizedURL;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	BOOL bRetVal;
	BOOL bMustFree = FALSE;
	DWORD dwCanonicalizeFlags = dwFlags &
		(ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META |
		ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);
	DWORD dwCrackFlags = dwFlags & (ICU_ESCAPE | ICU_USERNAME);

	bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,
		&dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		bMustFree = TRUE;
		bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return FALSE;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,
						dwCrackFlags, lpComponents);
	if (bMustFree)
		delete [] pstrCanonicalizedURL;

	// convert to MFC-style service ID

	if (!bRetVal)
//		dwServiceType = AFX_INET_SERVICE_UNK
		;
	else
//	{
		nPort = lpComponents->nPort;
//		switch (lpComponents->nScheme)
//		{
//		case INTERNET_SCHEME_FTP:
//			dwServiceType = AFX_INET_SERVICE_FTP;
//			break;
//
//		case INTERNET_SCHEME_GOPHER:
//			dwServiceType = AFX_INET_SERVICE_GOPHER;
//			break;
//
//		case INTERNET_SCHEME_HTTP:
//			dwServiceType = AFX_INET_SERVICE_HTTP;
//			break;
//
//		case INTERNET_SCHEME_HTTPS:
//			dwServiceType = AFX_INET_SERVICE_HTTPS;
//			break;
//
//		case INTERNET_SCHEME_FILE:
//			dwServiceType = AFX_INET_SERVICE_FILE;
//			break;
//
//		case INTERNET_SCHEME_NEWS:
//			dwServiceType = AFX_INET_SERVICE_NNTP;
//			break;
//
//		case INTERNET_SCHEME_MAILTO:
//			dwServiceType = AFX_INET_SERVICE_MAILTO;
//			break;
//
//		default:
//			dwServiceType = AFX_INET_SERVICE_UNK;
//		}
//	}

	return bRetVal;
}



//------ InternetUtil -------------------------------------------------

//
// ParseURL
//
bool InternetUtil::ParseURL(	LPCTSTR			pstrURL, 
								std::string&	server, 
								INTERNET_PORT&	nPort	)
{
//	dwServiceType = AFX_INET_SERVICE_UNK;

//	ASSERT(pstrURL != NULL);
	if (pstrURL == NULL)
		return FALSE;

	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

	TCHAR	_server	[ INTERNET_MAX_URL_LENGTH+1 ];
	TCHAR	_object [ INTERNET_MAX_URL_LENGTH+1 ];

	memset(	_server, 0, INTERNET_MAX_URL_LENGTH+1 );
	memset(	_object, 0, INTERNET_MAX_URL_LENGTH+1 );

	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = _server;
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = _object;


	bool bRetVal = _ParseURLWorker(pstrURL, &urlComponents,	nPort, ICU_BROWSER_MODE) ? true : false;

//	strServer.ReleaseBuffer();
//	strObject.ReleaseBuffer();

	server = _server;

	return bRetVal;
}


//
// ProcessInternetError
//

DWORD InternetUtil::ProcessInternetError( MyTraceClient& _tc, InternetException* pExcept)
{
	DWORD dwErr;

	if (pExcept != NULL)
	{
		char stError[512];
		pExcept->GetErrorMessage(stError,512);

		std::string s = stError;
		StrUtil::replaceAll( s, "\r", "" );
		StrUtil::replaceAll( s, "\n", "" );
		_tc.critical("internet exception, error = \"%s\"", s.c_str());

		dwErr = pExcept->m_dwError;
	}
	else
	{
		dwErr = ::GetLastError();

		// nothing to do if success
		if (dwErr == ERROR_SUCCESS)
			return AVISTX_SUCCESS;

		LPVOID lpMsgBuf;
		FormatMessage(
			  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			  FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS,
			  NULL,
			  dwErr,
			  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// Default language
			  (LPTSTR) &lpMsgBuf, 0, NULL );				// Process any inserts in lpMsgBuf.

    /*
     * Some muppet forgot to put the NULL check in here.
     * Actually, it just produces an ugly log entry, as opposed
     * to a grievous crash.
     */
    if (lpMsgBuf)
    {
		  _tc.critical("internet error = \"%s\"", lpMsgBuf);

		  // Free the buffer.
		  LocalFree( lpMsgBuf );
    }
	}

	switch(dwErr)
	{
		// recoverable errors (these errors are due to problems in the network)
		case ERROR_INTERNET_CANNOT_CONNECT:
		case ERROR_INTERNET_CONNECTION_ABORTED:
		case ERROR_INTERNET_CONNECTION_RESET:
		case ERROR_INTERNET_SERVER_UNREACHABLE:
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
		case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
		case ERROR_INTERNET_TIMEOUT:
			_tc.debug("error was due to a problem in the network, and is recoverable.");
			return AVISTX_NETWORK_FAILURE;

		// critical errors (expect that these errors can occur, but they 
		// are unrecoverable)
		case ERROR_HTTP_DOWNLEVEL_SERVER:
		case ERROR_HTTP_HEADER_ALREADY_EXISTS:
		case ERROR_HTTP_HEADER_NOT_FOUND:
		case ERROR_HTTP_INVALID_HEADER:
		case ERROR_HTTP_INVALID_QUERY_REQUEST:
		case ERROR_HTTP_REDIRECT_FAILED:
		case ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION:
		case ERROR_HTTP_INVALID_SERVER_RESPONSE:
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
		case ERROR_INTERNET_SEC_CERT_REVOKED:
		case ERROR_INTERNET_SEC_INVALID_CERT:
		case ERROR_INTERNET_SECURITY_CHANNEL_ERROR:
		case ERROR_INTERNET_SHUTDOWN:
		case ERROR_INTERNET_TCPIP_NOT_INSTALLED:
		case ERROR_INVALID_HANDLE:
			_tc.debug("error was unrecoverable.");
			if (pExcept == NULL)
				::ThrowInternetException(::GetCurrentThreadId(), dwErr);
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
		case ERROR_INTERNET_DISCONNECTED:
		case ERROR_INTERNET_FORCE_RETRY:
		case ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR:
		case ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR:
		case ERROR_INTERNET_HANDLE_EXISTS:
		case ERROR_INTERNET_INCORRECT_FORMAT:
		case ERROR_INTERNET_INCORRECT_PASSWORD:
		case ERROR_INTERNET_INCORRECT_USER_NAME:
		case ERROR_INTERNET_INSERT_CDROM:
		case ERROR_INTERNET_INVALID_OPERATION:
		case ERROR_INTERNET_INVALID_OPTION:
		case ERROR_INTERNET_ITEM_NOT_FOUND:
		case ERROR_INTERNET_LOGIN_FAILURE:
		case ERROR_INTERNET_NEED_UI:
		case ERROR_INTERNET_NO_CALLBACK:
		case ERROR_INTERNET_NO_CONTEXT:
		case ERROR_INTERNET_OPERATION_CANCELLED:
		case ERROR_INTERNET_OPTION_NOT_SETTABLE:
		case ERROR_INTERNET_PROTOCOL_NOT_FOUND:
		case ERROR_INTERNET_REDIRECT_SCHEME_CHANGE:
		case ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND:
		case ERROR_INTERNET_UNABLE_TO_CACHE_FILE:
		case ERROR_INTERNET_UNABLE_TO_DOWNLOAD_SCRIPT:
		case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
		case ERROR_NO_MORE_FILES:
		case ERROR_NO_MORE_ITEMS:
		default:
			_tc.debug("error was unrecoverable.");
			if (pExcept == NULL)
				::ThrowInternetException(::GetCurrentThreadId(), dwErr);
			return AVISTX_CRITICAL_ERROR;
	}
}
