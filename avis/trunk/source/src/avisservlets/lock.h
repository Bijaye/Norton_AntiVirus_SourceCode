// lock.h - another nobrainer abstraction

#if !defined (__LOCK_H__)
#define __LOCK_H__

#include "lckobj.h"

//
//: Encapsulation of a lock operation.  The lock is "grabbed" on
//: construction, and released on destruction of this object
//
// This abstracts the locking operation itself from the synchronization
// object itself.  The semantics are nice and simple for single
// locks.  Used in conjunction with the NamedMutex and CriticalSection
// classes (both children of LockableObject) they work very nicely.
// Especially in the "exception handling" stickiness of C++


class Lock
{
public:

  //
  //: Constructor: Attempts to grab the lock passed in as a parameter
  //
  // Note that this constructor takes a non-const reference to a child
  // of the LockableObject class.  This allows an external resource
  // manager to maintain the lock objects.  This class is only an
  // operational interface, used to encapsulate the operation of
  // locking and unlocking a shared resource.
  //

  Lock(LockableObject& lockobj_);

  //
  //: Virtual Destructor: Releases the encapsulated lock object and
  //: exits.
  //

  virtual ~Lock();


private:

  //
  //: "The actual lock" acted upon in the constructor and
  //: deststructor.
  //
  // THe Lock class does not own this object, therefore does not
  // attempt any cleanup.  However, because the capture() and
  // release() methods in LockableObject are non-const operations,
  // this must be a non-const reference; a small price to pay for such
  // clean semantics.
  //

  LockableObject& _lockobj;
};

inline Lock::Lock(LockableObject& lockobj_)
  :_lockobj(lockobj_)
{
  _lockobj.capture();
}
inline Lock::~Lock()
{
  _lockobj.release();
}


#endif // __LOCK_H__
