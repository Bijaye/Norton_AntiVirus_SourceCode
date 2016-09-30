// Dec2RTF.h : Decomposer 2 Engine for RTF files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2RTF_H)
#define DEC2RTF_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2RTF_CPP
#define DEC2RTF_IE __declspec(dllexport) 
#else
#define DEC2RTF_IE __declspec(dllimport) 
#endif

DEC2RTF_IE HRESULT DecNewRTFEngine(IDecEngine** ppEngine);
DEC2RTF_IE HRESULT DecDeleteRTFEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewRTFEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteRTFEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2RTF_H
