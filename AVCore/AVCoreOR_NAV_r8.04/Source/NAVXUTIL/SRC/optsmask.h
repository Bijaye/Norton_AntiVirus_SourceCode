// Copyright 1997 Symantec Corporation, Peter Norton Group
//***************************************************************************
//
// $Header:   S:/navxutil/VCS/optsmask.h_v   1.0   06 Feb 1997 21:05:40   RFULLER  $
//
// Description:
//   Provides prototypes for the OptsMask module.
//
// Contains:
//
//***************************************************************************
// $Log:   S:/navxutil/VCS/optsmask.h_v  $
// 
//    Rev 1.0   06 Feb 1997 21:05:40   RFULLER
// Initial revision
// 
//    Rev 1.0   24 Jan 1997 15:06:50   JBRENNA
// Initial revision.
// 
//***************************************************************************

#ifndef OPS_MASK_H_INCLUDED
#define OPS_MASK_H_INCLUDED

#ifdef SYM_WIN32

#include "options.h"

// These functions are used to modify the options structure as it is read
// from and written to disk. There is even the capability of having an option
// change only in memory and never on the disk.

void OptsMaskReadPostHook  (LPNAVOPTIONS lpNavOpts);
void OptsMaskWritePostHook (LPNAVOPTIONS lpNavOpts);
void OptsMaskWritePreHook (LPNAVOPTIONS lpNavOpts);

#else

// These are only available on Win32 platform. On other platforms, these are
// a NOP.
#define OptsMaskReadPostHook(param1)
#define OptsMaskWritePostHook(param1)
#define OptsMaskWritePreHook(param1)

#endif // SYM_WIN32

#endif // OPS_MASK_H_INCLUDED
