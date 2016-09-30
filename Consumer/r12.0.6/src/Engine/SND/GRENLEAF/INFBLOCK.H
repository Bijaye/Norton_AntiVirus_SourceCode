//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/INFBLOCK.H_v   1.0   12 Mar 1998 12:12:30   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/INFBLOCK.H_v  $
// 
//    Rev 1.0   12 Mar 1998 12:12:30   DCHI
// Initial revision.
// 
//************************************************************************

//
// INFBLOCK.H
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

/* infblock.h -- header to use infblock.c
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

struct inflate_blocks_state;

extern struct inflate_blocks_state ZL_FAR * inflate_blocks_new __P((
    z_stream *,
    check_func,                 /* check function */
    uInt));                     /* window size */

extern int inflate_blocks __P((
    struct inflate_blocks_state ZL_FAR *,
    z_stream *,
    int));                      /* initial return code */

extern void inflate_blocks_reset __P((
    struct inflate_blocks_state ZL_FAR *,
    z_stream *,
    uLong ZL_FAR *));                  /* check value on output */

extern int inflate_blocks_free __P((
    struct inflate_blocks_state ZL_FAR *,
    z_stream *,
    uLong ZL_FAR *));                  /* check value on output */
