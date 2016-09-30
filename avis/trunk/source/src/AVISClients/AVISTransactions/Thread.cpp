// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactions.h"
#include <process.h>
#include <SystemException.h>
#include "Thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
: m_hThread(NULL),
  m_uiThreadId(0)
{
}
/*****/
CThread::~CThread()
{
	End();
}
/*****/
unsigned _stdcall CThread::ThreadProcStatic(LPVOID lpVoid)
{
	// force all system exceptions to be thrown as typed C++ exceptions
	SystemException::Init();

	// Cast the thread parameter to a pointer to our thread object
	CThread* pThread = (CThread* )lpVoid;

	// Call the virtual thread procedure
	DWORD rc = pThread->ThreadProcWrapper(lpVoid);

	// make sure we clear out the message queue
	pThread->PurgeMessages();

	// return the return code from the thread procedure
	return rc;
}
/*****/
DWORD CThread::ThreadProcWrapper(LPVOID lpVoid)
{
	// perform initialization

	// call base class thread function
	DWORD rc = ThreadProc(lpVoid);

	// perform cleanup
	PurgeMessages();

	m_uiThreadId = 0;

	return rc;
}
/*****/
BOOL CThread::Run(void)
{
	UINT uiThreadId;

	ULONG hThread = _beginthreadex(
		NULL,	// security,
		0,		// use same stack size as current thread
		ThreadProcStatic,
		(LPVOID )this, // arglist
		CREATE_SUSPENDED, // init flags
		&uiThreadId);

	if ( hThread == NULL ) // error creating the thread
	{
		CAVISTransactionsApp::s_pTc->critical("Failed creating thread, err = %d", GetLastError());
		return(FALSE);
	}

	// successfully created the thread
	m_hThread = (HANDLE )hThread;
	m_uiThreadId = uiThreadId;

	// now that critical data has been saved in this thread's object, start running it
	::ResumeThread(m_hThread);

	return(TRUE);
}
/*****/
BOOL CThread::WaitForThreadToTerminate(DWORD dwTimeout, BOOL boKillIfTimedOut)
{
	DWORD rc;

	if (m_uiThreadId == ::GetCurrentThreadId())
	{
		// prevent deadlock (i.e. thread waiting for itself to terminate)
		CAVISTransactionsApp::s_pTc->warning("Ignoring request to wait for self thread to terminate in order to avoid deadlock.");
		rc = TRUE;

	} else
	{
		// wait until the thread has exited
		CAVISTransactionsApp::s_pTc->debug("Waiting for thread with id = %d to terminate.", m_uiThreadId);
		switch (::WaitForSingleObject(m_hThread, dwTimeout))
		{
			case WAIT_OBJECT_0:
				CAVISTransactionsApp::s_pTc->debug("Thread terminated successfully.");
				rc = TRUE;
				break;

			case WAIT_TIMEOUT:
				CAVISTransactionsApp::s_pTc->debug("Timed out waiting for thread to terminate.");

				if (boKillIfTimedOut)
				{
					CAVISTransactionsApp::s_pTc->debug("Forcefully terminating thread...");
					::TerminateThread(m_hThread, 1);
					CAVISTransactionsApp::s_pTc->debug("Forcefully terminated thread.");
				}
				else
				{
					CAVISTransactionsApp::s_pTc->debug("Allowing unresponding thread to continue without terminating.");
				}
				rc = FALSE;
				break;

			case WAIT_FAILED:
				CAVISTransactionsApp::s_pTc->debug("Failed waiting for thread to terminate, err = %d.", GetLastError());
				rc = FALSE;
				break;

			default:
				rc = FALSE;
				break;
		}
	}
	return(rc);
}
/*****/
BOOL CThread::PostMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!::PostThreadMessage(m_uiThreadId, msg, wParam, lParam))
	{
		DWORD dwErr = GetLastError();

		if (dwErr == ERROR_INVALID_THREAD_ID)
			CAVISTransactionsApp::s_pTc->debug("Unable to post message to thread; it terminated or doesn't exist.");
		else
			CAVISTransactionsApp::s_pTc->error("Failed posting message to thread (err=%u).\n", dwErr);

		return FALSE;
	}

	CAVISTransactionsApp::s_pTc->debug("Posted message (id=%u, wParam=%u, lParam=%u) to thread.", msg, wParam, lParam);

	return TRUE;
}
/*****/
BOOL CThread::PostQuitMessage()
{
	return PostMessage(CThreadMessages::TM_QUIT);
}
/*****/
BOOL CThread::End(DWORD dwTimeout, BOOL boKillIfTimedOut)
{
	BOOL rc;

	// is the thread running?
	if (m_uiThreadId == 0)
	{
		CAVISTransactionsApp::s_pTc->debug("Cannot request thread to terminate; it isn't running.");
		return(FALSE);
	}

	// thread is running; tell it to terminate itself
	CAVISTransactionsApp::s_pTc->debug("Posting quit message to thread, so it can terminate itself...");
	if (PostQuitMessage())
	{
		CAVISTransactionsApp::s_pTc->debug("Posted TM_QUIT message to thread.");
		rc = WaitForThreadToTerminate(dwTimeout, boKillIfTimedOut);
	}

	// no longer need the handle
	::CloseHandle(m_hThread);

	m_hThread = NULL;
	m_uiThreadId = 0;

	return(rc);
}
/*****/
void CThread::PurgeMessages(UINT uMsg)
{
	MSG msg;

	CAVISTransactionsApp::s_pTc->debug("Removing remaining messages from thread's message queue...");

	// dequeue any remaining messages
	while (::PeekMessage(&msg, NULL, uMsg, uMsg, PM_REMOVE))
		PurgeMessage(&msg);

	CAVISTransactionsApp::s_pTc->debug("Removed remaining messages from thread's message queue.");
}
/*****/
void CThread::PurgeMessage(PMSG pMsg)
{
	// do nothing; just in case a derived class doesn't override this virtual function
}
/*****/
BOOL CThread::ReceivedQuitMessage(BOOL boRemove)
{
	MSG msg;

	return (::PeekMessage(&msg, NULL, CThreadMessages::TM_QUIT, CThreadMessages::TM_QUIT, boRemove ? PM_REMOVE : PM_NOREMOVE));
}
/*****/
BOOL CThread::Sleep(DWORD dwTimeout)
{
	CThreadMessages tm;

	return (tm.WaitForThreadMessage(NULL, dwTimeout) == -1);
}
/*****/
/*****/
/*****/
/*****/
CThreadMessages::CThreadMessages()
{
}
/*****/
CThreadMessages::~CThreadMessages()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_uiaMsgs.RemoveAll();
}
/*****/
int CThreadMessages::AddThreadMessage(UINT uMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_uiaMsgs.Add(uMsgId);
}
/*****/
int CThreadMessages::RemoveThreadMessageByValue(UINT uMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int iSize = m_uiaMsgs.GetSize();

	for (int i = 0; i < iSize; i++)
		if (m_uiaMsgs[i] == uMsgId)
		{
			m_uiaMsgs.RemoveAt(i);
			return 0;
		}

	return -1;
}
/*****/
int CThreadMessages::RemoveThreadMessageByIndex(UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_uiaMsgs.RemoveAt(index);
	return 0;
}
/*****/
/* Waits for the specified message to be received in the current thread. Will return
   when the specified timeout occurs. */
