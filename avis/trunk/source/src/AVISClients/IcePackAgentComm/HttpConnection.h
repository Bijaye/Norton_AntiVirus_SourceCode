#ifndef __HTTPCONNECTION__H__
#define __HTTPCONNECTION__H__

//#include <afxinet.h>
#include "HttpFile.h"
#include "MyTraceClient.h"
#include <string>

class InternetSession;

class HttpConnection
{
public:
	typedef enum	{
				_HTTP_VERB_MIN      = 0,
				HTTP_VERB_POST      = 0,
				HTTP_VERB_GET       = 1,
				HTTP_VERB_HEAD      = 2,
				HTTP_VERB_PUT       = 3,
				HTTP_VERB_LINK      = 4,
				HTTP_VERB_DELETE    = 5,
				HTTP_VERB_UNLINK    = 6,
				_HTTP_VERB_MAX      = 6,
  } connectionVerb;

	HttpConnection	(	InternetSession&		Session, 
						LPCTSTR					pstrServer,
						INTERNET_PORT			nPort			= INTERNET_INVALID_PORT_NUMBER,
						LPCTSTR					pstrUserName	= NULL, 
						LPCTSTR					pstrPassword	= NULL,
						DWORD					dwContext		= 1);

	ZHttpFile* 
	OpenRequest		(	int						nVerb, 
						LPCTSTR					pstrObjectName,
						LPCTSTR					pstrReferer			= NULL, 
						DWORD					dwContext			= 1,
						LPCTSTR*				ppstrAcceptTypes	= NULL, 
						LPCTSTR					pstrVersion			= NULL,
						DWORD					dwFlags				= INTERNET_FLAG_EXISTING_CONNECT);

	ZHttpFile*
	OpenRequest		(	LPCTSTR					pstrVerb,
						LPCTSTR					pstrObjectName,
						LPCTSTR					pstrReferer			= NULL, 
						DWORD					dwContext			= 1,
						LPCTSTR*				ppstrAcceptTypes	= NULL, 
						LPCTSTR					pstrVersion			= NULL,
						DWORD					dwFlags				= INTERNET_FLAG_EXISTING_CONNECT);

	~HttpConnection();
	virtual void Close();

private:
	HINTERNET				_connection;
	std::string				_serverName;
	INTERNET_PORT			_port;
	DWORD					_context;
	InternetSession&		_session;

	static const LPCTSTR	szHtmlVerbs[];
};

#endif __HTTPCONNECTION__H__
