/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SETUPBLD/VCS/Ros95NEC.h_v   1.0   19 Feb 1998 16:04:40   mdunn  $
//
//  Roswell 95 NEC Setup Builder Include file
//
//    File Name:  Ros95NEC.H
//
//  Description:  Include file to use for building NAV for Win95 NEC 5.0
//
//       Author:  Jim Brennan       Date:  7/7/97
//
//     Comments:  This include file defines certain values to be used in
//                creation of the NAV setup
//
////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/SETUPBLD/VCS/Ros95NEC.h_v  $
// 
//    Rev 1.0   19 Feb 1998 16:04:40   mdunn
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

#define STR_MAINWIND_CAPTION    "Roswell 95 NEC Setup Builder"
#define STR_DIALOG_CAPTION      "Roswell 95 NEC Setup Builder"
#define STR_ERRMSG_CAPTION      "Roswell 95 NEC Setup Builder Error"

#define STR_INSTALL_COMPLETE    "This part of the Norton AntiVirus\nsetup creation is complete.\nIf you have InstallShield 5\ninstalled on your system, Setup\ncan launch it for you with the\ncurrent project.\nWould you like to launch it now?"

#define STR_INSTALL_INCOMPLETE  "This part of the Norton AntiVirus setup creation was not successfull.  Please correct the problems and run the setup builder again."
#define STR_WELCOME             "Welcome to the %P.  This setup program will do the initial processing of the installation for Norton AntiVirus."

                                        // Install Constants ( Do not translate )

#define APP_NAME                "Roswell 95 NEC Setup Builder"
#define FILE_DIRECTORIES        "direct.txt"
#define FILE_COPYINFO           "Ros95NEC.csv"

#define APPBASE_PATH            "S:\\FINAL\\Ros95NEC"

#define ZIPPED_IS5FILE          "ISNAV95.EXE"
#define PROJ_NAME               "Roswell 95.ipr"

                                        // Override %CORE_PLAT% to "I"
#define PMODE_OVERRIDE          1
#define PMODE_OVERRIDE_VALUE    "I"
