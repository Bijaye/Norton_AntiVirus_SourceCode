// Dec2LNEx.h : Decomposer 2 Engine for Lotus Notes files
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/Dec2LNEx.h_v   1.0   16 Mar 1998 19:04:18   MKEATIN  $


#if !defined(DEC2LNEX_H)
#define DEC2LNEX_H


/////////////////////////////////////////////////////////////////////////////
// Global Constants

#ifndef DEC_TYPE_LN
#define DEC_TYPE_LN 'LN'
#endif


/////////////////////////////////////////////////////////////////////////////
// Global Functions

#ifdef DEC2LNEX_CPP
#define DEC2LNEX_IE __declspec(dllexport) 
#else
#define DEC2LNEX_IE __declspec(dllimport) 
#endif

DEC2LNEX_IE HRESULT DecNewLNEngineEx(IDecEngine** ppEngine);
DEC2LNEX_IE HRESULT DecDeleteLNEngineEx(IDecEngine* pEngine);

#endif  // DEC2LNEX_H