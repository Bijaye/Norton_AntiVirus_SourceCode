// strutil.h - a class with a bundle of string utility methods (all static)

#if !defined (__STRUTIL_H__)
#define __STRUTIL_H__

#include <string>
#include <list>


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

  // The simple "trim" functionality.
  // Kills whitespace from either or both ends of the string
  // NOTE: the return value is the same as the parameter
  //   so it can be used inline
  static std::string& trim (std::string& str );
  static std::string& ltrim(std::string& str );
  static std::string& rtrim(std::string& str );
  
  // Str upper and str lower
  static std::string& strupr(std::string& str);
  static std::string& strlwr(std::string& str);

  // Classifiers
  static bool isWhitespace (const std::string& str) ;
  static bool isInteger    (const std::string& str) ; 
  
  // Some stuff of mine
  static std::string nextToken(const std::string& str,const std::string& delim);

  // Padding
  static std::string& padLeft(std::string& str,char withme, int maxsize);
  static std::string& padRight(std::string& str, char withme, int maxsize);
  
  // From Numerics (formatting, but still a little primitive, uses the *toa 
  // standard library methods)  I'll be adding these as needed
  static std::string itos(int  i,int radix = 10);
  static std::string ltos(long l,int radix = 10);
  
  // Change all instances of a character in a string to some other character
  static std::string& repchr(std::string& str,char fromme,char tome);
  // Remove all instances of a parcicular character
  static std::string& removeAll(std::string& str,char killme);

  // Cut the first (or last) 'qty' chars off the string & return 
  // a ref to itself.  If you cut off too many, the string becomes ""
  static std::string& chopLeft(std::string& str,int qty);
  static std::string& chopRight(std::string& str,int qty);


  // tokenize a string to a list, based on "any of a string of delimiters"
  // NOTE:  THIS TAKES A CONST CHAR*, NOT A CONST STRING&
  // (I would just do that conversion anyway, so why waste the operation)
  static std::list<std::string> tokenizeToList(const char* str_,const char delim_);

  //: tokenize a string by the first occurrance of delim_ into a pair of 
  // strings. This is just a bit different from tokenizeToList
  static std::pair<std::string,std::string> tokenizeToPair(const char* str_,const char* delim_);
};
#endif // __STRUTIL_H__

