// Dec2RAR.h : Decomposer 2 Engine for RAR files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2RAR_H)
#define DEC2RAR_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2RAR_CPP
#define DEC2RAR_IE __declspec(dllexport) 
#else
#define DEC2RAR_IE __declspec(dllimport) 
#endif

DEC2RAR_IE HRESULT DecNewRAREngine(IDecEngine** ppEngine);
DEC2RAR_IE HRESULT DecDeleteRAREngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewRAREngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteRAREngine(IDecEngine* pEngine);
#endif

#endif  // DEC2RAR_H
