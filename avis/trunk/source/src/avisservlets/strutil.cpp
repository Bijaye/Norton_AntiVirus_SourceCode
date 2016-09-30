// strutil.cpp - a simple set of string utility methods.


#pragma warning ( disable : 4786 )
#include <fstream>

#include "strutil.h"

using namespace std;

#ifdef _DEBUG

#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void bananna_boat(const char* logmsg_)
{

  std::string _message;
  char _sTime[128];
  SYSTEMTIME _sys_time;
  _strtime(_sTime);
  _message = _sTime;
  GetLocalTime(&_sys_time);
  _message.append(".");
  std::string _fine_time(StringUtil::ltos(_sys_time.wMilliseconds));
  StringUtil::padLeft(_fine_time,'0',3);
  _message.append(_fine_time);
  _message.append(" ");

  DWORD threadid = 0;
  threadid = GetCurrentThreadId();
  _message.append(StringUtil::ltos((unsigned long)threadid));
  _message.append(" ");
  ofstream _log("\\rtrimbug_servlet.log");
  _log << _message << logmsg_ << "\n";
  _log.flush();
  _log.close();
}
#endif 

string& StringUtil::trim(string& str)
{
  return (ltrim(rtrim(str)));
}
string& StringUtil::ltrim(string& str)
{
  string temp("");
  int idx = 0;

  if ((idx = str.find_first_not_of(" \t")) != string::npos)
    {
      temp = ( str.c_str() + idx);
      str = temp;
    }
  else
    {
      // It's only spaces
      str = "";
    }
  return str;
}
string& StringUtil::rtrim(string& str)
{
#ifdef _DEBUG
  // save a copy
  std::string _dupe(str);
#endif 
  int idx = (str.size() -1);
  while((idx >= 0) && ((str[idx] == ' ') || (str[idx] == '\t')))
    {
      idx--;
	}

#ifdef _DEBUG
  if (idx == -1 && ((str[idx] == ' ') || (str[idx] == '\t')))
  {
      std::string _message("The bananna boat has arrived on string [");
      _message.append(_dupe);
      _message.append("]");
      bananna_boat(_message.c_str());          
  }
#endif
  
  if (idx >= -1)
    str = str.substr(0,idx + 1);
  return str;
}

string& StringUtil::strupr(string& str)
{
  int idx = 0;
  while (idx != str.size())
    {
      if ((str[idx] <= 'z') && (str[idx] >= 'a'))
        str[idx] = (str[idx] + ('A' - 'a'));
      idx++;
    }
  return str;
}

string& StringUtil::strlwr(string& str)
{
  int idx = 0;
  while (idx != str.size())
    {
      if ((str[idx] <= 'Z') && (str[idx] >= 'A'))
        str[idx] = (str[idx] - ('A' - 'a'));
      idx++;
    }
  return str;
}
// Classifiers
bool StringUtil::isWhitespace(const string& str)
{
  return ( (str.find_first_not_of(" \t") == string::npos) || (str.size() == 0));
}
bool StringUtil::isInteger(const string& str)
{
  return ( str.find_first_not_of("0123456789") == string::npos);
}

string StringUtil::nextToken(const string& str,const string& delim)
{
  return (str.substr(0,str.find(delim)));
}

string& StringUtil::padLeft(std::string& str,char withme,int maxsize)
{
  // else, nothing to do!
  int padsize = 0;
  if ((padsize = (maxsize - str.length())) > 0)
    {
      char* temp = new char[maxsize + 1];
      temp[maxsize] = '\0';
      memset(temp,withme,(maxsize * sizeof(char)));
      strcpy(temp + padsize,str.c_str());
      str = temp;
      delete[] temp;
    }
  return str;
}
string& StringUtil::padRight(std::string& str,char withme,int maxsize)
{
  // The only difference between padLeft and padRight 
  // is tht padRight doesn't require the "copy in" of the old buffer
  // be offset by the size of the padding (maxsize - oldstringlength)
  
  // else, nothing to do!
  int padsize = 0;
  if ((padsize = (maxsize - str.length())) > 0)
    {
      char* temp = new char[maxsize + 1];
      memset(temp,withme,(maxsize * sizeof(char)));
      strcpy(temp,str.c_str());
      str = temp;
      delete[] temp;
    }
  return str;
}

string StringUtil::itos(int i, int radix)
{
  string retval("");
  char* _buffer = new char[CONVERSION_BUFFER_SIZE + 1];
  memset(_buffer,'\0',CONVERSION_BUFFER_SIZE);
  itoa(i,_buffer,radix);
  retval = _buffer;
  delete[] _buffer;
  rtrim(retval);
  return (retval);
}
string StringUtil::ltos(long l,int radix)
{
  string retval("");
  char* _buffer = new char[CONVERSION_BUFFER_SIZE + 1];
  memset(_buffer,'\0',CONVERSION_BUFFER_SIZE);

  itoa(l,_buffer,radix);
  retval = _buffer;
  delete[] _buffer;
  return (retval);
}


string& StringUtil::repchr(std::string& str,char fromme,char tome)
{
  for (int i = 0; i < str.size();i++)
    {
      if (str[i] == fromme)
        str[i] = tome;
    }
  return str;
}
string& StringUtil::removeAll(std::string& str,char killme)
{
  string _temp("");
  for (int i = 0; i < str.size();i++)
    {
      if (str[i] != killme)
        _temp += (char) str[i];
    }
  str = _temp;
  return (str);
}

// That delightful overloaded "<<" operator
std::string& operator <<(std::string& left_,const std::string& right_)
{
  left_ += right_;
  return left_;
}

std::list<std::string> StringUtil::tokenizeToList(const char* str_,const char delim_)
{
  std::list < std::string > _retval;
  const char* _ptr = str_;
  const char* _pos = strchr(_ptr,delim_);
  int _src_length = strlen(_ptr);
  while (( _pos > _ptr) && (_pos <= (str_ + _src_length)))
    {
      int _length = _pos - _ptr;
      char* _buffer = new char[_length + 1];
      strncpy(_buffer,_ptr,(_length));
      _buffer[_length] = 0;
      _retval.push_back(_buffer);

      delete[] _buffer;
      _buffer = 0;

      _ptr = _pos + 1;
      _pos = strchr(_ptr,delim_);
    }
  if (_pos == NULL)
    {
      _retval.push_back(_ptr);
    }
  return _retval;

}

std::pair<std::string,std::string> StringUtil::tokenizeToPair(const char* str_,
                                                              const char* delim_)
{
  std::pair<std::string,std::string> _retval;
    
  _retval.first = "";
  _retval.second = "";
  // It costs so little to be sure
  if ( ( (str_         > 0 ) && (delim_         > 0) ) &&
       ( (strlen(str_) > 0 ) && (strlen(delim_) > 0) )
       )
    {
      if (char* _pos = strstr(str_,delim_))
        {
          if (int _len = _pos - str_)
            {
              char* _temp = new char[_len + 1];
              strncpy(_temp,str_,_len);
              _temp[_len] = '\0';
              _retval.first = _temp;
              delete[] _temp;
            }
          _retval.second = (_pos + (strlen(delim_)));
        }
    }
  return _retval;
}

