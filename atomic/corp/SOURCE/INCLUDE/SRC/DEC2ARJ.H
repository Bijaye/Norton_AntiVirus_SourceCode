// Dec2ARJ.h : Decomposer 2 Engine for ARJ files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ARJ_H)
#define DEC2ARJ_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2ARJ_CPP
#define DEC2ARJ_IE __declspec(dllexport) 
#else
#define DEC2ARJ_IE __declspec(dllimport) 
#endif

DEC2ARJ_IE HRESULT DecNewARJEngine(IDecEngine** ppEngine);
DEC2ARJ_IE HRESULT DecDeleteARJEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewARJEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteARJEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2ARJ_H
