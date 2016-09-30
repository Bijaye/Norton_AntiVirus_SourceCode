// namedmutex.h - another no-brainer lock object wrapper

#if !defined (__NAMEDMUTEX_H__)
#define __NAMEDMUTEX_H__

#include <process.h> 
#include <windows.h>
#include <string>
#include "lckobj.h"

class NamedMutex : virtual public LockableObject
{
public:
  NamedMutex(const char* name_);
  virtual ~NamedMutex();
  virtual void capture();
  virtual void release();

  // After much deliberation I've decided to 
  // allow this.
  // Stupid practices can botch this up 
  // terribly

  HANDLE get_handle() { return _mutexhandle;};

private:
  HANDLE _mutexhandle;
  std::string _name;
};

// I want as little overhead as possible in this class
// so everything is inline

inline NamedMutex::NamedMutex(const char* name_)
:_name(name_)
{ 
    // Create the mutex 
  _mutexhandle = CreateMutex(0,false,name_);
}
inline NamedMutex::~NamedMutex()
{
    CloseHandle(_mutexhandle);
}
inline void NamedMutex::capture()
{
  WaitForSingleObject(_mutexhandle,INFINITE);
}
inline void NamedMutex::release()
{
  ReleaseMutex(_mutexhandle);
}


#endif /// __NamedMutex_H__ Sentinel
