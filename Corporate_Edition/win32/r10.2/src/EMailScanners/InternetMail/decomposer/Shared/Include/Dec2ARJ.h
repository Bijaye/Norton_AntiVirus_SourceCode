// Dec2ARJ.h : Decomposer 2 Engine for ARJ files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ARJ_H)
#define DEC2ARJ_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2ARJ_CPP
	#define DECARJLINKAGE __declspec(dllexport) 
  #else
	#define DECARJLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECARJLINKAGE EXTERN_C
#endif

DECARJLINKAGE DECRESULT DecNewARJEngine(IDecEngine** ppEngine);
DECARJLINKAGE DECRESULT DecDeleteARJEngine(IDecEngine* pEngine);

#endif	// DEC2ARJ_H
