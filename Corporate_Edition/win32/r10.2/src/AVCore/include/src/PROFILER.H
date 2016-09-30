/////////////////////////////////////////////////////////////////////
// Profiler.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/profiler.h_v   1.0   11 Dec 1996 16:26:26   DSACKIN  $
//
// Description:
//    This header file defines or undefines DEBUG_PROFILE.  All projects
//    that will be using the DEBUG_PROFILE information need to include
//    this file.  (Currently anything using CService or CServiceControl).
//
//    Defining DEBUG_PROFILE changes the size of our objects, and therefore
//    all products using the objects must be recompiled when this define
//    is set.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/profiler.h_v  $
// 
//    Rev 1.0   11 Dec 1996 16:26:26   DSACKIN
// Initial revision.
// 
//    Rev 1.0   08 Nov 1996 11:35:46   DKEEFER
// Initial revision
// 
//    Rev 1.0   20 Jun 1996 02:25:22   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__PROFILER_H)
#define __PROFILER_H

// Either Define DEBUG_PROFILE or UnDefine it, but make sure one way
// or the other.

#define DEBUG_PROFILE


//#ifdef DEBUG_PROFILE
//#undef DEBUG_PROFILE
//#endif

#endif // __PROFILER_H
