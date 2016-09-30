// Dec2Http.h : Decomposer 2 Engine for HTTP files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2HTTP_H)
#define DEC2HTTP_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2HTTP_CPP
#define DEC2HTTP_IE __declspec(dllexport) 
#else
#define DEC2HTTP_IE __declspec(dllimport) 
#endif

DEC2HTTP_IE HRESULT DecNewHttpEngine(IDecEngine** ppEngine);
DEC2HTTP_IE HRESULT DecDeleteHttpEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewHttpEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteHttpEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2HTTP_H
