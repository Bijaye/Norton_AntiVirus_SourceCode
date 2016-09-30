// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   N:/AVAPI1/VCS/AVAPISTR.CPv   1.1   30 May 1997 19:43:54   MKEATIN  $
//
// Description:  This file contains the strings for NAVEX UI functions
//
// See Also: avapi_cb.cpp
//************************************************************************
// $Log:   N:/AVAPI1/VCS/AVAPISTR.CPv  $
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

char SZ_OK []       = "Ok";
char SZ_CANCEL []   = "Cancel";
char SZ_CONTINUE [] = "Continue";
char SZ_YES []      = "Yes";
char SZ_NO []       = "No";
char SZ_PREVIOUS [] = "Previous";

char SZ_ENTER_A_NUMBER [] = "Enter a Number";


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
