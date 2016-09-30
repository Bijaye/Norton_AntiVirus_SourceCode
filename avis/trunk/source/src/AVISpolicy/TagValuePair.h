//
// IBM AntiVirus Immune System
//
// File Name: TagValuePair.h
// Author:    Milosz Muszynski
//
// Generic class representing a tag name and value pair
//
// $Log: $
//

#ifndef _TAGVALUEPAIR_H_
#define _TAGVALUEPAIR_H_


class TagValuePair
{
private:
	std::string	   _tag;
	std::string	   _value;
	bool           _valid;

	static char    CommentChar;

public:
// constructor decodes a string in a format
// TAG_NAME<separator>value
// tag name gets automatically converted to uppercase
// leading and trailing white space characters are removed from
// the tag name part
// leading white space characters are removed from the value part
// object is set to valid state if it is not a comment (doesn't start
// with a '#' character), the line contains a separator (<separator>),
// and the tag name length is not zero

	TagValuePair( const std::string& line, const std::string& separator = ":" );

	const std::string& tag    () const { return _tag  ; }
	const std::string& value  () const { return _value; }
	const bool         isValid() const { return _valid; }
};


#endif _TAGVALUEPAIR_H_
