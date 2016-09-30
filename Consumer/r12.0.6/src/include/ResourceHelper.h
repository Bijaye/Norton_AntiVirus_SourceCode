#pragma once

#include <string>
#include "wchar.h"
#include "windows.h"

class CResourceHelper
{
public:
    // Returns false if the ID couldn't be found.
    //
    static bool LoadString ( UINT uID, std::basic_string<char>& strOut, HINSTANCE hInstance = NULL );
    static bool LoadString ( UINT uID, std::basic_string<WCHAR>& strOut, HINSTANCE hInstance = NULL );

    static std::basic_string<char> LoadString ( UINT uID, HINSTANCE hInstance = NULL );
    static std::basic_string<WCHAR> LoadStringW ( UINT uID, HINSTANCE hInstance = NULL );

private:
    CResourceHelper(void);
    ~CResourceHelper(void);

};
