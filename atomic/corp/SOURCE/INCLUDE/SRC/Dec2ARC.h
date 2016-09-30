// Dec2ARC.h : Decomposer 2 Engine for ARC files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ARC_H)
#define DEC2ARC_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2ARC_CPP
#define DEC2ARC_IE __declspec(dllexport) 
#else
#define DEC2ARC_IE __declspec(dllimport) 
#endif

DEC2ARC_IE HRESULT DecNewARCEngine(IDecEngine** ppEngine);
DEC2ARC_IE HRESULT DecDeleteARCEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewARCEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteARCEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2ARC_H
