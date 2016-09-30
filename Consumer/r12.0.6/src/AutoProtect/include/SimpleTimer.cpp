#include "StdAfx.h"
#include "simpletimer.h"

CSimpleTimer::CSimpleTimer(void)
: m_dwStartTime(0)
, m_dwElapsedTime(0)
{
    InitializeCriticalSection (&m_crit);
}

CSimpleTimer::~CSimpleTimer(void)
{
    DeleteCriticalSection (&m_crit);
}

void CSimpleTimer::Start(void)
{
    EnterCriticalSection (&m_crit);
    m_dwStartTime = ::GetTickCount ();
    LeaveCriticalSection (&m_crit);
}

DWORD CSimpleTimer::Stop(void)
{
    EnterCriticalSection (&m_crit);
    if ( m_dwStartTime )
    {
        DWORD dwTickCount = ::GetTickCount ();

        if ( m_dwStartTime > dwTickCount)   // roll-over
        {                                        
            m_dwElapsedTime = (UINT_MAX - m_dwStartTime) + dwTickCount;
        }
        else
            m_dwElapsedTime = dwTickCount - m_dwStartTime;
    }
    LeaveCriticalSection (&m_crit);

    return m_dwElapsedTime;
}

void CSimpleTimer::Reset (void)
{
    EnterCriticalSection (&m_crit);
    m_dwStartTime = 0;
    m_dwElapsedTime = 0;
    LeaveCriticalSection (&m_crit);
}

DWORD CSimpleTimer::GetElapsedTime(void)
{
    return m_dwElapsedTime;
}
