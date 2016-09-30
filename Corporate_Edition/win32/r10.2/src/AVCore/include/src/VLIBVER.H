/////////////////////////////////////////////////////////////////////////////
//
// VLIBVER.H -- Symantec AntiVirus VirLib product version information
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2004, 2005 Symantec Corporation. All rights reserved.
//
// $Header:   S:/INCLUDE/VCS/vlibver.h_v   1.255   09 Dec 1998 17:56:06   HKIM  $
//
// The #defines's in this file are used by VLIB.VER to create the VERSIONINFO
// resource, and can also be used in the standard about box.
//
// When incrementing the build number, be sure to change it in all places!
// You must update VIRLIB_BUILD_NUMBER, VIRLIB_BUILD_STRING, and
// VIRLIB_PRODUCT_VERSION.
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/vlibver.h_v  $
// 
//    Rev 1.255   09 Dec 1998 17:56:06   HKIM
// Savannah Build 27
//
//    Rev 1.254   25 Nov 1998 11:29:14   HKIM
// Atomic Build 34
//
//    Rev 1.253   19 Nov 1998 18:03:22   HKIM
// Savannah Build 26
//
//    Rev 1.252   02 Nov 1998 13:45:08   hkim
// Savannah Build 25
//
//    Rev 1.251   29 Oct 1998 19:25:04   HKIM
// Atomic Build 33
//
//    Rev 1.250   21 Oct 1998 17:31:08   HKIM
// Atomic Build 32
//
//    Rev 1.249   13 Oct 1998 19:24:30   HKIM
// Atomic Build 31
//
//    Rev 1.248   08 Oct 1998 11:42:02   RFULLER
// Savannah Build 24
//
//    Rev 1.247   06 Oct 1998 17:49:18   HKIM
// Atomic build 30
//
//    Rev 1.246   29 Sep 1998 19:42:56   HKIM
// Atomic Build 29
//
//    Rev 1.245   24 Sep 1998 09:51:12   hkim
// Savannah Build 23
//
//    Rev 1.244   22 Sep 1998 17:55:24   hkim
// Atomic Build 28
//
//    Rev 1.243   18 Sep 1998 18:37:52   dwilson
// Luke Build 21
//
//    Rev 1.242   15 Sep 1998 18:00:52   HKIM
// Atomic Build 26
//
//    Rev 1.241   15 Sep 1998 17:57:46   HKIM
// Luke Build 20
//
//    Rev 1.240   09 Sep 1998 17:26:40   HKIM
// Savannah Build 22
//
//    Rev 1.239   08 Sep 1998 18:12:50   HKIM
// Luke Build 19
//
//    Rev 1.238   08 Sep 1998 17:58:18   HKIM
// Atomic Build 26
//
//    Rev 1.237   01 Sep 1998 18:19:08   hkim
// Luke build 18
//
//    Rev 1.236   25 Aug 1998 17:37:48   HKIM
// Luke Build 17
//
//    Rev 1.235   18 Aug 1998 18:36:46   HKIM
// Luke build 16
//
//    Rev 1.234   11 Aug 1998 17:19:46   HKIM
// Luke build 15
//
//    Rev 1.233   04 Aug 1998 18:51:40   JMARQUE
// Luke Build 14
//
//    Rev 1.232   22 Jul 1998 19:14:04   hkim
// Roswell Build 30
//
//    Rev 1.231   22 Jul 1998 15:33:42   dwilson
// Atomic Build 25
//
//    Rev 1.230   15 Jul 1998 16:28:18   dwilson
// Atomic Build 24
//
//    Rev 1.229   08 Jul 1998 18:41:06   hkim
// Atomic Build 23
//
//    Rev 1.228   01 Jul 1998 14:06:26   dwilson
// Atomic Build 22
//
//    Rev 1.227   23 Jun 1998 13:24:18   dwilson
// Atomic Build 21
//
//    Rev 1.226   17 Jun 1998 17:00:42   dwilson
// Atomic Build 20
//
//    Rev 1.225   10 Jun 1998 16:26:22   dwilson
// Atomic Build 19
//
//    Rev 1.224   10 Jun 1998 16:25:14   dwilson
// Atomic Build 19
//
//    Rev 1.223   08 Jun 1998 00:36:48   jchang
// Roswell Build 29
//
//    Rev 1.222   03 Jun 1998 19:22:40   hkim
// Atomic Build 18
//
//    Rev 1.221   28 May 1998 19:48:02   hkim
// Updated for Roswell build 27
//
//    Rev 1.220   27 May 1998 16:30:48   DWILSON
// Atomic Build 17
//
//    Rev 1.219   21 May 1998 22:26:10   DLEWIS
// Atomic Build 16
//
//    Rev 1.218   21 May 1998 13:56:20   hkim
// Savannah Build 21
//
//    Rev 1.217   20 May 1998 18:34:06   dwilson
// Atomic Build 15
//
//    Rev 1.216   18 May 1998 13:38:32   DWILSON
// Atomic Build 14
//
//    Rev 1.215   18 May 1998 10:17:52   DWILSON
// Savannah Build 20
//
//    Rev 1.214   14 May 1998 16:30:22   DWILSON
// Savannah Build 19
//
//    Rev 1.213   11 May 1998 15:28:52   DWILSON
// Atomic Build 13
//
//    Rev 1.212   08 May 1998 18:16:46   DWILSON
// Savannah Build 18
//
//    Rev 1.211   08 May 1998 18:16:00   DWILSON
// Savannah Build 18
//
//    Rev 1.210   04 May 1998 16:17:40   DWILSON
// Atomic Build 12
//
//    Rev 1.209   01 May 1998 13:38:00   DWILSON
// Savannah Build 17
//
//    Rev 1.208   29 Apr 1998 18:34:50   hkim
// Savannah build 16
//
//    Rev 1.207   27 Apr 1998 17:36:22   DWILSON
// Atomic Build 11
//
//    Rev 1.206   24 Apr 1998 15:56:38   DWILSON
// Savannah Build 15
//
//    Rev 1.205   20 Apr 1998 16:57:18   DWILSON
// Atomic Build 10
//
//    Rev 1.204   15 Apr 1998 16:28:30   DWILSON
// Roswell Build 27
//
//    Rev 1.203   13 Apr 1998 15:24:22   DWILSON
// Atomic Build 09
//
//    Rev 1.202   09 Apr 1998 20:47:44   RFULLER
// Updated for Savannah Build 13
//
//    Rev 1.201   08 Apr 1998 18:50:28   HKIM
// Savannah Build 12
//
//    Rev 1.200   06 Apr 1998 15:56:48   DWILSON
// Atomic Build 08
//
//    Rev 1.199   01 Apr 1998 19:45:38   HKIM
// Savannah Build 10
//
//    Rev 1.198   30 Mar 1998 20:13:42   HKIM
// Savannah Build 09
//
//    Rev 1.197   30 Mar 1998 18:10:26   DWILSON
// Atomic Build 07
//
//    Rev 1.196   23 Mar 1998 19:28:54   DWILSON
// Atomic Build 06
//
//    Rev 1.195   16 Mar 1998 17:58:08   dalvidr
// Savannah Build 07
//
//    Rev 1.194   16 Mar 1998 15:57:22   DWILSON
// Atomic Build 05
//
//    Rev 1.193   12 Mar 1998 18:46:40   dalvidr
// Savannah Build 06
//
//    Rev 1.192   09 Mar 1998 15:38:12   DWILSON
// Atomic Build 04
//
//    Rev 1.191   09 Mar 1998 15:27:08   dalvidr
// Savannah Build 05
//
//    Rev 1.190   06 Mar 1998 15:53:10   DWILSON
// Savannah Build 04
//
//    Rev 1.189   02 Mar 1998 21:35:40   DALVIDR
// SAVANNAH Build #03
//
//    Rev 1.188   02 Mar 1998 20:40:12   DLEWIS
// ATOMIC.03
//
//    Rev 1.187   23 Feb 1998 22:56:40   dalvidr
// Savannah Build 02
//
//    Rev 1.186   18 Feb 1998 21:38:12   HKIM
// Pyramid Build 08
//
//    Rev 1.185   18 Feb 1998 21:34:22   dalvidr
// Atlanta Build 21
//
//    Rev 1.184   18 Feb 1998 18:32:48   mdunn
// Updated version number to 5.3 for Atomic, and changed the copyright date
// to 1998.
//
//    Rev 1.183   17 Feb 1998 13:55:56   HKIM
// Atlanta build 20
//
//    Rev 1.182   17 Feb 1998 11:11:36   DWILSON
// Atomic Build 01
//
//    Rev 1.181   10 Feb 1998 08:37:36   DWILSON
// Atlanta Build 19
//
//    Rev 1.180   06 Feb 1998 09:16:06   DWILSON
// Savannah Build 00
//
//    Rev 1.179   03 Feb 1998 08:17:48   DWILSON
// Atlanta Build 17
//
//    Rev 1.178   29 Jan 1998 15:10:10   DWILSON
// Atlanta Build 16
//
//    Rev 1.177   28 Jan 1998 13:11:16   DWILSON
// Atlanta Build 15
//
//    Rev 1.176   27 Jan 1998 09:06:00   DWILSON
// Atlanta Build 14
//
//    Rev 1.175   26 Jan 1998 16:23:36   dalvidr
// Savanna Build 27
//
//    Rev 1.174   23 Jan 1998 13:29:50   dalvidr
// Atlanta Build 12
//
//    Rev 1.173   20 Jan 1998 08:28:54   DWILSON
// Atlanta Build 10
//
//    Rev 1.172   19 Jan 1998 19:56:58   dalvidr
// Savanna Build 26
//
//    Rev 1.171   15 Jan 1998 11:52:28   DWILSON
// Atlanta Build 09
//
//    Rev 1.170   13 Jan 1998 09:01:46   DWILSON
// Atlanta Build 08
//
//    Rev 1.169   09 Jan 1998 08:25:22   DWILSON
// Atlanta Build 07
//
//    Rev 1.168   08 Jan 1998 18:25:18   DALVIDR
// Savanna Build 25
//
//    Rev 1.167   08 Jan 1998 08:43:42   DWILSON
// Atlanta Build 06
//
//    Rev 1.166   07 Jan 1998 07:59:58   DWILSON
// Atlanta Build 05
//
//    Rev 1.165   06 Jan 1998 07:09:18   DALVIDR
// Savanna Build 24
//
//    Rev 1.164   05 Jan 1998 10:43:50   DWILSON
// Atlanta Build 04
//
//    Rev 1.163   29 Dec 1997 16:52:00   HKIM
// Atlanta Build 03
//
//    Rev 1.162   23 Dec 1997 09:28:08   DWILSON
// Atlanta Build 02
//
//    Rev 1.161   19 Dec 1997 20:58:22   HKIM
// NAVSDK build 06
//
//    Rev 1.160   19 Dec 1997 14:18:52   DWILSON
// Atlanta Build 01
//
//    Rev 1.159   18 Dec 1997 18:09:54   DWILSON
// Savanna Build 23
//
//    Rev 1.158   18 Dec 1997 15:07:30   HKIM
// NAVSDK build 05
//
//    Rev 1.157   16 Dec 1997 17:11:14   DWILSON
// Pyramid Build 07
//
//    Rev 1.156   15 Dec 1997 14:52:12   DWILSON
// Savanna Build 22
//
//    Rev 1.155   11 Dec 1997 14:44:16   HKIM
// NAVSDK build 04
//
//    Rev 1.154   09 Dec 1997 12:24:44   HKIM
// NAVSDK build 03
//
//    Rev 1.153   08 Dec 1997 18:21:20   DALVIDR
// Savanna Build 21
//
//    Rev 1.152   08 Dec 1997 18:02:44   HKIM
// Pyramid Build 06
//
//    Rev 1.151   04 Dec 1997 16:41:04   HKIM
// NAVSDK build 02
//
//    Rev 1.150   02 Dec 1997 20:34:42   HKIM
// Luke build 05
//
//    Rev 1.149   25 Nov 1997 13:13:54   DWILSON
// Pyramid Build 04
//
//    Rev 1.148   06 Nov 1997 21:54:22   HKIM
// Luke build 13
//
//    Rev 1.147   03 Nov 1997 23:38:58   HKIM
// Luke build 12
//
//    Rev 1.146   03 Nov 1997 18:33:36   DALVIDR
// Savanna Build 21
//
//    Rev 1.145   30 Oct 1997 19:54:08   DLEWIS
// Luke 11
//
//    Rev 1.144   27 Oct 1997 16:38:12   DWILSON
// Luke Build 10
//
//    Rev 1.143   23 Oct 1997 19:43:26   HKIM
// Luke Build 09
//
//    Rev 1.142   20 Oct 1997 18:20:58   HKIM
// Savanna Build 20
//
//    Rev 1.141   20 Oct 1997 09:50:10   DWILSON
// Rosnec Build 06
//
//    Rev 1.140   13 Oct 1997 16:39:16   DWILSON
// Luke Build 07
//
//    Rev 1.139   13 Oct 1997 16:37:48   DALVIDR
// Savanna Build 19
//
//    Rev 1.138   10 Oct 1997 19:17:42   DLEWIS
// Rosnec Build 05
//
//    Rev 1.137   06 Oct 1997 18:54:44   DALVIDR
// Savanna Build 18
//
//    Rev 1.136   03 Oct 1997 18:14:30   DALVIDR
// ROSNEC Build 04
//
//    Rev 1.135   02 Oct 1997 19:12:16   DALVIDR
// Savanna Build 17
//
//    Rev 1.134   26 Sep 1997 19:07:40   DALVIDR
// Rosnec Build 03
//
//    Rev 1.133   25 Sep 1997 15:01:36   DALVIDR
// Savanna Build 16
//
//    Rev 1.132   23 Sep 1997 18:11:28   DALVIDR
// Savanna Build 15
//
//    Rev 1.131   01 Oct 1997 02:06:08   dalvidr
// Savanna Build 14
//
//    Rev 1.130   11 Sep 1997 17:15:52   DALVIDR
// Savanna Build 13
//
//    Rev 1.129   05 Sep 1997 09:10:18   DALVIDR
// Savanna Build 12
//
//    Rev 1.128   29 Aug 1997 11:15:58   DALVIDR
// Savanna Build 11
//
//    Rev 1.127   24 Aug 1997 11:40:38   DWILSON
// Roswell Build 26
//
//    Rev 1.126   22 Aug 1997 19:01:54   DWILSON
// Roswell Build 25
//
//    Rev 1.125   21 Aug 1997 19:17:38   DALVIDR
// Savanna Build 10
//
//    Rev 1.124   20 Aug 1997 18:15:14   dalvidr
// Roswell Build 24
//
//    Rev 1.123   15 Aug 1997 16:33:24   DALVIDR
// Roswell Build 23
//
//    Rev 1.122   14 Aug 1997 19:00:06   DALVIDR
// Savanna Build 09
//
//    Rev 1.121   13 Aug 1997 17:54:50   DWILSON
// Roswell Build 22
//
//    Rev 1.120   10 Aug 1997 20:43:26   DWILSON
// Roswell Build 21
//
//    Rev 1.119   07 Aug 1997 16:04:52   DALVIDR
// Savanna Build 08
//
//    Rev 1.118   06 Aug 1997 15:41:04   DWILSON
// Roswell Build 20
//
//    Rev 1.117   05 Aug 1997 20:30:10   DLEWIS
// Savanna Build 07
//
//    Rev 1.116   04 Aug 1997 16:40:18   RFULLER
// Gryphnec Build 07
//
//    Rev 1.115   01 Aug 1997 15:18:34   DWILSON
// Roswell Build 19
//
//    Rev 1.114   31 Jul 1997 21:55:16   DALVIDR
// Savanna Build 06
//
//    Rev 1.113   31 Jul 1997 11:38:42   DWILSON
// Roswell Build 18
//
//    Rev 1.112   24 Jul 1997 19:28:04   RFULLER
// Savanna Build 05
//
//    Rev 1.111   18 Jul 1997 19:14:46   DWILSON
// Roswell Build 17
//
//    Rev 1.110   18 Jul 1997 08:48:16   DALVIDR
// Savanna Build 04
//
//    Rev 1.109   16 Jul 1997 17:29:02   DWILSON
// Roswell Build 16
//
//    Rev 1.108   10 Jul 1997 16:53:30   HKIM
// Savanna Build 03
//
//    Rev 1.107   09 Jul 1997 14:54:08   DWILSON
// GRYPHONNEC Build 01
//
//    Rev 1.106   08 Jul 1997 14:19:32   HKIM
// Savanna Build 02
//
//    Rev 1.105   03 Jul 1997 15:45:02   DWILSON
// Roswell Build 14
//
//    Rev 1.104   03 Jul 1997 09:46:06   JBRENNA
// Change the version from 4.5.3 to 5.0.0 for Roswell and Tesla.
//
//    Rev 1.103   01 Jul 1997 18:43:34   HKIM
// Savana build 01
//
//    Rev 1.102   30 Jun 1997 04:18:50   RFULLER
// Updated for ROSWELL Build 13
//
//    Rev 1.101   20 Jun 1997 20:39:36   DWILSON
// Roswell Build 12
//
//    Rev 1.100   19 Jun 1997 10:43:10   RFULLER
// Updated for ROSWELL Build 11
//
//    Rev 1.99   17 Jun 1997 19:33:10   DLEWIS
// KIRNNEC 21
//
//    Rev 1.98   13 Jun 1997 19:18:12   HKIM
// Roswell Build Roswell
//
//    Rev 1.97   10 Jun 1997 15:44:38   DLEWIS
// Kirinnec Build 20
//
//    Rev 1.96   06 Jun 1997 16:33:28   RFULLER
// Roswell Build ROSWELL
//
//    Rev 1.95   05 Jun 1997 18:39:34   DLEWIS
// Roswell Build ROSWELL
//
//    Rev 1.94   03 Jun 1997 16:01:14   DWILSON
// Kirinnec Build 19
//
//    Rev 1.93   30 May 1997 16:19:52   DWILSON
// Roswell Build roswell
//
//    Rev 1.92   27 May 1997 18:52:00   DLEWIS
// Kirinnec Build 18
//
//    Rev 1.91   21 May 1997 09:14:40   DWILSON
// Kirinnec Build 17
//
//    Rev 1.90   19 May 1997 11:22:38   HKIM
// Roswell Build 05
//
//    Rev 1.89   13 May 1997 17:16:34   DWILSON
// Kirinnec Build 16
//
//    Rev 1.88   09 May 1997 16:26:26   DWILSON
// Roswell Build 04
//
//    Rev 1.87   06 May 1997 15:18:54   dwilson
// Kirinnec Build 15
//
//    Rev 1.86   29 Apr 1997 16:39:34   DWILSON
// Kirinnec Build 14
//
//    Rev 1.85   22 Apr 1997 15:25:28   DWILSON
// Kirinnec Build 13
//
//    Rev 1.84   10 Apr 1997 13:28:46   RFULLER
// Roswell CM Test Build
//
//    Rev 1.83   08 Apr 1997 10:45:26   DWILSON
// Kirinnec Build 11
//
//    Rev 1.82   26 Mar 1997 09:56:38   RFULLER
// KirinNEC Build 11
//
//    Rev 1.81   18 Mar 1997 19:38:02   RFULLER
// KirinNEC Build 10
//
//    Rev 1.80   12 Mar 1997 13:04:00   rfuller
// KirinNEC Build 09
//
//    Rev 1.79   03 Feb 1997 16:23:40   DWILSON
// Kirinnec Build 07
//
//    Rev 1.78   27 Jan 1997 16:28:00   DWILSON
// Kirinnec Build 06
//
//    Rev 1.77   20 Jan 1997 16:24:08   DWILSON
// Kirinnec Build 05
//
//    Rev 1.76   14 Jan 1997 13:10:12   JBRENNA
// Update the version for Kirin/NEC to 4.5.3. Our current buld is 4, so updated
// that as well.
//
//    Rev 1.75   13 Jan 1997 17:00:20   DWILSON
// Kirinnec Build 04
//
//    Rev 1.74   06 Jan 1997 16:49:58   DWILSON
// Kirinnec Build 03
//
//    Rev 1.73   23 Dec 1996 12:14:28   RFULLER
// Commando Build 12
//
//    Rev 1.72   18 Dec 1996 13:09:54   SMAGENN
// COMMANDO 11
//
//    Rev 1.71   13 Dec 1996 12:07:16   RFULLER
// COMMANDO 10
//
//    Rev 1.70   11 Dec 1996 11:59:38   SMAGENN
// COMMANDO 09
//
//    Rev 1.69   04 Dec 1996 13:09:26   SMAGENN
// COMMANDO.08
//
//    Rev 1.68   03 Dec 1996 15:21:58   JBRENNA
// Kirin Port: BGHERHAR: Fix copyright string.
//
//    Rev 1.67   27 Nov 1996 11:52:32   SMAGENN
// Commando 07
//
//    Rev 1.66   20 Nov 1996 09:54:08   RFULLER
// Commando Build 06
//
//    Rev 1.65   12 Nov 1996 15:42:20   DWILSON
// Kirin Build 13
//
//    Rev 1.64   08 Nov 1996 16:11:38   DWILSON
// Kirin Build 12
//
//    Rev 1.63   05 Nov 1996 17:19:06   RFULLER
// DOSNAV TTY Commando Build 04
//
//    Rev 1.62   05 Nov 1996 16:09:18   DWILSON
// Kirin Build 11
//
//    Rev 1.61   01 Nov 1996 17:15:30   DWILSON
// Kirin Build 10
//
//    Rev 1.60   29 Oct 1996 22:42:50   RFULLER
// DOSNAV TTY Build (Commando)
//
//    Rev 1.59   15 Oct 1996 15:27:44   RFULLER
// DOSNAV TTY Build 02
//
//    Rev 1.58   09 Oct 1996 11:29:40   RFULLER
// DOSNAV TTY Build 01
//
//    Rev 1.57   04 Oct 1996 16:29:36   GWESTER
// Updated version number to 4.52, and build number to 06 .
//
//    Rev 1.56   20 Sep 1996 18:21:14   DWILSON
// Kirin Build 04
//
//    Rev 1.55   13 Sep 1996 18:27:42   DWILSON
// Kirin Build 03
//
//    Rev 1.54   04 Sep 1996 17:29:58   DWILSON
// Kirin Build 01
//
//    Rev 1.53   29 Aug 1996 10:15:04   RFULLER
// Kirin Build 00
//
//    Rev 1.52   12 Aug 1996 17:45:14   DWILSON
// Gryphon Build 21
//
//    Rev 1.51   12 Aug 1996 13:33:20   RFULLER
// Sniper Build 20
//
//    Rev 1.50   05 Aug 1996 17:06:06   DWILSON
// Gryphon Build 20
//
//    Rev 1.49   05 Aug 1996 14:56:30   JBRENNA
// LuigiPlus Port: JBRENNA: Update the version number from 4.0.1 to 4.5.0.
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef INCL_VIRLIBVER
#define INCL_VIRLIBVER

