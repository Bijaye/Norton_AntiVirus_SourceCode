// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/****************************************************************************

    FILE:       lapicinc.h

    PURPOSE:    C runtime includes for lapiplat.h

    DESCRIPTION:
    This file must be included in all Notes API Toolkit samples that also
    include lapiplat.h.  I should be included after the C runtime library
    includes and before the Notes API includes.  This is because the Notes
    API headers redefine some of the C runtime symbols.

****************************************************************************/

#ifndef LAPICINC_DEFS
#define LAPICINC_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DOS) && !defined(W)
/****************************************************************************

    DOS (QuickWindows) DEFINITIONS

    These definitions are different from the Windows macro definitions
    because Quick Windows applications do not use NotesMain();
    therefore NotesInitExtended() and NotesTerm() need to be called.

****************************************************************************/

#include <io.h>
#include <stdio.h>

#elif defined(MAC)
/****************************************************************************

    Macintosh DEFINITIONS

****************************************************************************/

#include <Types.h>
#include <Quickdraw.h>
#define thePort qd.thePort
#include <Dialogs.h>
#include <Menus.h>
#include <Windows.h>
#include <Fonts.h>
#include <Memory.h>
#include <SegLoad.h>    /* ExitToShell() */
#include <StdLib.h>     /* malloc, free  */

#ifndef __PPCC__
#include <LibraryManager.h>
#endif

#ifdef __MWERKS__
#include <stdio.h>
#endif

#else
/****************************************************************************
*
*  DEFAULT DEFINITIONS
*
****************************************************************************/

#endif

#ifdef __cplusplus
}
#endif

#endif
