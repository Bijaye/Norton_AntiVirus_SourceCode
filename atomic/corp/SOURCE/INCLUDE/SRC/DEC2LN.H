// Dec2LN.h : Decomposer 2 Engine for Lotus Notes files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/Dec2LN.h_v   1.0   04 Mar 1998 18:01:34   rglenn  $


#if !defined(DEC2LN_H)
#define DEC2LN_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2LN_CPP
#define DEC2LN_IE __declspec(dllexport) 
#else
#define DEC2LN_IE __declspec(dllimport) 
#endif

DEC2LN_IE HRESULT DecNewLNEngine(IDecEngine** ppEngine);
DEC2LN_IE HRESULT DecDeleteLNEngine(IDecEngine* pEngine);

#endif  // DEC2LN_H
