// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _DEC2LHA_H_
#define _DEC2LHA_H_

#if defined(_WINDOWS)
  #ifdef _DEC2LHA_CPP_
	#define DECLHALINKAGE __declspec(dllexport) 
  #else
	#define DECLHALINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECLHALINKAGE EXTERN_C
#endif

DECLHALINKAGE DECRESULT DecNewLHAEngine(IDecEngine **ppEngine);
DECLHALINKAGE DECRESULT DecDeleteLHAEngine(IDecEngine *pEngine);

#endif
