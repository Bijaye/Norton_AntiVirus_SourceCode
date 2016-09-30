// Dec2SS.h : Decomposer 2 Engine for OLE structured storage files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2SS_H)
#define DEC2SS_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2SS_CPP
	#define DECSSLINKAGE __declspec(dllexport) 
  #else
	#define DECSSLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECSSLINKAGE EXTERN_C
#endif

DECSSLINKAGE DECRESULT DecNewSSEngine(IDecEngine** ppEngine);
DECSSLINKAGE DECRESULT DecDeleteSSEngine(IDecEngine* pEngine);

#endif	// DEC2SS_H
