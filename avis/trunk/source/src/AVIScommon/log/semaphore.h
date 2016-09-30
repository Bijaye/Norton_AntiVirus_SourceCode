// semaphore.h - full inline implementation of a win32 semaphore

#if !defined (__SEMAPHORE_H__)
#define __SEMAPHORE_H__

#include <process.h>
#include <windows.h>
#include <string>
#include "lckobj.h"

class Semaphore : virtual public LockableObject
{
public:
  Semaphore(const char* name_,
            long initialcount_,
            long max_count_);
  virtual ~Semaphore();
  virtual void capture();
  virtual void release();

private:
  HANDLE _semaphore_handle;
  std::string _name;
  long _max_count;
  long _current_count;
};

// To minimize overhead I'm dropping everything inline (as with the
// NamedMutex and CriticalSection class headers)

inline Semaphore::Semaphore(const char* name_,
                            long initialcount_,
                            long max_count_)
  :_semaphore_handle(NULL),
   _name(name_),
   _current_count(initialcount_),
   _max_count(max_count_)
{
  // First try to grab the semaphore (it may already exist)

  _semaphore_handle = OpenSemaphore(SEMAPHORE_ALL_ACCESS,
                                    false,
                                    _name.c_str());
  if (_semaphore_handle == NULL)
    {
      _semaphore_handle = CreateSemaphore(NULL,
                                          _current_count,
                                          _max_count,
                                          _name.c_str());
    }
}

inline Semaphore::~Semaphore()
{
  CloseHandle(_semaphore_handle);
}
inline void Semaphore::capture()
{
  WaitForSingleObject(_semaphore_handle,INFINITE);
}
inline void Semaphore::release()
{
  ReleaseSemaphore(_semaphore_handle,1,&_current_count);
}
   


#endif // __SEMAPHORE_H__ Sentinel
