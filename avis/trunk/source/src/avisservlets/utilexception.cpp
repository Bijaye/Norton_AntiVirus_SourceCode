// utilexception.cpp - implementation of the AvisServletsException and child classes

#include "utilexception.h"


//

//----------------------------------------------
// UtilityException
//----------------------------------------------
UtilityException::UtilityException(const char* message_)
  :exception(),
   _message(message_)
{}

UtilityException::UtilityException(const UtilityException& rhs_)
  :exception(rhs_),
   _message(rhs_._message)
{}

UtilityException& UtilityException::operator =(const UtilityException& rhs_)
{
  if (&rhs_ != this)
  {
    _message = rhs_._message;
  }
  return *this;
}

UtilityException::~UtilityException() {}

const char* UtilityException::what() const 
{
  return _message.c_str();
}


//

//----------------------------------------------
// AvisServletsException
//----------------------------------------------

AvisServletsException::AvisServletsException(const char* message_)
  :UtilityException(message_)
{}

AvisServletsException::AvisServletsException(const AvisServletsException& rhs_)
  :UtilityException(rhs_)
{}

AvisServletsException& AvisServletsException::operator =(const AvisServletsException& rhs_)
{
  if (&rhs_ != this)
  {
    UtilityException::operator =(rhs_);
  }
  return *this;
}

AvisServletsException::~AvisServletsException() {}

//----------------------------------------------
// ServletRecoverable
//----------------------------------------------

ServletRecoverable::ServletRecoverable(const char* reason_,const char* response_)
  :AvisServletsException(reason_),   
  _response(response_)
{}

ServletRecoverable::ServletRecoverable(const ServletRecoverable& rhs_)
  :AvisServletsException(rhs_),
   _response(rhs_._response)
{}

ServletRecoverable& ServletRecoverable::operator =(const ServletRecoverable& rhs_)
{
  if (&rhs_ != this)
  {
    AvisServletsException::operator =(rhs_);
    _response = rhs_._response;
  }
  return *this;
}

ServletRecoverable::~ServletRecoverable() {}

const std::string& ServletRecoverable::response() const
{ 
  return _response;
}

//----------------------------------------------
// InfrastructureException
//----------------------------------------------

InfrastructureException::InfrastructureException(const char* reason_)
  :ServletRecoverable(reason_,"internal")
{}

InfrastructureException::InfrastructureException(const InfrastructureException& rhs_)
  :ServletRecoverable(rhs_)
{}

InfrastructureException& InfrastructureException::operator =(const InfrastructureException& rhs_)
{
  if (&rhs_ != this)
  {
    ServletRecoverable::operator =(rhs_);
  }
  return *this;
}

InfrastructureException::~InfrastructureException() {}

//----------------------------------------------
// BadFilename
//----------------------------------------------

BadFilename::BadFilename(const char* filename_)
  :UtilityException(filename_)
{}

BadFilename::BadFilename(const BadFilename& rhs_)
  :UtilityException(rhs_)
{}

BadFilename& BadFilename::operator =(const BadFilename& rhs_)
{
  if (&rhs_ != this)
  {
    UtilityException::operator =(rhs_);
  }
  return *this;
}

BadFilename::~BadFilename() {}

//----------------------------------------------
// BadPath
//----------------------------------------------

BadPath::BadPath(const char* pathname_)
  :UtilityException(pathname_)
{}

BadPath::BadPath(const BadPath& rhs_)
  :UtilityException(rhs_)
{}

BadPath& BadPath::operator =(const BadPath& rhs_)
{
  if (&rhs_ != this)
  {
    UtilityException::operator =(rhs_);
  }
  return *this;
}

BadPath::~BadPath() {}

//----------------------------------------------
// InvalidConfigVariable
//----------------------------------------------

InvalidConfigVariable::InvalidConfigVariable(const char* key_)
  :AvisServletsException(key_)
{}

InvalidConfigVariable::InvalidConfigVariable(const InvalidConfigVariable& rhs_)
  :AvisServletsException(rhs_)
{}

InvalidConfigVariable& InvalidConfigVariable::operator =(const InvalidConfigVariable& rhs_)
{
  if (&rhs_ != this)
  {
    AvisServletsException::operator =(rhs_);
  }
  return *this;
}

InvalidConfigVariable::~InvalidConfigVariable() {}

//----------------------------------------------
// InvalidHttpHeader
//----------------------------------------------

InvalidHttpHeader::InvalidHttpHeader(const char* key_)
  :ServletRecoverable(key_,"missing ")
{
  // Yes, I'm manually editing the parent class response member
  // so the servlet can return the proper error message in the
  // X-Error attribute
  _response += key_;
}

InvalidHttpHeader::InvalidHttpHeader(const InvalidHttpHeader& rhs_)
  :ServletRecoverable(rhs_)
{}

InvalidHttpHeader& InvalidHttpHeader::operator =(const InvalidHttpHeader& rhs_)
{
  if (&rhs_ != this)
  {
    AvisServletsException::operator =(rhs_);
  }
  return *this;
}

InvalidHttpHeader::~InvalidHttpHeader() {}

//----------------------------------------------
// InvalidObject
//----------------------------------------------

InvalidObject::InvalidObject(const char* classname_)
  :UtilityException(classname_)
{}

InvalidObject::InvalidObject(const InvalidObject& rhs_)
  :UtilityException(rhs_)
{}

InvalidObject& InvalidObject::operator =(const InvalidObject& rhs_)
{
  if (&rhs_ != this)
  {
    UtilityException::operator =(rhs_);
  }
  return *this;
}

InvalidObject::~InvalidObject() {}
