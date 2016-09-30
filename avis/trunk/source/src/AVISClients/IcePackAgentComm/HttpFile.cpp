// ------ standard headers --------------------------------------------
#include "base.h"
#include <memory>

#include "InternetException.h"
#include "HttpFile.h"
#include "StrUtil.h"


//------ XHttpFile ----------------------------------------------------

//XHttpFile::XHttpFile(	HINTERNET hFile, 
//						HINTERNET hSession, 
//						LPCTSTR pstrObject,
//						LPCTSTR pstrServer, 
//						LPCTSTR pstrVerb, 
//						DWORD dwContext)
//		: 	CHttpFile(	hFile, 
//						hSession, 
//						pstrObject,
//						pstrServer, 
//						pstrVerb, 
//						dwContext)
//{
//}


//------ InternetFile -------------------------------------------------

//
// constructor
//
InternetFile::InternetFile(	HINTERNET	hFile, 
							HINTERNET	hSession,
							LPCTSTR		pstrFileName, 
							LPCTSTR		pstrServer, 
							DWORD		dwContext,
							BOOL		bReadMode	)
:	_context	( dwContext ),
	_fileName	( pstrFileName ),
	_serverName	( pstrServer ),
	_hFile		( hFile ),
	_readMode	( bReadMode ),
	_connection	( NULL )
{
}


//
// Close
//
void InternetFile::Close()
{
	if (_hFile != NULL)
	{
//		Flush();
		InternetCloseHandle(_hFile);
//		_afxSessionMap.RemoveKey(m_hFile);
		_hFile = NULL;

//		if (m_pbWriteBuffer != NULL)
//		{
//			delete [] m_pbWriteBuffer;
//			m_pbWriteBuffer = NULL;
//		}
//
//		if (m_pbReadBuffer != NULL)
//		{
//			delete [] m_pbReadBuffer;
//			m_pbReadBuffer = NULL;
//		}
	}
}


//
// Read
//
UINT InternetFile::Read(LPVOID lpBuf, UINT nCount)
{
//	ASSERT_VALID(this);
//	ASSERT(AfxIsValidAddress(lpBuf, nCount));
//	ASSERT(m_hFile != NULL);
//	ASSERT(m_bReadMode);

	DWORD dwBytes;

//	if (!m_bReadMode || m_hFile == NULL)
//		ThrowInternetException(m_dwContext, ERROR_INVALID_HANDLE);

//	if (m_pbReadBuffer == NULL)
//	{
		if (!InternetReadFile(_hFile, (LPVOID) lpBuf, nCount, &dwBytes))
				ThrowInternetException(_context);
		return dwBytes;
//	}

/*	LPBYTE lpbBuf = (LPBYTE) lpBuf;

	// if the requested size is bigger than our buffer,
	// then handle it directly

	if (nCount >= m_nReadBufferSize)
	{
		DWORD dwMoved = max(0, (long)m_nReadBufferBytes - (long)m_nReadBufferPos);
		memcpy(lpBuf, m_pbReadBuffer + m_nReadBufferPos, dwMoved);
		m_nReadBufferPos = m_nReadBufferSize;
		if (!InternetReadFile(m_hFile, lpbBuf+dwMoved, nCount-dwMoved, &dwBytes))
				ThrowInternetException(m_dwContext);
		dwBytes += dwMoved;
	}
	else
	{
		if (m_nReadBufferPos + nCount >= m_nReadBufferBytes)
		{
			DWORD dwMoved = max(0, (long)m_nReadBufferBytes - (long)m_nReadBufferPos);
			memcpy(lpbBuf, m_pbReadBuffer + m_nReadBufferPos, dwMoved);

			DWORD dwRead;
			if (!InternetReadFile(m_hFile, m_pbReadBuffer, m_nReadBufferSize,
					&dwRead))
				ThrowInternetException(m_dwContext);
			m_nReadBufferBytes = dwRead;

			dwRead = min(nCount - dwMoved, m_nReadBufferBytes);
			memcpy(lpbBuf + dwMoved, m_pbReadBuffer, dwRead);
			m_nReadBufferPos = dwRead;
			dwBytes = dwMoved + dwRead;
		}
		else
		{
			memcpy(lpbBuf, m_pbReadBuffer + m_nReadBufferPos, nCount);
			m_nReadBufferPos += nCount;
			dwBytes = nCount;
		}
	}

	return dwBytes;*/
}


