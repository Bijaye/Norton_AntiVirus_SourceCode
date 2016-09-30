// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2005, Symantec Corporation, All Rights Reserved.

#ifndef SYMSAFERPORTABLE_H_INCLUDED
#define SYMSAFERPORTABLE_H_INCLUDED
#if _MSC_VER > 1000
#pragma once
#endif

#if defined(NLM) || defined (LINUX)
    #include "winStrings.h"
#endif

// Tell SymSafer that there is no wchar_t type on NLM platform.
#ifdef NLM
    #define NO_WCHAR_T
#endif

#ifdef NLM
    #define SYMSAFER_HAS_VSNPRINTF_WIN32_BEHAVIOR
#endif

#endif // SYMSAFERPORTABLE_H_INCLUDED
