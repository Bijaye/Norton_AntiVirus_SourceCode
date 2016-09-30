// CheckSum.cpp: implementation of the CheckSum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include "CheckSum.h"


using namespace std;

const ushort CheckSum::checkSumLength	= 34;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CheckSum::~CheckSum()
{

}

std::string	CheckSum::ToDebugString()	// returns a string representation of the
{										// object suitable for debugging messages
	string	dbStr("CheckSum{ ");
	dbStr	+= value;
	dbStr	+= " }\n";
	return dbStr;
}
