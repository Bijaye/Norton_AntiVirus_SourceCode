// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
{
    m_hThread = NULL;

    // The thread needs to shut down
    //
    m_hShutdownEvent = CreateEvent ( NULL, TRUE, FALSE, NULL );     // Manual event     
    
    // The thread has started
    //
    m_hThreadStartedEvent = CreateEvent ( NULL, TRUE, FALSE, NULL );    // Manual event     
}

CThread::~CThread()
{
    Stop();

    if ( m_hShutdownEvent )
    {
        CloseHandle ( m_hShutdownEvent );
        m_hShutdownEvent = NULL;
    }

    if ( m_hThreadStartedEvent )
    {
        CloseHandle ( m_hThreadStartedEvent );
        m_hShutdownEvent = NULL;
    }

    if (m_hThread != NULL)
    {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

bool CThread::Run ()
{
    // Don't start if we are already running.
    //
    if ( !m_hThread )
    {
        UINT uiTemp = 0;

        ::ResetEvent ( m_hThreadStartedEvent );
        m_hThread = (HANDLE)_beginthreadex ( NULL, 0, threadMain, this, 0, &uiTemp );
        if ( m_hThread == NULL )
            return false;

        // We have to still process messages while we wait
        // otherwise we will never get the IDispatch messages
        // which are sent as Windows messages.
        //
        StahlSoft::WaitForSingleObjectWithMessageLoop(m_hThreadStartedEvent,INFINITE);

        return true;
    }    
    else
        return false;
}

bool CThread::Stop ()
{
    if ( m_hThread )
    {
        // Send the shut down message
        //
        ::SetEvent ( m_hShutdownEvent );

        // We have to still process messages while we wait
        // otherwise we will never get the IDispatch messages
        // which are sent as Windows messages.
        //
        StahlSoft::WaitForSingleObjectWithMessageLoop(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = NULL;

        return true;
    }
    else
        return false;
}

unsigned int __stdcall CThread::threadMain( void* pv )
{
    if ( !pv )
        return 0;
   
    CThread* pThis = static_cast<CThread*> (pv);

    // Thread must call ::SetEvent ( m_hThreadStartedEvent );
    // When it has started.

    return pThis->DoWork();
}
