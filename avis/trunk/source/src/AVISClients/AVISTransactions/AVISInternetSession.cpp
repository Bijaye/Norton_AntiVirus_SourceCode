/////////////////////////////////////////////////////////////////////////////
// CAVISInternetSession object

#include "stdafx.h"
#include <winsock.h>
#include "AVISInternetSession.h"
#include "AVISTransactions.h"

/*****/
CAVISInternetSession::CAVISInternetSession(
		LPCTSTR pstrAgent,
		DWORD dwContext,
		DWORD dwAccessType,
		LPCTSTR pstrProxyName,
		LPCTSTR pstrProxyBypass,
		DWORD dwFlags)
  : CInternetSession(
		pstrAgent,
		dwContext,
		dwAccessType,
		pstrProxyName,
		pstrProxyBypass,
		dwFlags)
{
}
/*****/
void CAVISInternetSession::OnStatusCallback(
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLen)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState( ) );

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
					CAVISTransactionsApp::s_pTc->debug("WinInet resolving internet name \"%s\"...", szInfo);
					break;

				case INTERNET_STATUS_NAME_RESOLVED:
					CAVISTransactionsApp::s_pTc->debug("WinInet resolved internet name \"%s\".", szInfo);
					break;

				case INTERNET_STATUS_CONNECTING_TO_SERVER:
					CAVISTransactionsApp::s_pTc->debug("WinInet connecting to server with address \"%s\".", szInfo);
					break;

				case INTERNET_STATUS_CONNECTED_TO_SERVER:
					CAVISTransactionsApp::s_pTc->debug("WinInet connected to server with address \"%s\".", szInfo);
					break;
			}
			break;
		}

		case INTERNET_STATUS_SENDING_REQUEST:
			CAVISTransactionsApp::s_pTc->debug("WinInet sending request...");
			break;

		case INTERNET_STATUS_REQUEST_SENT:
			CAVISTransactionsApp::s_pTc->debug("WinInet sent request.");
			break;

		case INTERNET_STATUS_RECEIVING_RESPONSE:
			CAVISTransactionsApp::s_pTc->debug("WinInet receiving response...");
			break;

		case INTERNET_STATUS_RESPONSE_RECEIVED:
			CAVISTransactionsApp::s_pTc->debug("WinInet received response.");
			break;

		case INTERNET_STATUS_CLOSING_CONNECTION:
			CAVISTransactionsApp::s_pTc->debug("WinInet closing connection...");
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			CAVISTransactionsApp::s_pTc->debug("WinInet closed connection.");
			break;

		case INTERNET_STATUS_HANDLE_CREATED:
			CAVISTransactionsApp::s_pTc->debug("WinInet created a new handle.");
			break;

		case INTERNET_STATUS_HANDLE_CLOSING:
			CAVISTransactionsApp::s_pTc->debug("WinInet is closing a handle.");
			break;

		case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
			CAVISTransactionsApp::s_pTc->debug("WinInet received a control response.");
			break;

		case INTERNET_STATUS_PREFETCH:
			CAVISTransactionsApp::s_pTc->debug("WinInet \"prefetch\".");
			break;

		case INTERNET_STATUS_REQUEST_COMPLETE:
			CAVISTransactionsApp::s_pTc->debug("WinInet request complete.");
			break;

		case INTERNET_STATUS_REDIRECT:
			CAVISTransactionsApp::s_pTc->debug("WinInet redirect.");
			break;

		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
			CAVISTransactionsApp::s_pTc->debug("WinInet intermediate response.");
			break;

		case INTERNET_STATUS_STATE_CHANGE:
			CAVISTransactionsApp::s_pTc->debug("WinInet state change.");
			break;

		default:
			CAVISTransactionsApp::s_pTc->debug("WinInet unknown status.");
			break;
	}
}


