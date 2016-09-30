/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SETUPBLD/VCS/navcom.h_v   1.6   05 Oct 1998 17:38:36   rchinta  $
//
//  Common Setup Builder Include file
//
//    File Name:  NavCom.H
//
//  Description:  Include file to use for building NAV 4.0
//
//       Author:  Jim Brennan       Date:  7/7/97
//
//     Comments:  This include file defines certain values to be used in
//                creation of the NAV setup
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/SETUPBLD/VCS/navcom.h_v  $
// 
//    Rev 1.6   05 Oct 1998 17:38:36   rchinta
// Update the time stamp to 05:31:00 for Atomic inline (Barstow)
// 
//    Rev 1.5   22 Apr 1998 11:29:26   JBRENNA
// Update the TimeStamp to 05:30:00 for Atomic.
// 
//    Rev 1.4   12 Feb 1998 18:58:02   MKEATIN
// Port from Roswell
// 
//    Rev 1.4   16 Jan 1998 12:00:24   jtaylor
// Update the time stamp for the build to 5:04....
//
//    Rev 1.3   04 Dec 1997 16:51:10   JBELDEN
// set time back to 5:03
//
//    Rev 1.2   01 Dec 1997 17:48:34   JBRENNA
// Update time to 5:04am.
//
//    Rev 1.1   31 Oct 1997 11:18:04   JBRENNA
// Update time to 5:03 am.
//
//    Rev 1.0   11 Jul 1997 20:09:10   JBRENNA
// Fulling integrate Access database and Trial versions.
//
////////////////////////////////////////////////////////////////////////////

                                        // Define dialog/window titles

#define ENV_DATE                "_FILEDATE"
#define ENV_TIME                "_FILETIME"
#define DEF_TIME                "05:31:00"

#define FILE_GRP_SRC            "data"
#define FILE_GRP_DEST           "File Groups"

#define FILE_GRP_LINE1          "[General]"
#define FILE_GRP_LINE2          "Type=FILELIST"
#define FILE_GRP_LINE3          "Version=1.00.000"
#define FILE_GRP_TOPDIR         "[TopDir]"
#define FILE_GRP_EXT            ".FGL"

#define ZIP_LOCATION            "S:\\ISNAVNT\\SRC"
#define ZIP_DEST                "IS5"
#define PLACEHOLDER_FILE        "PH.TXT"

#define ENV_BMODE               "CORE_BMODE"
#define ENV_DBG_SUFF            "ROSWELL_DBG_SUFF"
#define ENV_PMODE               "CORE_PLAT"

#define INST_RC                 "S:\\ISHIELD3\\SRC"
