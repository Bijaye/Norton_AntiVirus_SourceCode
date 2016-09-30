// Dec2LZ.h : Decomposer 2 Engine for Microsoft Compress (LZ) files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2LZ_H)
#define DEC2LZ_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2LZ_CPP
#define DEC2LZ_IE __declspec(dllexport) 
#else
#define DEC2LZ_IE __declspec(dllimport) 
#endif

DEC2LZ_IE HRESULT DecNewLZEngine(IDecEngine** ppEngine);
DEC2LZ_IE HRESULT DecDeleteLZEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewLZEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteLZEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2LZ_H
