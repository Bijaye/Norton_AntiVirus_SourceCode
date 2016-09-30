////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xstring> // contains wstring definitions
#include <tchar.h>

__if_not_exists(tstring)
{
	typedef std::basic_string<TCHAR> tstring;
}