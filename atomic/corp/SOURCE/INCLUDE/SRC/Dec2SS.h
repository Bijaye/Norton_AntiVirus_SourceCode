// Dec2SS.h : Decomposer 2 Engine for OLE structured storage files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2SS_H)
#define DEC2SS_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2SS_CPP
#define DEC2SS_IE __declspec(dllexport) 
#else
#define DEC2SS_IE __declspec(dllimport) 
#endif

DEC2SS_IE HRESULT DecNewSSEngine(IDecEngine** ppEngine);
DEC2SS_IE HRESULT DecDeleteSSEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewSSEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteSSEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2SS_H
