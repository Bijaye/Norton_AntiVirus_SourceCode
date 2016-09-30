// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0502
#define _WIN32_WINDOWS _WIN32_WINNT
#define _WIN32_IE 0x0550

#include <windows.h>
#include <msi.h>
#include <msiquery.h>

// link in static cclib
#include "ccLibStd.h"

// global handle if anyone needs it
extern HINSTANCE ghInstance;