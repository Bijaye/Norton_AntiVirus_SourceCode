// FSV_PCH.H
//
// Contains common includes for File Services
//
// These includes will make up the pre-compiled header

#ifndef SYM_BUILD_SM
#pragma message("NOTE: Re-compiling PCH file")
#endif

// SDK include files
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <wtypes.h>
#include <io.h>
#include <dos.h>
#include <ctype.h>
#include <malloc.h>
#include <limits.h>

#ifdef SYM_BUILD_SM
#include "wnfsv.h"
#else

// Shared Tech include files
#include <regkeys.h>
#include <registry.h>

// File Services include files
#include "..\inc\wnfsv.h"
#include "..\rc\fsv.hrc"
#endif
