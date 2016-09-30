// Dec2ID.h : Decomposer 2 Engine for file identification
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ID_H)
#define DEC2ID_H


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2ID_CPP
#define DEC2ID_IE __declspec(dllexport) 
#else
#define DEC2ID_IE __declspec(dllimport) 
#endif

#if defined(_WINDOWS)
DEC2ID_IE HRESULT DecNewIDEngine(IDecEngine** ppEngine);
DEC2ID_IE HRESULT DecDeleteIDEngine(IDecEngine* pEngine);
#endif

#if defined(UNIX)
EXTERN_C HRESULT DecNewIDEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteIDEngine(IDecEngine* pEngine);
#endif


#endif  // DEC2ID_H
