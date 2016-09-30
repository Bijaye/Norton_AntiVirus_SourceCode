/*
 *  Dec2AS.h
 *  Decomposer 2 engine for AS (AppleSingle) files
 *
 *  Created by Bill Allen on Wed May 15 2002.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
 *  Copyright (c) 2002, 2005 Symantec Corp. All rights reserved.
 *
 */


#if !defined(DEC2AS_H)
#define DEC2AS_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2AS_CPP
	#define DECASLINKAGE __declspec(dllexport) 
  #else
	#define DECASLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECASLINKAGE EXTERN_C
#endif

DECASLINKAGE DECRESULT DecNewASEngine(IDecEngine** ppEngine);
DECASLINKAGE DECRESULT DecDeleteASEngine(IDecEngine* pEngine);

#endif	// DEC2AS_H

