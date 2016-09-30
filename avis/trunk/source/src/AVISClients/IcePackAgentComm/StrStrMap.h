//
// IBM AntiVirus Immune System
//
// File Name: StrStrMap.h
// Author:    Milosz Muszynski
//
// Generic Hash Table Utility Class
//
// $Log: $
//

#ifndef _STRSTRMAP_H_
#define _STRSTRMAP_H_

typedef std::map< std::string, std::string > StringStringMap;


//
// instances of class ConstStrStrMap are read only - once the
// map is constructed, its contents cannot be changed
// if you need a map that allows for adding and modifying -
// use the StrStrMap class 
//
class ConstStrStrMap
{
protected:
	StringStringMap			_collection;
	const std::string		_separator;
	const std::string		_delimiter;
	const std::string		_last_delimiter;

public:
	// the constructor of this class converts a multi-line string
	// formatted as follows:
	//
	// tag-name1<separator>value-string2<delimiter>
	// ...
	// tag-nameN<separator>value-stringN<last_delimiter>
	//
	// to a hash table (string to string map)
	// default separator, delimiter and last_delimiter are provided but other may be specified
	ConstStrStrMap( const std::string& str, const std::string& delimiter = "\n", const std::string& separator = ": ", const std::string& last_delimiter = "\n\n\0" );

	std::string				getString	( const std::string& key );
	long					getLong		( const std::string& key );

	bool					contains	( const std::string& key );
	bool					exists	    ( const std::string& key );

	long					length		();
	void					copyTo		( char * ptr );
};


//
// class StrStrMap is based on the ConstStrStrMap class 
// but in addition it allows for adding or modifying new
// keys and values
//
class StrStrMap : public ConstStrStrMap
{
public:
	StrStrMap( const std::string& str, const std::string& delimiter = "\n", const std::string& separator = ":", const std::string& last_delimiter = "\n\n\0" ) 
		: ConstStrStrMap( str, delimiter, separator, last_delimiter ) {}

	void					set			( const std::string& key, const std::string& value );
};


#endif _STRSTRMAP_H_
