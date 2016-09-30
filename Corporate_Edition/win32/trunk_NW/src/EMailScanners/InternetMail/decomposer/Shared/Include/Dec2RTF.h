// Dec2RTF.h : Decomposer 2 Engine for RTF files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2RTF_H)
#define DEC2RTF_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2RTF_CPP
	#define DECRTFLINKAGE __declspec(dllexport) 
  #else
	#define DECRTFLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECRTFLINKAGE EXTERN_C
#endif

DECRTFLINKAGE DECRESULT DecNewRTFEngine(IDecEngine **ppEngine);
DECRTFLINKAGE DECRESULT DecDeleteRTFEngine(IDecEngine *pEngine);

#endif	// DEC2RTF_H
