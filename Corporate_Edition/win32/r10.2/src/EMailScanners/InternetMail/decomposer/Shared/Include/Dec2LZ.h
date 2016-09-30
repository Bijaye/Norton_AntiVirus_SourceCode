// Dec2LZ.h : Decomposer 2 Engine for Microsoft Compress (LZ) files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2LZ_H)
#define DEC2LZ_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2LZ_CPP
	#define DECLZLINKAGE __declspec(dllexport) 
  #else
	#define DECLZLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECLZLINKAGE EXTERN_C
#endif

DECLZLINKAGE DECRESULT DecNewLZEngine(IDecEngine** ppEngine);
DECLZLINKAGE DECRESULT DecDeleteLZEngine(IDecEngine* pEngine);

#endif	// DEC2LZ_H
