////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/qresinit.h_v   1.0   27 Feb 1998 14:44:36   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// QResInit.h - contains exported functions from QConRes.DLL
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/qresinit.h_v  $
// 
//    Rev 1.0   27 Feb 1998 14:44:36   DBuches
// Initial revision.

#ifndef _QRESINIT_H
#define _QRESINIT_H

// Call this function to force loading of the resource DLL
BOOL _declspec( dllimport ) InitQConsoleResources();

#endif


