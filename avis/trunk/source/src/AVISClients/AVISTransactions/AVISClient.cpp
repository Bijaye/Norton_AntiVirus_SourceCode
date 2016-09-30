// AVISClient.cpp: implementation of the CAVISClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactions.h"
#include "AVISClient.h"
#include "AVISGatewayComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISClient::CAVISClient(
	LPCSTR pszGatewayURL, 
	UINT uiIdGateway, 
	LPCSTR pszAgentName,
	CAVISGatewayComm* pGwc,
	BOOL boUseSSL)
  :	m_uiIdGateway(uiIdGateway),
	m_strGatewayURL(pszGatewayURL),
	m_strAgentName(pszAgentName),
	m_pGwc(pGwc),
	m_boUseSSL(boUseSSL)
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISClient constructor...");
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISClient constructor.");
}
/*****/
CAVISClient::~CAVISClient()
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISClient destructor...");
	DestroyTransactions();
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISClient destructor.");
}
/*****/
POSITION CAVISClient::AddTransaction(CAVISTransaction* pTx)
{
	return m_transactionList.AddTail(pTx);
}
/*****/
void CAVISClient::DestroyTransactions()
{
	CAVISTransaction* pTransaction;

	// stop all of the clients for all of the gateways
	for (POSITION pos = m_transactionList.GetHeadPosition(); pos != NULL; )
	{
		// get the next gateway in the list
		pTransaction = (CAVISTransaction* )m_transactionList.GetNext(pos);

		delete pTransaction;
	}

	// empty the list
	m_transactionList.RemoveAll();
}
/*****/
DWORD CAVISClient::ThreadProc(LPVOID)
{
	// do the following in a code block, so the automatic CString objects will be
	// deleted (freeing up their memory) upon exiting the block
	{
		CString strLogFileName;
		CString strTraceFileName;
		CString strNumberedAgentName;
		CString strStrippedName;

		for (int i = 0; i < m_strAgentName.GetLength(); i++)
		{
			char ch = m_strAgentName[i];

			if (ch != ' ')
				strStrippedName += ch;
		}

		// register this thread for trace/log output
		strLogFileName.Format("%s%d.log", strStrippedName, m_uiIdGateway);
		strTraceFileName.Format("%s%d.trc", strStrippedName, m_uiIdGateway);
		strNumberedAgentName.Format("%s%d:", m_strAgentName, m_uiIdGateway);
		CAVISTransactionsApp::s_pTc->RegisterThread(
			strLogFileName, 
			strTraceFileName, 
			strNumberedAgentName);

		// start the transaction timeout monitor thread
		// When the current client object is destroyed, so will the transaction timeout
		// monitor thread object, which will also terminate the thread
		m_threadTransactionTimeoutMonitor.Run(::GetCurrentThreadId());
	}

	CAVISTransactionsApp::s_pTc->info("*******************************************************************************************");
	CAVISTransactionsApp::s_pTc->msg("%s client for gateway (URL = %s) has started.", m_strAgentName, m_strGatewayURL);
	DWORD rc = 0;
	if (CreateTransactions(m_strGatewayURL, m_boUseSSL))
	{
		rc = ClientMain();
	}

	CAVISTransactionsApp::s_pTc->msg("%s client has ended.", m_strAgentName);

	return rc;
}
/*****/
CString& CAVISClient::GetAgentName()
{
	return m_strAgentName;
}
/*****/
Server CAVISClient::GetGateway()
{
	return m_pGwc->GetGateway();
}
/*****/
BOOL CAVISClient::End(DWORD dwTimeout, BOOL boKillIfTimedOut)
{
	// let the ClientMain do the destroytransactions
	CAVISTransaction* pTransaction;

	CAVISTransactionsApp::s_pTc->debug("Cancelling all transactions for client...");

	PostQuitMessage();

	// stop all of the clients for all of the gateways
	for (POSITION pos = m_transactionList.GetHeadPosition(); pos != NULL; )
	{
		// get the next gateway in the list
		CAVISTransactionsApp::s_pTc->debug("Getting next transaction in list...");
		pTransaction = (CAVISTransaction* )m_transactionList.GetNext(pos);

		// synchronous call; returns if transaction is not running or when it has
		// completed.
		CAVISTransactionsApp::s_pTc->debug("Issuing request to transaction to cancel itself...");
		pTransaction->CancelTransaction();
		CAVISTransactionsApp::s_pTc->debug("Transaction has cancelled itself.");
	}

	CAVISTransactionsApp::s_pTc->debug("All transactions for client have been cancelled...");

	// post message to client thread so that it will exit; 
	// returns only when the thread has terminated/timed out
	return CThread::End(dwTimeout, boKillIfTimedOut);
}
/*****/
CTransactionTimeoutMonitorThread* CAVISClient::GetTransactionTimeoutMonitorThread()
{
	return &m_threadTransactionTimeoutMonitor;
}
