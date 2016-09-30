// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "base.h"
//#include "Transactions.h"
#include <process.h>
#include "Thread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Thread::Thread( MyTraceClient& traceClient )
: _handle(),
  _threadId(0),
  _tc( traceClient )
{
}
/*****/
Thread::~Thread()
{
	//End();
}
/*****/
unsigned _stdcall Thread::ThreadProcStatic(LPVOID lpVoid)
{
	// Cast the thread parameter to a pointer to our thread object
	Thread* pThread = static_cast<Thread*>(lpVoid);

	// Call the virtual thread procedure
	DWORD rc = pThread->ThreadProcWrapper(lpVoid);

	// make sure we clear out the message queue
	pThread->PurgeMessages();
		
	// return the return code from the thread procedure
	return rc;
}
/*****/
DWORD Thread::ThreadProcWrapper(LPVOID lpVoid)
{
	// perform initialization

	// call base class thread function
	DWORD rc = ThreadProc(lpVoid);

	// perform cleanup
	PurgeMessages();

	_threadId = 0;

	_handle.close();

	return rc;
}
/*****/
BOOL Thread::Run(void)
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
		_tc.critical("error creating thread, err = %d", GetLastError());
		return(FALSE);
	}

	// successfully created the thread
	_handle.set( reinterpret_cast<HANDLE>(hThread) );
	_threadId = uiThreadId;

	// now that critical data has been saved in this thread's object, start running it
	::ResumeThread(_handle.get());

	return(TRUE);
}
/*****/
BOOL Thread::WaitForThreadToTerminate(DWORD dwTimeout, BOOL boKillIfTimedOut)
{
	DWORD rc;
	DWORD e = NO_ERROR;

	if (_threadId == ::GetCurrentThreadId())
	{
		// prevent deadlock (i.e. thread waiting for itself to terminate)
		_tc.warning("Ignoring request to wait for self thread to terminate in order to avoid deadlock.");
		rc = TRUE;

	} else
	{
		if ( !_handle.isValid() )
			return TRUE;
		// wait until the thread has exited
		_tc.debug("waiting for thread with id = %d to terminate.", _threadId);
		switch (::WaitForSingleObject(_handle.get(), dwTimeout))
		{
			case WAIT_OBJECT_0:
				_tc.debug("thread terminated successfully");
				rc = TRUE;
				break;

			case WAIT_TIMEOUT:
				_tc.debug("timed out waiting for thread to terminate");

				if (boKillIfTimedOut)
				{
					_tc.debug("forcefully terminating thread...");
					::TerminateThread(_handle.get(), 1);
					_tc.debug("forcefully terminated thread");
				}
				else
				{
					_tc.debug("allowing unresponding thread to continue without terminating");
				}
				rc = FALSE;
				break;

			case WAIT_FAILED:
				e = GetLastError();
				if ( e == ERROR_INVALID_HANDLE )	// that's OK, it means that thread
				{									// terminated and the handle is closed
					rc = TRUE;
				}
				else
				{
					_tc.debug("failed waiting for thread to terminate, err = %d.", e );
					rc = FALSE;
				}
				break;

			default:
				rc = FALSE;
				break;
		}
	}
	return(rc);
}
/*****/
BOOL Thread::End(DWORD dwTimeout, BOOL boKillIfTimedOut)
{
	BOOL rc;

	// is the thread running?
	if (_threadId == 0)
	{
		_tc.debug("cannot request thread to terminate; it isn't running");
		return(FALSE);
	}

	// thread is running; tell it to terminate itself
	_tc.debug("posting WM_QUIT message to thread, so it can terminate itself...");
	if (!::PostThreadMessage(_threadId, WM_QUIT, 0, 0))
	{
		if (GetLastError() == ERROR_INVALID_THREAD_ID)
		{
			_tc.debug("no need to terminate thread; it already terminated");
			rc = TRUE;

		} else
		{
			_tc.error("failed calling PostThreadMessage() (err=%d).\n", GetLastError());
			rc = FALSE;
		}

	} else
	{
		_tc.debug("posted WM_QUIT message to thread");
		rc = WaitForThreadToTerminate(dwTimeout, boKillIfTimedOut);
	}

	// no longer need the handle
	_handle.close();//::CloseHandle(m_hThread); // we don't need it since we use AutoHandle
	//m_hThread = NULL;

	_threadId = 0;

	return(rc);
}
/*****/
void Thread::PurgeMessages(UINT uMsg)
{
	MSG msg;

	_tc.debug("removing remaining messages from thread's message queue...");

	// dequeue any remaining messages
	while (::PeekMessage(&msg, NULL, uMsg, uMsg, PM_REMOVE))
		PurgeMessage(&msg);

	_tc.debug("removed remaining messages from thread's message queue");
}
/*****/
void Thread::PurgeMessage(PMSG pMsg)
{
	// do nothing; just in case a derived class doesn't override this virtual function
}
/*****/
BOOL Thread::ReceivedQuitMessage()
{
	MSG msg;

	return (::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE));
}
/*****/
//MM//BOOL Thread::Sleep(DWORD dwTimeout)
//MM//{
//MM//	ThreadMessages tm( _tc );
//MM//
//MM//	return (tm.WaitForThreadMessageXXX(NULL, dwTimeout) == -1);
//MM//}
/*****/
/*****/
/*****/
/*****/
//MM//ThreadMessages::ThreadMessages( MyTraceClient& traceClient )
//MM//: _tc( traceClient )
//MM//{
//MM//}
/*****/
//MM//ThreadMessages::~ThreadMessages()
//MM//{
//MM//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//MM//
//MM//	m_uiaMsgs.RemoveAll();
//MM//}
/*****/
//MM//int ThreadMessages::AddThreadMessage(UINT uMsgId)
//MM//{
//MM//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//MM//
//MM//	return m_uiaMsgs.Add(uMsgId);
//MM//}
/*****/
//MM//int ThreadMessages::RemoveThreadMessageByValue(UINT uMsgId)
//MM//{
//MM//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//MM//
//MM//	int iSize = m_uiaMsgs.GetSize();
//MM//
//MM//	for (int i = 0; i < iSize; i++)
//MM//		if (m_uiaMsgs[i] == uMsgId)
//MM//		{
//MM//			m_uiaMsgs.RemoveAt(i);
//MM//			return 0;
//MM//		}
//MM//
//MM//	return -1;
//MM//}
/*****/
//MM//int ThreadMessages::RemoveThreadMessageByIndex(UINT index)
//MM//{
//MM//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//MM//
//MM//	m_uiaMsgs.RemoveAt(index);
//MM//	return 0;
//MM//}
/*****/
/* Waits for the specified message to be received in the current thread. Will return
   when the specified timeout occurs. */
