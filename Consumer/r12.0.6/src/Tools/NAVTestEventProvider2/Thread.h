// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__E6CD0749_F5C2_4DDF_8BA7_7AAFE86CAB3C__INCLUDED_)
#define AFX_THREAD_H__E6CD0749_F5C2_4DDF_8BA7_7AAFE86CAB3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StahlSoft.h"

class CThread  
{
public:
	CThread();
	virtual ~CThread();

    virtual bool    Run ();             // Returns when the thread has started
    virtual bool    Stop ();            // Returns when the thread has stopped
    static unsigned int __stdcall threadMain( void* pv );

protected:
    HANDLE m_hShutdownEvent;
    HANDLE m_hThreadStartedEvent;
    virtual int DoWork () = 0;          // You must listen for m_hShutdownEvent
    HANDLE m_hThread;
};

#endif // !defined(AFX_THREAD_H__E6CD0749_F5C2_4DDF_8BA7_7AAFE86CAB3C__INCLUDED_)
