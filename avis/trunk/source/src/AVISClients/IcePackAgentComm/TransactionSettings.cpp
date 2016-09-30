// TransactionSettings.cpp: implementation of the TransactionSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "base.h"
#include "AttributeKeys.h"
#include "InternetSession.h"
#include "TransactionSettings.h"

TransactionSettings::FirewallType TransactionSettings::s_fwType = TransactionSettings::FW_NONE;
std::string TransactionSettings::s_strSocksAddress;
std::string TransactionSettings::s_strProxyAddress;
std::string TransactionSettings::s_strProxyUsername;
std::string TransactionSettings::s_strProxyPassword;

int TransactionSettings::s_iConnectTimeout = -1;
int TransactionSettings::s_iConnectRetries = -1;

int TransactionSettings::s_iSendRequestTimeout = -1;
int TransactionSettings::s_iSendRequestDataTimeout = -1;

int TransactionSettings::s_iReceiveResponseTimeout = -1;
int TransactionSettings::s_iReceiveResponseDataTimeout = -1;

bool TransactionSettings::s_boEnableWinInetLogging	= false;
bool TransactionSettings::s_ignoreSSLNameMismatch	= false;

/*****/
void TransactionSettings::EnableWinInetLogging(bool boEnable)
// set a static flag to enable the callback for status
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//_tc.debug("WinInet logging is enabled.");
	s_boEnableWinInetLogging = boEnable;
}
/*****/
/* Static function used to define global internet data */
void TransactionSettings::InitInternetCommunicationsData(
		TransactionSettings::FirewallType fwType,
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
		bool ignoreSSLNameMismatch )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//_tc.debug("initializing internet communications parameters...");

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
	s_ignoreSSLNameMismatch = ignoreSSLNameMismatch;
}


//
// SetSessionOptions
//
void TransactionSettings::SetSessionOptions( InternetSession& internetSession )
{
	UINT uiThreadId = ::GetCurrentThreadId();

	if (s_iConnectRetries != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, s_iConnectRetries))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iConnectTimeout != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, s_iConnectTimeout))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iSendRequestTimeout != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_SEND_TIMEOUT, s_iSendRequestTimeout))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iSendRequestDataTimeout != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, s_iSendRequestDataTimeout))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iReceiveResponseTimeout != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, s_iReceiveResponseTimeout))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (s_iReceiveResponseDataTimeout != -1)
		if (!internetSession.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, s_iReceiveResponseDataTimeout))
			::ThrowInternetException(uiThreadId, ::GetLastError());

	if (!internetSession.SetOption(INTERNET_OPTION_SETTINGS_CHANGED, TRUE))
		::ThrowInternetException(uiThreadId, ::GetLastError());

}

