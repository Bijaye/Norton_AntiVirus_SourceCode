// Dec2Mime.h : Decomposer 2 Engine for MIME files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2MIME_H)
#define DEC2MIME_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2MIME_CPP
#define DEC2MIME_IE __declspec(dllexport) 
#else
#define DEC2MIME_IE __declspec(dllimport) 
#endif

DEC2MIME_IE HRESULT DecNewMimeEngine(IDecEngine** ppEngine);
DEC2MIME_IE HRESULT DecDeleteMimeEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewMimeEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteMimeEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2MIME_H
