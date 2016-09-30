// critsec.h - a no-brainer Critical section wrapper

#if !defined (__CRITSEC_H__)
#define __CRITSEC_H__

#include <process.h>
#include <windows.h>

#include "lckobj.h"

//
//: Encapsulation of a Win32 CriticalSection
//
// This is used to lock a particular section of code that may be
// accessed through multiple threads within the same process.
//
// CriticalSection is decended off of LockableObject to provide
// a clean abstract interface implementation.
//
// It would be trivial to implement a "posix_CriticalSection" etc. to
// provide multi platform support.

class CriticalSection : virtual public LockableObject
{

public:

  //
  //: Constructor: Initializes (but does not lock) the critical
  //: section.
  //

  CriticalSection          () { InitializeCriticalSection (
                                                           &_critsec);
  };

  //
  //: Virtual Destructor
  //

  virtual ~CriticalSection () { DeleteCriticalSection     ( &_critsec ); };

  //
  //: Lock the critical section.
  //
  // This will block if another thread has locked this critical
  // section, and return only when that thread calls the 
  // release() method.
  //

  virtual void capture     () { EnterCriticalSection      ( &_critsec
                                                            ); };
  //
  //: Release the critical section
  //
  // This will release the CriticalSection lock, making the shared
  // resource (whatever it may be) eligible for use by another
  // thread. 
  //

  virtual void release     () { LeaveCriticalSection      ( &_critsec ); };

private: 

  //
  //: The Critical Section handle
  //

  CRITICAL_SECTION _critsec;
};
#endif // __CRITSEC_H__
