/////////////////////////////////////////////////////////////////////////////
//
// NAVVER.H -- Norton AntiVirus product version information
//
// Copyright 1995 by Symantec Corporation
//
// The #defines's in this file are used by NAV.VER to create the VERSIONINFO
// resource, and can also be used in the standard about box.
//
// When incrementing the build number, be sure to change it in all places!
// You must update NAV_BUILD_NUMBER, NAV_BUILD_STRING, and NAV_PRODUCT_VERSION.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/navver.h_v  $
// 
//    Rev 1.73   03 Feb 1999 13:50:08   hkim
// Atomic build 37
//
//    Rev 1.72   26 Jan 1999 13:59:50   HKIM
// Atomic build 36
//
//    Rev 1.71   25 Nov 1998 11:29:58   HKIM
// Atomic Build 34
//
//    Rev 1.70   29 Oct 1998 19:25:52   HKIM
// Atomic Build 33
//
//    Rev 1.69   21 Oct 1998 17:31:52   HKIM
// Atomic Build 32
//
//    Rev 1.68   13 Oct 1998 19:25:16   HKIM
// Atomic Build 31
//
//    Rev 1.67   06 Oct 1998 17:50:48   HKIM
// Atomic build 30
//
//    Rev 1.66   29 Sep 1998 19:43:42   HKIM
// Atomic Build 29
//
//    Rev 1.65   22 Sep 1998 17:56:28   hkim
// Atomic Build 28
//
//    Rev 1.64   22 Sep 1998 17:43:08   rchinta
// Changed NAV_VERSION_INLINE, NAV_PRODUCT_VERSION, NAV_INSTALLKEY,
// NAV_INSTALLKEY_VER, NAV_INSTCOMPKEY to reflect Atomic inline
// (from 5.3 to 5.31)
//
//    Rev 1.63   15 Sep 1998 18:02:30   HKIM
// Atomic Build 26
//
//    Rev 1.62   08 Sep 1998 18:00:58   HKIM
// Atomic Build 26
//
//    Rev 1.61   22 Jul 1998 15:34:52   dwilson
// Atomic Build 25
//
//    Rev 1.60   15 Jul 1998 17:03:10   dwilson
// No change.
//
//    Rev 1.59   15 Jul 1998 16:29:24   dwilson
// Atomic Build 24
//
//    Rev 1.58   08 Jul 1998 18:43:22   hkim
// Atomic Build 23
//
//    Rev 1.57   01 Jul 1998 14:07:14   dwilson
// Atomic Build 22
//
//    Rev 1.56   23 Jun 1998 13:25:10   dwilson
// Atomic Build 21
//
//    Rev 1.55   17 Jun 1998 17:01:08   dwilson
// Atomic Build 20
//
//    Rev 1.54   10 Jun 1998 16:25:38   dwilson
// Atomic Build 19
//
//    Rev 1.53   03 Jun 1998 19:23:06   hkim
// Atomic Build 18
//
//    Rev 1.52   27 May 1998 16:31:18   DWILSON
// Atomic Build 17
//
//    Rev 1.51   21 May 1998 22:26:50   DLEWIS
// Atomic Build 16
//
//    Rev 1.50   20 May 1998 18:34:32   dwilson
// Atomic Build 15
//
//    Rev 1.49   18 May 1998 13:39:00   DWILSON
// Atomic Build 14
//
//    Rev 1.48   11 May 1998 15:29:14   DWILSON
// Atomic Build 13
//
//    Rev 1.47   04 May 1998 16:18:02   DWILSON
// Atomic Build 12
//
//    Rev 1.46   27 Apr 1998 17:37:14   DWILSON
// Atomic Build 11
//
//    Rev 1.45   20 Apr 1998 16:57:44   DWILSON
// Atomic Build 10
//
//    Rev 1.44   13 Apr 1998 15:24:46   DWILSON
// Atomic Build 09
//
//    Rev 1.43   06 Apr 1998 15:57:16   DWILSON
// Atomic Build 08
//
//    Rev 1.42   30 Mar 1998 18:10:48   DWILSON
// Atomic Build 07
//
//    Rev 1.41   23 Mar 1998 19:29:20   DWILSON
// Atomic Build 06
//
//    Rev 1.40   16 Mar 1998 15:57:48   DWILSON
// Atomic Build 05
//
//    Rev 1.39   09 Mar 1998 15:38:48   DWILSON
// Atomic Build 04
//
//    Rev 1.38   18 Feb 1998 18:27:24   mdunn
// Updated version numbers to 5.3.  I set the build number to 2 in
// anticipation of the next build in 4.5 days... :)
//
//    Rev 1.37   17 Feb 1998 11:12:00   DWILSON
// Atomic Build 01
//
//    Rev 1.36   11 Feb 1998 10:50:14   DALLEE
// To go along with change in NAVAPW on NT to minimize the Agent now instead
// of closing, I added a reg value NAV_ALLOWCLOSEAGENT ("AllowCloseAgent")
// which reverts back to the old behavior if set != 0.
//
//    Rev 1.35   16 Dec 1997 17:12:16   DWILSON
// Pyramid Build 07
//
//    Rev 1.34   08 Dec 1997 18:03:30   HKIM
// Pyramid Build 06
//
//    Rev 1.33   02 Dec 1997 20:37:00   HKIM
// Luke build 05
//
//    Rev 1.32   25 Nov 1997 13:17:30   DWILSON
// Pyramid Build 04
//
//    Rev 1.31   05 Nov 1997 15:07:12   HKIM
// Einstein32 Build 04
//
//    Rev 1.30   22 Sep 1997 17:23:12   DALVIDR
// EINSTEIN32 BUILD 03
//
//    Rev 1.29   18 Sep 1997 16:45:52   HKIM
// ESTEIN32 Build 02
//
//    Rev 1.28   16 Sep 1997 14:47:28   hkim
// Estein32 Build 01
//
//    Rev 1.27   24 Aug 1997 11:41:46   DWILSON
// Roswell Build 26
//
//    Rev 1.26   22 Aug 1997 19:02:24   DWILSON
// Roswell Build 25
//
//    Rev 1.25   20 Aug 1997 18:15:58   dalvidr
// Roswell Build 24
//
//    Rev 1.24   15 Aug 1997 16:36:18   DALVIDR
// Roswell Build 23
//
//    Rev 1.23   13 Aug 1997 17:55:12   DWILSON
// Roswell Build 22
//
//    Rev 1.22   12 Aug 1997 15:14:32   DALLEE
// Updated version from "4.0" to "5.0" in install keys.
//
//    Rev 1.21   10 Aug 1997 20:44:28   DWILSON
// Roswell Build 21
//
//    Rev 1.20   06 Aug 1997 15:41:28   DWILSON
// Roswell Build 20
//
//    Rev 1.19   01 Aug 1997 15:18:56   DWILSON
// Roswell Build 19
//
//    Rev 1.18   31 Jul 1997 11:39:06   DWILSON
// Roswell Build 18
//
//    Rev 1.17   18 Jul 1997 19:15:08   DWILSON
// Roswell Build 17
//
//    Rev 1.16   16 Jul 1997 17:29:46   DWILSON
// Roswell Build 16
//
//    Rev 1.15   14 Jul 1997 11:29:04   DWILSON
// Roswell Build 15
//
//    Rev 1.14   03 Jul 1997 15:45:28   DWILSON
// Roswell Build 14
//
//    Rev 1.13   03 Jul 1997 11:49:34   JBRENNA
// Changed NAV_INSTALLKEY_VER to 5.0. This matches what Roswell and Tesla
// installs use.
//
//    Rev 1.12   03 Jul 1997 09:38:22   JBRENNA
// Update the Internal version number from 4.5.3 to 5.0.0 for Roswell/Tesla.
// Do we need to update the NAV_INSTALLKEY_VER?
//
//    Rev 1.11   30 Jun 1997 04:17:16   RFULLER
// Updated for ROSWELL Build 13
//
//    Rev 1.10   20 Jun 1997 20:41:04   DWILSON
// Roswell Build 12
//
//    Rev 1.9   19 Jun 1997 10:45:56   RFULLER
// Update for ROSWELL Build 11
//
//    Rev 1.8   13 Jun 1997 19:18:50   HKIM
// Roswell Build Roswell
//
//    Rev 1.7   06 Jun 1997 16:33:56   RFULLER
// Roswell Build ROSWELL
//
//    Rev 1.6   05 Jun 1997 18:40:16   DLEWIS
// Roswell Build ROSWELL
//
//    Rev 1.5   30 May 1997 16:20:44   DWILSON
// Roswell Build roswell
//
//    Rev 1.4   19 May 1997 11:23:22   HKIM
// Roswell Build 05
//
//    Rev 1.3   09 May 1997 16:26:50   DWILSON
// Roswell Build 04
//
//    Rev 1.2   24 Apr 1997 18:32:22   JBRENNA
// Tesla port:
// SEDWARD r1.21
//  Added new #define statements for checking Netscape installations.
//
// SEDWARD r1.19
//  Added NS_INSTALLVER40.
//
//    Rev 1.1   10 Apr 1997 13:31:40   RFULLER
// Roswell CM Test Build
//
//    Rev 1.0   06 Feb 1997 20:51:30   RFULLER
// Initial revision
//
//    Rev 1.48   03 Feb 1997 16:25:42   DWILSON
// Kirinnec Build 07
//
//    Rev 1.47   27 Jan 1997 16:29:38   DWILSON
// Kirinnec Build 06
//
//    Rev 1.46   20 Jan 1997 16:26:50   DWILSON
// Kirinnec Build 05
//
//    Rev 1.45   14 Jan 1997 13:37:40   JBRENNA
// Update copyright to include 1997.
//
//    Rev 1.44   14 Jan 1997 12:19:44   JBRENNA
// Update the version information. Kirin/NEC is 4.5.3 with the current build
// being build 4.
//
//    Rev 1.43   13 Jan 1997 17:02:20   DWILSON
// Kirinnec Build 04
//
//    Rev 1.42   06 Jan 1997 16:55:48   DWILSON
// Kirinnec Build 03
//
//    Rev 1.41   12 Nov 1996 15:44:44   DWILSON
// Kirin Build 13
//
//    Rev 1.40   08 Nov 1996 16:13:16   DWILSON
// Kirin Build 12
//
//    Rev 1.39   05 Nov 1996 16:12:10   DWILSON
// Kirin Build 11
//
//    Rev 1.38   01 Nov 1996 17:17:24   DWILSON
// Kirin Build 10
//
//    Rev 1.37   27 Oct 1996 23:01:38   BGERHAR
// Update copyright string
//
//    Rev 1.36   25 Oct 1996 17:08:32   DWILSON
// Kirin Build 09
//
//    Rev 1.35   18 Oct 1996 15:59:44   SMAGENN
// KIRIN BUILD 08
//
//    Rev 1.34   11 Oct 1996 16:09:14   DWILSON
// kirin build 7
//
//    Rev 1.33   04 Oct 1996 17:40:40   DWILSON
// Kirin Build 06
//
//    Rev 1.32   04 Oct 1996 16:24:30   GWESTER
// Kirin build 6.
//
//    Rev 1.31   04 Oct 1996 16:20:08   GWESTER
// Changed Kirin version to 4.52 .
//
//    Rev 1.30   01 Oct 1996 13:38:52   MKEATIN
// Added NAV_ONRAMP_INSTALLEDVAL1
//
//
//    Rev 1.29   27 Sep 1996 16:11:14   DWILSON
// Kirin Build 05
//
//    Rev 1.28   20 Sep 1996 18:24:12   DWILSON
// Kirin Build 04
//
//    Rev 1.27   13 Sep 1996 18:29:28   DWILSON
// Kirin Build 03
//
//    Rev 1.26   09 Sep 1996 16:03:08   RPULINT
// No change.
//
//    Rev 1.25   04 Sep 1996 17:31:18   DWILSON
// Kirin Build 01
//
//    Rev 1.24   29 Aug 1996 10:16:28   RFULLER
// Kirin Build 00
//
//    Rev 1.23   07 Aug 1996 15:19:26   JBRENNA
// LuigiPlus Port: Update the version number to 4.5.0; ported
// NAV_SHAREDCOMPS_NT & NAV_SHAREDCOMPSSUB_NT; Changed NAV_WINVERKEY
// from ...\Windows NT\CurrentVersion to ...\Windows\CurrentVersion
// under a NAV95 #ifdef.
//
//
//    Rev 1.22   05 Aug 1996 17:15:32   dwilson
// Gryphon build 20
//
//    Rev 1.21   29 Jul 1996 11:33:18   DWILSON
// Gryphon Build 19
//
//    Rev 1.20   11 Jul 1996 13:29:02   DWILSON
// Gryphon Build 18
//
//    Rev 1.19   01 Jul 1996 18:08:32   DWILSON
// Gryphon Build 17
//
//    Rev 1.18   26 Jun 1996 18:41:34   DALLEE
// LuigiPlus port:
// Extra #defines for reg values/key for Netscape, On Ramp.
/////////////////////////////////////////////////////////////////////////////

