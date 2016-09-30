#include "spthread.h"



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sInitThread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sInitThread ( pthread_attr_t * pthread_attr )
{

#if defined (SYM_WIN32)
    pthread_attr;

#elif defined(SYM_UNIX)
    pthread_attr_init ( pthread_attr );
    pthread_attr_setscope ( pthread_attr, PTHREAD_SCOPE_SYSTEM );

#elif defined(SYM_NLM)
	pthread_attr;

#else
#error
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sDestroyThread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sDestroyThread ( pthread_attr_t * pthread_attr )
{

#if defined (SYM_UNIX)
    pthread_attr_destroy ( pthread_attr );

#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		SHANDLE sCreateThread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		SHANDLE
//
///////////////////////////////////////////////////////////////////////////////
SHANDLE sCreateThread ( LPVOID start_address, LPVOID arglist, pthread_attr_t * pthread_attr )
{
    SHANDLE m_handle;

#if defined (SYM_WIN32)
    pthread_attr;

    m_handle = _beginthread( ( void ( __cdecl * ) ( void * ) ) start_address, 0, arglist );
    if ( m_handle == -1 )
		return ( NULL );

#elif defined(SYM_UNIX)
    if ( pthread_create( &m_handle, pthread_attr, ( void * ( * ) ( void * ) ) start_address, arglist ) != 0 ) 
		return ((SHANDLE) NULL );

    pthread_detach ( m_handle );

#elif defined (SYM_NLM)
	// Not sure if this is 100% correct.
	BeginThread( (void(*)(void*))start_address, NULL, 262144, arglist);

#elif
#error
#endif

    return ( m_handle );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sExitThread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sExitThread ( void )
{

#if defined (SYM_WIN32)
    _endthread();

#elif defined(SYM_UNIX)
     pthread_exit(NULL);

#elif defined(SYM_NLM)
	// Not sure if this is 100% correct.
	ExitThread();

#else
#error
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sCreateMutex()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sCreateMutex 
( 
    SMUTEXHANDLE    *m_mutex
)
{

#if defined (SYM_WIN32)
    InitializeCriticalSection( m_mutex );

#elif defined (SYM_UNIX)
    pthread_mutex_init( m_mutex, NULL );

#elif defined (SYM_NLM)
	m_mutex;	// No mutexes on Netware, we just block the whole application
				// when entering a critical section, I think (?).

#else
#error
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sReleaseMutex()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
////////////////////////////////////////////////////////////////////////////////
void sReleaseMutex 
( 
    SMUTEXHANDLE    *m_mutex
)
{

#if defined (SYM_WIN32)
    DeleteCriticalSection( m_mutex );

#elif defined(SYM_UNIX)
    pthread_mutex_destroy( m_mutex );

#elif defined (SYM_NLM)
	m_mutex;		// No mutexes on Netware, we just block the whole application
					// when entering a critical section, I think (?).

#else
#error
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sLockMutex()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sLockMutex
( 
    SMUTEXHANDLE    *m_mutex
)
{

#if defined (SYM_WIN32)
    EnterCriticalSection( m_mutex );

#elif defined (SYM_UNIX)
    pthread_mutex_lock( m_mutex );

#elif defined (SYM_NLM)
	// Not sure if this is 100% correct.
	EnterCritSec();

#else
#error
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sUnLockMutex()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sUnLockMutex ( SMUTEXHANDLE *m_mutex )
{

#if defined (SYM_WIN32)
    LeaveCriticalSection( m_mutex );

#elif defined (SYM_UNIX)
    pthread_mutex_unlock( m_mutex );

#elif defined (SYM_NLM)
	// Not sure if this is 100% correct.
	ExitCritSec();

#else
#error
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void sSleep()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void sSleep ( WORD delay )
{

#if defined (SYM_WIN32)
    Sleep ( delay );

#elif defined (SYM_UNIX)
    poll(NULL, 0, delay);

#elif defined (SYM_NLM)
	ThreadSwitchWithDelay();	// Not sure if this is the right call here (?).

#else
#error
#endif

}
