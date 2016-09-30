// Attributes.cpp: implementation of the Attributes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>
#include <list>

#include "AttributesParser.h"
#include "attributekeys.h"

#include "Attributes.h"
#include "SQLStmt.h"
#include "Internal.h"
#include "AVISDBException.h"
#include "AnalysisRequest.h"
#include "logger.h"

// This is the ugliest 5 lines of code I have ever written

#define INLINE_STRING_TOOLKIT
#ifdef INLINE_STRING_TOOLKIT
#include "../aviscommon/log/strutil.h"
#include "../aviscommon/log/strutil.cpp"
#endif

//#define USE_DATABASE_PERSISTENCE 1
// #define USE_FILE_PERSISTENCE 1
// #define USE_NO_PERSISTENCE 1

// Internal utility methods
// These NEVER need to be seen in the interface to this class.

//
//	This method returns true if this key is a special key,
//	false if it is not a special key.
//	If it is a special key then the value is modified in the
//	AnalysisRequest table.

const AVISDBException::TypeOfException	exceptType = AVISDBException::etAttributes;


#ifdef USE_DATABASE_PERSISTSENCE  
MStringString load_attributes(uint localCookie)
{
  // Drop code in here to load the full block of attributes from the
  // databsase. (If we care.  I suspect we'll never need to implement
  // this.)
}
void save_attributes(uint localCookie,const MStringString& attribute_block_)
{
}
#endif
void Attributes::load_attributes_block()
{
  // Load all the attributes from the "persistent storage" and clear the _dirty flag.
  // Sorry, we don't load.
#ifdef USE_DATABASE_PERSISTENCE
  _attribute_block = load_attributes(localCookie);
#endif
  _dirty = false;
}
void Attributes::save_attributes_block()
{
  if (_dirty)
    {
#ifdef USE_DATABASE_PERSISTENCE
      save_attributes(localCookie,_attribute_block);
#endif
      // save the attribute block off to where we're saving it to.
      // At the moment, that's nowhere.
      _dirty = false;
    }
}


// Note: This is a direct copy of StringMapUtil::fromHttpHeaders
// however, StringMapUtil is not visible to AvisDB, so I'm redefining
// it at the file scope
MStringString map_from_headers_string(const std::string& headers_)
{
  MStringString _retval;

  
  std::list<std::string> _lines = StringUtil::tokenizeToList(headers_.c_str(),'\n');
  std::list<std::string>::const_iterator _it = _lines.end();
  for(_it = _lines.begin();_it != _lines.end(); _it++)
    {
      const std::string& _line = (*_it);
      if (_line.length())
        {
          std::pair<std::string, std::string > _keyvalue = 
			  StringUtil::tokenizeToPair(_line.c_str(),":");

	  // The first string in the key/value pair contains an
	  // attribute name and the second string contains an
	  // attribute value.  If the last character of the attribute
	  // value is a carriage return character, (left after
	  // tokenizing the HTTP header string on linefeed
	  // characters), delete it before proceeding.  (EJP 4/12/00)

	  long size = _keyvalue.second.length();
	  if (size && _keyvalue.second[size-1]=='\r') _keyvalue.second.erase((size - 1), 1);
	  
          // Slam it home if there's something there
          // No empties
	  StringUtil::trim(_keyvalue.first);
	  StringUtil::trim(_keyvalue.second);

          if (_keyvalue.first.length() && _keyvalue.second.length())
            _retval[_keyvalue.first] = _keyvalue.second;
        }
    }
  return _retval;
}
std::string to_debug_string(const MStringString& attribute_block_)
{
  std::string _retval("");
  // It's possible to screw this up really bad without checking
  // a reference pointer. VC6 may take care of this, but I trust M$oft
  // about as far as I can throw them.  Maybe less. :)
      for (MStringString::const_iterator _it = attribute_block_.begin();_it != attribute_block_.end();_it++)
        {
          // Note:  This is using the following operator:
          // " std::string& operator <<(std::string&,const std::string&);"
          // defined in strutil.h
          
          _retval.append("\n");
          _retval.append((*_it).first);
          _retval.append(": ");
          _retval.append((*_it).second);
    }
  return _retval;

}
  
