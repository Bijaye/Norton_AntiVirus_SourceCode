// namedmutex.h - another no-brainer lock object wrapper

#if !defined (__NAMEDMUTEX_H__)
#define __NAMEDMUTEX_H__

#include <process.h> 
#include <windows.h>
#include <string>
#include "lckobj.h"


//
//: Encapsulation of a Win32 named mutex kernel object
//
// The NamedMutex class takes a const char* parameter in the
// constructor representing the name of the mutex to create.  The
// constructor calls CreateMutex, which will attempt to create a mutex
// with the specified name.  If there is already a mutex in the kernel
// with that name, CreateMutex grabs a "handle" to that mutex such
// that it may be used here.
//
// Note that the act of creating or "referencing" a mutex does not
// attempt to lock it.
//
// Because of the possible overhead and the number of times mutexes
// are accessed and used, all methods are declared as inline and exist
// in this header file.
//

class NamedMutex : virtual public LockableObject
{
public:

  //
  //: Explicit constructor.
  //
  // Creates a new mutex, or references an existing one.  Does not
  // attempt to lock the mutex.
  //

  explicit NamedMutex(const char* name_);

  //
  //: Virtual destructor
  //
  // Releases our handle to the kernel's mutex object.  Win32 will
  // take care of destroying the kernel object if it's reference count
  // drops to zero.
  //

  virtual ~NamedMutex();

  //
  //: "Lock" the mutex.
  //
  // This operation will block if the mutex is held by another thread
  // or process, until that lock is released. 
  //

  virtual void capture();

  //
  //: Release a held mutex
  //
  // Releases the lock held on the mutex, allowing other threads and
  // or processes to access the shared resource.
  
  virtual void release();

  //
  //: "Short-Circuit" the encapsulation to allow access to the mutex
  //: handle.
  //
  // I'll go on record saying that the requirement for this method is
  // destructive.  However, it was not able to be avoided at the time.
  //
  // get_handle() returns the mutex handle held by this object.  This
  // violates encapsulation very badly and should never ever be used.
  // If you actually call this method you're doing something WRONG.
  //

  HANDLE get_handle() { return _mutexhandle;};

private:

  //
  //: Handle to the kernel mutex object
  //

  HANDLE _mutexhandle;

  //
  //: Name of the mutex referenced.
  //
  // This is a string representation of the parameter passed into the
  // constructor used to create or reference the mutex.
  //

  std::string _name;


  //
  //: Intentionally unimplemented default constructor
  //
  
  NamedMutex();
  
  //
  //: Intentionally unimplemented const copy constructor
  //
  
  NamedMutex(const NamedMutex&);
  
  //
  //: Intentionally unimplemented assignment operator
  //

  NamedMutex& operator =(const NamedMutex&);
};


inline NamedMutex::NamedMutex(const char* name_)
  :LockableObject(),
   _name(name_),
   _mutexhandle(0)
{ 
    // Create the mutex 
  _mutexhandle = CreateMutex(0,false,name_);
}
inline NamedMutex::~NamedMutex()
{
    CloseHandle(_mutexhandle);
    // This is allocated and freed by
    // the API
    _mutexhandle = 0;
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
