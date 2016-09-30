// Copyright Symantec Corporation, Peter Norton Product Group
//*****************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/ccritsec.h_v   1.0   05 Aug 1998 07:58:10   mmendon  $
//
// cCriticalSection and cCriticalSectionLock interfaces
//
//*****************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/ccritsec.h_v  $
// 
//    Rev 1.0   05 Aug 1998 07:58:10   mmendon
// Initial revision.
// 
//    Rev 1.0   21 Nov 1997 11:22:24   ERAHHAL
// Initial revision.
// 
//*****************************************************************************

//-----------------------------------------------------------------------------
// Usage:
//
//   cCriticalSection implements a standard Win32 CRITICAL_SECTION type.
//
//   To use it:
//
//      - declare a cCriticalSection object in your class, or module; it is
//        created via the default constructor:
//
//              cCriticalSection    criticalSectionObj;
//
//      - when entering the critical section, call the Enter() method:
//
//              criticalSectionObj.Enter();
//
//      - when leaving the critical section, call the Leave() method:
//
//              criticalSectionObj.Leave();
//
//   Or better yet, use the cCriticalSectionLock object (see next section)!
//
//-----------------------------------------------------------------------------
//
//   To use the cCriticalSectionLock:
//
//      - declare a cCriticalSection object in your class, or module; it is
//        created via the default constructor:
//
//              cCriticalSection    criticalSectionObj;
//
//      - when entering the critical section, create a cCriticalSectionLock
//        object and pass it a pointer to the appropriate cCriticalSection
//        object:
//
//              cCriticalSectionLock   csl(&criticalSectionObj);
//
//      - when leaving the critical section, don't do anything special!
//        The cCriticalSectionLock will automatically release the
//        cCriticalSection object when it goes out of scope.
//
//        Why is this good?  If an exception is thrown while processing
//        inside of the critical section, the cCriticalSectionLock object
//        will automatically be destroyed (as it goes out scope) and will
//        release the lock.
//
//-----------------------------------------------------------------------------

#ifndef _CCRITSEC_H_
#define _CCRITSEC_H_

#include "platform.h"
#include <windows.h>

//-----------------------------------------------------------------------------
// cCriticalSection class
//-----------------------------------------------------------------------------

class cCriticalSection
    {
    public:

        // constructor
        cCriticalSection()
            {InitializeCriticalSection(&m_CriticalSection);}

        // destructor
        ~cCriticalSection()
            {DeleteCriticalSection(&m_CriticalSection);}

        // lock
        void Enter()
            {EnterCriticalSection(&m_CriticalSection);}

        // unlock
        void Leave()
            {LeaveCriticalSection(&m_CriticalSection);}

    private:

        // copy constructor removed from interface
        cCriticalSection(const cCriticalSection &source);
 
        // assignment op removed from interface
        cCriticalSection & operator = (const cCriticalSection &source);

        // private data
        CRITICAL_SECTION  m_CriticalSection;
};


//-----------------------------------------------------------------------------
// cCriticalSectionLock class
//-----------------------------------------------------------------------------

class cCriticalSectionLock
    {
    public:

        // constructor
        cCriticalSectionLock(cCriticalSection *cs)
            : m_pCriticalSection(cs)
            {if (m_pCriticalSection) m_pCriticalSection->Enter();}

        // destructor
        ~cCriticalSectionLock()
            {if (m_pCriticalSection) m_pCriticalSection->Leave();}

    private:

        // default constructor removed from interface
        cCriticalSectionLock();

        // copy constructor removed from interface
        cCriticalSectionLock(const cCriticalSectionLock &source);

        // assignment op removed from interface
        cCriticalSectionLock & operator = (const cCriticalSectionLock &source);

        // private data
        cCriticalSection *m_pCriticalSection;
};


#endif

/* end of header file */
