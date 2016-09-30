// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

#ifdef SYM_LIBC_NLM
#include <stdlib.h>
#else
#include "nlm.h"
#endif
#include "winBaseTypes.h"
#include "winStrings.h"

#include "nwreg.h"      // nwreg now handles dynamic registry defines

// Tell SymSaferRegistry that NLM does not support wchar_t data types. Yes, 
// LIBC NLMs to support wchar_t, we do not need the wchar_t registry funcs
// on NLMs.
#define NO_WCHAR_T

#include "SymSaferRegistry.h"

// Ensure that all LPSTR versions of RegQueryValueEx in the C++ file is mapped
// to the normal RegQueryValueEx.
#define RegQueryValueExA  RegQueryValueEx

// Now that the headers cause the appropiate symbol definitions, include the
// implementation of the safer registry functions.
#include "..\src\SymSaferRegistry.cpp"