#ifndef INCL_NAVVER
#define INCL_NAVVER

#define NAV_PRODUCT_NAME         "Norton AntiVirus"
#define NAV_VERSION_MAJOR        5
#define NAV_VERSION_MINOR        3
#define NAV_VERSION_INLINE       1
#define NAV_BUILD_NUMBER         45
#define NAV_BUILD_STRING         "45"
#define NAV_PRODUCT_VERSION      "5.3.1.45"
#define NAV_LEGAL_COPYRIGHT      "Copyright (C) Symantec Corporation 1991-1998"

/*
    The following defines are used INTERNALLY by our products for non UI
    related needs (such as creating mutex's, registry lookups, etc).
    **THIS STRINGS SHOULD NOT BE INTERNATIONALIZED**  **THESE STRINGS SHOULD
    NOT BE USED AS UI COMPONENTS (window titles, etc)**
*/

#define NAV_APP_APW             "Norton AntiVirus Auto-Protect Agent"
#define NAV_APP_APVXD           "Norton AntiVirus Auto-Protect VxD"
#define NAV_APP_CLINIC          "Norton AntiVirus Clinic"
#define NAV_APP_DOWNLOAD        "Norton AntiVirus Download Clinic"

/*
    The following two values are replacements for the values above...  This
    is so that we can use Quake's APIs to read/write registry keys.  Please
    use these if possible.
*/
#define NAV_MODULE_AUTOPROTECT  "Auto-Protect"
#define NAV_MODULE_CLINIC       "Clinic"

