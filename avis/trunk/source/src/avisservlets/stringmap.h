// stringmap.h - Stringmap Prototype

#if !defined (__STRINGMAP_H__)
#define __STRINGMAP_H__


// StringMapUtil is a library of static public methods
// for generating, composing, and decomposing string hash tables
// of the following type:
//    std::map<std::string,std::string,F_stricmp> MStringString
//  (As defined in types.h)


// Note: These methods are likely to use the StringUtil library methods
// as well.

#include "types.h"


#if !defined (LINE_BUFFER_SIZE)
#define LINE_BUFFER_SIZE 1024
#endif // LINE_BUFFER_SIZE

//
//: static library of string map "helper" routines.
//
// This class is not designed to be instantiated, although there is no
// particular harm in doing so.  It provides little more than
// namespace sheltering to a number of static methods used for
// converting strings into and out of an STL map construct common
// throughout the Immune System infrastructure.
//

class StringMapUtil

{
public:

  //
  //: Build a new map from a block of http headers of the format
  // "name: value\n"... 
  //


  static MStringString fromHttpHeaders(const std::string& headers_);

  //
  //: Build a new map from a block of http headers of the format
  //: "name=value\n"...  (as send back by the Go api)
  //

  static MStringString fromGoEnvironment(const std::string& headers_);

  //
  //: Convert a map of strings back to an HTTP header block format.
  //
  // This is the compliment to the above.
  // If you were to do "toHttpHeaders(fromHttpHeaders(blah_))"
  // you really should get a copy of blah_ back (but sorted)
  //
  // Note, this doesn't make any assumptions about the contents
  // of the map_. I don't know if an empty string is a valid
  // key.  But it oughtn't be. 
  // Nor does it append an "extra" "\n" to the string.
  // 'cause god only knows what you're gonna do with this
  // best not to make any assumptions.
  //

  static std::string   toHttpHeaders(const MStringString& map_);


  //
  //: Verify/Query key lookup method for a string map.
  //
  // I'm tired of getting a value in two steps from a 
  // map.  Strings are a simpler case, so they warrant
  // this type of access.
  // This method returns a string with the value of the key_
  // in the map, or an empty string. NOT an invalid string
  // object.
  //

  static std::string valueIfExists(const MStringString& map_,
                                      const std::string& key_);


  //
  //: Load a string table from a configuration file, given the
  //: filename.
  //
  // The Configuration file format is as follows:
  //
  // - Any line starting with # is ignored as a comment.
  //
  // - any blank line is ignored as a comment
  //
  // - any other line is expected to have the following form:
  //
  // "key: value"  and will be parsed into a hash table
  // based on that format.
  //
  // Note that duplicate keys will be successively overwritten.
  //
  // This is used to read various configuration files throughout the
  // system, including the avisservlets.prf file.
  //

  static MStringString fromConfigFile(const char* filename_);

  //
  //: Build a string map from the process environment strings.
  //
  // Load a hash table from the 3rd parameter to main, which 
  // contains a copy of the process environment represented
  // as an array of char* that look like "name=value"
  // (I wish everything were this easy)
  //

  static MStringString fromEnvParam( char** env_);

  //
  //: Create a human-readable debug string from a string map
  //
  // Turn a hash table into a simple "debug" string form 
  // that looks like:
  //
  //  "[name] = 'value'\n"
  //
  // I don't have a fromDebugString, because it's just silly.
  //

  static std::string toDebugString(const MStringString& map_);


  //
  //: Slightly more complex version of toDebugString
  //
  //
  // Turn an MStringString into a debug string similar to above, but
  // with a prefixed string code and a space.  The resulting lines
  // will look like:
  //
  // "prefix [name] = 'value'\n"
  //
  // This is used to create entries in log files that can be easily
  // parsed.
  //

  static std::string toPrefixedDebugString(const std::string& prefix_,
                                           const MStringString& map_);


  //
  //: Merge two MStringStrings into one.
  //
  //  Note: The first parameter
  //  is copied into a fresh MStringString, then the second parameter
  //  is iterated across and overlaid on top (destructively)
  static MStringString hashMerge(const MStringString& first_,
                                 const MStringString& second_);


};


#endif // __STRINGMAP_H__ Sentinel
