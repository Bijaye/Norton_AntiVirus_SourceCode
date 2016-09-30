//------------------------------------------------------------------------
// DecNet.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2000, 2005. All rights reserved.
//------------------------------------------------------------------------

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// Project headers
#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"
#include "DecNet.h"


CNetData::CNetData()
{
#if defined(_WINDOWS)
	m_hNet = NULL;

	for (int index = 0; index < MAX_CONNECTION_HANDLES; index++)
	{
		m_hConn[index] = NULL;
		m_hConnFile[index] = NULL;
		m_hConnFind[index] = NULL;
	}
#endif
}


CNetData::~CNetData()
{
	Close();
}


int CNetData::Open(void)
{
#if defined(_WINDOWS)
	m_hNet = InternetOpen("Symantec Net Data Browser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0x00000000);
	if (m_hNet == NULL)
		return(NET_ERROR);

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::Close(void)
{
#if defined(_WINDOWS)
	for (int index = 0; index < MAX_CONNECTION_HANDLES; index++)
	{
		if (m_hConnFile[index] != NULL)
		{
			InternetCloseHandle(m_hConnFile[index]);
			m_hConnFile[index] = NULL;
		}

		if (m_hConnFind[index] != NULL)
		{
			InternetCloseHandle(m_hConnFind[index]);
			m_hConnFind[index] = NULL;
		}

		if (m_hConn[index] != NULL)
		{
			InternetCloseHandle(m_hConn[index]);
			m_hConn[index] = NULL;
		}
	}

	if (m_hNet != NULL)
	{
		InternetCloseHandle(m_hNet);
		m_hNet = NULL;
	}
#endif

	return(NET_OK);
}


void CNetData::CloseConnection(int iHandle)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return;

#if defined(_WINDOWS)
	// Begin critical section

	if (m_hConn[iHandle] != NULL)
	{
		if (m_hConnFile[iHandle] != NULL)
		{
			InternetCloseHandle(m_hConnFile[iHandle]);
			m_hConnFile[iHandle] = NULL;
		}

		if (m_hConnFind[iHandle] != NULL)
		{
			InternetCloseHandle(m_hConnFind[iHandle]);
			m_hConnFind[iHandle] = NULL;
		}

		if (m_hConn[iHandle] != NULL)
		{
			InternetCloseHandle(m_hConn[iHandle]);
			m_hConn[iHandle] = NULL;
		}
	}

	// End critical section
#endif
}


int CNetData::ReadyForConnect(void)
{
	int rc = 0;	// Assume all handles are taken

#if defined(_WINDOWS)
	// Begin critical section

	for (int index = 0; index < MAX_CONNECTION_HANDLES; index++)
	{
		if (m_hConn[index] == NULL)
		{
			// Found an empty handle so we are ready for a new connection.
			rc = 1;
			break;
		}
	}

	// End critical section
#endif

	return(rc);
}


int CNetData::ConnectFTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword)
{
	int		iHandle = -1;

#if defined(_WINDOWS)
	// Begin critical section here...

	// Fail if no previously successful Open call has been made.
	if (m_hNet == NULL)
		goto done_connectftp;

	iHandle = GetNewHandle();
	if (iHandle == -1)
		goto done_connectftp;

	m_hConn[iHandle] = InternetConnect(m_hNet,
									 lpszServerName,
									 INTERNET_DEFAULT_FTP_PORT,
									 lpszUsername,
									 lpszPassword,
									 INTERNET_SERVICE_FTP,
									 0,
									 0x12345678);
	if (m_hConn[iHandle] == NULL)
	{
		iHandle = -1;
		goto done_connectftp;
	}

done_connectftp:
	// End critical section here...
#endif

	return(iHandle);
}


int CNetData::FindFirstFileFTP(int iHandle, const char *lpszSearchFile)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return(NET_ERROR);

#if defined(_WINDOWS)
	if (m_hConn[iHandle] == NULL)
		return(NET_ERROR);

	m_hConnFind[iHandle] = FtpFindFirstFile(m_hConn[iHandle],
										  lpszSearchFile,
										  &m_fd,
										  0,
										  0);
	if (m_hConnFind[iHandle] == NULL)
		return(NET_ERROR);

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::FindNextFileFTP(int iHandle)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return(NET_ERROR);