//MM//int ThreadMessages::WaitForThreadMessageXXX(PMSG pMsg, DWORD dwTimeout)
//MM//{
//MM//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//MM//
//MM//	UINT uMsgId;
//MM//	int iSize = m_uiaMsgs.GetSize();
//MM//	MSG msg;
//MM//
//MM//	while (1)
//MM//	{
//MM//		// check if there is a quit message
//MM//		if (::PeekMessage(&msg, NULL, TM_QUIT, TM_QUIT, PM_REMOVE))
//MM//		{
//MM//			if (pMsg != NULL)
//MM//				*pMsg = msg;
//MM//
//MM//			return( TM_QUIT );
//MM//		}
//MM//
//MM//		// if there are messages to be waited for...
//MM//		if (iSize > 0)
//MM//			for (int i = 0; i < iSize; i++)
//MM//			{
//MM//				uMsgId = m_uiaMsgs[i];
//MM//
//MM//				// check if the requested message is in the queue
//MM//				if (::PeekMessage(&msg, NULL, uMsgId, uMsgId, PM_REMOVE))
//MM//				{
//MM//					*pMsg = msg;
//MM//					return( uMsgId );
//MM//				}
//MM//			}
//MM//
//MM//		// no messages that we care about are in the queue, so wait until a new message
//MM//		// appears, then loop around and check again
//MM//		switch(::MsgWaitForMultipleObjects(
//MM//			0,				// no objects
//MM//			NULL,			// no objects
//MM//			FALSE,			// wait for any object to become signaled (including message arrival)
//MM//			dwTimeout,		// stop waiting if timeout reached
//MM//			QS_ALLINPUT))	// wait for any kind of message to arrive
//MM//		{
//MM//			case WAIT_OBJECT_0:	// message received
//MM//				// continue, and then peekmessage at top of loop
//MM//				break;
//MM//
//MM//			case WAIT_TIMEOUT:	// timeout occurred
//MM//				return -1;
//MM//
//MM//			case WAIT_FAILED:	// error
//MM//				// failed the wait, so sleep for a while
//MM//				_tc.critical("Failed waiting for message. (err=%d).\n", GetLastError());
//MM//				Sleep(100);
//MM//				break;
//MM//		} 
//MM//	}
//MM//}
/*****/
//MM//BOOL ThreadMessages::WaitForThreadMessageXXX(PMSG pMsg, UINT uMsg, DWORD dwTimeout)
//MM//{
//MM//	ThreadMessages tm( _tc );
//MM//
//MM//	tm.AddThreadMessage(uMsg);
//MM//
//MM//	return( tm.WaitForThreadMessageXXX(pMsg, dwTimeout) != TM_QUIT );
//MM//}
