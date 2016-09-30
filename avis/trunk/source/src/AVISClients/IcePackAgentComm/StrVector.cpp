//
// IBM AntiVirus Immune System
//
// File Name: StrVector.cpp
// Author:    Milosz Muszynski
//
// Generic Vector Utility Class
//
// $Log: $
//

//------ pragmas ------------------------------------------------------

#pragma warning ( disable : 4786 ) // identifier truncated

//------ standard includes --------------------------------------------

#include <string>
#include <vector>

//------ includes -----------------------------------------------------

#include "StrVector.h"


//
// the constructor of this class converts a multi-token string to a vector 
// of separate strings representing particular tokens
// default separator is provided but another separator may be specified
//
StrVector::StrVector( const std::string& string, const std::string separator )
{
	std::string t = string;

	int pos;

	do
	{
	    pos = t.find( separator );
		if ( pos != 0 )
		{
			std::string elem = t.substr( 0, pos );
			this->push_back( elem );
		}
		t = t.substr( pos + 1, std::string::npos );
	}
	while ( pos != std::string::npos );
}
