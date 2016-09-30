// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _WIN32_WINNT   0x0500
#define _WIN32_WINDOWS 0x0500	// defined here only to shut ccLib.h's warnings up
#define WINVER         0x0500

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <windows.h>
#include <oaidl.h>
