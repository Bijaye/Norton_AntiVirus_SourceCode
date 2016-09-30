// Dec2ZOO.h : Decomposer 2 Engine for ZOO files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ZOO_H)
#define DEC2ZOO_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2ZOO_CPP
	#define DECZOOLINKAGE __declspec(dllexport) 
  #else
	#define DECZOOLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECZOOLINKAGE EXTERN_C
#endif

DECZOOLINKAGE DECRESULT DecNewZOOEngine(IDecEngine **ppEngine);
DECZOOLINKAGE DECRESULT DecDeleteZOOEngine(IDecEngine *pEngine);

#endif	// DEC2ZOO_H