#if defined(_WINDOWS)
	if (m_hConnFind[iHandle] == NULL)
		return(NET_ERROR);

	if (!InternetFindNextFile(m_hConnFind[iHandle], &m_fd))
	{
		InternetCloseHandle(m_hConnFind[iHandle]);
		m_hConnFind[iHandle] = NULL;
		return(NET_ERROR);
	}

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::FindCloseFTP(int iHandle)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return(NET_ERROR);

#if defined(_WINDOWS)
	if (m_hConnFind[iHandle] == NULL)
		return(NET_OK);

	InternetCloseHandle(m_hConnFind[iHandle]);
	m_hConnFind[iHandle] = NULL;
	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::OpenFileFTP(int iHandle, const char *lpszFileName)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return(NET_ERROR);

#if defined(_WINDOWS)
	if (m_hConn[iHandle] == NULL)
		return(NET_ERROR);

	m_hConnFile[iHandle] = FtpOpenFile(m_hConn[iHandle],
									 lpszFileName,
									 GENERIC_READ,
									 FTP_TRANSFER_TYPE_BINARY,
									 0);
	if (m_hConnFile[iHandle] == NULL)
	{
		DWORD dwError;
		
		dwError = GetLastError();
		if (dwError == ERROR_INTERNET_EXTENDED_ERROR)
		{
			DWORD	dwLen;
			char	szMsg[100];

			dwLen = sizeof(szMsg);
			InternetGetLastResponseInfo(&dwError, szMsg, &dwLen);
		}

		return(NET_ERROR);
	}

/*
	// Try and retrieve the file's size.
	m_fd.nFileSizeHigh = 0;
	m_fd.nFileSizeLow = InternetSetFilePointer(m_hConnFile[iHandle],
											  0,
											  NULL,
											  FILE_END,
											  0);
	if (m_fd.nFileSizeLow == -1)
		m_fd.nFileSizeLow = 0;

	// Reset file position to start of file.
	InternetSetFilePointer(m_hConnFile[iHandle],
						  0,
						  NULL,
						  FILE_BEGIN,
						  0);
*/
	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::CloseFileFTP(int iHandle)
{
	int rc = NET_ERROR;

	// Begin critical section here...

	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		goto done_closefileftp;

#if defined(_WINDOWS)
	if (m_hConnFile[iHandle] == NULL)
		goto done_closefileftp;

	if (InternetCloseHandle(m_hConnFile[iHandle]))
		rc = NET_OK;

	m_hConnFile[iHandle] = NULL;
#endif

done_closefileftp:
	// End critical section here...

	return(rc);
}


