//------------------------------------------------------------------------
// DecNet.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

#ifndef DECNET_H
#define DECNET_H

#if defined(_WINDOWS)
#include <wininet.h>
#endif

#define NET_OK			0
#define NET_ERROR		1

#define PLATFORM_UNKNOWN	0
#define PLATFORM_WINDOWS	1
#define PLATFORM_UNIX		2

#define MAX_CONNECTION_HANDLES 4

/////////////////////////////////////////////////////////////////////////////
// Interface IDecFiles - interface definition for Decomposer file system
// C++ interface for Decomposer file system functions.

class INetData
{
public:
	INetData() { }
	virtual ~INetData() { }

	virtual int		Open(void) = 0;
	virtual int		Close(void) = 0;
	virtual void	CloseConnection(int iHandle) = 0;
	virtual int		ReadyForConnect(void) = 0;

	virtual int		ConnectFTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword) = 0;
	virtual int		FindFirstFileFTP(int iHandle, const char *lpszSearchFile) = 0;
	virtual int		FindNextFileFTP(int iHandle) = 0;
	virtual int		FindCloseFTP(int iHandle) = 0;
	virtual int		OpenFileFTP(int iHandle, const char *lpszFileName) = 0;
	virtual int		CloseFileFTP(int iHandle) = 0;
	virtual int		SetFilePositionFTP(int iHandle, long lPos, int origin) = 0;
	virtual int		ReadFileFTP(int iHandle, LPVOID lpBuffer, unsigned long int dwNumberOfBytesToRead, unsigned long int *lpNumberOfBytesRead) = 0;
	virtual int		SetCurrentDirectoryFTP(int iHandle, const char *lpszDirectory) = 0;

	virtual int		ConnectHTTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword) = 0;
	virtual int		OpenFileHTTP(int iHandle, const char *lpszFileName) = 0;

	virtual int		GetFileAttributes(unsigned long int *pdwAttrs, int *piPlatform) = 0;
	virtual int		GetFileTime(time_t *pCreate, time_t *pLastAccess, time_t *pLastWrite) = 0;
	virtual int		GetFileSize(unsigned long int *pulSize) = 0;
	virtual int		GetFileName(char *pszName, int iBufSize) = 0;

	virtual char	*GetFTPUser() = 0;
	virtual void	SetFTPUser(char *pszUser) = 0;
	virtual char	*GetHTTPUser() = 0;
	virtual void	SetHTTPUser(char *pszUser) = 0;
	virtual char	*GetFTPPassword() = 0;
	virtual void	SetFTPPassword(char *pszPassword) = 0;
	virtual char	*GetHTTPPassword() = 0;
	virtual void	SetHTTPPassword(char *pszPassword) = 0;
};


class CNetData :
	public INetData
{
public:
	CNetData();
	~CNetData();

	int			Open(void);
	int			Close(void);
	void		CloseConnection(int iHandle);
	int			ReadyForConnect(void);

	int			ConnectFTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword);
	int			FindFirstFileFTP(int iHandle, const char *lpszSearchFile);
	int			FindNextFileFTP(int iHandle);
	int			FindCloseFTP(int iHandle);
	int			OpenFileFTP(int iHandle, const char *lpszFileName);
	int			CloseFileFTP(int iHandle);
	int			SetFilePositionFTP(int iHandle, long lPos, int origin);
	int			ReadFileFTP(int iHandle, LPVOID lpBuffer, unsigned long int dwNumberOfBytesToRead, unsigned long int *lpNumberOfBytesRead);
	int			SetCurrentDirectoryFTP(int iHandle, const char *lpszDirectory);

	int			ConnectHTTP(const char *lpszServerName, const char *lpszUsername, const char *lpszPassword);
	int			OpenFileHTTP(int iHandle, const char *lpszFileName);

	int			GetFileAttributes(unsigned long int *pdwAttrs, int *piPlatform);
	int			GetFileTime(time_t *pCreate, time_t *pLastAccess, time_t *pLastWrite);
	int			GetFileSize(unsigned long int *pulSize);
	int			GetFileName(char *pszName, int iBufSize);

	char		*GetFTPUser();
	void		SetFTPUser(char *pszUser);
	char		*GetHTTPUser();
	void		SetHTTPUser(char *pszUser);
	char		*GetFTPPassword();
	void		SetFTPPassword(char *pszPassword);
	char		*GetHTTPPassword();
	void		SetHTTPPassword(char *pszPassword);

private:
	int			GetNewHandle(void);

	char		m_szFTPUser[64];
	char		m_szFTPPassword[64];
	char		m_szHTTPUser[64];
	char		m_szHTTPPassword[64];

#if defined(_WINDOWS)
	WIN32_FIND_DATA m_fd;

	HINTERNET	m_hNet;

	HINTERNET	m_hConn[MAX_CONNECTION_HANDLES];
	HINTERNET	m_hConnFile[MAX_CONNECTION_HANDLES];
	HINTERNET	m_hConnFind[MAX_CONNECTION_HANDLES];
#else
	int			m_hNet;

	int			m_hConn[MAX_CONNECTION_HANDLES];
	int			m_hConnFile[MAX_CONNECTION_HANDLES];
	int			m_hConnFind[MAX_CONNECTION_HANDLES];
#endif
};

#endif // DECNET_H
