// Copyright 1994 - 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   N:/VIRSCAN/VCS/regstrs.cpv   1.2   30 Jul 1996 11:46:48   CNACHEN  $
//
// Description:
//      This file defines VIRLIB's strings.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   N:/VIRSCAN/VCS/regstrs.cpv  $
// 
//    Rev 1.2   30 Jul 1996 11:46:48   CNACHEN
// Removed NAV95 ifdefs for NAVEMUL support from registry...
// 
//    Rev 1.1   28 Jun 1996 12:25:36   JBRENNA
// Merge LuigiPlus into Gryphon:
//    Add NAV95_CODE #ifdef so that the strings are only defined when
//    the code that uses them is defined.
// 
//    Rev 1.0   18 Jun 1996 11:25:28   CNACHEN
// Initial revision.
// 
//***************************************************************************

#include "platform.h"

char szNAVWConfigRegProduct[]   = "Norton AntiVirus";
char szNAVWConfigRegClinic[]    = "Clinic";
char szNAVWConfigRegKeyname[]   = "NAVEMUL";