// Same as Tomahawk:
#define NAV_APP_RESCUE          "Norton Rescue"
#define NAV_APP_SCHEDULER       "Norton Scheduler"

// These defines are for Netscape so that we can read their registry values
#define NS_APPINFOSECT      "Software\\Netscape\\Netscape Navigator\\Main"
#define NS_MOZILLA          "Mozilla"
#define NS_INSTALLDIR       "Install Directory"
#define NS_INSTALLVER       "Good-2.0"
#define NS_INSTALLVER2      "Good-2"
#define NS_INSTALLVER3      "Good-3"
#define NS_INSTALLVER4      "Good-4"

#ifdef NAVWNT
    #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus NT"
    #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\5.31"
    #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\5.31\\Components"
    //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
    #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                            // Used for NT scheduler, and
                                            // location of registry keys.
    #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus NT"
    #define NAV_INSTALLKEY_FUNC      "Install"
    #define NAV_INSTALLKEY_VER       "5.31"
#else
    #ifdef NAVSCANW
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus Scanner"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\5.31\\Components"
        //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                                // Used for NT scheduler, and
                                                // location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus Scanner"
    #else
        // These keys are for Symantec shared DLL's (e.g., SYMEVNT).
        // They are the same as Tomahawk:
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus\\Install\\5.31\\Components"
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"

                                                // Used for location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus"
    #endif

    #define NAV_INSTALLKEY_FUNC      "Install"
    #define NAV_INSTALLKEY_VER       "5.31"
