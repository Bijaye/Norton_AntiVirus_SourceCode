#if !defined (__SIGFILEHND_H__)
#define __SIGFILEHND_H__

#include <string>
class SignatureCache;

//
//: Client accessible handle to a SignatureFile object, created by a SignatureCache object
//
// This represents the client-available interface to signature
// packages.  All accessor methods are proxied into the SignatureCache
// object.  This achieves two aims: 1) It prevents the user
// (programmer) from illegally accessing the contents of the
// SignatureCache; 2) provides a mechanism for the SignatureCache to
// maintain reference counts on each in-memory definition package, so
// that they may be safely deleted when that count reaches zero
//
// This class is required to have full reference level semantics with
// the exception of default construction.
//

class SignatureFileHandle
{
  friend SignatureCache;
  
public:

  //
  //: Copy Constructor
  //
  // Copies internal members and increments the reference count in the 
  // SignatureCache
  //

  SignatureFileHandle(const SignatureFileHandle&);

  //
  //: Assignment operator
  //
  // Copies internal members and increments the reference count in the 
  // SignatureCache
  //

  SignatureFileHandle& operator =(const SignatureFileHandle&);

  //
  //: Equality Comparitor on sequence number
  //

  bool operator ==(const SignatureFileHandle& r_) const 
  {
    return (_sequence == (r_._sequence));
  }

  //
  //: Virtual destructor
  //

  virtual ~SignatureFileHandle();

  //
  //: Get MD5 checksum for the signature package content
  //
  // This method is proxied through the
  // SignatureCache::get_checksum(sequence_) method.
  //

  const std::string& get_checksum       () const;

  //
  //: Get content length for the signature package
  //
  // This method is proxied through the
  // SignatureCache::get_content_length(sequence_) method
  //
  
  long               get_content_length () const;

  //
  //: Get a pointer to the content block for this signature package
  //
  // This method is proxied through the
  // SignatureCache::get_content(sequence_) method.
  //
  
  const char*        get_content        () const;

  //
  //: Get the content-type of this signature package.
  //
  // This method is proxied through the
  // SignatureCache::get_content_type(sequence_) method.
  //

  const std::string& get_content_type   () const;

protected:

  //
  //: increment the reference count for this signature package
  //
  // This method calls
  // SignatureCache::add_signature_reference(sequence_) in the
  // spirit of decoupling.
  //

  void increment_reference_count();

  //
  //: decrement the reference count for this signature package
  //
  // This method calls
  // SignatureCache::remove_signature_reference(sequence_).
  // Note that if that reference count equals zero after this
  // operation, the SignatureCache will delete the content from
  // memory.
  //

  void decrement_reference_count();
  
private:

  //
  //: Normal constructor, used by the SignatureCache for initial
  //: creation
  //

  SignatureFileHandle(SignatureCache& sig_cache_,
                      const std::string& sequence_);


  //
  //: Reference to the SignatureCache object from which we came.
  //
  
  SignatureCache& _sig_cache;

  //
  //: Sequence number of the signature package we're accessing
  //

  std::string _sequence;
  
  //
  //: Intentionally unimplemented default constructor
  //

  SignatureFileHandle();
  
};
#endif // __SIGFILEHND_H__ Sentinel
