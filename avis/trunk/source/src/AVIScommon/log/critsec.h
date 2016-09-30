// critsec.h - a no-brainer Critical section wrapper

#if !defined (__CRITSEC_H__)
#define __CRITSEC_H__


#include <process.h>
#include <windows.h>

#include "lckobj.h"

class CriticalSection : virtual public LockableObject
{

public:
  CriticalSection          () { InitializeCriticalSection ( &_critsec ); };
  virtual ~CriticalSection () { DeleteCriticalSection     ( &_critsec ); };
  virtual void capture     () { EnterCriticalSection      ( &_critsec ); };
  virtual void release     () { LeaveCriticalSection      ( &_critsec ); };

private: 

  CRITICAL_SECTION _critsec;
};
#endif // __CRITSEC_H__
