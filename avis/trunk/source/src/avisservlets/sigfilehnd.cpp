// 

#pragma warning (disable : 4786)
#include "sigfilehnd.h"
#include "sigcache.h"

SignatureFileHandle::SignatureFileHandle(const SignatureFileHandle& r_)
  :_sig_cache(r_._sig_cache),
   _sequence(r_._sequence)
{
  increment_reference_count();
}

SignatureFileHandle::~SignatureFileHandle()
{
  // The SignatureCache object will detect if the reference count = 0
  // and delete the internal buffers accordingly.
  decrement_reference_count();
}

SignatureFileHandle& SignatureFileHandle::operator =(const SignatureFileHandle& r_)
{
  if (&r_ != this)
    {
      // Note: the "_sig_cache" reference can't be assigned. 
      // The reason it's ok is because if we're using an assignment
      // operator, we must already have a valid object.  The only way
      // that can happen is through normal construction, which will
      // prime the _sig_cache value.  Since there's only one
      // SignatureCache object per process, this can't cause a
      // problem.
      // (Unless someone dynamically casts a void* into a
      // SignatureFileHandle, in which case I'll be happy to execute
      // them)

      _sequence = r_._sequence;
      increment_reference_count();
    }
  return (*this);
}

// Simple public access methods that proxy through to the SignatureCache
const std::string& SignatureFileHandle::get_checksum       () const
{
  return _sig_cache.get_checksum(_sequence);
}
long               SignatureFileHandle::get_content_length () const
{
  return _sig_cache.get_content_length(_sequence);
}
const char*        SignatureFileHandle::get_content        () const
{
  return _sig_cache.get_content(_sequence);
}
const std::string& SignatureFileHandle::get_content_type   () const
{
  return _sig_cache.get_content_type(_sequence);
}

// Protected Methods

void SignatureFileHandle::increment_reference_count()
{
  // The SignatureFileHandle object manipulates the reference count
  // through protected methods in the SignatureCache object referenced 
  // in object scope.
  _sig_cache.add_signature_reference(_sequence);
}
void SignatureFileHandle::decrement_reference_count()
{
  _sig_cache.remove_signature_reference(_sequence);
}


// ACTUAL construction method (called by SignatureCache object only)
SignatureFileHandle::SignatureFileHandle(SignatureCache& sig_cache_,
                                         const std::string& sequence_)
  :_sig_cache(sig_cache_),
   _sequence(sequence_)
{

  // When this is created, the content has already been loaded into
  // the cache manager.  However, the reference counter is still set
  // to zero so we need to increment here.
  increment_reference_count();
}