//
// Write
//
void InternetFile::Write(const void* lpBuf, UINT nCount)
{
//	ASSERT_VALID(this);
//	ASSERT(m_hFile != NULL);
//	ASSERT(AfxIsValidAddress(lpBuf, nCount, FALSE));
//	ASSERT(m_bReadMode == FALSE || m_bReadMode == -1);
//
//	if (m_bReadMode == TRUE || m_hFile == NULL)
//		ThrowInternetException(m_dwContext, ERROR_INVALID_HANDLE);

	DWORD dwBytes;
//	if (m_pbWriteBuffer == NULL)
//	{
		if (!InternetWriteFile(_hFile, lpBuf, nCount, &dwBytes))
			ThrowInternetException(_context);

		if (dwBytes != nCount)
			ThrowInternetException(_context);
//	}
/*	else
	{
		if ((m_nWriteBufferPos + nCount) >= m_nWriteBufferSize)
		{
			// write what is in the buffer just now

			if (!InternetWriteFile(m_hFile, m_pbWriteBuffer,
					m_nWriteBufferPos, &dwBytes))
				ThrowInternetException(m_dwContext);

			// reset the buffer position since it is now clean

			m_nWriteBufferPos = 0;
		}

		// if we can't hope to buffer the write request,
		// do it immediately ... otherwise, buffer it!

		if (nCount >= m_nWriteBufferSize)
		{
			if (!InternetWriteFile(m_hFile, (LPVOID) lpBuf, nCount, &dwBytes))
				ThrowInternetException(m_dwContext);
		}
		else
		{
			memcpy(m_nWriteBufferPos + m_pbWriteBuffer, lpBuf, nCount);
			m_nWriteBufferPos += nCount;
		}
	}*/
}


//------ ZHttpFile ----------------------------------------------------

//
// constructor
//
ZHttpFile::ZHttpFile(	HINTERNET	hFile, 
						HINTERNET	hSession, 
						LPCTSTR		pstrObject,
						LPCTSTR		pstrServer, 
						LPCTSTR		pstrVerb, 
						DWORD		dwContext)
 :	InternetFile(		hFile, 
						hSession, 
						pstrObject, 
						pstrServer, 
						dwContext, 
						TRUE),
	_verb( pstrVerb ), 
	_object(pstrObject)
{
}


//
// AddRequestHeaders
//
BOOL ZHttpFile::AddRequestHeaders(	LPCTSTR	pstrHeaders,
									DWORD	dwModifiers		/* = HTTP_ADDREQ_FLAG_ADD */,
									int		dwHeadersLen	/* = -1 */)
{
//	ASSERT(AfxIsValidString(pstrHeaders));
//	ASSERT(dwHeadersLen == 0 || pstrHeaders != NULL);
//	ASSERT_VALID(this);
//	ASSERT(m_hFile != NULL);

	if (dwHeadersLen == -1)
		if (pstrHeaders == NULL)
			dwHeadersLen = 0;
		else
			dwHeadersLen = lstrlen(pstrHeaders);

	return HttpAddRequestHeaders(_hFile, pstrHeaders, dwHeadersLen, dwModifiers);
}

//
// SendRequest
//
BOOL ZHttpFile::SendRequest(	LPCTSTR		pstrHeaders		/* = NULL */,
								DWORD		dwHeadersLen	/* = 0 */, 
								LPVOID		lpOptional		/* = NULL */,
								DWORD		dwOptionalLen	/* = 0 */)
{
//	ASSERT(dwOptionalLen == 0 || lpOptional != NULL);
//	ASSERT(dwHeadersLen == 0 || pstrHeaders != NULL);
//	ASSERT_VALID(this);
//	ASSERT(m_hFile != NULL);

	BOOL bRet = HttpSendRequest(_hFile, pstrHeaders, dwHeadersLen, lpOptional, dwOptionalLen);

	if (!bRet)
		ThrowInternetException(_context);

	return bRet;
}


//
// EndRequest
//
BOOL ZHttpFile::EndRequest(		DWORD					dwFlags		/* = 0 */,
								LPINTERNET_BUFFERS		lpBuffIn	/* = NULL */, 
								DWORD					dwContext	/* = 1 */)
{
//	ASSERT(m_hFile != NULL);
//	ASSERT(m_bReadMode == -1);

	if (dwContext == 1)
		dwContext = _context;

	BOOL bRet = HttpEndRequest(_hFile, lpBuffIn, dwFlags, dwContext);

	if (!bRet)
		ThrowInternetException(_context);

	return bRet;
}


//
// QueryInfo
//
BOOL ZHttpFile::QueryInfo(		DWORD		dwInfoLevel,
								LPVOID		lpvBuffer, 
								LPDWORD		lpdwBufferLength, 
								LPDWORD		lpdwIndex) const
{
//	ASSERT((HTTP_QUERY_HEADER_MASK & dwInfoLevel) <= HTTP_QUERY_MAX &&
//		dwInfoLevel != 0);
//	ASSERT(lpvBuffer != NULL && *lpdwBufferLength > 0);
//	ASSERT_VALID(this);
//	ASSERT(m_hFile != NULL);

	return HttpQueryInfo(_hFile, dwInfoLevel, lpvBuffer, lpdwBufferLength, lpdwIndex);
}


