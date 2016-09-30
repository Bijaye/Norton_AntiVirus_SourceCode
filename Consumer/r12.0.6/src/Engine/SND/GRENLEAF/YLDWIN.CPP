//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/YLDWIN.CPv   1.0   12 Mar 1998 12:18:02   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/YLDWIN.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:18:02   DCHI
// Initial revision.
// 
//************************************************************************

//
// YLDWIN.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::YieldTime()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// void ALStorage::YieldTime()
//
// ARGUMENTS:
//
//  None.
//
// RETURNS
//
//  Nothing.
//
// DESCRIPTION
//
//  This function has two important things to do.  It gets called
//  at a few different points in the process of reading or writing data
//  from storage objects.  During normal reading and writing, it
//  will get called every time the buffer is loaded or flushed.
//
//  If we are in Windows mode, we execute a PeekMessage() loop.  This
//  makes sure that we aren't hogging the CPU.  By doing it this way,
//  the programmer can be ensure that he/she is being a good citizen
//  without any significant effort.
//
//  The second important function is that of calling the monitor function.
//  The user interface elements need to be updated regularly, and this
//  is done via this call.
//
// SEE ALSO
//
//  ALStorage::LoadBuffer(), ALStorage::FlushBuffer()
//
// REVISION HISTORY
//
//   May 26, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release
//

void AL_PROTO
ALStorage::YieldTime()  /* Tag public function */
{
    if ( mpMonitor )
        mpMonitor->Progress( Tell(), *this );
#if defined( AL_WINDOWS )
    MSG msg;

    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &msg );
        DispatchMessage(&msg);
    }
#endif
}


