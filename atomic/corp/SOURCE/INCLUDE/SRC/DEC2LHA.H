// Dec2LHA.h : Decomposer 2 Engine for LHA/LZH files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2LHA_H)
#define DEC2LHA_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2LHA_CPP
#define DEC2LHA_IE __declspec(dllexport) 
#else
#define DEC2LHA_IE __declspec(dllimport) 
#endif

DEC2LHA_IE HRESULT DecNewLHAEngine(IDecEngine** ppEngine);
DEC2LHA_IE HRESULT DecDeleteLHAEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewLHAEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteLHAEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2LHA_H
