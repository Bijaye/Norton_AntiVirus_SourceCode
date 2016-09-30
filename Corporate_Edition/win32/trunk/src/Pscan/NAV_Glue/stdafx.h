// stdafx.h:  Required by the common client libraries

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Configure the Platform to use - Win2000 w/ IE5.5+
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER
#define WINVER          0x0500
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0500
#endif						
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  0x0500
#endif
#ifndef _WIN32_IE
#define _WIN32_IE       0x0550
#endif

// Configure ATL
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include "ccLibDllLink.h"

using namespace ATL;