// Dec2LHA.h : Decomposer 2 Engine for LHA/LZH files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/dec2lha.h_v   1.0   16 Mar 1998 19:04:14   MKEATIN  $


#if !defined(DEC2LHA_H)
#define DEC2LHA_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_LHA 'LHA'


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2LHA_CPP
#define DEC2LHA_IE __declspec(dllexport) 
#else
#define DEC2LHA_IE __declspec(dllimport) 
#endif

DEC2LHA_IE HRESULT DecNewLHAEngine(IDecEngine** ppEngine);
DEC2LHA_IE HRESULT DecDeleteLHAEngine(IDecEngine* pEngine);


#endif  // DEC2LHA_H
