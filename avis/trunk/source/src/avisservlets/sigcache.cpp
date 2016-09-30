#include "sigcache.h"

// Serialization headers
#include "aviscommon/log/lock.h"
#include "aviscommon/log/critsec.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/log/mtrace.h"
#include <stdio.h>

SignatureCache::SignatureCache(MTrace& trace_server_,
                               const std::string& sigfile_path_)
  :_trace_server(trace_server_),
   _sigfile_path(sigfile_path_),
   _tracer(NULL),
   _lock(NULL)
{
  _tracer = new TraceClient(trace_server_,
                            "SignatureCache");
                            
  // For all the work this does, there's remarkably little startup code
  //
  // We do, however, have to create the critical section.
  // (Note: Creating this object does NOT obtain the lock)
  // The CriticalSection class is a subclass of LockableObject
  // The SignatureCache declaration maintains a pointer to a
  // LockableObject to keep this as abstract as possible.
  _lock = new CriticalSection();
  // See, wasn't that easy ;)
  _tracer->debug("Signature Cache constructor...");
}

SignatureCache::~SignatureCache()
{
  _tracer->debug("Signature Cache destructing...");
  // The map "should" be empty at this point, and all content blocks cleaned up
  // However, we don't live in a perfect world and there are dumb
  // programmers who don't pay attention.
  // So we're going to lock all access to the map, then flush it.
  // If anything tries to grab this lock in here then we have a VERY
  // careless programmer on the loose.
  if (!_content_cache.empty())
    {
      Lock _held_lock(*_lock);
      // Flush out the cache
    }
  // Delete the critical section object
  delete _lock;
  // and set the pointer to null in case things that can't happen,
  // happen.
  _lock = NULL;
  delete _tracer;
  _tracer = NULL;
}

// The biggie ;)

SignatureFileHandle SignatureCache::get_signature_file(const std::string& sequence_)
{
  StackTrace _stack(*_tracer,"get_signature_file");
  Lock _held_lock(*_lock);

  // Normalize the sequence string
  std::string _sequence(normalize_sequence_string(sequence_));

  // Test for existance of the signature in the cache
  // If it does NOT exist, create one
  // Create a handle object to the "no doubt now exists" cache object
  //
  std::string _msg("signature set: ");
  _msg.append(_sequence);
  _msg.append(" requested");
  _stack.info(_msg);

  std::map<std::string,std::pair<int,SignatureFile*> >::const_iterator i;
  i = _content_cache.find(_sequence);
  if (i == _content_cache.end())
    {
      _stack.info("Sigs not already preloaded...");
      // Dump list of available sigs 
      for (i = _content_cache.begin();i != _content_cache.end();++i)
        {
          _msg = "Available '";
          _msg.append((*i).first);
          _msg.append("'");
          _stack.debug(_msg);
        }
      // Make a new one.
      SignatureFile* _body = new SignatureFile(_trace_server,_sigfile_path,_sequence);
      // add it to the map with a reference count of ZERO!!!
      std::pair<int,SignatureFile*> _value_pair(0,_body);
      _content_cache.insert(std::map<std::string,std::pair<int,SignatureFile*> >::value_type(_sequence,_value_pair));
    }
  else
    {
      _stack.info("Using pre-cached signature content ;)");
    }
  // now that we "know" this exists, create a handle to it and 
  // return that.

  SignatureFileHandle _handle(*this,_sequence);
  
  //TADA!!!
  return _handle;
}

// Protected Service Methods
const std::string& SignatureCache::get_checksum(const std::string& sequence_) const
{
  Lock _held_lock(*_lock);
  // Create an iterator object;
  std::map<std::string,std::pair<int,SignatureFile*> >::const_iterator i;
  // Assign it in the next line for readability
  i = _content_cache.find(sequence_);
  // return the result of the cached SignatureFile's accessor
  return ((*i).second.second)->get_checksum();
}
long SignatureCache::get_content_length(const std::string& sequence_) const
{
  Lock _held_lock(*_lock);
  // Create an iterator object;
  std::map<std::string,std::pair<int,SignatureFile*> >::const_iterator i;
  // Assign it in the next line for readability
  i = _content_cache.find(sequence_);
  // return the result of the cached SignatureFile's accessor
  return ((*i).second.second)->get_content_length();
}
const char* SignatureCache::get_content(const std::string& sequence_) const
{
  Lock _held_lock(*_lock);
  // Create an iterator object;
  std::map<std::string,std::pair<int,SignatureFile*> >::const_iterator i;
  // Assign it in the next line for readability
  i = _content_cache.find(sequence_);
  // return the result of the cached SignatureFile's accessor
  return ((*i).second.second)->get_content();
}
const std::string& SignatureCache::get_content_type(const std::string& sequence_) const
{
  Lock _held_lock(*_lock);
  // Create an iterator object;
  std::map<std::string,std::pair<int,SignatureFile*> >::const_iterator i;
  // Assign it in the next line for readability
  i = _content_cache.find(sequence_);
  // return the result of the cached SignatureFile's accessor
  return ((*i).second.second)->get_content_type();
}

// Reference count manipulation

void SignatureCache::add_signature_reference(const std::string& sequence_)
{
  Lock _held_lock(*_lock);
  (_content_cache[sequence_]).first++;
}

void SignatureCache::remove_signature_reference(const std::string& sequence_)
{
  Lock _held_lock(*_lock);
  
  (_content_cache[sequence_]).first--;
  
  // Do we need to delete this?
  if ((_content_cache[sequence_]).first <= 0)
    {
      // Remove the entry from the map.  We have no more referents.
      SignatureFile* _content_object = ((_content_cache[sequence_]).second);
      delete _content_object;
      _content_object = NULL;
      _content_cache.erase(sequence_);
    }
}

// Primitives

std::string SignatureCache::normalize_sequence_string(const std::string& sequence_) const
{
  // This is a simple const method that has no effect on the internals 
  // of the class whatsoever (it could, by all rights, be made into a
  // static, but there's no utility in that) so it needs NO
  // serialization.
  int  _sequence = atoi(sequence_.c_str());
  char _char_sequence[64];
  memset(_char_sequence,'\0',64);
  sprintf(_char_sequence,"%08d",_sequence);
  std::string _normalized(_char_sequence);
  return _normalized;
}