#endif

// Keys for customizing a version (only Netscape at present)
#define NAV_INSTALLKEY_CUSTOMVERSION "CustomVersion"
#define NAV_INSTALLKEY_NETSCAPE  "Netscape"

// These keys are for Symantec shared DLL's (e.g., SYMEVNT).
// They are the same as Tomahawk:
#define NAV_SHAREDCOMPS          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"
#define NAV_SHAREDCOMPSSUB       "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"

#define NAV_SHAREDCOMPS_NT       "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SharedDLLs"
#define NAV_SHAREDCOMPSSUB_NT    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SharedDLLs"

// MOREWORK: Why SharedUsage and not SharedDLLs?
#define SYMC_SHAREDUSAGE         "SOFTWARE\\Symantec\\SharedUsage"
                                        // These 3 are same keyword as Tomahawk:
#define NAV_INSTALLEDVAL         "Installed"
#define NAV_INSTALLDIR           "InstallDir"
#define NAV_INSTALL              "Install"

                                        // The "LiveUpdate" install key
                                        // should be deleted and replaced
                                        // with "LiveUpdate1" in future
                                        // versions of NAV95 and NAVNT
#define NAV_ONRAMP_INSTALLEDVAL  "LiveUpdate"
#define NAV_ONRAMP_INSTALLEDVAL1 "LiveUpdate1"

// Key for disabling the splash screen.  This is not global for NAV, but
// for each NAV module (AP, Clinic)

#define NAV_DISABLESPLASHSCREEN  "DisableSplashScreen"

// Key to allow NAVAPW agent to close. Only valid for Win NT Agent.
// Default is 0 (FALSE). Non-zero allows users to close Agent (instead of minimize).

#define NAV_ALLOWCLOSEAGENT      "AllowCloseAgent"

#ifdef NAVSCANW
#define NAV_ENABLEAUTOMATION     "EnableAutomation"
#endif

#endif // INCL_NAVVER
