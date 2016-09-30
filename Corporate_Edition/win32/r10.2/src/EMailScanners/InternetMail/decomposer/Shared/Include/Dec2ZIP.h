// Dec2ZIP.h : Decomposer 2 Engine for Zip files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ZIP_H)
#define DEC2ZIP_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2ZIP_CPP
	#define DECZIPLINKAGE __declspec(dllexport) 
  #else
	#define DECZIPLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECZIPLINKAGE EXTERN_C
#endif

DECZIPLINKAGE DECRESULT DecNewZipEngine(IDecEngine **ppEngine);
DECZIPLINKAGE DECRESULT DecDeleteZipEngine(IDecEngine *pEngine);

#endif	// DEC2ZIP_H
