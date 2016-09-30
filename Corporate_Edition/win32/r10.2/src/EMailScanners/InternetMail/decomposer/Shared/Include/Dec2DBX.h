// Dec2DBX.h : Decomposer 2 Engine for DBX (Outlook Express Folder) files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2DBX_H)
#define DEC2DBX_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2DBX_CPP
	#define DECDBXLINKAGE __declspec(dllexport) 
  #else
	#define DECDBXLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECDBXLINKAGE EXTERN_C
#endif

DECDBXLINKAGE DECRESULT DecNewDBXEngine(IDecEngine** ppEngine);
DECDBXLINKAGE DECRESULT DecDeleteDBXEngine(IDecEngine* pEngine);

#endif	// DEC2DBX_H
