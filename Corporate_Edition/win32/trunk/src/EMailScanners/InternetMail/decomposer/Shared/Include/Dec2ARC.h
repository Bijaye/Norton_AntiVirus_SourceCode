// Dec2ARC.h : Decomposer 2 Engine for ARC files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ARC_H)
#define DEC2ARC_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2ARC_CPP
	#define DECARCLINKAGE __declspec(dllexport) 
  #else
	#define DECARCLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECARCLINKAGE EXTERN_C
#endif

DECARCLINKAGE DECRESULT DecNewARCEngine(IDecEngine** ppEngine);
DECARCLINKAGE DECRESULT DecDeleteARCEngine(IDecEngine* pEngine);

#endif	// DEC2ARC_H
