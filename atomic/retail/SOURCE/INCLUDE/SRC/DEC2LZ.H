// Dec2LZ.h : Decomposer 2 Engine for Microsoft Compress (LZ) files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/DEC2LZ.H_v   1.0   16 Mar 1998 19:04:08   MKEATIN  $


#if !defined(DEC2LZ_H)
#define DEC2LZ_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_LZ 'LZ'


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2LZ_CPP
#define DEC2LZ_IE __declspec(dllexport) 
#else
#define DEC2LZ_IE __declspec(dllimport) 
#endif

DEC2LZ_IE HRESULT DecNewLZEngine(IDecEngine** ppEngine);
DEC2LZ_IE HRESULT DecDeleteLZEngine(IDecEngine* pEngine);


#endif  // DEC2LZ_H