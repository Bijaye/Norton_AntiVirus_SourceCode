// utilexception.h - Some rather simple C++ exceptions to be thrown by utilities

#if !defined (__UTILEXCEPTION_H__)
#define __UTILEXCEPTION_H__

#include <exception>
#include <string>

// This is a very minimal exception hierarchy.
// I define the mechanics of inserting a const char* in the constructor
// and assigning it into the contained std::string _message internally
// If the pointer is bad, I set it to a null string, because the 
// std::string sure doesn't like getting a bad pointer to convert
// from.  Last thing we want is our exception class throwing exceptions.

// Of course, everything is inline.

// The children of UtilityException just override the constructor
// and provide typing.  They do not add or change any functionality.
// (they could, but they don't.)

class UtilityException : virtual public std::exception
{
public:
  // I'm using the std::string so I don't have anything 
  // to cleanup in the destructor.
  UtilityException(const char* message_) : _message("")
    {
      // Really make sure this'll work
      // This may look silly, but I don't want to expose undefined
      // behavior in the exception class
      if (message_ != 0)
        if (strlen(message_))
          _message = message_;
    };
  virtual ~UtilityException() {};
  virtual const char* what() const throw() { return _message.c_str();};
protected:
  std::string _message;
};

class InvalidTraceSink : virtual public std::exception
{
public:
  InvalidTraceSink(const char* message_) : _message("")
    {
      // Really make sure this'll work
      // This may look silly, but I don't want to expose undefined
      // behavior in the exception class
      if (message_ != 0)
        if (strlen(message_))
          _message = message_;
    };
  virtual ~InvalidTraceSink(){};
  virtual const char* what() const throw() { return _message.c_str();};
protected:
  std::string _message;
};

class BadFilename :virtual public UtilityException
{
public:
  BadFilename(const char* filename_) : UtilityException(filename_){};
};

class BadPath : virtual public UtilityException
{
public:
  BadPath(const char* pathname_) : UtilityException(pathname_) {};
};

class InvalidConfigVariable : virtual public UtilityException
{
public:
  InvalidConfigVariable(const char* key_):UtilityException(key_){};
};


class InvalidHttpHeader : virtual public UtilityException
{
public:
  InvalidHttpHeader(const char* key_):UtilityException(key_){};
};

class InvalidObject : virtual public UtilityException
{
public:
  InvalidObject(const char* classname_) : UtilityException(classname_) {};
};


#endif // __UTILEXCEPTION_H__ Sentinel
