// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_)
#define AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_

#include "AutoHandle.h"
#include "MyTraceClient.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// *** Warning: never create a Thread-derived object on the stack or as
//              a member of a short-lived object, unless the thread you are creating
//              is guaranteed to complete before the object gets destroyed.  If a
//              Thread-derived object goes out of scope while the thread is still
//              running, the object's data will become invalid before the thread
//              attempts to access it.  Instead, create the Thread-derived object
//              on the heap (using new).  Do not delete the Thread-derived object
//              until the thread has completed.  Alternatively, declare the 
//              Thread-derived object statically, or as a member of another static
//              object.

class Thread  
{
private:
	AutoHandle		_handle;
	UINT			_threadId;
	MyTraceClient&	_tc;

public:
	Thread( MyTraceClient& traceClient );
	virtual ~Thread();

	virtual BOOL Run(void);
	virtual BOOL End(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = FALSE);
	BOOL WaitForThreadToTerminate(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = FALSE);
//MM//	BOOL Sleep(DWORD dwTimeout = INFINITE);

private:
	DWORD ThreadProcWrapper(LPVOID lpVoid);
	static unsigned _stdcall ThreadProcStatic(LPVOID lpVoid);

protected:
	virtual DWORD ThreadProc(LPVOID lpVoid) = 0;
	void PurgeMessages(UINT uMsg = 0); // default is to purge all messages
	virtual void PurgeMessage(PMSG pMsg);
	static BOOL ReceivedQuitMessage();
};

/*********************/
/* utility class     */
/*********************/

//MM//class ThreadMessages
//MM//{
//MM//private:
//MM//	CUIntArray m_uiaMsgs;
//MM//	MyTraceClient&	_tc;
//MM//
//MM//public:
//MM//	ThreadMessages( MyTraceClient& traceClient );
//MM//	~ThreadMessages();
//MM//
//MM//	enum { TM_QUIT = WM_QUIT } THREAD_MSG_ID;
//MM//
//MM//
//MM//	int AddThreadMessage(UINT uMsgId);
//MM//	int RemoveThreadMessageByValue(UINT uMsgId);
//MM//	int RemoveThreadMessageByIndex(UINT index);
//MM//	int WaitForThreadMessageXXX(PMSG pMsg, DWORD dwTimeout = INFINITE);
//MM//	int WaitForThreadMessageXXX(PMSG pMsg, UINT uMsg, DWORD dwTimeout = INFINITE);
//MM//};

#endif // !defined(AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_)
