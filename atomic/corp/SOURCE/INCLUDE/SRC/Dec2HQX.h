// Dec2HQX.h : Decomposer 2 Engine for HQX (BinHex) files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2HQX_H)
#define DEC2HQX_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2HQX_CPP
#define DEC2HQX_IE __declspec(dllexport) 
#else
#define DEC2HQX_IE __declspec(dllimport) 
#endif

DEC2HQX_IE HRESULT DecNewHQXEngine(IDecEngine** ppEngine);
DEC2HQX_IE HRESULT DecDeleteHQXEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewHQXEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteHQXEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2HQX_H
