////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
// $Header:   S:/INCLUDE/VCS/NavAppIDs.h_v   1.6   26 Mar 1998 18:32:34   mdunn  $
//
// Description:
//   This file defines all of the NAV Application Identifiers (IDs) used
//   for accessing the CDefUtils class. CDefUtils provides access to the
//   shared definition directory.
//
//   Do NOT translate any string in this file!!!!
//
//   There are several #defines that alter the definition of this file.
//   An component should define any of the following symbols to get the
//   desired functionality:
//
//      NAVAPIDS_TCHAR      Define when you want all of the symbols 
//                          declared using the TCHAR based typedef. This
//                          symbol should be either defined or undefined
//                          for your entire project.
//
//      NAVAPIDS_BUILDING   This symbol should be defined once within
//                          a given project. This define provides the
//                          actual instances of the symbols. This is 
//                          defined in one and only one of your CPP files.
//
//      NAVAPIDS_WANT_ARRAY Define this symbol when you want the AppIds in
//                          in an array. This is useful for looping through
//                          all of the AppIds. Install and Uninstall programs
//                          use this functionality.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/NavAppIDs.h_v  $
// 
//    Rev 1.6   26 Mar 1998 18:32:34   mdunn
// Added Quarantine app IDs.
// 
//    Rev 1.5   02 Jul 1997 13:11:58   JBRENNA
// Changed the version information from 30 to 50. This matches our real
// internal version number for Roswell & Tesla.
// 
//    Rev 1.4   02 Jul 1997 09:53:26   JBRENNA
// Add NAVAPPIDS_TCHAR support.
// 
//    Rev 1.3   11 Jun 1997 12:31:38   JBRENNA
// Change the number of array elements from a "const WORD" to a #define.
// 
//    Rev 1.2   09 Jun 1997 16:18:40   JBRENNA
// Do not use MYINIT when building the g_lpszNavAppId*Array. The "," characters
// tell the preprocessor that there multiple parameters. Really there is only
// 1 parameter that includes ","s.
// 
//    Rev 1.1   06 Jun 1997 11:42:18   JBRENNA
// Have two AppIds for Auto-Protect: AP1 and AP2. Note that NAVAP.SYS and
// NAVAP.VXD should use AP1. The AP2 is used only by the agent to make sure
// the old definitions do not go away while the device driver is still using
// them.
// 
//    Rev 1.0   04 Jun 1997 15:17:42   JBRENNA
// Initial revision.
// 
//************************************************************************

#ifndef __NAVAPPIDS_H
#define __NAVAPPIDS_H

//************************************************************************
// Setup our local macros
//************************************************************************

// This symbol should be defined when you want TCHAR strings.
#ifdef NAVAPPIDS_TCHAR
    #define MYLPCSTR    LPCTSTR
#else
    #define MYLPCSTR    LPCSTR
#endif

// This should only be defined once in any CPP file for this project. When
// defined, an instance of the data is created.
#ifdef NAVAPPIDS_BUILDING
    #define MYEXTERN
    #define MYINIT(x)   = x
#else
    #define MYEXTERN    extern
    #define MYINIT(x)
#endif

//************************************************************************
// NAV Application IDs Actual strings. 
// Do NOT translate any string in this file!!!!
//************************************************************************

MYEXTERN MYLPCSTR g_szNavAppIdNAVW32        MYINIT("NAV_NAVW32");
MYEXTERN MYLPCSTR g_szNavAppIdNAVProxy      MYINIT("NAV_NAVProxy");
MYEXTERN MYLPCSTR g_szNavAppIdNAVOfficeAV   MYINIT("NAV_NAVOfficeAV");
MYEXTERN MYLPCSTR g_szNavAppId1Ap95         MYINIT("NAV95_50_AP1");
MYEXTERN MYLPCSTR g_szNavAppId1ApNt         MYINIT("NAVNT_50_AP1");
MYEXTERN MYLPCSTR g_szNavAppId2Ap95         MYINIT("NAV95_50_AP2");
MYEXTERN MYLPCSTR g_szNavAppId2ApNt         MYINIT("NAVNT_50_AP2");
MYEXTERN MYLPCSTR g_szNavAppIdQuar95        MYINIT("NAV_50_QUAR");
MYEXTERN MYLPCSTR g_szNavAppIdQuarNt        MYINIT("NAV_50_QUAR");

//************************************************************************
// Define NAVAPPIDS_WANT_ARRAY when you want all of the AppIds for the
// Win95 or WinNT platform. Product uninstalls can loop through the array
// when calling CDefUtils::StopUsingDefs().
//************************************************************************
#ifdef NAVAPPIDS_WANT_ARRAY

    /* Win95 AppIds */
    #define g_ucNavAppId95ArrayNum   4
    MYEXTERN MYLPCSTR g_lpszNavAppId95Array[g_ucNavAppId95ArrayNum]
    #ifdef NAVAPPIDS_BUILDING
                                                      = { g_szNavAppIdNavw95,
                                                          g_szNavAppId1Ap95,
                                                          g_szNavAppId2Ap95,
                                                          g_szNavAppIdQuar95 }
    #endif
        ;

    /* WinNT AppIds */
    #define g_ucNavAppIdNtArrayNum   4
    MYEXTERN MYLPCSTR g_lpszNavAppIdNtArray[g_ucNavAppIdNtArrayNum]
    #ifdef NAVAPPIDS_BUILDING
                                                      = { g_szNavAppIdNavwNt,
                                                          g_szNavAppId1ApNt,
                                                          g_szNavAppId2ApNt,
                                                          g_szNavAppIdQuarNt }
    #endif
        ;

#endif // NAVAPPIDS_WANT_ARRAY


//************************************************************************
// Cleanup our local macros
//************************************************************************
#undef MYLPCSTR
#undef MYEXTERN
#undef MYINIT

#endif //  __NAVAPPIDS_H
