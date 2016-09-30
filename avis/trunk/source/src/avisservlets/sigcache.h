#if !defined (__SIGCACHE_H__)
#define __SIGCACHE_H__

#pragma warning (disable : 4786)

// This needs to be included in the GoServletHost object as an
// "external Service" (nothing to do with NT services at all)

#include "sigfilehnd.h"
#include "sigfile.h"
// For critical sections and mutexes.  The "LockableObject" class
// defined in this header is merely an AbstractBaseClass interface and 
// includes no other files whatsoever. It's very lightweight
#include "aviscommon/log/lckobj.h"

// Standard library headers.
#include <string>
#include <map>

class MTrace;
class TraceClient;

//
//: Maintains a cache of definition package file content to minimize
//: memory consumption of duplicate package requests, NOT to minimize
//: transaction turnaround time.
//
// SignatureCache is created with only the path containing the
// signature package files.  It does not actually load any content
// into memory upon creation.  When a definition package is requested
// for the first time, the content is loaded and a reference count is
// set to 1.  If subsequent requests are received for the same
// definition package file, the cached copy will be used.  However, if
// the reference count on a cached package drops to zero the content
// will be released from memory. 
//
// More simply said, this cache is only to minimize concurrent file
// loads and in-memory copies of 4meg definition packages. (Imagine
// 500 threads trying to download 500 independent images of the same
// file at 4meg each.)
//
// This was NOT designed to solve a speed bottleneck, since the
// transfer of these packages is far more time consuming than the file 
// load.
//
// The net effect of this caching is that parallel signature requests
// for the same package do execute more quickly, but that is a
// beneficial side-effect.
//
// Note the match between the interface (public) methods in the
// SignatureFileHandle class and the protected methods in the
// SignatureCache class.  This, combined with the "friendship" between 
// the two, allows a consumer of a SignatureFileHandle object to
// access the resources (access to a particular definition package
// image and it's attributes) it requires inside the cache while
// maintaining isolation to protect the cache from undesirable
// accesses.
//

class SignatureCache
{
  friend SignatureFileHandle;
public:

  //
  //: Class Constructor
  //

  SignatureCache(MTrace& trace_server_,const std::string& sigfile_path_);

  //
  //: Virtual destructor
  //

  virtual ~SignatureCache();

  //
  //: Client side method to retrieve a definition package.
  //
  // Note that a SignatureFile object is not actually returned, but a
  // handle to that object whose responsibility is little more than to 
  // maintain a "checkout point" for the reference counting scheme.
  //

  SignatureFileHandle get_signature_file(const std::string& sequence_);

protected:


  //
  //: Retrieve content checksum for a particular definition package
  //

  const std::string& get_checksum       (const std::string& sequence_) const;

  //
  //: Retrieve the content length of a particular definition package
  //

  long               get_content_length (const std::string& sequence_) const;

  //
  //: Retrieve a pointer to the content buffer of a particular cached
  //: definition package.
  //

  const char*        get_content        (const std::string& sequence_) const;

  //
  //: Retrieve the content type of a particular definition package
  //: suitable for an http transaction response.
  //

  const std::string& get_content_type   (const std::string& sequence_) const;

  //------------------------------------------------------------------------


  //
  //: Increment the cache's internal reference count for a particular
  //: definition package
  //

  void add_signature_reference    (const std::string& sequence_);

  //
  //: Decrement the cache's internal reference count for a particular
  //: definition package
  //

  void remove_signature_reference (const std::string& sequence_);

private:
  
  //
  //: Pad a sequence number out to 8 characters with "0"s.
  //
  // We like sequence numbers that are padded out to 8 characters with 
  // zeros.  This method enforces that and is called before any
  // compares or scans are done. (last thing we want is an entry for
  // "00000001" and "1" separate in the cache)
  //

  std::string normalize_sequence_string(const std::string& sequence_) const;


  //
  //: Internal table of cached signature files and their reference
  //: count, indexed by sequence number.
  //
  // This table maps normalized sequence numbers to pairs of reference 
  // count and content pointer.  This is a pretty beefy structure, and 
  // all access to it (even const access) is serialized.
  //

  std::map<std::string,std::pair<int,SignatureFile*> > _content_cache;


  //
  //: file path where the definition packages are kept.
  //

  std::string _sigfile_path;


  //
  //: Serialization object, used to lock access to the
  //: content cache map
  //

  LockableObject* _lock;


  //
  //: Intentionally unimplemented default constructor
  //
  
  SignatureCache();
  
  //
  //: Intentionally unimplemented copy constructor
  //

  SignatureCache(const SignatureCache&);

  //
  //: Intentionally unimplemented assignment operator
  //

  SignatureCache& operator = (const SignatureCache&);


  //
  //: Reference to an MTrace object to use in constructing
  //: SignatureFile objects
  //

  MTrace& _trace_server;

  //
  //: Standard Trace object
  //
  
  TraceClient* _tracer;
};
#endif // __SIGCACHE_H__ Sentinel
