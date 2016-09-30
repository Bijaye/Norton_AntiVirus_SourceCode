////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlbase.h>
#include <atlconv.h>

#include <string>
#include <wchar.h>

class CResourceHelper
{
public:
    // Returns false if the ID couldn't be found.
    //
    static bool LoadResourceString ( UINT uID, std::basic_string<char>& strOut, HINSTANCE hInstance = NULL );
    static bool LoadResourceString ( UINT uID, std::basic_string<WCHAR>& strOut, HINSTANCE hInstance = NULL );

    static std::basic_string<char> LoadResourceString ( UINT uID, HINSTANCE hInstance = NULL );
    static std::basic_string<WCHAR> LoadResourceStringW ( UINT uID, HINSTANCE hInstance = NULL );

private:
    CResourceHelper(void);
    ~CResourceHelper(void);

};
