// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

/***
 * Main header file for NAV Exclusions Engine
 *  Includes files required across the entire project
 * and ... stuff.
 ***/
#ifndef __N32EXCLU_navexclu_h__
#define __N32EXCLU_navexclu_h__

// Disable stupid inheritance warning
#ifdef WIN32
    #pragma warning ( disable : 4250 )
    #pragma warning( disable : 4290 )
#endif

// C++/Win32 Headers
#include <windows.h>
#include <wchar.h>  // wide-character string handling functions

#define DLLExport __declspec( dllexport )

// Common Client headers
#include "SymInterface.h"
#include "ccKeyValueCollectionInterface.h"
#include "ccStringInterface.h"
#include "ccLib.h"

namespace NavExclusions
{
typedef ULONGLONG ExclusionId;
typedef ULONGLONG ExclusionBitvector;
#define _NEVER_EXPIRE 0
#define _INVALID_ID 0
#define _INVALID_VID 0

enum ExResult
{
    // Success return codes
    Success = 0,
    False,

    // Failure return codes
    Fail = 1000,        // Generic Failure
    OutOfMemory,        // Problem allocating memory
    NotInitialized,     // The object is not initialized
    InvalidArgument,    // An invalid argument was passed in
    InsufficientBuffer, // The buffer was not large enough to hold the data
    OutOfRange,         // The item specificed does not exist
    NoInterface,        // SymInterface error
    InvalidPath,        // File or registry path was invalid
    InvalidOS,          // The OS does not support this feature	     
    NotImplemented      // The method is not implemented by this object
};

// Change this to change the CC Settings folder where the exclusions
// are stored.
#define EXCLUSIONS_SETTINGS_KEY L"Norton AntiVirus\\Exclusions"

// Change these to change the name of the exclusions-changed event
#define SYM_REFRESH_NAV_EXCLUSIONS L"SYM_REFRESH_NAV_EXCLUSIONS"
#define SYM_REFRESH_NAV_EXCLUSIONS_TCHAR _T("SYM_REFRESH_NAV_EXCLUSIONS")
#define EXCLUSIONS_DEFAULTS_FILENAME_TCHAR _T("defexcl.dat")
}
#endif
