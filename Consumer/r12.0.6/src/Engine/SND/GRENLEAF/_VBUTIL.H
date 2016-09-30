//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/_VBUTIL.H_v   1.0   12 Mar 1998 12:12:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/_VBUTIL.H_v  $
// 
//    Rev 1.0   12 Mar 1998 12:12:00   DCHI
// Initial revision.
// 
//************************************************************************

/*
 * _VBUTIL.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the prototypes used for VB interfacing.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : First release
 */

#ifndef __VBUTIL_H
#define __VBUTIL_H

#if !defined( AL_FLAT_MODEL )

extern "C" long _far _pascal
ALCreateVBString( const char _far *string, unsigned short int l );

#endif

#endif
