// Dec2ID.h : Decomposer 2 Engine for file identification
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/dec2id.h_v   1.0   16 Mar 1998 19:04:20   MKEATIN  $


#if !defined(DEC2ID_H)
#define DEC2ID_H


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2ID_CPP
#define DEC2ID_IE __declspec(dllexport) 
#else
#define DEC2ID_IE __declspec(dllimport) 
#endif

DEC2ID_IE HRESULT DecNewIDEngine(IDecEngine** ppEngine);
DEC2ID_IE HRESULT DecDeleteIDEngine(IDecEngine* pEngine);


#endif  // DEC2ID_H
