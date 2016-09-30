// Dec2UUE.h : Decomposer 2 Engine for UUE files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2UUE_H)
#define DEC2UUE_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2UUE_CPP
#define DEC2UUE_IE __declspec(dllexport) 
#else
#define DEC2UUE_IE __declspec(dllimport) 
#endif

DEC2UUE_IE HRESULT DecNewUUEEngine(IDecEngine** ppEngine);
DEC2UUE_IE HRESULT DecDeleteUUEEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewUUEEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteUUEEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2UUE_H
