// lock.h - another nobrainer abstraction

#if !defined (__LOCK_H__)
#define __LOCK_H__


// This abstracts the locking operation itself from the synchronization
// object itself.  The semantics are nice and simple for single
// locks.  Used in conjunction with the NamedMutex and CriticalSection
// classes (both children of LockableObject) they work very nicely.
// Especially in the "exception handling" stickiness of C++

#include "lckobj.h"

class Lock
{
public:
  Lock(LockableObject& lockobj_);
  virtual ~Lock();
private:
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
