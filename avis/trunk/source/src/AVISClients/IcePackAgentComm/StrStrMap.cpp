//
// IBM AntiVirus Immune System
//
// File Name: StrStrMap.cpp
// Author:    Milosz Muszynski
//
// Generic Hash Table Utility Class
//
// $Log: $
//

//------ pragmas ------------------------------------------------------

#pragma warning ( disable : 4786 ) // identifier truncated

//------ standard includes --------------------------------------------

#include <map>
#include <string>

//------ includes -----------------------------------------------------

#include "StrStrMap.h"
#include "TagValuePair.h"
#include "StrUtil.h"


//
// constructor
//
// the constructor of this class converts a multi-line string
// formatted as follows:
//
// tag-name1<separator>value-string2<delimiter>
// ...
// tag-nameN<separator>value-stringN<last_delimiter>
//
// to a hash table (string to string map)
// default separator, delimiter and last_delimiter are provided but other may be specified
//
// NOTE:
// keys are always uppercase
//
ConstStrStrMap::ConstStrStrMap( const std::string& str, const std::string& delimiter, const std::string& separator, const std::string& last_delimiter )
:	_separator( separator ),
	_delimiter( delimiter ),
	_last_delimiter( last_delimiter )
{
	std::string t = str;

	int pos = t.find( delimiter );

	while ( pos != std::string::npos && pos != 0 )
	{
		std::string line = t.substr( 0, pos );
		t = t.substr( pos + 1, std::string::npos );
		pos = t.find( delimiter );

		StrUtil::delAll( line, "\r" );
		TagValuePair tagValuePair( line, separator );

		_collection[ tagValuePair.tag() ] = tagValuePair.value();
	}
}


//
// contains
//
bool ConstStrStrMap::contains( const std::string& key )
{
	std::string k = key;
	StrUtil::strupr( k );

	StringStringMap::iterator iter = _collection.find( k );
	return iter != _collection.end();
}


//
// getString
//
std::string ConstStrStrMap::getString( const std::string& key )
{
	std::string s;

	std::string k = key;
	StrUtil::strupr( k );

	StringStringMap::iterator iter = _collection.find( k );

	if ( iter != _collection.end() )
	{
		s = iter->second;
	}

	return s;
}


//
// getLong
//
long	ConstStrStrMap::getLong( const std::string& key )
{
	std::string s = getString( key );

	long l = 0;

	if ( StrUtil::isInteger( s ) )
		l = StrUtil::asInteger( s );

	return l;
}


//
// exists
//
// returns true if not only the key exists but but als
// the value associated with the key is not an empty 
// (or containing white space characters) string
//
bool ConstStrStrMap::exists( const std::string& key )
{
	std::string s = getString( key );

	return ! StrUtil::isWhitespace( s );
}


//
// length
//
// returns the total length of the contents of the map including
// all keys, all separators, all values, all delimiters and a last delimiter
//
long ConstStrStrMap::length()
{
	long len = 0;

	for ( StringStringMap::iterator iter = _collection.begin(); iter != _collection.end(); iter++ )
	{
		len += iter->first.length() + iter->second.length() + _separator.length() + _delimiter.length();
	}

	len += _last_delimiter.length();

	return len;
}


//
// copyTo
//
// copies the contents of the map to a flat string
// this is the opposite conversion to the one performed
// by the constructor of this class
//
void ConstStrStrMap::copyTo( char * ptr )
{
	strcpy( ptr, "" );

	for ( StringStringMap::iterator iter = _collection.begin(); iter != _collection.end(); iter++ )
	{
		strcat( ptr, iter->first.c_str()  );
		strcat( ptr, _separator.c_str()   );
		strcat( ptr, iter->second.c_str() );
		strcat( ptr, _delimiter.c_str()   );
	}

	strcat( ptr, _last_delimiter.c_str() );
}


//
// set
//
// this method allows for setting a new key and/or a new value for 
// the key
// string representing a key is converted to uppercase
//
void StrStrMap::set( const std::string& key, const std::string& value )
{
	std::string k = key;
	StrUtil::strupr( k );

	_collection[ k ] = value;
}