int CNetData::SetFilePositionFTP(int iHandle, long lPos, int origin)
{
#if defined(_WINDOWS)
	if (m_hConnFile[iHandle] == NULL)
		return(NET_ERROR);

	if (!InternetSetFilePointer(m_hConnFile[iHandle], lPos, NULL, origin, 0))
		return(NET_ERROR);

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::ReadFileFTP(int iHandle, LPVOID lpBuffer, unsigned long int dwNumberOfBytesToRead, unsigned long int *lpNumberOfBytesRead)
{
#if defined(_WINDOWS)
	if (m_hConnFile[iHandle] == NULL)
		return(NET_ERROR);

	if (!InternetReadFile(m_hConnFile[iHandle], lpBuffer, dwNumberOfBytesToRead, lpNumberOfBytesRead))
	{
		DWORD dwError;
		
		dwError = GetLastError();
		if (dwError == ERROR_INTERNET_EXTENDED_ERROR)
		{
			DWORD	dwLen;
			char	szMsg[100];

			dwLen = sizeof(szMsg);
			InternetGetLastResponseInfo(&dwError, szMsg, &dwLen);
		}
		return(NET_ERROR);
	}

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::SetCurrentDirectoryFTP(int iHandle, const char *lpszDirectory)
{
#if defined(_WINDOWS)
	if (m_hConn[iHandle] == NULL)
		return(NET_ERROR);

	if (!FtpSetCurrentDirectory(m_hConn[iHandle], lpszDirectory))
		return(NET_ERROR);

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::ConnectHTTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword)
{
	int		iHandle = -1;

#if defined(_WINDOWS)
	// Begin critical section here...

	// Fail if no previously successful Open call has been made.
	if (m_hNet == NULL)
		goto done_connecthttp;

	iHandle = GetNewHandle();
	if (iHandle == -1)
		goto done_connecthttp;

	m_hConn[iHandle] = InternetConnect(m_hNet,
									 lpszServerName,
									 INTERNET_DEFAULT_HTTP_PORT,
									 lpszUsername,
									 lpszPassword,
									 INTERNET_SERVICE_HTTP,
									 0,
									 0x12345678);
	if (m_hConn[iHandle] == NULL)
	{
		iHandle = -1;
		goto done_connecthttp;
	}

done_connecthttp:
	// End critical section here...
#endif

	return(iHandle);
}


int CNetData::OpenFileHTTP(int iHandle, const char *lpszFileName)
{
	if (iHandle < 0 || iHandle >= MAX_CONNECTION_HANDLES)
		return(NET_ERROR);

#if defined(_WINDOWS)
	if (m_hConn[iHandle] == NULL)
		return(NET_ERROR);

	m_hConnFile[iHandle] = HttpOpenRequest(m_hConn[iHandle],
										  NULL, // GET
										  lpszFileName,
										  NULL, // HTTP/1.0
										  NULL, // no referrer???
										  NULL, // no accept types
										  INTERNET_FLAG_RAW_DATA,
										  0x12345678);
	if (m_hConnFile[iHandle] == NULL)
	{
		DWORD dwError;
		
		dwError = GetLastError();
		if (dwError == ERROR_INTERNET_EXTENDED_ERROR)
		{
			DWORD	dwLen;
			char	szMsg[100];

			dwLen = sizeof(szMsg);
			InternetGetLastResponseInfo(&dwError, szMsg, &dwLen);
		}

		return(NET_ERROR);
	}

	return(NET_OK);
#else
	return(NET_ERROR);
#endif
}


int CNetData::GetFileAttributes(unsigned long int *pdwAttrs, int *piPlatform)
{
#if defined(_WINDOWS)
	*pdwAttrs = m_fd.dwFileAttributes;
	*piPlatform = PLATFORM_WINDOWS;
#endif

	return (NET_OK);
}


int CNetData::GetFileTime(time_t *pCreate, time_t *pLastAccess, time_t *pLastWrite)
{
	*pCreate = 0;
	*pLastAccess = 0;
	*pLastWrite = 0;

	return (NET_OK);
}


int CNetData::GetFileSize(unsigned long int *pulSize)
{
#if defined(_WINDOWS)
	*pulSize = m_fd.nFileSizeLow;
#endif

	return (NET_OK);
}


int CNetData::GetFileName(char *pszName, int iBufSize)
{
#if defined(_WINDOWS)
	strncpy(pszName, m_fd.cFileName, iBufSize);
#endif

	return (NET_OK);
}


char *CNetData::GetFTPUser()
{
	return (&m_szFTPUser[0]);
}


void CNetData::SetFTPUser(char *pszUser)
{
	strncpy(m_szFTPUser, pszUser, sizeof(m_szFTPUser) - 1);
}


char *CNetData::GetHTTPUser()
{
	return (&m_szHTTPUser[0]);
}


void CNetData::SetHTTPUser(char *pszUser)
{
	strncpy(m_szHTTPUser, pszUser, sizeof(m_szHTTPUser) - 1);
}


char *CNetData::GetFTPPassword()
{
	return (&m_szFTPPassword[0]);
}


void CNetData::SetFTPPassword(char *pszPassword)
{
	strncpy(m_szFTPPassword, pszPassword, sizeof(m_szFTPPassword) - 1);
}


char *CNetData::GetHTTPPassword()
{
	return (&m_szHTTPPassword[0]);
}


void CNetData::SetHTTPPassword(char *pszPassword)
{
	strncpy(m_szHTTPPassword, pszPassword, sizeof(m_szHTTPPassword) - 1);
}


int CNetData::GetNewHandle(void)
{
	int index;
	int iHandle = -1;

#if defined(_WINDOWS)
	for (index = 0; index < MAX_CONNECTION_HANDLES; index++)
	{
		if (m_hConn[index] == NULL)
		{
			iHandle = index;
			break;
		}
	}
#endif

	return (iHandle);
}


DECSDKLINKAGE int STDCALL DAPI_NewNetService(INetData **ppDecNet)
{
	try
	{
		*ppDecNet = new CNetData;
		return DAPI_OK;
	}
	catch (...)
	{
		*ppDecNet = NULL;
		return DAPI_ERROR;
	}
}


DECSDKLINKAGE int STDCALL DAPI_DeleteNetService(INetData *pDecNet)
{
	if (pDecNet)
	{
		delete pDecNet;
	}

	return DAPI_OK;
}
