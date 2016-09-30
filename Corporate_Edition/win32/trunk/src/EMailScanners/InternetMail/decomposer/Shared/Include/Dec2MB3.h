// Dec2MB3.h : Decomposer 2 Engine for MB3 (BinHex) files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2MB3_H)
#define DEC2MB3_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2MB3_CPP
	#define DECMB3LINKAGE __declspec(dllexport) 
  #else
	#define DECMB3LINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECMB3LINKAGE EXTERN_C
#endif

DECMB3LINKAGE DECRESULT DecNewMB3Engine(IDecEngine** ppEngine);
DECMB3LINKAGE DECRESULT DecDeleteMB3Engine(IDecEngine* pEngine);

#endif	// DEC2MB3_H
