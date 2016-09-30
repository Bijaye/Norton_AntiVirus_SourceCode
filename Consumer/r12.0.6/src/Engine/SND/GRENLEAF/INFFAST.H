//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/INFFAST.H_v   1.0   12 Mar 1998 12:12:32   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/INFFAST.H_v  $
// 
//    Rev 1.0   12 Mar 1998 12:12:32   DCHI
// Initial revision.
// 
//************************************************************************

//
// INFFAST.H
//
//  Source file for ArchiveLib 2.0
//
//  No Copyright claimed by Greenleaf Software!
//
// DESCRIPTION
//
//  This is one of the ZLIB source files, with as few changes as possible.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

/* inffast.h -- header to use inffast.c
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

extern int inflate_fast __P((
    uInt,
    uInt,
    inflate_huft ZL_FAR *,
    inflate_huft ZL_FAR *,
    struct inflate_blocks_state ZL_FAR *,
    z_stream *));
