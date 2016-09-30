// Dec2TNEF.h : Decomposer 2 Engine for TNEF files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2TNEF_H)
#define DEC2TNEF_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2TNEF_CPP
	#define DECTNEFLINKAGE __declspec(dllexport) 
  #else
	#define DECTNEFLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECTNEFLINKAGE EXTERN_C
#endif

DECTNEFLINKAGE DECRESULT DecNewTNEFEngine(IDecEngine **ppEngine);
DECTNEFLINKAGE DECRESULT DecDeleteTNEFEngine(IDecEngine *pEngine);

#endif	// DEC2TNEF_H
