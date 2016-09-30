// BlockingSocket.cpp: implementation of the CBlockingSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "BlockingSocket.h"
#include "SendMailException.h"

#include <winsock.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CBlockingSocket::CBlockingSocket()
{

	m_hSocket = NULL;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CBlockingSocket::~CBlockingSocket()
{

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::CleanUp()
{

	// doesn't throw an exception because it's called in a catch block
	if(m_hSocket == NULL) 
		return;

	closesocket(m_hSocket);

	m_hSocket = NULL;


}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CBlockingSocket::Create(int nType)
{

	int nReturnCode = 0;

//	ASSERT(m_hSocket == NULL);

	if( ( m_hSocket = socket(AF_INET, nType, 0) ) == INVALID_SOCKET ) 
	{ // if

	    nReturnCode = WSAGetLastError( );

	} // end if

	return nReturnCode;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::Close()
{

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::Bind(LPCSOCKADDR psa)
{

//	ASSERT(m_hSocket != NULL);

	if(bind(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) 
	{
		throw new CSendMailException("Bind");
	}


}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::Listen()
{

//	ASSERT(m_hSocket != NULL);

	if (listen( m_hSocket, 5 ) == SOCKET_ERROR) 
	{
		throw new CSendMailException("Listen");
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::Connect(LPCSOCKADDR psa)
{
//	ASSERT(m_hSocket != NULL);

	// should timeout by itself
	if (connect(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) 
	{
		throw new CSendMailException("Connect");
	}

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
BOOL CBlockingSocket::Accept(CBlockingSocket &sConnect, LPSOCKADDR psa)
{

//	ASSERT(m_hSocket != NULL);

//	ASSERT(sConnect.m_hSocket == NULL);

	int nLengthAddr = sizeof(SOCKADDR);

	sConnect.m_hSocket = accept(m_hSocket, psa, &nLengthAddr);

	if (sConnect == INVALID_SOCKET) 
	{
		// no exception if the listen was canceled
		if (WSAGetLastError() != WSAEINTR) 
		{
			throw new CSendMailException("Accept");
		}

		return FALSE;
	}

	return TRUE;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CBlockingSocket::Send(const char *pch, const int nSize, const int nSecs)
{

//	ASSERT(m_hSocket != NULL);

	// returned value will be less than nSize if client cancels the reading

	FD_SET fd = {1, m_hSocket};

	TIMEVAL tv = {nSecs, 0};

	if (select(0, NULL, &fd, NULL, &tv) == 0) 
	{
		throw new CSendMailException("Send timeout");
	}

	int nBytesSent;

	if ( (nBytesSent = send(m_hSocket, pch, nSize, 0) ) == SOCKET_ERROR) 
	{
		throw new CSendMailException("Send");
	}

	return nBytesSent;
}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CBlockingSocket::Write(const char *pch, const int nSize, const int nSecs)
{

	int nBytesSent = 0;
	int nBytesThisTime;

	const char* pch1 = pch;

	do 
	{
		nBytesThisTime = Send(pch1, nSize - nBytesSent, nSecs);

		nBytesSent += nBytesThisTime;

		pch1 += nBytesThisTime;

	} while (nBytesSent < nSize);

	return nBytesSent;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CBlockingSocket::Receive(char *pch, const int nSize, const int nSecs)
{

//	ASSERT(m_hSocket != NULL);

	FD_SET fd = {1, m_hSocket};

	TIMEVAL tv = {nSecs, 0};

	if ( select(0, &fd, NULL, NULL, &tv) == 0 ) 
	{
		throw new CSendMailException("Receive timeout");
	}

	int nBytesReceived;

	if ( (nBytesReceived = recv(m_hSocket, pch, nSize, 0) ) == SOCKET_ERROR) 
	{
		throw new CSendMailException("Receive");
	}

	return nBytesReceived;
}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::GetPeerAddr(LPSOCKADDR psa)
{

//	ASSERT(m_hSocket != NULL);

	// gets the address of the socket at the other end

	int nLengthAddr = sizeof(SOCKADDR);

	if ( getpeername(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR ) 
	{
		throw new CSendMailException("GetPeerName");
	}

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CBlockingSocket::GetSockAddr(LPSOCKADDR psa)
{

//	ASSERT(m_hSocket != NULL);

	// gets the address of the socket at this end
	int nLengthAddr = sizeof(SOCKADDR);

	if( getsockname(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR ) 
	{
		throw new CSendMailException("GetSockName");
	}

}