int CThreadMessages::WaitForThreadMessage(PMSG pMsg, DWORD dwTimeout)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	UINT uMsgId;
	int iSize = m_uiaMsgs.GetSize();
	MSG msg;

	while (1)
	{
		// check if there is a quit message
		if (::PeekMessage(&msg, NULL, TM_QUIT, TM_QUIT, PM_REMOVE))
		{
			if (pMsg != NULL)
				*pMsg = msg;

			return( TM_QUIT );
		}

		// if there are messages to be waited for...
		if (iSize > 0)
			for (int i = 0; i < iSize; i++)
			{
				uMsgId = m_uiaMsgs[i];

				// check if the requested message is in the queue
				if (::PeekMessage(&msg, NULL, uMsgId, uMsgId, PM_REMOVE))
				{
					*pMsg = msg;
					return( uMsgId );
				}
			}

		// no messages that we care about are in the queue, so wait until a new message
		// appears, then loop around and check again
		switch(::MsgWaitForMultipleObjects(
			0,				// no objects
			NULL,			// no objects
			FALSE,			// wait for any object to become signaled (including message arrival)
			dwTimeout,		// stop waiting if timeout reached
			QS_ALLINPUT))	// wait for any kind of message to arrive
		{
			case WAIT_OBJECT_0:	// message received
				// continue, and then peekmessage at top of loop
				break;

			case WAIT_TIMEOUT:	// timeout occurred
				return -1;

			case WAIT_FAILED:	// error
				// failed the wait, so sleep for a while
				CAVISTransactionsApp::s_pTc->critical("Failed waiting for message. (err=%d).\n", GetLastError());
				Sleep(100);
				break;
		} 
	}
}
/*****/
int CThreadMessages::WaitForSingleThreadMessage(PMSG pMsg, UINT uMsg, DWORD dwTimeout)
{
	CThreadMessages tm;

	tm.AddThreadMessage(uMsg);

	return tm.WaitForThreadMessage(pMsg, dwTimeout);
}
