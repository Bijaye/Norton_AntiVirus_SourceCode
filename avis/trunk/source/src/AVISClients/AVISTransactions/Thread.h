// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_)
#define AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"

// *** Warning: never create a Cthread-derived object on the stack or as
//              a member of a short-lived object, unless the thread you are creating
//              is guaranteed to complete before the object gets destroyed.  If a
//              CThread-derived object goes out of scope while the thread is still
//              running, the object's data will become invalid before the thread
//              attempts to access it.  Instead, create the CThread-derived object
//              on the heap (using new).  Do not delete the CThread-derived object
//              until the thread has completed.  Alternatively, declare the 
//              CThread-derived object statically, or as a member of another static
//              object.

class IMPEXP CThread  
{
private:
	HANDLE m_hThread;
	UINT m_uiThreadId;

public:
	CThread();
	virtual ~CThread();
	virtual BOOL Run(void);
	BOOL PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostQuitMessage();
	virtual BOOL End(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = TRUE);
	BOOL WaitForThreadToTerminate(DWORD dwTimeout = INFINITE, BOOL boKillIfTimedOut = TRUE);
	static BOOL Sleep(DWORD dwTimeout = INFINITE);
	static BOOL ReceivedQuitMessage(BOOL boRemove);

private:
	DWORD ThreadProcWrapper(LPVOID lpVoid);
	static unsigned _stdcall ThreadProcStatic(LPVOID lpVoid);

protected:
	virtual DWORD ThreadProc(LPVOID lpVoid) = 0;
	void PurgeMessages(UINT uMsg = 0); // default is to purge all messages
	virtual void PurgeMessage(PMSG pMsg);
};

/*********************/
/* utility class     */
/*********************/

class CThreadMessages
{
private:
	CUIntArray m_uiaMsgs;

public:
	CThreadMessages();
	~CThreadMessages();

	enum { TM_QUIT = WM_QUIT } THREAD_MSG_ID;


	int AddThreadMessage(UINT uMsgId);
	int RemoveThreadMessageByValue(UINT uMsgId);
	int RemoveThreadMessageByIndex(UINT index);
	int WaitForThreadMessage(PMSG pMsg, DWORD dwTimeout = INFINITE);
	static int WaitForSingleThreadMessage(PMSG pMsg, UINT uMsg, DWORD dwTimeout = INFINITE);
};

#endif // !defined(AFX_THREAD_H__DB48F9BA_B167_11D2_8A8D_00203529AC86__INCLUDED_)
