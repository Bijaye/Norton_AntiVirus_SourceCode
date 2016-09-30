#include "resourcehelper.h"

#include <vector>

CResourceHelper::CResourceHelper(void)
{
}

CResourceHelper::~CResourceHelper(void)
{
}

bool CResourceHelper::LoadString ( UINT uID, std::basic_string<char>& strOut, HINSTANCE hInstance /* NULL */ )
{
	// Load the string resources.
	std::vector< char > vBuffer( 1024 );
    int iNumChars = ::LoadStringA( hInstance, uID, &vBuffer[0], 1024 );

    if ( iNumChars )
    {
	    vBuffer.resize ( iNumChars+1 ); // null terminator
        strOut = &vBuffer[0];
        return true;
    }

    return false;
}

bool CResourceHelper::LoadString ( UINT uID, std::basic_string<WCHAR>& strOut, HINSTANCE hInstance /* NULL */ )
{
	// Load the string resources.
	std::vector< WCHAR > vBuffer( 1024 );										 
    if ( 0 != ::LoadStringW( hInstance, uID, &vBuffer[0], 1024 ))
    {
	    strOut = &vBuffer[0];
        return true;
    }

    return false;
}

std::basic_string<char> CResourceHelper::LoadString ( UINT uID, HINSTANCE hInstance /* NULL */ )
{
	// Load the string resources.
    std::basic_string<char> strTemp;
    CResourceHelper::LoadString ( uID, strTemp, hInstance );
    return strTemp;
}

std::basic_string<WCHAR> CResourceHelper::LoadStringW ( UINT uID, HINSTANCE hInstance /* NULL */ )
{
	// Load the string resources.
    std::basic_string<WCHAR> strTemp;
    CResourceHelper::LoadString ( uID, strTemp, hInstance );
    return strTemp;
}