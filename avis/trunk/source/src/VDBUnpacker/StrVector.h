//
// IBM AntiVirus Immune System
//
// File Name: StrVector.h
// Author:    Milosz Muszynski
//
// Generic Vector Utility Class
//
// $Log: $
//

#ifndef _STRVECTOR_H_
#define _STRVECTOR_H_



typedef std::vector< std::string > StringVector;

class StrVector : public StringVector
{
public:
	// the constructor of this class converts a multi-token string to a vector 
	// of separate strings representing particular tokens
	// default separator is provided but another separator may be specified
	StrVector( const std::string& string, const std::string separator = " " );
};



#endif _STRVECTOR_H_
