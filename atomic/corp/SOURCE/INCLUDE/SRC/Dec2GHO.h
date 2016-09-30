// Dec2GHO.h : Decomposer 2 Engine for Ghost (.GHO) files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2GHO_H)
#define DEC2GHO_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2GHO_CPP
#define DEC2GHO_IE __declspec(dllexport) 
#else
#define DEC2GHO_IE __declspec(dllimport) 
#endif

DEC2GHO_IE HRESULT DecNewGHOEngine(IDecEngine** ppEngine);
DEC2GHO_IE HRESULT DecDeleteGHOEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewGHOEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteGHOEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2GHO_H
