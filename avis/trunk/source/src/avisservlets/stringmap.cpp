// stringmap.cpp - StringMapUtil Implementation

#if defined (_DEBUG)
#pragma warning ( disable : 4786 )
#endif 
#include "stringmap.h"
#include "strutil.h"

#include <fstream>
#include <list>
#include "utilexception.h" // This sucks
using namespace std;

MStringString StringMapUtil::fromHttpHeaders(const std::string& headers_)
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

MStringString StringMapUtil::fromGoEnvironment(const std::string& headers_)
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
            StringUtil::tokenizeToPair(_line.c_str(),"=");
          // Slam it home if there's something there
          // No empties
          if (_keyvalue.first.length() && _keyvalue.second.length())
            _retval[_keyvalue.first] = _keyvalue.second;
        }
    }
  return _retval;
}

std::string StringMapUtil::toHttpHeaders(const MStringString& map_)
{
  std::string _retval("");
  for (MStringString::const_iterator it = map_.begin();
       it != map_.end(); it++)
    {
      const std::string& _key = ((*it).first);
      const std::string& _value = ((*it).second);
      // Note, this doesn't make any assumptions about the contents
      // of the map_. I don't know if an empty string is a valid
      // key.  But it oughtn't be.
      if (_key.length() && _value.length())
        {
          _retval += _key;
          _retval += ": ";
          _retval += _value;
          _retval += "\n";
        }
    }
  return _retval;
  // There, now.  That wasn't too bad, was it?
}


std::string StringMapUtil::valueIfExists( const MStringString& map_,
                                             const std::string& key_)
{
  MStringString::const_iterator _it = map_.find(key_);
  std::string _retval(" ");
  if (_it != map_.end())
    {
      _retval = (*_it).second;
    }
  return _retval;
}
MStringString StringMapUtil::fromConfigFile(const char* filename_)
{
  MStringString _retval;

  if (filename_ == 0)
    {
      throw (BadFilename("Invalid filename pointer"));
    }

  ifstream _ifile(filename_);
  if (_ifile.is_open() == false)
    {
      throw BadFilename(filename_);
    }

  char _linebuffer[LINE_BUFFER_SIZE + 1];
  while (!_ifile.eof())
    {
      _ifile.getline(_linebuffer,LINE_BUFFER_SIZE);
      if (strlen(_linebuffer) > 0)
        if (_linebuffer[0] != '#')
          {
            // We now suspect this is a content line
            // There's one more test
            char* _pos = 0;
            if ((_pos = strchr(_linebuffer,'=')) != 0)
              {
                // Use the strtok style. 
                _pos[0] = '\0';
                string _name = StringUtil::trim(string(_linebuffer));
                string _value = StringUtil::trim(string(_pos + 1));
                _retval[_name] = _value;
              } // If there's a '=' in the line
          } // if the line is not a comment
    } // if the line is not null
  _ifile.close();
  return _retval;
}

MStringString StringMapUtil::fromEnvParam( char** env_)
{
  MStringString _retval;
  const char* _ptr = env_[0];
  for (int _i = 0;env_[_i] != 0;_i++)
    {
      char _linebuffer[LINE_BUFFER_SIZE + 1];
      strcpy(_linebuffer,env_[_i]);
      char* _pos = 0;
      if ((_pos = strchr(_linebuffer,'=')) != 0)
        {
          _pos[0] = '\0';
          _retval[StringUtil::trim(string(_linebuffer))] = (_pos + 1);
        }
    }
  return _retval;
}

std::string StringMapUtil::toDebugString(const MStringString& map_)
{
  std::string _retval("");
  // It's possible to screw this up really bad without checking
  // a reference pointer. VC6 may take care of this, but I trust M$oft
  // about as far as I can throw them.  Maybe less. :)
  if (&map_ != 0)
    {
      for (MStringString::const_iterator _it = map_.begin();_it != map_.end();_it++)
        {
          // Note:  This is using the following operator:
          // " std::string& operator <<(std::string&,const std::string&);"
          // defined in strutil.h
          
          _retval.append("\n");
          _retval.append((*_it).first);
          _retval.append(": ");
          _retval.append((*_it).second);
        }
    }
  return _retval;
}

std::string StringMapUtil::toPrefixedDebugString(const std::string& prefix_,
                                                 const MStringString& map_)
{
  std::string _retval("");
  MStringString::const_iterator _it = map_.begin();
  for (;_it != map_.end();++_it)
    {
      _retval.append("\n");
      _retval.append(prefix_);
      _retval.append(" ");
      _retval.append((*_it).first);
      _retval.append(": ");
      _retval.append((*_it).second);
    }
  return _retval;
}

MStringString StringMapUtil::hashMerge(const MStringString& first_,
                                       const MStringString& second_)
{
  MStringString _return(first_);
  MStringString::const_iterator _it = second_.end();
  for (_it = second_.begin();_it != second_.end();_it++)
    {
      _return[(*_it).first] = (*_it).second;
    }
  return _return;
}
