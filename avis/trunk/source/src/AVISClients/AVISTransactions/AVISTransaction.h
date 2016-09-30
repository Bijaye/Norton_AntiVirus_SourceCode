// AVISTransaction.h: interface for the CAVISTransaction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"

#include <AFXINET.H>
#include "AVISInternetSession.h"
#include "TransactionTimeoutMonitorThread.h"

#pragma	warning( disable : 4251 )

class IMPEXP CAVISTransaction  
{
public:
	enum FirewallType { FW_SOCKS, FW_PROXY, FW_DEFINED_IN_REGISTRY, FW_NONE };

// the following statics are used by all transactions
private:
	static FirewallType s_fwType;

	static CString s_strSocksAddress;
	static CString s_strProxyAddress;
	static CString s_strProxyUsername;
	static CString s_strProxyPassword;

	static int s_iConnectTimeout;
	static int s_iConnectRetries;

	static int s_iSendRequestTimeout;
	static int s_iSendRequestDataTimeout;

	static int s_iReceiveResponseTimeout;
	static int s_iReceiveResponseDataTimeout;

	static BOOL s_boEnableWinInetLogging;

	static BOOL s_boIgnoreInvalidCertificateServerName;

private:
	// name the transaction owner
	CString m_strAgentName;

	// the headers to send along with the HTTP request
	CString m_strRequestHeaders;

	// verb identifier (i.e. HTTP_VERB_POST)
	int m_nVerb;

	// parsed URL parameters
	CString m_strGatewayAddress;
	CString m_strTarget;
	INTERNET_PORT m_nGatewayPort;

	// cookie data
	UINT m_uiResponseAnalysisCookie;

	// security
	BOOL m_boUseSSL;

	DWORD m_dwResponseStatusCode;
	CString m_strGatewayError;

	HANDLE m_hEventTxRunning;

	BOOL m_boIgnoreXErrorCrumbled;

	UINT m_uiTimeoutTransactionConnected;
	UINT m_uiTimeoutTransactionCompleted;

	CTransactionTimeoutMonitorThread* m_pthreadTransactionTimeoutMonitor;

private:
	CAVISInternetSession* m_pInetSession;
	CHttpConnection* m_pHttpConnection;
	CHttpFile* m_pHttpFile;

public:
	// constructor
	CAVISTransaction(
		LPCSTR pszAgentName,
		LPCSTR pszGatewayURL,
		int nVerb,
		UINT uiTimeoutTransactionConnected,
		UINT uiTimeoutTransactionCompleted,
		CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
		LPCTSTR pszTarget = NULL,
		BOOL boUseSSL = FALSE);

	// destructor
	virtual ~CAVISTransaction();

	// call this static to initalize the static data used by all transactions
	static void InitInternetCommunicationsData(
		CAVISTransaction::FirewallType fwType = FW_NONE,
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
		BOOL boIgnoreInvalidCertificateServerName = FALSE);

	// call to enable logging of internet events
	static void EnableWinInetLogging(BOOL boEnable = TRUE);

	// call to do request and response processing
	int Execute(int cbRequestContent = 0, PVOID pvRequestContent = NULL);

	// retrieve the response headers after the request has been issued
	int GetResponseHeaders(PVOID pvResponseHeaders, PDWORD dwBufLength);

	// Add an individual request header before the request has been issued
	void AddRequestHeader(LPCSTR pszNewHeader);
	void AddRequestHeader(CString& strNewHeader);
	void AddRequestHeader(std::string strHeaderName, std::string strHeaderValue);
	void AddRequestHeader(std::string strHeaderName, UINT uiHeaderValue);

	// retrieve the http status code after the request has completed
	DWORD GetResponseStatusCode();

	// set the buffer size for reading response content
	BOOL SetReadBufferSize(DWORD dwBufSize);

	// retrieve the AVIS cookie associated with the response headers
	UINT GetResponseAnalysisCookie();

	// retrieve any named header value after request has completed
	BOOL GetCustomHeaderValue(const CString& strHeaderName, CString& strHeaderValue);

	// reset the request headers
	void EmptyRequestHeaders();

	CString GetGatewayAddress();

	void CancelTransaction();

	void SetIgnoreXErrorCrumbled();

protected:
	// allow any initialization to be done before the request is executed
	virtual DWORD InitRequest();

	// override to send any request content in blocks
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual int StartRequest(DWORD cbSendContent, PVOID pvSendContent);

	// override to perform any post-send request cleanup
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual void EndRequest();

	// override to read any response content
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual int ReadResponseContent();

	// override to specify data block to be sent (only called when SendRequestEx() is
	// called from StartRequest() override)
	// the overridden function should return the the actual number of bytes read.
	virtual int GetNextRequestContentBlock(PVOID pvRequestContentBlock);

protected: // helper functions for derived classes
	// send request content in blocks
	int SendRequestEx(DWORD cbRequestContent, DWORD nBlockSize);

	// send request with specified content to be written all at once
	int SendRequest(DWORD cbRequestContent = 0, PVOID pvRequestContent = NULL);

	// call after finishing transaction
	void Close(BOOL boCloseHttpFileOnly = FALSE);

	// retrieve the length of the response content
	int GetResponseContentLength();

	// Read the next block of response content
	int ReadNextResponseContentBlock(PVOID pvResponseContentBlock, DWORD cbToRead);
#define RNRCB_NETWORK_ERROR	-1
#define RNRCB_TERMINATE_TX	-2

private:
	// receive the analysis cookie associated with the request headers
	BOOL FindAnalysisCookieHeaderValue();
	BOOL FindAVISGatewayErrorHeaderValue();

	// ***call before calling execute
	void ConnectToGateway();

	void OpenRequest();

	DWORD ProcessInternetError(CInternetException* pExcept = NULL);

	BOOL WaitForTransactionToComplete();

	BOOL IsValidRequestHandle();
};

// error macros
#define AVISTX_RC_BASE			1000
#define AVISTX_SUCCESS			AVISTX_RC_BASE + 1
#define AVISTX_NETWORK_FAILURE	AVISTX_RC_BASE + 2
#define AVISTX_TERMINATE		AVISTX_RC_BASE + 3

#endif // !defined(AFX_AVISTRANSACTION_H__ABEE3323_7FEA_11D2_8A64_00203529AC86__INCLUDED_)
