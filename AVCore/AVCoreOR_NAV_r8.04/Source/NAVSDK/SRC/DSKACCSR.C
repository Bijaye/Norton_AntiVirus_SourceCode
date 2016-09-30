// Copyright 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/dskaccsr.c_v   1.0   08 Dec 1997 17:22:28   DALLEE  $
//
// Description:
//
//      This file works some klugery of our build system. NAVSDK and
//      NAVSDKR are built together and updated to the same LIB.xxx
//      area. This means they share the same .OBJ files.
//
//      DSK_ACCS.OBJ from SYMRKNL contains the name of the module which
//      houses it. This must be different for NAVSDK and NAVSDKR.
//
//      This file simply includes DSK_ACCS.C from SYMKRNL so we
//      can compile to an .OBJ of a different name.
//
//      NOTE: The NAVSDKR makefile must list DSKACCSR.C as the first
//      dependency of DSKACCSR.OBJ. DSK_ACCS.C should be listed after
//      that.
//
// Contains:
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/dskaccsr.c_v  $
// 
//    Rev 1.0   08 Dec 1997 17:22:28   DALLEE
// Initial revision.
//***************************************************************************

#include "dsk_accs.c"

