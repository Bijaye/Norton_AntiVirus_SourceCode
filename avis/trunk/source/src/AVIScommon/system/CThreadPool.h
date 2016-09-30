//
// FILE: CThreadPool.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#ifndef __CThreadPool_H__
#define __CThreadPool_H__

// CThreadPool is an abstract base class that defines the
// semantics of a thread pool object.
//
class AVISCOMMON_API CThreadPool
{
    public:
        virtual ~CThreadPool() {}

        // The DispatchThread function grabs an idle thread from the
        // pool and causes it to resume execution by invoking the
        // supplied thread handler object's ThreadHandlerProc
        // procedure.  This is the same thread handler class that is
        // used by the CMclThread class.
        //
        virtual BOOL DispatchThread( CMclThreadHandler *pHandler ) = 0;
};

#endif // __CThreadPool_H__
