// Dec2ID.h : Decomposer 2 Engine for file identification
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ID_H)
#define DEC2ID_H

/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef _WINDOWS
  #ifdef DEC2ID_CPP
	#define DECIDLINKAGE __declspec(dllexport) 
  #else
	#define DECIDLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECIDLINKAGE EXTERN_C
#endif

DECIDLINKAGE DECRESULT DecNewIDEngine(IDecEngine** ppEngine);
DECIDLINKAGE DECRESULT DecDeleteIDEngine(IDecEngine* pEngine);

#endif	// DEC2ID_H
