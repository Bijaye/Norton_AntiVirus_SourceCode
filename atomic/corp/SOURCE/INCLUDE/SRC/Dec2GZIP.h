// Dec2GZIP.h : Decomposer 2 Engine for GZIP files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2GZIP_H)
#define DEC2GZIP_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2GZIP_CPP
#define DEC2GZIP_IE __declspec(dllexport) 
#else
#define DEC2GZIP_IE __declspec(dllimport) 
#endif

DEC2GZIP_IE HRESULT DecNewGZIPEngine(IDecEngine** ppEngine);
DEC2GZIP_IE HRESULT DecDeleteGZIPEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewGZIPEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteGZIPEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2GZIP_H
