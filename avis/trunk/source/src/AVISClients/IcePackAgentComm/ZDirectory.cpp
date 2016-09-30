//
// IBM AntiVirus Immune System
//
// File Name: ZDirectory.cpp
// Author:    Milosz Muszynski
//
// Class ZDirectory provides a C++ wrapper around C runtime library
// functions dealing with iterating over files in a directory
//
// $Log: $
//



// ------ standard headers ------------------------------------------
#include <io.h>
#include <fcntl.h>
#include <string>

// ------ custom headers --------------------------------------------
#include "ZDirectory.h"



// ------ ZDirectory members ----------------------------------------------------------

bool ZDirectory::getFirst( const std::string& filePattern, std::string& fileName ) const
{
	((ZDirectory*)this)->_handle = _findfirst( (_path + filePattern).c_str(), (struct _finddata_t*)&_findData );

    bool success = _handle != -1;

	if ( success )
		fileName = std::string( _findData.name );

	return success;
}


bool ZDirectory::getNext( std::string& fileName ) const
{
    int result = _findnext( _handle, (struct _finddata_t*)&_findData );

    bool success = result == 0;

	if ( success )
		fileName = std::string( _findData.name );

	return success;
}


void ZDirectory::findClose() const
{
	_findclose( _handle );
}


bool ZDirectory::isNormalFile() const
{
	unsigned int a = _findData.attrib;
	return (a & (_A_SYSTEM | _A_SUBDIR | _A_HIDDEN )) == 0;
}


bool ZDirectory::isDirectory() const
{
	unsigned int a = _findData.attrib;
	bool b = ((a & _A_SUBDIR) != 0);
	if ( b )
	{
		std::string dirName( _findData.name );
		if ( dirName == "." || dirName == ".." )
			b = false;
	}

	return b;
}

