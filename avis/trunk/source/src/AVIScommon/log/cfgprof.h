// cfgprof.h - Prototype for the ConfigProfile class

#if !defined (__CFGPROF_H__)
#define __CFGPROF_H__

#include <string>
#include <map>
#include "types.h"
#include "aviscommon.h"

#pragma warning (disable : 4251)
// This really should be plenty
#if !defined (LINE_BUFFER_SIZE)
#define LINE_BUFFER_SIZE 1024
#endif // LINE_BUFFER_SIZE

 
class CriticalSection;

//  Loads a configuration file with the following format:
//     "%name: value" one per line, with all lines not beginning in '%' ignored.
//     into an STL map of type (std::map<std::string,std::string>) 

// Provides a thread-safe wrapper around access points to the hash table
// itself.

// Note: All access to the hash table is const-based, unless the object
//       is asked to reaload the file from disk.  This is to keep the 
//       intent a little more pure and seal the thread safety up tight.

class AVISCOMMON_API ConfigProfile
{
public:
  // Constructor takes the config filename as an argument
  ConfigProfile(const char* filename_);
  virtual ~ConfigProfile();

  // "Testing the Invariant"
  virtual bool validObject() const;

  //-------------------------------------------------------
  // This is just a sampling of a proxy interface
  // into the stl map container
  // Since I'm shielding for thread-safety, I've got to
  // reexpose these primitives.  Besides, we're read-only
  // here anyway. (which may mean the thread-safety is
  // superfluous, but I'm not taking any chances.)
  //-------------------------------------------------------

  bool exists(const std::string& key_) const;

  // A couple permutations of accessor operators
  // I'd return a reference, but even a const ref
  // could violate the thread-safety, if the 
  // const-ness was cast away & it was modified.
  // std::string is handle-body copy on write anyway,
  // so, no biggie.
  std::string operator[](const std::string& ) const;

  //: A debug method, toString just serializes the hash table
  // iteratively & returns a neatened copy
  std::string toString() const;

protected:

  MStringString _hashTable;
  
  bool loadFile(const char* filename_);
private:
  bool _loadedOk;
  // Prevent pass-by-value semantics by not implementing these
  //
  ConfigProfile(const ConfigProfile&); // Copy constructor.
  ConfigProfile& operator =(const ConfigProfile&);

  std::string _configfile;

  //: Static lock object to retain 'constness' while maintaining
  //: locking capabilities
  // The "EnterCriticalSection" and "LeaveCriticalSection" calls
  // modify the critical section.  Therefore, in order to 
  // make a synchronized method 'const' the critical section must
  // exist outside the object scope.  Making the critical section 
  // a static member achieves this aim nicely.  
  // (Note also that the nature of this class is that the number
  // of object instantiations and the number of accesses is very
  // low, so the potential performance loss from a single static
  // lock object is actually minimal.  There are any number of 
  // situations where this would be an entirely inappropriate
  // solution.)
  static CriticalSection& _critsec;

};

#endif // __CFGLOAD_H__ Sentinel

