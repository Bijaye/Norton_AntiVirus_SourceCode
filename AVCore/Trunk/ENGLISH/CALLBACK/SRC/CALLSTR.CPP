// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   N:/CALLBACK/VCS/callstr.cpv   1.0   18 Jun 1997 13:18:58   MKEATIN  $
//
// Description:  This file contains the strings for NAVEX UI functions
//
// See Also: avapi_cb.cpp
//************************************************************************
// $Log:   N:/CALLBACK/VCS/callstr.cpv  $
// 
//    Rev 1.0   18 Jun 1997 13:18:58   MKEATIN
// Initial revision.
// 
//    Rev 1.1   17 Jun 1997 16:04:46   MKEATIN
// 'ifdefs' strings for their own platform.
// 
//    Rev 1.0   17 Jun 1997 15:06:14   MKEATIN
// Initial revision.
// 
//    Rev 1.1   30 May 1997 19:43:54   MKEATIN
// Added platform.h
// 
//    Rev 1.0   30 May 1997 19:06:20   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"

//-----------------------------------------------------------------------
//
//   NAVEX UI callback strings for DOSX
//
//-----------------------------------------------------------------------

#if defined(SYM_DOSX)

  char SZ_OK []       = "Ok";
  char SZ_CANCEL []   = "Cancel";
  char SZ_CONTINUE [] = "Continue";
  char SZ_YES []      = "Yes";
  char SZ_NO []       = "No";
  char SZ_PREVIOUS [] = "Previous";

  char SZ_ENTER_A_NUMBER [] = "Enter a Number";

  // strings for

  char SZ_INSERT_RESCUE_DISK [] = "Please insert the Rescue Disk labeled:";
  char SZ_PRESS_ENTER []        = "Press ENTER when ready or ESCape to abort...";

#elif defined(SYM_VXD)

//-----------------------------------------------------------------------
//
//   NAVEX UI callback strings for NAVAP (VxD)
//
//-----------------------------------------------------------------------

 char apszNAVEXMessageBoxTitle[] = "Norton AntiVirus Auto-Protect";

 char *apszNAVEXButtons[] =
 {
     NULL,
     "<&OK>",
     "<&Cancel>",
     "<Con&tinue>",
     "<&Yes>",
     "<&No>",
     "<&Previous>"
 };

#endif  // SYM_DOSX