//
// QueryInfoStatusCode
//
BOOL ZHttpFile::QueryInfoStatusCode(	DWORD&	dwStatusCode	) const
{
//	ASSERT_VALID(this);
//	ASSERT(m_hFile != NULL);

	TCHAR szBuffer[80];
	DWORD dwLen = sizeof( szBuffer );
	BOOL bRet;

	bRet = HttpQueryInfo( _hFile, HTTP_QUERY_STATUS_CODE, szBuffer, &dwLen, NULL );

	if (bRet)
		dwStatusCode = (DWORD) _ttol(szBuffer);

	return bRet;
}


//
// GetCustomHeaderValue
//
bool ZHttpFile::GetCustomHeaderValue(const std::string& headerName, std::string& headerValue)
{
////////	BOOL bRet;
////////
////////	char buf[500]; // make a large buffer, so that it can hold any-sized header name and value
////////	DWORD cbBuf = sizeof(buf);
////////	::ZeroMemory(buf, cbBuf);
////////	::strcpy(buf, (LPCSTR )strHeaderName.c_str());
////////
////////	if (::HttpQueryInfo(_hFile, HTTP_QUERY_CUSTOM, buf, &cbBuf, 0))
////////	{
////////		// ensure the header is null-terminated
////////		strHeaderValue = buf;
////////
////////		bRet = TRUE;
////////	}
////////	else // couldn't find the custom header
////////	{
////////		bRet = FALSE;
////////	}
////////
////////	return bRet;
	// this procedure should work with any length of the header name
	// and any length of the header value
	bool success = false;

	enum { InitialBufferSize = 4096 };

	char buf[ InitialBufferSize ]; 

	DWORD bufSize = sizeof(buf);

	ZeroMemory( buf, bufSize );
	strncpy( buf, (LPCSTR )headerName.c_str(), bufSize );

	if (::HttpQueryInfo( _hFile, HTTP_QUERY_CUSTOM, buf, &bufSize, 0) )
	{
		headerValue = buf;
		success = true;
	}
	else 
	{
		DWORD e = GetLastError();
		if ( e == ERROR_INSUFFICIENT_BUFFER )
		{
			// one more try with a bigger buffer as requested
			// bufSize contains requested buffer size now
			std::auto_ptr<char> dynBuf( new char [ bufSize ] );
			// we need to initialize the buffer again 
			ZeroMemory( dynBuf.get(), bufSize );
			strncpy( dynBuf.get(), (LPCSTR )headerName.c_str(), bufSize );
			if (::HttpQueryInfo( _hFile, HTTP_QUERY_CUSTOM, static_cast<void*>(dynBuf.get()), &bufSize, 0) )
			{
				headerValue = dynBuf.get();
				success = true;
			}
			else
			{
				success = false;
			}
		}
		else // couldn't find the custom header
		{
			success = false;
		}
	}

	return success;
}


//
// GetCustomHeaderValue2
//
bool ZHttpFile::GetCustomHeaderValue2( const std::string& headerName, std::string& headerValue)
{
	std::string s = headerName;
	StrUtil::replaceAll( s, "-", "_");

	bool b = this->GetCustomHeaderValue( s, headerValue );

	if (!b)
	{
		// if didn't find header, try again, with '-' instead of '_' character
		b = this->GetCustomHeaderValue( headerName, headerValue );

	}

	return b;
}


//
// GetResponseHeaders
//
bool ZHttpFile::GetResponseHeaders( std::string& headers )
{
////	int rv = this->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, pvResponseHeaders, pdwBufLength, NULL);
////
////	PCHAR pHeaders = (PCHAR )pvResponseHeaders;
////	for (DWORD i = 0; i < *pdwBufLength; i++)
////	{
////		// the following block is here to fix a bug in Mike's code
////		// convert the "_" to "-"
////		if (pHeaders[i] == '_')
////			pHeaders[i] = '-';
////	}
////
////	return rv;
	enum { InitialBufferSize = 16384 };

	char buf[ InitialBufferSize ]; 

	DWORD	bufSize = sizeof( buf );
	bool	success = false;

	if ( this->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, buf, &bufSize, NULL) )
	{
		headers = buf;
		success = true;
	}
	else
	{
		DWORD e = GetLastError();
		if ( e == ERROR_INSUFFICIENT_BUFFER )
		{
			std::auto_ptr<char> dynBuf( new char[ bufSize ] );
			// a second try	in case the initial buffer was too small
			if ( this->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, dynBuf.get(), &bufSize, NULL) )
			{
				headers = dynBuf.get();
				success = true;
			}
			else
			{
				success = false;
			}
		}
		else
		{
			success = false;
		}
	}

	if ( success )
	{
		StrUtil::replaceAll( headers, "_", "-");
	}

	return success;
}


