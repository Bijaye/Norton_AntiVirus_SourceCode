// Dec2Mime.h : Decomposer 2 Engine for MIME files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/dec2mime.h_v   1.0   16 Mar 1998 19:04:12   MKEATIN  $


#if !defined(DEC2MIME_H)
#define DEC2MIME_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_MIME 'MIME'


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2MIME_CPP
#define DEC2MIME_IE __declspec(dllexport) 
#else
#define DEC2MIME_IE __declspec(dllimport) 
#endif

DEC2MIME_IE HRESULT DecNewMimeEngine(IDecEngine** ppEngine);
DEC2MIME_IE HRESULT DecDeleteMimeEngine(IDecEngine* pEngine);


#endif  // DEC2MIME_H