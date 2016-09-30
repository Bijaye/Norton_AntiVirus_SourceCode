// AVISClient.h: interface for the CAVISClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISCLIENT_H__73B0D9A3_DD6E_11D2_8A96_00203529AC86__INCLUDED_)
#define AFX_AVISCLIENT_H__73B0D9A3_DD6E_11D2_8A96_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "thread.h"
#include "AVISTransaction.h"
#include "Server.h"
#include "TransactionTimeoutMonitorThread.h"

class CAVISGatewayComm;

#pragma	warning( disable : 4251 )

class IMPEXP CAVISClient : public CThread  
{
private:
	UINT m_uiIdGateway;
	CString m_strGatewayURL;
	CString m_strAgentName;
	CAVISGatewayComm* m_pGwc;
	CPtrList m_transactionList;
	BOOL m_boUseSSL;
	CTransactionTimeoutMonitorThread m_threadTransactionTimeoutMonitor;

public:
	CAVISClient(
		LPCSTR pszGatewayURL, 
		UINT uiIdGateway, 
		LPCSTR pszAgentName,
		CAVISGatewayComm* pGwc,
		BOOL boUseSSL = FALSE);
	virtual ~CAVISClient();

private:
	DWORD ThreadProc(LPVOID);
	void DestroyTransactions();

protected:
	virtual DWORD ClientMain() = 0;
	virtual BOOL CreateTransactions(CString& strGatewayURL, BOOL boUseSSL) = 0;
	POSITION AddTransaction(CAVISTransaction* pTx);

public:
	CString& GetAgentName();
	virtual BOOL End(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = FALSE);
	Server GetGateway();
	CTransactionTimeoutMonitorThread* GetTransactionTimeoutMonitorThread();
};

#endif // !defined(AFX_AVISCLIENT_H__73B0D9A3_DD6E_11D2_8A96_00203529AC86__INCLUDED_)
