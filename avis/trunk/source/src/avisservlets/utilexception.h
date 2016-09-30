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

// The children of ServletException just override the constructor
// and provide typing.  They do not add or change any functionality.
// (they could, but they don't.)

// The base class exception merely implements some semantics and
// provides typing.

// (PC-Lint:  Note 1932:  Base class 'exception' is not abstract)
// This is ok.

class UtilityException : public std::exception
{
public:
  explicit UtilityException(const char* message_);
  UtilityException(const UtilityException&);
  UtilityException& operator =(const UtilityException&);
  virtual ~UtilityException();
  virtual const char* what() const throw();
protected:
  std::string _message;
};

  // AvisServletsException provides simple subtyping for servlet
  // exceptions.

class AvisServletsException :  public UtilityException
{
public:
  // I'm using the std::string so I don't have anything 
  // to cleanup in the destructor.

  // Pure virtuals from AVISException
  explicit AvisServletsException(const char* message_);
  AvisServletsException(const AvisServletsException&);
  AvisServletsException& operator =(const AvisServletsException&);
  virtual ~AvisServletsException();
  // Intentionaly declared and not defined;
private:
  AvisServletsException();
};


// A ServletRecoverable exception is thrown in the case where an error
// or exception condition is encountered within a servlet transaction,
// but is local to that transaction.  Internally, the "reason" code is
// logged as an "error" and the "response_" is returned to the client
// in the "X-Error" http attribute.
class ServletRecoverable :  public AvisServletsException
{
public:
  ServletRecoverable(const char* reason_,const char* response_);
  ServletRecoverable(const ServletRecoverable&);
  virtual ~ServletRecoverable();

  ServletRecoverable& operator =(const ServletRecoverable&);
  const std::string& response() const;
  
protected:
  std::string _response;

  // Intentionaly declared and not defined;
private:
  ServletRecoverable();
};

// An infrastructure exception is thrown when an internal error of
// either unknown origin, or critical configuration error occurs.
// It is usually thrown from within the catch block of a more-specific
// exception. This way, the "real" exception is logged, but the
// client-side notification is very generic.  
// NOTE:  the "InfrastructureException" value passed to the
// ServletRecoverable constructor as the response_ parameter will be
// changed to something a little more client-friendly.

class InfrastructureException : public ServletRecoverable
{
public:
  explicit InfrastructureException(const char* reason_);
  InfrastructureException(const InfrastructureException&);
  virtual ~InfrastructureException();
  InfrastructureException& operator =(const InfrastructureException&);

  // Intentionally declared and not defined
private:
  InfrastructureException();
};

// Thrown when:
// 1) file of specified name did not exist
// (or could not be opened for read)
class BadFilename : public UtilityException
{
public:
  explicit BadFilename(const char* filename_);
  BadFilename(const BadFilename&);
  virtual ~BadFilename();
  BadFilename& operator =(const BadFilename&);

private:
  // Intentionally declared and not defined
  BadFilename();
};

// Thrown in cases where a directory path was either:
// 1) Expected and did not exist
// 2) malformed
class BadPath :  public UtilityException
{
public:
  explicit BadPath(const char* pathname_);
  BadPath(const BadPath&);
  virtual ~BadPath();
  BadPath& operator =(const BadPath&);
  
private:
  // Intentionally declared and not defined
  BadPath();
};

// Thrown when an expected configuration variable 
// (such as one loaded from a profile) was malformed
// or missing.
class InvalidConfigVariable :  public AvisServletsException
{
public:
  explicit InvalidConfigVariable(const char* key_);
  InvalidConfigVariable(const InvalidConfigVariable&);
  virtual ~InvalidConfigVariable();

  InvalidConfigVariable& operator =(const InvalidConfigVariable&);

private:
  // Intentionally declared and not defined
  InvalidConfigVariable();
  
};

// Thrown when an expected Http transaction attribute was malformed or
// missing.
class InvalidHttpHeader :  public ServletRecoverable
{
public:
  explicit InvalidHttpHeader(const char* key_);
  InvalidHttpHeader(const InvalidHttpHeader&);
  virtual ~InvalidHttpHeader();
  InvalidHttpHeader& operator =(const InvalidHttpHeader&);

private:
  InvalidHttpHeader();
  
};

// Thrown in response to a "isNull" method returning true from one of
// Andy's libraries. (this is deprecated and cycling out quickly)
class InvalidObject :  public UtilityException
{
public:
  explicit InvalidObject(const char* classname_);
  InvalidObject(const InvalidObject&);
  virtual ~InvalidObject();
  InvalidObject& operator =(const InvalidObject&);

private:
  InvalidObject();
};


#endif // __UTILEXCEPTION_H__ Sentinel
