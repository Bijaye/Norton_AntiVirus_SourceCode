#ifndef __TRANSACTIONSETTINGS__H__
#define __TRANSACTIONSETTINGS__H__

class TransactionSettings
{
public:
	enum FirewallType { FW_SOCKS, FW_PROXY, FW_DEFINED_IN_REGISTRY, FW_NONE };

	// call this static to initalize the static data used by all transactions
	static void InitInternetCommunicationsData(
		FirewallType fwType = FW_NONE,
		LPCTSTR pszSocksAddress = NULL,
		LPCTSTR pszProxyAddress = NULL,
		LPCTSTR pszProxyUsername = NULL,
		LPCTSTR pszProxyPassword = NULL,
		int iConnectTimeout = -1,
		int iConnectRetries = -1,
		int iSendRequestTimeout = -1,
		int iSendRequestDataTimeout = -1,
		int iReceiveResponseTimeout = -1,
		int iReceiveResponseDataTimeout = -1,
		bool ignoreSSLNameMismatch = false );

	// call to enable logging of internet events
	static void EnableWinInetLogging(bool boEnable = true);

	static void SetSessionOptions( InternetSession& internetSession );

// the following statics are used by all transactions
protected:
	static FirewallType s_fwType;

	static std::string s_strSocksAddress;
	static std::string s_strProxyAddress;
	static std::string s_strProxyUsername;
	static std::string s_strProxyPassword;

	static int s_iConnectTimeout;
	static int s_iConnectRetries;

	static int s_iSendRequestTimeout;
	static int s_iSendRequestDataTimeout;

	static int s_iReceiveResponseTimeout;
	static int s_iReceiveResponseDataTimeout;


	static bool getIgnoreSSLNameMismatch()	{ return s_ignoreSSLNameMismatch; }
	static bool getEnableWinInetLogging()	{ return s_boEnableWinInetLogging; }

private:
	static bool s_ignoreSSLNameMismatch;
	static bool s_boEnableWinInetLogging;
};


#endif __TRANSACTIONSETTINGS__H__
