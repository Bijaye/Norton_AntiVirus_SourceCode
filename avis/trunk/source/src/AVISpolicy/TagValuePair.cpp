//
// IBM AntiVirus Immune System
//
// File Name: TagValuePair.cpp
// Author:    Milosz Muszynski
//
// Generic class representing a tag name and value pair
//
// $Log: $
//

//------ standard includes --------------------------------------------

#include <string>

//------ includes -----------------------------------------------------

#include "TagValuePair.h"
#include "StrUtil.h"

//------ static data --------------------------------------------------

char TagValuePair::CommentChar = '#';


//------ constructor --------------------------------------------------

// constructor decodes a string in a format
// TAG_NAME<separator>value
// tag name gets automatically converted to uppercase
// leading and trailing white space characters are removed from
// the tag name part
// leading white space characters are removed from the value part
// object is set to valid state if it is not a comment (doesn't start
// with a '#' character), the line contains a separator,
// and the tag name length is not zero



TagValuePair::TagValuePair( const std::string& line, const std::string& separator )
{
	_valid = false;

	std::string::size_type p = line.find_first_of( separator );

	if ( p != std::string::npos )
	{
		_tag = line.substr( 0, p );
		StrUtil::trim   ( _tag );
		StrUtil::strupr ( _tag );

		_value = line.substr( p + 1, std::string::npos );
		StrUtil::ltrim  ( _value );

		_valid = true;
	}

	if ( _tag.size() == 0 || _tag[ 0 ] == CommentChar )
		_valid = false;
}


