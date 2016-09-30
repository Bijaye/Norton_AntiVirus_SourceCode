// Dec2Text.h : Decomposer Engine for text-bases data formats.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2TEXT_H)
#define DEC2TEXT_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2TEXT_CPP
	#define DECTEXTLINKAGE __declspec(dllexport) 
  #else
	#define DECTEXTLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECTEXTLINKAGE EXTERN_C
#endif

DECTEXTLINKAGE DECRESULT DecNewTextEngine(IDecEngine **ppEngine);
DECTEXTLINKAGE DECRESULT DecDeleteTextEngine(IDecEngine *pEngine);

#endif	// DEC2TEXT_H
