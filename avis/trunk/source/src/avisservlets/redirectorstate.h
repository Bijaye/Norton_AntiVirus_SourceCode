#if !defined (__REDIRECTORSTATE_H__)
#define __REDIRECTORSTATE_H__ 

#include "stringmap.h" // for definition of MStringString
#include <vector>
#include <string>

// Trace classes
class MTrace;
class TraceClient;


//
//: Maintain the list of servers for redirection servlets, and provide
//: accessors for selecting one.
//
// RedirectorState takes a reference to the table of name value pairs
// loaded from the process environment strings and a configuration
// file. It iterates through that list and populates two vectors of
// strings, representing the list of eligible servers that can behave
// as "redirection targets" for definition package (signature)
// download transactions or sample submission transactions.
//
// It does this by looking for the following two patterns in the key
// list of the string map construct referenced in the constructor:
//
// - sample servers: key starts with "redirectsamples" (case
//   insensitive)
//
// - definition servers: key starts with "redirectsignatures" (also
//   case insensitive)
//
// When a key is found that starts with one of those two tokens, the
// value associated with that key is added to the appropriate vector.
//
// The get_*_server methods then select an item randomly from the
// appropriate vector based on a div/mod with the unix seconds / size
// of vector scheme.  This means that redirections won't be strictly
// random, but that all requests in a particular second will go to the
// same target. This is acceptable.
//

class RedirectorState
{
public:
  
  //
  //: Class constructor, with the environment table.
  //

  RedirectorState(MTrace& trace_server_,const MStringString&
                  environment_);

  //
  //: Virtual destructor
  //

  virtual ~RedirectorState();
  

  //
  //: Retrieve a "random" definition download server from the vector
  //

  const std::string get_definition_server();

  //
  //: Retrieve a "random" sample submission server from the vector
  //

  const std::string get_sample_server();

protected:

private:
  
  //
  //: Iterate across the name value pairs in the string table, loading
  //: the redirection target vectors.
  //

  void load_server_lists(const MStringString& environment_);

  //
  //: String Vector of definition download server IP addresses
  //: (without port numbers)
  //
  
  std::vector<std::string> _definition_server_list;

  //
  //: String Vector of sample submission server IP addresses (without
  //: port numbers)
  //

  std::vector<std::string> _sample_server_list;

  //
  //: Standard Tracing Object
  //

  TraceClient* _tracer;
};

#endif // __REDIRECTORSTATE_H__ Sentinel
