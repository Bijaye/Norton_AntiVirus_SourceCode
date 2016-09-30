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
#include <string>
#include <map>


#if !defined (LINE_BUFFER_SIZE)
#define LINE_BUFFER_SIZE 4096
#endif // LINE_BUFFER_SIZE

class StringMapUtil

{
public:
  // Build a new map from a block of http headers
  // of the format "name: value\n"...
  static MStringString fromHttpHeaders(const std::string& headers_);

  //: Build a new map from a block of http headers
  // of the format "name=value\n"...  (as send back by the Go
  // api)
  static MStringString fromGoEnvironment(const std::string& headers_);

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
  static std::string   toHttpHeaders(const MStringString& map_);


  // I'm tired of getting a value in two steps from a 
  // map.  Strings are a simpler case, so they warrant
  // this type of access.
  // This method returns a string with the value of the key_
  // in the map, or an empty string. NOT an invalid string
  // object.
  static std::string valueIfExists(const MStringString& map_,
                                      const std::string& key_);
  // There will be other stuff to put in here as well, I'm sure.
  // I'll add as I go along.

  // Load from a configuration file.
  // The Configuration file format is as follows:
  // Any line starting with # is ignored as a comment.
  // any blank line is ignored as a comment
  // any other line is expected to have the following form:
  // "key: value"  and will be parsed into a hash table
  // based on that format.
  // Note that duplicate keys will be successively overwritten.

  static MStringString fromConfigFile(const char* filename_);

  // Load a hash table from the 3rd parameter to main, which 
  // contains a copy of the process environment represented
  // as an array of char* that look like "name=value"
  // (I wish everything were this easy)
  static MStringString fromEnvParam( char** env_);

  // Turn a hash table into a simple "debug" string form 
  // that looks like:
  //  "[name] = 'value'\n"
  // I don't have a fromDebugString, because it's just silly.
  static std::string toDebugString(const MStringString& map_);


  //: Merge two MStringStrings into one.  Note: The first parameter
  //  is copied into a fresh MStringString, then the second parameter
  //  is iterated across and overlaid on top (destructively)
  static MStringString hashMerge(const MStringString& first_,
                                 const MStringString& second_);


};


#endif // __STRINGMAP_H__ Sentinel








