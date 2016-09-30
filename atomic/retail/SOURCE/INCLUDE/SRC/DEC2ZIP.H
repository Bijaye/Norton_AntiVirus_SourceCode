// Dec2Zip.h : Decomposer 2 Engine for Zip files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/DEC2ZIP.H_v   1.0   16 Mar 1998 19:04:06   MKEATIN  $


#if !defined(DEC2ZIP_H)
#define DEC2ZIP_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_ZIP 'ZIP'


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2ZIP_CPP
#define DEC2ZIP_IE __declspec(dllexport) 
#else
#define DEC2ZIP_IE __declspec(dllimport) 
#endif

DEC2ZIP_IE HRESULT DecNewZipEngine(IDecEngine** ppEngine);
DEC2ZIP_IE HRESULT DecDeleteZipEngine(IDecEngine* pEngine);


#endif  // DEC2ZIP_H