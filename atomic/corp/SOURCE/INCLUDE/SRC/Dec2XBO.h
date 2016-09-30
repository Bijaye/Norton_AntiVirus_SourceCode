// Dec2XBO.h : Decomposer 2 Engine for XBO files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2XBO_H)
#define DEC2XBO_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_XBO    9999


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2XBO_CPP
#define DEC2XBO_IE __declspec(dllexport) 
#else
#define DEC2XBO_IE __declspec(dllimport) 
#endif

DEC2XBO_IE HRESULT DecNewXBOEngine(IDecEngine** ppEngine);
DEC2XBO_IE HRESULT DecDeleteXBOEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewXBOEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteXBOEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2XBO_H