ushort GetID(const std::string& key)
{
  SQLINTEGER	retLength;
  ushort		id	= 0;
  std::string		select("SELECT AttributeKey FROM AttributeKeys WHERE Text = '");
  select	+= key;
  select	+= "'";
  
  SQLStmt		sql(Internal::DefaultSession(), "GetID", exceptType);
  sql.ExecDirect(select);
  sql.BindColumn(1, &id, &retLength);
  
  if (!sql.Fetch(true, false))
    id = 0;
  
  return id;
}

void AddKey(const std::string& keyAsString, ushort& keyNum)
{
  std::string	tmp(keyAsString);
  SQLStmt::PrepString(tmp);

  std::string	insert("INSERT INTO AttributeKeys VALUES (");
  insert	+=	"( SELECT MAX(AttributeKey) + 1 FROM AttributeKeys ) , '";
  insert	+= tmp;
  insert	+= "' )";

  SQLStmt		sql(Internal::DefaultSession(), "AddKey", exceptType);
  if (!sql.ExecDirect(insert, true, false) && SQLStmt::CausedDuplicate != sql.GetErrorType())
    sql.ThrowException("");

  keyNum = GetID(tmp);
}

//
//	These tags are stored in the AnalysisRequest object, when
//	the user asks for one of these get the value from the
//	AnalysisRequest object, if they want to set the value, set
//	it in AnalysisRequest.  Throw an exception if they try to
//	delete one of these.

