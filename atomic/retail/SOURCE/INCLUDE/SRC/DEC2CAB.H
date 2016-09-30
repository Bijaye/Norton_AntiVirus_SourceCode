// Dec2Cab.h : Decomposer 2 Engine for Cab files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/DEC2CAB.H_v   1.0   16 Mar 1998 19:04:04   MKEATIN  $


#if !defined(DEC2CAB_H)
#define DEC2CAB_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#define DEC_TYPE_CAB 'CAB'


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2CAB_CPP
#define DEC2CAB_IE __declspec(dllexport) 
#else
#define DEC2CAB_IE __declspec(dllimport) 
#endif

DEC2CAB_IE HRESULT DecNewCabEngine(IDecEngine** ppEngine);
DEC2CAB_IE HRESULT DecDeleteCabEngine(IDecEngine* pEngine);


#endif  // DEC2CAB_H