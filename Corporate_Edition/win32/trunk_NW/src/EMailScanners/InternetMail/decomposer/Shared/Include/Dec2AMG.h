// Dec2AMG.h : Decomposer 2 Engine for ArcManager AMG files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2AMG_H)
#define DEC2AMG_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2AMG_CPP
	#define DECAMGLINKAGE __declspec(dllexport) 
  #else
	#define DECAMGLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECAMGLINKAGE EXTERN_C
#endif

DECAMGLINKAGE DECRESULT DecNewAMGEngine(IDecEngine **ppEngine);
DECAMGLINKAGE DECRESULT DecDeleteAMGEngine(IDecEngine *pEngine);

#endif	// DEC2AMG_H
