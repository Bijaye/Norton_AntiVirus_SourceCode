// Dec2AMG.h : Decomposer 2 Engine for ArcManager AMG files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2AMG_H)
#define DEC2AMG_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2AMG_CPP
#define DEC2AMG_IE __declspec(dllexport) 
#else
#define DEC2AMG_IE __declspec(dllimport) 
#endif

DEC2AMG_IE HRESULT DecNewAMGEngine(IDecEngine** ppEngine);
DEC2AMG_IE HRESULT DecDeleteAMGEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewAMGEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteAMGEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2AMG_H
