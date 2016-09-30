// Dec29660.h : Decomposer 2 Engine for 9660 files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC29660_H)
#define DEC29660_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC29660_CPP
	#define DEC9660LINKAGE __declspec(dllexport) 
  #else
	#define DEC9660LINKAGE __declspec(dllimport) 
  #endif
#else
  #define DEC9660LINKAGE EXTERN_C
#endif

DEC9660LINKAGE DECRESULT DecNew9660Engine(IDecEngine** ppEngine);
DEC9660LINKAGE DECRESULT DecDelete9660Engine(IDecEngine* pEngine);

#endif	// DEC29660_H
