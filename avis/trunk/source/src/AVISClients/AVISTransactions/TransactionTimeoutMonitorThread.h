// TransactionTimeoutMonitorThread.h: interface for the CTransactionTimeoutMonitorThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSACTIONTIMEOUTMONITORTHREAD_H__6B07CA43_C21C_11D3_8AD9_00203529AC86__INCLUDED_)
#define AFX_TRANSACTIONTIMEOUTMONITORTHREAD_H__6B07CA43_C21C_11D3_8AD9_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"

class CTransactionTimeoutMonitorThread : public CThread
{
private:
	DWORD m_dwClientThreadId;

public:
	CTransactionTimeoutMonitorThread();
	virtual ~CTransactionTimeoutMonitorThread();

protected:
	virtual DWORD ThreadProc(LPVOID lpVoid);

public:
	BOOL PostTransactionStartingMessage(UINT uiTimeoutTransactionConnect);
	BOOL PostTransactionConnectedMessage(UINT uiTimeoutTransactionCompleted);
	BOOL PostTransactionCompletedMessage();

public:
	BOOL Run(DWORD dwClientThreadId);
};

#endif // !defined(AFX_TRANSACTIONTIMEOUTMONITORTHREAD_H__6B07CA43_C21C_11D3_8AD9_00203529AC86__INCLUDED_)
