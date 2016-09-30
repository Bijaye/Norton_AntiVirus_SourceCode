// strutil.h - a class with a bundle of string utility methods (all static)

#if !defined (__STRUTIL_H__)
#define __STRUTIL_H__

#pragma warning ( disable : 4786 )

#include <string>
#include <list>


//#include "aviscommon/log/mtrace.h"

// CONVERSION_BUFFER_SIZE is the size of the initial allocation buffer
// used for numeric conversions into string representations.
// Note that this is the size of the temporary buffer, which will be 
// trimmed flush to the required size.
#define CONVERSION_BUFFER_SIZE 128

// Just because I need this for my own sanity
// overloading for use as an "append" operator
// like so :  string _val; _val << "blah blah" << otherstring << "\n";
std::string& operator <<(std::string&,const std::string&);


// Many of these methods, you'll notice take a string& and return a string&
// these methods modify the string inline, and pass back a reference 
// as well.  This way, they can be chained together in all kinds of
// unholy ways.

class StringUtil
{
public:

  //
  //: Trim whitespace from both sides of a string
  //
  // trim will call ltrim and rtrim, thereby removing any left or
  // right side space padding that may exist.  If a "whitespace only"
  // string is passed in, it will be truncated to the empty string.
  //
  static std::string& trim (std::string& str );
  
  //
  //: Trim whitepace off the left side of a string
  //
  // This will return an empty string if given a string containing
  // only whitespace
  //
  static std::string& ltrim(std::string& str );

  //
  //: Trim whitepace off the right side of a string
  //
  // This will return an empty string if given a string containing
  // only whitespace
  //
  static std::string& rtrim(std::string& str );
  
  //
  //: upper case the passed in string
  //

  static std::string& strupr(std::string& str);

  //
  //: lower case the passed in string
  //
  static std::string& strlwr(std::string& str);


  //
  //: Determine whether a string contains only whitespace
  //
  // Whitespace here is inadequately defined to include the space and
  // tab characters.  Note that an empty string is questionably
  // considered to be 'whitespace'
  //

  static bool isWhitespace (const std::string& str) ;
  
  //
  //: Determine whether a string is a character representation of an
  //: integer
  //
  // This method returns true if the string contains nothing other
  // than ['0'-'9']
  //
  static bool isInteger    (const std::string& str) ; 
  
  //
  //: Given a string and a delimiter, copy the first "token" off the
  //: list and return it as a concrete string object.
  //
  //

  static std::string nextToken(const std::string& str,const std::string& delim);

  //
  //: Left-pad a string with a given character to match a maximum
  //: length.
  //
  // If the string length >= the size "maxsize" it is returned as is.
  //

  static std::string& padLeft(std::string& str,char withme, int maxsize);

  //
  //: Right-pad a string with a given character to match a maximum
  //: length.
  //
  // If the string length >= the size "maxsize" it is returned as is.
  //

  static std::string& padRight(std::string& str, char withme, int maxsize);
  
  //
  //: Convert an integer to a string
  //
  
  static std::string itos(int  i,int radix = 10);

  //
  //: Convert a long to a string
  //

  static std::string ltos(long l,int radix = 10);

  //
  //: Replace all instances of a particular in a string with some other
  //: character.
  //

  static std::string& repchr(std::string& str,char fromme,char tome);

  //
  //: Remove all instances of a parcicular character
  //

  static std::string& removeAll(std::string& str,char killme);

  //
  //: Cut the first 'qty' chars off the string.
  //
  // If "qty" is greater than the length of the string, the string is
  // truncated to zero-length
  //
  static std::string& chopLeft(std::string& str,int qty);

  //
  //: Cut the first 'qty' chars off the string.
  //
  // If "qty" is greater than the length of the string, the string is
  // truncated to zero-length
  //

  static std::string& chopRight(std::string& str,int qty);

  //
  //: Tokenize a string to a list, based on a specified delimiter
  //
  // Returns a list of strings containing the tokens parsed out of the 
  // string.  zero-length tokens are not returned. (i.e. if you have
  // two consecutive delimiters, they will be effectively collapsed)
  //

  static std::list<std::string> tokenizeToList(const char* str_,const char delim_);

  //
  //: tokenize a string by the first occurrance of delim_ into a pair of 
  //: strings. This is just a bit different from tokenizeToList.
  //
  // Note that this method might have been more appropriately named
  // "split_to_pair"
  //
  
  static std::pair<std::string,std::string> tokenizeToPair(const char* str_,const char* delim_);
};
#endif // __STRUTIL_H__

