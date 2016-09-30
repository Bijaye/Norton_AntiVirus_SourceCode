// Dec2OLE1.h : Decomposer 2 Engine for OLE1 files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2OLE1_H)
#define DEC2OLE1_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2OLE1_CPP
	#define DECOLE1LINKAGE __declspec(dllexport) 
  #else
	#define DECOLE1LINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECOLE1LINKAGE EXTERN_C
#endif

DECOLE1LINKAGE DECRESULT DecNewOLE1Engine(IDecEngine **ppEngine);
DECOLE1LINKAGE DECRESULT DecDeleteOLE1Engine(IDecEngine *pEngine);

#endif	// DEC2OLE1_H
