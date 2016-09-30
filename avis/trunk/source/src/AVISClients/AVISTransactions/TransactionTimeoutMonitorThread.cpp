// TransactionTimeoutMonitorThread.cpp: implementation of the CTransactionTimeoutMonitorThread class.
//
//////////////////////////////////////////////////////////////////////

/* Objects of this class should only be members of the CAVISClient class. Each client
   is, itself, a CThread-derived object. The goal of THIS class is to monitor the
   execution of transactions within a client thread. Because these clients cannot
   inherently determine whether the transactions have blocked or become frozen, the
   clients, themselves can become blocked indefinitely, without warning.
*/

#include "stdafx.h"
#include "TransactionTimeoutMonitorThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TM_TRANSACTION_STARTING		(WM_USER + 1)
#define TM_TRANSACTION_CONNECTED	(WM_USER + 2)
#define TM_TRANSACTION_COMPLETED	(WM_USER + 3)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransactionTimeoutMonitorThread::CTransactionTimeoutMonitorThread()
{
}
/*****/
CTransactionTimeoutMonitorThread::~CTransactionTimeoutMonitorThread()
{
}
/*****/
BOOL CTransactionTimeoutMonitorThread::Run(DWORD dwClientThreadId)
{
	m_dwClientThreadId = dwClientThreadId;

	return CThread::Run();
}
/*****/
DWORD CTransactionTimeoutMonitorThread::ThreadProc(LPVOID lpVoid)
{
	int iRet;
	MSG msg;

	UINT uiTimeoutTransactionCompleted;
	UINT uiTimeoutTransactionConnected;

	// register for log/trace
	CAVISTransactionsApp::s_pTc->RegisterThread(NULL, NULL, NULL, m_dwClientThreadId);

	// loop forever
	while (TRUE)
	{
		// wait for a "transaction starting" message
		CAVISTransactionsApp::s_pTc->info("Transaction Monitor is waiting for a 'transaction starting' message...");
		iRet = CThreadMessages::WaitForSingleThreadMessage(
			&msg, 
			TM_TRANSACTION_STARTING, 
			INFINITE);

		switch(iRet)
		{
			// exit this thread if a quit message was received while waiting
			case CThreadMessages::TM_QUIT:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a quit message.");
				return 0;

			// this should be the only other return value
			case TM_TRANSACTION_STARTING:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a 'transaction starting' message.");
				uiTimeoutTransactionConnected = msg.wParam;
				break;

			// this should never happen, since we can't receive a timeout.
			default:
				break;
		}

		CThreadMessages threadMessages;
		threadMessages.AddThreadMessage(TM_TRANSACTION_CONNECTED);
		threadMessages.AddThreadMessage(TM_TRANSACTION_COMPLETED);

		// wait for a "transaction connected" message
		CAVISTransactionsApp::s_pTc->info("Transaction Monitor is waiting for a 'transaction connected' message.");
		iRet = threadMessages.WaitForThreadMessage(&msg, uiTimeoutTransactionConnected);

		switch(iRet)
		{
			// exit this thread if a quit message was received while waiting
			case CThreadMessages::TM_QUIT:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a quit message.");
				return 0;

			// everything's ok
			case TM_TRANSACTION_CONNECTED:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a 'transaction connected' message.");
				uiTimeoutTransactionCompleted = msg.wParam;
				break;

			// just in case an error occurs during connection, we will receive
			// a "completed" message, but never see a "connected" message
			case TM_TRANSACTION_COMPLETED:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a 'transaction completed' message.");
				// go to the top of the loop, where we wait for a "starting" message.
				continue;

			// timeout occurred
			case -1:
				// *** generate a critical error ***
				(*CAVISTransactionsApp::s_pAutoTerminateFunction)("Timed out waiting for a 'transaction connected' message; automatically shutting process down...");
				return 1;
		}

		// wait for a "transaction complete" message
		iRet = CThreadMessages::WaitForSingleThreadMessage(
			&msg, 
			TM_TRANSACTION_COMPLETED, 
			uiTimeoutTransactionCompleted);

		switch(iRet)
		{
			// exit this thread if a quit message was received while waiting
			case CThreadMessages::TM_QUIT:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a quit message.");
				return 0;

			// everything's ok, go do another iteration of the loop
			case TM_TRANSACTION_COMPLETED:
				CAVISTransactionsApp::s_pTc->info("Transaction Monitor received a 'transaction completed' message.");
				break;

			// timeout occurred
			case -1:
				// *** generate a critical error ***
				(*CAVISTransactionsApp::s_pAutoTerminateFunction)("Timed out waiting for a 'transaction completed' message; automatically shutting process down...");
				return 1;
		}
	}
}
/*****/
BOOL CTransactionTimeoutMonitorThread::PostTransactionStartingMessage(UINT uiTimeoutTransactionConnect)
{
	return PostMessage(TM_TRANSACTION_STARTING, uiTimeoutTransactionConnect);
}
/*****/
BOOL CTransactionTimeoutMonitorThread::PostTransactionConnectedMessage(UINT uiTimeoutTransactionCompleted)
{
	return PostMessage(TM_TRANSACTION_CONNECTED, uiTimeoutTransactionCompleted);
}
/*****/
BOOL CTransactionTimeoutMonitorThread::PostTransactionCompletedMessage()
{
	return PostMessage(TM_TRANSACTION_COMPLETED);
}
