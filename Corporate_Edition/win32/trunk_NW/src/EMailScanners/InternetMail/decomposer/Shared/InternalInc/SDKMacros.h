// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////////
// These macros have been moved here from DecFS.h.  The reason is that we need to
// use binary_strnicmp and that is a private function that we don't want clients
// to use.

//
// Include the header file bstricmp.h for binary_stricmp(). This is replacement
// for stricmp(). Code for this is included in Dec2.cpp
//

#ifndef _SDK_MACROS_H_
#define _SDK_MACROS_H_

#include "bstricmp.h"

#define IS_MEM_NAMESPACE(path) (!binary_strnicmp(path, NAMESPACE_MEMORY, NAMESPACE_MEMORY_U, NAMESPACE_MEMORY_LEN))

#if defined(_WINDOWS)
	#define IS_UNI_NAMESPACE(path) (!binary_strnicmp(path, NAMESPACE_UNICODE, NAMESPACE_UNICODE_U, NAMESPACE_UNICODE_LEN))
	#define IS_RAPI_NAMESPACE(path) (!binary_strnicmp(path, NAMESPACE_RAPI, NAMESPACE_RAPI_U, NAMESPACE_RAPI_LEN))
#else
	#define IS_UNI_NAMESPACE(path) (false)
	#define IS_RAPI_NAMESPACE(path) (false)
#endif

#endif