#define VIRLIB_PRODUCT_NAME         "Symantec AntiVirus Core Technology"
#define VIRLIB_VERSION_MAJOR        5
#define VIRLIB_VERSION_MINOR        3
#define VIRLIB_VERSION_INLINE       0
#define VIRLIB_BUILD_NUMBER         180
#define VIRLIB_BUILD_STRING         "180"
#define VIRLIB_PRODUCT_VERSION      "5.3.0.180"
#define VIRLIB_LEGAL_COPYRIGHT      "Copyright 1991 - 2006 Symantec Corporation. All rights reserved.\0"


/*
    The following defines are used INTERNALLY by our products for non UI
    related needs (such as creating mutex's, registry lookups, etc).
    **THIS STRINGS SHOULD NOT BE INTERNATIONALIZED**  **THESE STRINGS SHOULD
    NOT BE USED AS UI COMPONENTS (window titles, etc)**
*/

// These keys are for Symantec shared DLL's (e.g., SYMEVNT).
// They are the same as Tomahawk:
#define NAV_SHAREDCOMPS          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"
#define NAV_SHAREDCOMPSSUB       "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"
#define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Symantec AntiVirus\\Install\\95"
#define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Symantec AntiVirus\\Install\\95\\Components"
#define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"


// MOREWORK: Why SharedUsage and not SharedDLLs?
#define SYMC_SHAREDUSAGE         "SOFTWARE\\Symantec\\SharedUsage"
                                        // These 3 are same keyword as Tomahawk:
#define NAV_INSTALLEDVAL         "Installed"
#define NAV_INSTALLDIR           "InstallDir"
#define NAV_INSTALL              "Install"

#endif // INCL_VIRLIBVER

