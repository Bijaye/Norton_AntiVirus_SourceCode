//
// IBM AntiVirus Immune System
//
// File Name: StrUtil.cpp
// Author:    Milosz Muszynski
//
// Generic String Utility Module
//
// $Log: $
//

// credit to Mike Wilson


//------ standard includes --------------------------------------------

#include <string>
#include <fstream>

//------ includes -----------------------------------------------------

#include "strutil.h"

//------ namespaces ---------------------------------------------------

using namespace std;

#ifdef _DEBUG

#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
 * This bananna_boat (sic) is different from all the other ones, due
 * to the lack of a local ltos().  This now does it with sprintf()
 * instead.
 */
void bananna_boat(const char* logmsg_)
{
  std::string _message;
  char _sTime[128];
  SYSTEMTIME _sys_time;
  char tmpBuff[25];

  _strtime(_sTime);
  _message = _sTime;
  GetLocalTime(&_sys_time);
  _message.append(".");

  sprintf(tmpBuff, "%ld", _sys_time.wMilliseconds);
  _message.append(tmpBuff);
  _message.append(" ");

  DWORD threadid = 0;
  threadid = GetCurrentThreadId();

  sprintf(tmpBuff, "%ld", (unsigned long)threadid);
  _message.append(tmpBuff);
  _message.append(" ");
  ofstream _log("\\rtrimbug_servlet.log");
  _log << _message << logmsg_ << "\n";
  _log.flush();
  _log.close();
}
#endif 

//------ methods ------------------------------------------------------

//
// trim
//
string& StrUtil::trim( string& str )
{
	if ( str.size() )
		return ltrim( rtrim( str ) );
	return str;
}


//
// ltrim
//
string& StrUtil::ltrim(string& str)
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
		str = "";
    }
	return str;
}


//
// rtrim
//
string& StrUtil::rtrim(string& str)
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


//
// strupr
//
string& StrUtil::strupr(string& str)
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


//
// strlwr
//
string& StrUtil::strlwr(string& str)
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


//
// isWhitespace
//
bool StrUtil::isWhitespace(const string& str)
{
	return str.find_first_not_of(" \t") == string::npos || str.size() == 0;
}


//
// isInteger
//
bool StrUtil::isInteger(const string& str)
{
    return str.find_first_not_of("0123456789") == string::npos && str.size() != 0;
}


//
// asInteger
//
int StrUtil::asInteger( const std::string& str )
{
	int i;
    sscanf( str.c_str(), "%d", &i );
	return i;
}


//
// asLong
//
long StrUtil::asLong( const std::string& str )
{
	int l;
    sscanf( str.c_str(), "%ld", &l );
	return l;
}

