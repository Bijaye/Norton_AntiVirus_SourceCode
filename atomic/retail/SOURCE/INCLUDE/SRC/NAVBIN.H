/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/include/VCS/navbin.h_v   1.0   06 Feb 1997 20:51:18   RFULLER  $
 *                                                                      *
 * Description:                                                         *
 *      Defines layout of NAV.BIN file.  This file contains current     *
 *      settings for NAV.                                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/include/VCS/navbin.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:18   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:28   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:52   BARRY
// Initial revision.
// 
//    Rev 1.3   15 Dec 1994 18:56:04   MFALLEN
// Replaced good code with 'mature' code.
// 
//    Rev 1.2   23 Jun 1993 23:39:26   BRAD
// Pulled out header and put into Quake in a file called NAVBNHDR.H.
 ************************************************************************/

#ifndef _NAVBIN_H
#define _NAVBIN_H

#include "navbnhdr.h"
#include "options.h"

typedef struct tagNAVBIN
    {
    NAVBINHEADER        header;         // Header
    NAVOPTIONS          options;
    } NAVBIN;

#endif

