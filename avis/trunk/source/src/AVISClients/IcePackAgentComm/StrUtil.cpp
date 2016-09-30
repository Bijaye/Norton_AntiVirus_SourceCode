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

//------ includes -----------------------------------------------------

#include "strutil.h"

//------ namespaces ---------------------------------------------------

using namespace std;

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
  int idx = (str.size() - 1);
  while((idx >= 0) && ((str[idx] == ' ') || (str[idx] == '\t')))
  {
    idx--;
	}

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


//
// makeSureEndsWith
//
void StrUtil::makeSureEndsWith( std::string& s1, char c )
{
	char b[ 2 ];
	b[ 0 ] = c;
	b[ 1 ] = '\0';
	if ( s1.length() == 0 )
	{
		s1 = b;
	}
	else
	{
		if ( s1[ s1.length() - 1 ] != c )
			s1 += b;
	}
}


//
// delAll
//
void StrUtil::delAll( std::string& s1, const std::string& s2 )
{
	int pos;
	while ( (pos = s1.find( s2 )) != string::npos )
	{
		s1.replace( pos, s2.length(), "" );
	}
}


//
// replaceAll
//
void StrUtil::replaceAll( std::string& s,  const std::string& s1, const std::string& s2 )
{
	int pos;
	while ( (pos = s.find( s1 )) != string::npos )
	{
		s.replace( pos, s1.length(), s2 );
	}
}


//
// makeSureStartsWith
//
void StrUtil::makeSureStartsWith( std::string& s1, const std::string& s2 )
{
	string _s1 = s1;
	StrUtil::ltrim( _s1 );
	int pos = _s1.find( s2 );
	if ( pos != 0 )
		s1 = s2 + _s1;
}


//
// contains
//
bool StrUtil::contains( const std::string& s, const std::string& s1 )
{
	int pos = s.find( s1 );
	return pos != string::npos;
}


//
// containsAnyCase
//
bool StrUtil::containsAnyCase( const std::string& s, const std::string& s1 )
{
	string _s = s;
	string _s1 = s1;
	StrUtil::strlwr( _s );
	StrUtil::strlwr( _s1 );

	int pos = _s.find( _s1 );
	return pos != string::npos;
}


//
// startsWith
//
bool StrUtil::startsWith( const std::string& str, const std::string& str2 )
{
	int pos = str.find( str2 );
	return pos == 0;
}


