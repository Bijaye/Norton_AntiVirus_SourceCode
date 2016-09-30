// Dec2Zip.h : Decomposer 2 Engine for Zip files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2ZIP_H)
#define DEC2ZIP_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2ZIP_CPP
#define DEC2ZIP_IE __declspec(dllexport) 
#else
#define DEC2ZIP_IE __declspec(dllimport) 
#endif

DEC2ZIP_IE HRESULT DecNewZipEngine(IDecEngine** ppEngine);
DEC2ZIP_IE HRESULT DecDeleteZipEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewZipEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteZipEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2ZIP_H
