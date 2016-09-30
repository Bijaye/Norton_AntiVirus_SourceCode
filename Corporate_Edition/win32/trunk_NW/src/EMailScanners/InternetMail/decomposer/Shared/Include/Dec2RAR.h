// Dec2RAR.h : Decomposer 2 Engine for RAR files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2RAR_H)
#define DEC2RAR_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2RAR_CPP
	#define DECRARLINKAGE __declspec(dllexport) 
  #else
	#define DECRARLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECRARLINKAGE EXTERN_C
#endif

DECRARLINKAGE DECRESULT DecNewRAREngine(IDecEngine** ppEngine);
DECRARLINKAGE DECRESULT DecDeleteRAREngine(IDecEngine* pEngine);

#endif	// DEC2RAR_H
