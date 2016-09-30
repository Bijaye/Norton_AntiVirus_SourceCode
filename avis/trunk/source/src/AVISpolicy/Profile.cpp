//
// IBM AntiVirus Immune System
//
// File Name: Profile.cpp
// Author:    Milosz Muszynski
//
// Generic Profile Module
//
// $Log: $
//

//------ pragmas ------------------------------------------------------

#pragma warning ( disable : 4786 ) // identifier truncated

//------ standard includes --------------------------------------------

#include <fstream.h>
#include <string>
#include <map>
#include <strstrea.h>

//------ includes -----------------------------------------------------

#include "StrUtil.h"
#include "Profile.h"
#include "TagValuePair.h"


//------ class ProfileItem --------------------------------------------

ProfileItem::ProfileItem( const ProfileItemData& data )
:	_key			( data._key         ),
	_default		( data._default		),
	_value			( data._default		),	 // initially value equals the default
	_description	( data._description )
{
	StrUtil::strupr( _key );
}


ProfileItem::~ProfileItem()
{
}


//------ class Profile ------------------------------------------------


//
// constructor
//
Profile::Profile( const ProfileItemData * items, int count, std::string fileName )
: _items( items ), _count( count), _fileName( fileName )
{
	init();
	read();
}


//
// destructor
//
Profile::~Profile()
{
	clear();
}


//
// init
//
// reads the contents of a definition array containing information about
// all valid keys, default values and descriptions
//
void Profile::init()
{
	for ( int i = 0; i < _count; i++ )
	{
		const ProfileItemData& item = _items[ i ];
		ProfileItem * p = new ProfileItem( item );
		_collection[ std::string( p->key() ) ] = p;
	}
}


//
// read
//
// reads the contents of a file and updates values of all attributes that
// match the found key; profile lines with key not recognized will be ignored;
// returns true if everything went fine, otherwise returns false
//
bool Profile::read()
{
	bool ok = true;

	ifstream fin;

	fin.open( _fileName.c_str(), ios::in | ios::nocreate );

	if ( fin )
	{
		char line[ LineSize ];

		while( !fin.eof() && !fin.bad() )
		{
			fin.getline( line, LineSize );

			TagValuePair tagValuePair( line );

			if ( tagValuePair.isValid() )
			{
				ProfileCollection::iterator _iter = _collection.find( tagValuePair.tag() );
				if ( _iter != _collection.end() )
 					_iter->second->value() = tagValuePair.value();
			}
		}

		fin.close();
	}

	if ( fin.bad() )
		ok = false;

	return ok;
}


//
// clear
//
// deletes all items remembered - this function needs to be called before the 
// profile class instance is deleted
//
void Profile::clear()
{
	ProfileCollection::iterator _iter;

	for ( _iter = _collection.begin(); _iter != _collection.end(); _iter++ )
	{
		delete _iter->second;
	}

	_collection.clear();
}


//
// getString
//
// returns string value of a specific attribute
//
std::string Profile::getString ( const std::string& key )
{
	std::string s;

	ProfileCollection::iterator _iter;

	std::string k = key;

	StrUtil::strupr( k );

	_iter = _collection.find( k );

	if ( _iter != _collection.end() )
	{
		s = _iter->second->value();
	}


	StrUtil::trim( s );

	return s;
}


//
// getBool
//
// returns value of a specific attribute interpreted as a boolean
// anything that starts with 1, y, Y, t, T is treated as representing
// the boolean value true, all other strings are interpreted as false
//
bool	Profile::getBool( const std::string& key )
{
	std::string s = getString( key );

	char c = s[ 0 ];

	return c == '1' || c == 'Y' || c == 'T' || c == 'y' || c == 't';
}


//
// getInt
//
// returns value of a specific attribute converted to an integer
//
int		Profile::getInt( const std::string& key )
{
	std::string s = getString( key );

	int i = 0;

	if ( StrUtil::isInteger( s ) )
		i = StrUtil::asInteger( s );

	return i;
}


//
// getLong
//
// returns value of a specific attribute converted to a long integer
//
long	Profile::getLong( const std::string& key )
{
	std::string s = getString( key );

	long l = 0;

	if ( StrUtil::isInteger( s ) )
		l = StrUtil::asInteger( s );

	return l;
}


//
// info
//
// returns string with information about all current profile attributes -
// attribute names, current valueas, default values and descriptions
//
std::string	Profile::getAll()
{
	ostrstream s;

	ProfileCollection::iterator _iter;

	for ( _iter = _collection.begin(); _iter != _collection.end(); _iter++ )
	{
		s << _iter->second->key().c_str() << " " << _iter->second->value().c_str() << " (default=" << _iter->second->defaultValue().c_str() << ") //" << _iter->second->description().c_str() << endl;
	}

	s << ends;
	std::string t = s.str();
	return t;
}


