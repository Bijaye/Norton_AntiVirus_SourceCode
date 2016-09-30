// Dec2GZIP.h : Decomposer 2 Engine for GZIP files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2GZIP_H)
#define DEC2GZIP_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2GZIP_CPP
	#define DECGZIPLINKAGE	__declspec(dllexport) 
  #else
	#define DECGZIPLINKAGE	__declspec(dllimport) 
  #endif
#else
  #define DECGZIPLINKAGE EXTERN_C
#endif

DECGZIPLINKAGE DECRESULT DecNewGZIPEngine(IDecEngine** ppEngine);
DECGZIPLINKAGE DECRESULT DecDeleteGZIPEngine(IDecEngine* pEngine);

#endif	// DEC2GZIP_H
