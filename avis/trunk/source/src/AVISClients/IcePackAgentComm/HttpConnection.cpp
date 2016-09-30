// ------ standard headers --------------------------------------------
#include "base.h"

#include "InternetSession.h"
#include "HttpFile.h"


//------ static data --------------------------------------------------

const LPCTSTR HttpConnection::szHtmlVerbs[] = {
	_T("POST"),
	_T("GET"),
	_T("HEAD"),
	_T("PUT"),
	_T("LINK"),
	_T("DELETE"),
	_T("UNLINK"),
};

//------ HttpConnection -----------------------------------------------

//
// constructor
//
HttpConnection::HttpConnection	(	InternetSession&		Session, 
									LPCTSTR					pstrServer,
									INTERNET_PORT			nPort,
									LPCTSTR					pstrUserName, 
									LPCTSTR					pstrPassword,
									DWORD					dwContext)
	:	_session( Session ), 
		_connection( NULL ),
		_serverName( pstrServer ), 
		_port( nPort ),
		_context( dwContext )
{
//	ASSERT(pSession != NULL);
//	ASSERT_KINDOF(CInternetSession, pSession);
//	ASSERT(AfxIsValidString(pstrServer));

	_connection = InternetConnect( Session.GetHandle(), pstrServer,
		nPort, pstrUserName, pstrPassword, INTERNET_SERVICE_HTTP,
		0, _context);

	if ( _connection == NULL )
		ThrowInternetException( _context );
}


//
// destructor
//
HttpConnection::~HttpConnection()
{
}


//
// OpenRequest
//
ZHttpFile* HttpConnection::OpenRequest		(	int						nVerb, 
												LPCTSTR					pstrObjectName,
												LPCTSTR					pstrReferer			/*= NULL*/, 
												DWORD					dwContext			/*= 1*/,
												LPCTSTR*				ppstrAcceptTypes	/*= NULL*/, 
												LPCTSTR					pstrVersion			/*= NULL*/,
												DWORD					dwFlags				/*= INTERNET_FLAG_EXISTING_CONNECT*/)
{
//	ASSERT_VALID(this);
//	ASSERT(m_hConnection != NULL);
//	ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0);
//	ASSERT(AfxIsValidString(pstrObjectName));

//	ASSERT(nVerb >= _HTTP_VERB_MIN && nVerb <= _HTTP_VERB_MAX);

	LPCTSTR pstrVerb;
	if (nVerb >= _HTTP_VERB_MIN && nVerb <= _HTTP_VERB_MAX)
		pstrVerb = szHtmlVerbs[nVerb];
	else
		pstrVerb = _T("");

	return OpenRequest(pstrVerb, pstrObjectName, pstrReferer,
		dwContext, ppstrAcceptTypes, pstrVersion, dwFlags);
}


//
// OpenRequest
//
ZHttpFile* HttpConnection::OpenRequest(LPCTSTR pstrVerb,
	LPCTSTR pstrObjectName, LPCTSTR pstrReferer, DWORD dwContext,
	LPCTSTR* ppstrAcceptTypes, LPCTSTR pstrVersion, DWORD dwFlags)
{
//	ASSERT_VALID(this);
//	ASSERT(m_hConnection != NULL);
//	ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0);

	if (dwContext == 1)
		dwContext = _context;

	if (pstrVersion == NULL)
		pstrVersion = HTTP_VERSION;

	HINTERNET hFile;
	hFile = HttpOpenRequest( _connection, pstrVerb, pstrObjectName,
		pstrVersion, pstrReferer, ppstrAcceptTypes, dwFlags, dwContext);

////	CHttpFile* pRet = new CHttpFile(hFile, pstrVerb, pstrObjectName, this);
////	pRet->m_dwContext = dwContext;
	ZHttpFile* pRet = new ZHttpFile(	hFile,
							  			_session.GetHandle(),
							  			pstrObjectName,
							  			_serverName.c_str(),
							  			pstrVerb,
							  			dwContext	);

	return pRet;
}


//
// Close
//
void HttpConnection::Close()
{
	if ( _connection != NULL)
	{
		InternetCloseHandle( _connection);
		_connection = NULL;
	}
}


