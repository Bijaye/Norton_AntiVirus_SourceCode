#ifndef __HTTPFILE__H__
#define __HTTPFILE__H__

#include <string>
//#include <afxinet.h>
#include <wininet.h>

//class XHttpFile : public CHttpFile
//{
//public:
//	XHttpFile(HINTERNET hFile, HINTERNET hSession, LPCTSTR pstrObject,
//		LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD dwContext);
//};



class InternetFile
{
protected:
	InternetFile(	HINTERNET	hFile, 
					HINTERNET	hSession,
					LPCTSTR		pstrFileName, 
					LPCTSTR		pstrServer, 
					DWORD		dwContext,
					BOOL		bReadMode	);

public:
	virtual UINT	Read(void* lpBuf, UINT nCount);
	virtual void	Write(const void* lpBuf, UINT nCount);
	virtual void	Close();
	BOOL			SetReadBufferSize(UINT nReadSize){ return TRUE; }// for backward compatibility only
	BOOL			IsHFileValid() { return _hFile != NULL; };// for backward compatibility

protected:
	HINTERNET		_hFile;
	BOOL			_readMode;
	DWORD			_context;
	HINTERNET		_connection;
	std::string		_serverName;
	std::string		_fileName;
};


class ZHttpFile : public InternetFile
{
public:
	ZHttpFile(	HINTERNET	hFile, 
				HINTERNET	hSession, 
				LPCTSTR		pstrObject,
				LPCTSTR		pstrServer, 
				LPCTSTR		pstrVerb, 
				DWORD		dwContext);

	BOOL AddRequestHeaders(	LPCTSTR		pstrHeaders,
							DWORD		dwFlags		 = HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE, 
							int			dwHeadersLen = -1 );

	BOOL SendRequest(		LPCTSTR		pstrHeaders		= NULL, 
							DWORD		dwHeadersLen	= 0,
							LPVOID		lpOptional		= NULL, 
							DWORD		dwOptionalLen	= 0);
	BOOL EndRequest(		DWORD		dwFlags = 0,
							LPINTERNET_BUFFERS lpBuffIn = NULL, 
							DWORD		dwContext = 1);

	BOOL QueryInfo(			DWORD		dwInfoLevel, 
							LPVOID		lpvBuffer,
							LPDWORD		lpdwBufferLength, 
							LPDWORD		lpdwIndex = NULL) const;
	BOOL QueryInfoStatusCode(DWORD& dwStatusCode) const;

	bool GetCustomHeaderValue(	const std::string&	headerName, 
								std::string&		headerValue);

	bool GetCustomHeaderValue2(	const std::string&	headerName, // both '_' or '-'
								std::string&		headerValue);

	bool GetResponseHeaders	( std::string& headers );

protected:
	std::string		_object;
	std::string		_verb;
};

#endif __HTTPFILE__H__
