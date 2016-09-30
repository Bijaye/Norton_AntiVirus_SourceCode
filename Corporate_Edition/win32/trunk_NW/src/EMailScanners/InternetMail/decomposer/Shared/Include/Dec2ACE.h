// Dec2ACE.h : Decomposer 2 Engine for ACE files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ACE_H)
#define DEC2ACE_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2ACE_CPP
	#define DECACELINKAGE __declspec(dllexport) 
  #else
	#define DECACELINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECACELINKAGE EXTERN_C
#endif

DECACELINKAGE DECRESULT DecNewACEEngine(IDecEngine **ppEngine);
DECACELINKAGE DECRESULT DecDeleteACEEngine(IDecEngine *pEngine);

#endif	// DEC2ACE_H
