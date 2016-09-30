/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SETUPBLD/VCS/cros95.h_v   1.2   21 Sep 1998 02:13:14   VGLOSTE  $
//
//  Roswell 95 Setup Builder Include file
//
//    File Name:  Ros95.H
//
//  Description:  Include file to use for building NAV for Win95 4.0
//
//       Author:  Jim Brennan       Date:  7/7/97
//
//     Comments:  This include file defines certain values to be used in
//                creation of the NAV setup
//
////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/SETUPBLD/VCS/cros95.h_v  $
// 
//    Rev 1.2   21 Sep 1998 02:13:14   VGLOSTE
// Updated strings from "Roswell" to "Atomic" and "Tesla" to "Maxwell"
// 
//    Rev 1.1   21 Sep 1998 01:56:40   VGLOSTE
// Changed names for the banners and title bars.
// 
//    Rev 1.0   18 Sep 1998 17:15:06   VGLOSTE
// Initial revision.
// 
//    Rev 1.0   21 Oct 1997 17:20:08   jtaylor
// Initial revision.
//
//    Rev 1.5   17 Jul 1997 19:56:48   JBRENNA
// Ensure that PMODE_OVERRIDE is always defined.
//
//    Rev 1.4   11 Jul 1997 20:09:36   JBRENNA
// Fulling integrate Access database and Trial versions.
//
//    Rev 1.3   11 Jul 1997 10:55:54   JBRENNA
// Rework to support Access database.
//
//    Rev 1.2   09 Jul 1997 19:32:38   JBRENNA
// Moved the "Ros95" directory in front of "data" and "is5" directories.
//
//    Rev 1.1   08 Jul 1997 20:00:00   JBRENNA
// Update strings.
//
//    Rev 1.0   08 Jul 1997 11:35:38   JBRENNA
// Initial revision.
//
//    Rev 1.0   01 Jul 1997 19:14:42   BRUCEH
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

                                        // Define dialog/window titles

#define STR_MAINWIND_CAPTION    "Atomic 95 Custom OEM Setup Builder"
#define STR_DIALOG_CAPTION      "Atomic 95 Custom OEM Setup Builder"
#define STR_ERRMSG_CAPTION      "Atomic 95 Custom OEM Setup Builder Error"

#define STR_INSTALL_COMPLETE    "This part of the Norton AntiVirus\nsetup creation is complete.\nIf you have InstallShield 5\ninstalled on your system, Setup\ncan launch it for you with the\ncurrent project.\nWould you like to launch it now?"

#define STR_INSTALL_INCOMPLETE  "This part of the Norton AntiVirus setup creation was not successfull.  Please correct the problems and run the setup builder again."
#define STR_WELCOME             "Welcome to the %P.  This setup program will do the initial processing of the installation for Norton AntiVirus."

                                        // Install Constants ( Do not translate )

#define APP_NAME                "Atomic 95 Custom OEM Setup Builder"
#define FILE_DIRECTORIES        "direct.txt"
#define FILE_COPYINFO           "cRos95.csv"

#define APPBASE_PATH            "S:\\FINAL\\cRos95"

#define ZIPPED_IS5FILE          "ISNAV95.EXE"
#define PROJ_NAME               "Roswell 95.ipr"

                                        // Override %CORE_PLAT% to "I"
#define PMODE_OVERRIDE          1
#define PMODE_OVERRIDE_VALUE    "I"