ushort Attributes::CheckSumID()
{
  static ushort checkSumID = 0;
  
  if (0 == checkSumID)
    checkSumID = GetID(AttributeKeys::SampleCheckSum());
  
  if (0 == checkSumID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Sample-Checksum");
  
  return checkSumID;
}

ushort Attributes::PriorityID()
{
  static ushort priorityID = 0;

  if (0 == priorityID)
    priorityID = GetID(AttributeKeys::SamplePriority());

  if (0 == priorityID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Sample-Priority");

  return priorityID;
}

ushort Attributes::StateID()
{
  static ushort stateID = 0;
  
  if (0 == stateID)
    stateID = GetID(AttributeKeys::AnalysisState());

  if (0 == stateID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Analysis-State");

  return stateID;
}

ushort Attributes::SubmittorID()
{
  static ushort submittorID = 0;

  if (0 == submittorID)
    submittorID = GetID(AttributeKeys::CustomerID());

  if (0 == submittorID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Customer-Identifier");

  return submittorID;
}

ushort Attributes::AnalyzedID()
{
  static ushort analyzedID = 0;

  if (0 == analyzedID)
    analyzedID = GetID(AttributeKeys::DateAnalyzed());

  if (0 == analyzedID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Date-Analyzed");
  return analyzedID;
}

ushort Attributes::SignatureSequenceID()
{
  static ushort sigSeqID = 0;
  
  if (0 == sigSeqID)
    sigSeqID = GetID(AttributeKeys::SignatureSequence());
  
  if (0 == sigSeqID)
    throw AVISDBException(exceptType, "Permanent Attribute missing, X-Signature-Sequence");
  
  return sigSeqID;
}

//
// Attributes methods
//


Attributes::Attributes(uint lc) 
  :_attribute_block(),
   _dirty(false),
   localCookie(lc)
{
  load_attributes_block();
}

Attributes::Attributes(const std::string& http_headers_)
  :_attribute_block(),
   _dirty(false),
   localCookie(0)
{
  // I could've done this in the initializer list, but that's silly
  // and unnecessary.
  _attribute_block = map_from_headers_string(http_headers_);

}
Attributes::Attributes(uint lc,const std::string& http_headers_)
  :_attribute_block(),
   _dirty(false),
   localCookie(lc)
{
  // I could've done this in the initializer list, but that's silly
  // and unnecessary.
  _attribute_block = map_from_headers_string(http_headers_);
}

Attributes::Attributes(const Attributes& copy_from_)
  :_attribute_block(copy_from_._attribute_block),
   _dirty(copy_from_._dirty),
   localCookie(copy_from_.localCookie)
{
  
}
Attributes::~Attributes()
{
  save_attributes_block();
  _attribute_block.erase(_attribute_block.begin(),_attribute_block.end());
}

Attributes& Attributes::operator =(const Attributes& copy_from_)
{
  if (copy_from_ != *this)
    {
      _attribute_block = copy_from_._attribute_block;
      _dirty = copy_from_._dirty;
      localCookie = copy_from_.localCookie;
    }
  return (*this);
}
bool	Attributes::operator==(const std::string& attrs) const
{ 
  // I know of no EASY cheaper way than to convert first, then return
  // the comparison result
  MStringString _new(map_from_headers_string(attrs));
  return ( _attribute_block == _new);
}


bool	Attributes::operator!=(const std::string& attrs) const
{ 
  // Hackish looking, but clean.
  return !(operator==(attrs));
}

uint Attributes::LocalCookie(void) 
{
  return localCookie;
}

Attributes::operator std::string() const
{
  // Couldn't be easier,
  // Convert the _attributes_block into a http style debug string.
  std::string httpAttributes;
  return httpAttributes;
}



//
//	Add some number of attributes to the database
//	(input is in http attributes format)
Attributes& Attributes::operator+=(const std::string& addAttributes)
{
  
  _dirty = true;
  // Convert string to MStringString
  MStringString _new(map_from_headers_string(addAttributes));
  // Merge the new hash on top of the old
  MStringString::const_iterator _iterator(_new.begin());
  for (;_iterator != _new.end();++_iterator)
    {
      _attribute_block[(*_iterator).first] = (*_iterator).second;
    }
  return *this;
}


//
//	Remove some number of attributes from the database
//	(input is in http attributes format (we'll be a little more
//	forgiving, you don't have to include bogus values)
Attributes& Attributes::operator-=(const std::string& subAttributes)
{
  _dirty = true;
  // Convert string to MStringString
  MStringString _new(map_from_headers_string(subAttributes));
  // Iterate through all keys in the new block
  MStringString::const_iterator _iterator(_new.begin());
  for (;_iterator != _new.end();++_iterator)
    {
      const std::string& _key = (*_iterator).first;
      MStringString::iterator _candidate;
      // Does this key exist in the current block?
      _candidate = _attribute_block.find(_key);
      if (_candidate != _attribute_block.end())
        {
          // Erase it.
          _attribute_block.erase(_candidate);
        }
    }
  return *this;
}

//	Replace the current attributes with these attributes
//	(input is in http attributes format)

Attributes& Attributes::operator= (const std::string& replaceAttributes)
{
  _dirty = true;
  // Convert attribute string to MStringString
  MStringString _new(map_from_headers_string(replaceAttributes));
  // Assign it over the top of the old.
  _attribute_block = _new;
  return *this;
}

std::string& Attributes::operator[](const std::string& key_)
{
  // Set to dirty because we don't know if this is being used as an
  // lvalue
  _dirty = true;
  std::string _upper_string(key_);
  StringUtil::strupr(_upper_string);
  std::string _message("Attribute[");
  _message.append(_upper_string);
  _message.append("] = ");
  _message.append(_attribute_block[_upper_string]);
  Logger::Log(Logger::LogDebug,Logger::LogAvisDB,_message.c_str());
  return _attribute_block[_upper_string];
}

std::string Attributes::ToDebugString()	// returns a string representation of the
{									// object suitable for debugging messages
  std::string	dbStr("Attributes{\n");
  char	asChar[64];
  sprintf(asChar, "localCookie = %d\nattributes = [", localCookie);
  dbStr	+= asChar;
  dbStr += to_debug_string(_attribute_block);
  dbStr	+= "]}\n";

  return dbStr;
}

