// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navwstb.h_v   1.22   22 Sep 1998 23:09:18   tcashin  $
//
// Description:
//      String IDs for file: navw.stb
//
// &? 10/15/95 DALLEE - Couldn't run GENIDS to make this table since
//                      NAVW.STB now uses hard #defines for Help Tags
//                      from NAVW_HLP.H.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/navwstb.h_v  $
// 
//    Rev 1.22   22 Sep 1998 23:09:18   tcashin
// Added IDs for new activity log templates and filters.
// 
//    Rev 1.21   26 Jun 1998 16:07:04   jtaylor
// Added AP_UNAVAILABLE status
//
//    Rev 1.20   20 Jun 1998 14:09:56   tcashin
// Added IDs for the status window in the main UI and the info dialog.
//
//    Rev 1.19   03 Jun 1998 13:29:08   DBuches
// Added strings.
//
//    Rev 1.18   18 May 1998 15:42:46   DBuches
// Added scan path dialog stuff.
//
//    Rev 1.17   29 Apr 1998 14:17:44   DBuches
// Added toolbar tooltip strings.
//
//    Rev 1.16   23 Apr 1998 21:07:40   DALLEE
// Added strings for real-time status dispaly in main UI.
//
//    Rev 1.15   14 Apr 1998 23:42:34   DALLEE
// Added Quarantine strings.
// Removed some old unused AP button bar strings.
//
//    Rev 1.14   10 Apr 1998 11:14:16   tcashin
// Added IDS_TEMPLATE_QUARANTINE.
//
//    Rev 1.13   20 Nov 1997 11:44:02   jtaylor
// Added resource IDS_LIVEUPDATE that has the name of the LU executable.
//
//    Rev 1.12   20 Aug 1997 14:19:04   JBRENNA
// Remove IDS_VERSION. We no longer use it.
//
//    Rev 1.11   31 Jul 1997 09:00:14   BGERHAR
// Move help URL's and filenames to resource
//
//    Rev 1.10   27 Jul 1997 14:26:54   BGERHAR
// Added Scan:File support for Compressed Files
//
//    Rev 1.9   21 Jul 1997 01:57:56   BGERHAR
// removed /SHOWRES command-line text -- not implementing this switch
//
//    Rev 1.8   18 Jul 1997 17:27:04   BGERHAR
// Added ID's for /SHOWRESULTS, /NORESULTS, /VISIBLE
//
//    Rev 1.7   26 Jun 1997 16:40:18   TIVANOV
// new define for the new command line switch
//
//    Rev 1.6   11 Jun 1997 14:50:14   MKEATIN
// Defined IDS_HIMEM
//
//    Rev 1.5   31 May 1997 16:21:22   KSACKIN
// Changed IDS_SCHEDULEDLLNAME to IDS_SCHEDULERNT.
//
//    Rev 1.4   09 May 1997 13:48:24   TIVANOV
// new define for the quarantine filter - IDS_FILTER_QUARANTINE
//
//    Rev 1.3   24 Apr 1997 18:45:32   JBRENNA
// Add header comment for VCS comments.
//
//***************************************************************************

#define IDS_NAVW             	    261	/* ERR_NUM(  1,   5) */
#define IDS_NAVW_WIN95       	    262	/* ERR_NUM(  1,   6) */
#define IDS_NAVW_WINNT       	    263	/* ERR_NUM(  1,   7) */
#define IDS_NAVW_HLP         	    264	/* ERR_NUM(  1,   8) */
#define IDS_SCHEDULER        	    265	/* ERR_NUM(  1,   9) */
#define IDS_ACTILOG_TITLE    	    266	/* ERR_NUM(  1,  10) */
#define IDS_INFECTIONLOG_TITLE 	    267	/* ERR_NUM(  1,  11) */
#define IDS_NAVWOPTS_VXDADD_ERROR 	    268	/* ERR_NUM(  1,  12) */
#define IDS_NAVWOPTS_VXDREMOVE_ERROR 	    269	/* ERR_NUM(  1,  13) */
#define IDS_SCAN             	    270	/* ERR_NUM(  1,  14) */
#define IDS_COPYRIGHT        	    271	/* ERR_NUM(  1,  15) */
#define IDS_INFODESK         	    273	/* ERR_NUM(  1,  17) */
#define IDS_ERROR_STARTING          244 /* &? Is this supposed to be 274? */
#define IDS_NO_SYMSTAT              275
#define IDS_PATH                    276
#define IDS_NAV_APP_PATH_KEY        277
#define IDS_APW                     278
#define IDS_NOTFORWIN95             279
#define IDS_ERROR_TITLE             280
#define IDS_HELPFILE_INTERNET       281
#define IDS_HELPFILE_ONLINE         282
#define IDS_URL_SUPPORT             283
#define IDS_URL_SARC                284

#define IDS_SCANMAIN_BBAR_OPTIONS 	    512	/* ERR_NUM(  2,   0) */
#define IDS_SCANMAIN_BBAR_VIRUSLIST 	    513	/* ERR_NUM(  2,   1) */
#define IDS_SCANMAIN_BBAR_SCHEDULER 	    514	/* ERR_NUM(  2,   2) */
#define IDS_SCANMAIN_BBAR_ACTIVITYLOG 	    515	/* ERR_NUM(  2,   3) */
#define IDS_NAVWMODIFIED     	    516	/* ERR_NUM(  2,   4) */
#define IDS_ONEHALFINMEMORY  	    517	/* ERR_NUM(  2,   5) */
#define IDS_SCANMAIN_BBAR_NAV95           518 /* ERR_NUM(  2,   6) */
#define IDS_SCANMAIN_BBAR_NAV3            519 /* ERR_NUM(  2,   7) */
#define IDS_SCANMAIN_BBAR_HELP            520 /* ERR_NUM(  2,   8) */
#define IDS_SCANMAIN_BBAR_QUARANTINE      521 /* ERR_NUM(  2,   9) */
#define IDS_SCANMAIN_BBAR_ONRAMP    	   522 /* ERR_NUM(  2,   10)*/
#define IDS_SCANMAIN_BBAR_OPTIONS_TT       523
#define IDS_SCANMAIN_BBAR_VIRUSLIST_TT     524
#define IDS_SCANMAIN_BBAR_SCHEDULER_TT     525
#define IDS_SCANMAIN_BBAR_ACTIVITYLOG_TT   526
#define IDS_SCANMAIN_BBAR_QUARANTINE_TT    527
#define IDS_SCANMAIN_BBAR_ONRAMP_TT        528


#define IDS_COMMDLG_TITLE_SCAN_FILE         768 /* ERR_NUM(  3,   0) */
#define IDS_COMMDLG_SCAN_BUTTON 	    769	/* ERR_NUM(  3,   1) */
#define IDS_COMMDLG_SCANFILE_PROGRAMS 	    770	/* ERR_NUM(  3,   2) */
#define IDS_COMMDLG_SCANFILE_ALLFILES 	    771	/* ERR_NUM(  3,   3) */
#define IDS_TITLE_DIRSCAN    	    772	/* ERR_NUM(  3,   4) */
#define IDS_TITLE_DIRSCAN_DESCRIPTION 	    773	/* ERR_NUM(  3,   5) */
#define IDS_TITLE_DIRSCAN_OK 	    774	/* ERR_NUM(  3,   6) */
#define IDS_COMMDLG_SCANFILE_ZIPFILES 	    775	/* ERR_NUM(  3,   7) */

#define IDS_DIRECTORYNOTINOC 	   1024	/* ERR_NUM(  4,   0) */
#define IDS_FILENOTINEXTLIST 	   1025	/* ERR_NUM(  4,   1) */
#define IDS_COMMDLG_TITLE_INOC 	   1026	/* ERR_NUM(  4,   2) */
#define IDS_COMMDLG_TITLE_UNINOC 	   1027	/* ERR_NUM(  4,   3) */
#define IDS_INOCFILE_OK      	   1028	/* ERR_NUM(  4,   4) */
#define IDS_INOCFILE_NOK     	   1029	/* ERR_NUM(  4,   5) */
#define IDS_UNINOCFILE_OK    	   1030	/* ERR_NUM(  4,   6) */
#define IDS_UNINOCFILE_NOK   	   1031	/* ERR_NUM(  4,   7) */
#define IDS_FILEISINOCULATED 	   1032	/* ERR_NUM(  4,   8) */
#define IDS_FILEISNOTINOCULATED 	   1033	/* ERR_NUM(  4,   9) */
#define IDS_VIRUSINFILE_INOC 	   1034	/* ERR_NUM(  4,  10) */
#define IDS_VIRUSINFILE_UNINOC 	   1035	/* ERR_NUM(  4,  11) */
#define IDS_NODATFILE        	   1036	/* ERR_NUM(  4,  12) */
#define IDS_INOCDBCREATE     	   1037	/* ERR_NUM(  4,  13) */
#define IDS_GARBAGEFILENAME                1038 /* ERR_NUM(  4,  14) */
#define IDS_GARBAGEFOLDER                  1039 /* ERR_NUM(  4,  15) */
#define IDS_GARBAGETHING                   1040 /* ERR_NUM(  4,  16) */
#define IDS_NO_FOLDER_ACCESS               1041 /* ERR_NUM(  4,  17) */
#define IDS_INVALID_FOLDER_PATH                  1042

#define IDS_STATUS_SCAN_SELECTED 	   1286	/* ERR_NUM(  5,   6) */
#define IDS_STATUS_SCAN_DIRECTORY 	   1287	/* ERR_NUM(  5,   7) */
#define IDS_STATUS_SCAN_FILE               1288 /* ERR_NUM(  5,   8) */
#define IDS_STATUS_SCAN_EXIT               1289 /* ERR_NUM(  5,   9) */
#define IDS_STATUS_TOOLS_OPTIONS           1290 /* ERR_NUM(  5,  15) */
#define IDS_STATUS_TOOLS_SCHEDULER         1291 /* ERR_NUM(  5,  10) */
#define IDS_STATUS_TOOLS_INOCULATE         1292 /* ERR_NUM(  5,  11) */
#define IDS_STATUS_TOOLS_VIRUSLIST         1293 /* ERR_NUM(  5,  12) */
#define IDS_STATUS_TOOLS_ONRAMP            1294 /* ERR_NUM(  5,  13) */
#define IDS_STATUS_TOOLS_ACTIVITYLOG       1295 /* ERR_NUM(  5,  14) */
#define IDS_STATUS_TOOLS_QUARANTINE        1296 /* ERR_NUM(  5,  15) */
#define IDS_STATUS_HELP_ABOUT              1297 /* ERR_NUM(  5,  15) */
#define IDS_STATUS_HELP_INFODESK 	       1298	/* ERR_NUM(  5,  16) */

#define IDS_FILTER_KNOWN     	   1536	/* ERR_NUM(  6,   0) */
#define IDS_FILTER_UNKNOWN   	   1537	/* ERR_NUM(  6,   1) */
#define IDS_FILTER_INOCHANGE 	   1538	/* ERR_NUM(  6,   2) */
#define IDS_FILTER_VIRUSLIKE 	   1539	/* ERR_NUM(  6,   3) */
#define IDS_FILTER_STARTEND  	   1540	/* ERR_NUM(  6,   4) */
#define IDS_FILTER_VIRLIST   	   1541	/* ERR_NUM(  6,   5) */
#define IDS_FILTER_QUARANTINE      1542 /* ERR_NUM(  6,   6) */
#define IDS_FILTER_ACCESSDENIED    1543 /* ERR_NUM(  6,   7) */

#define IDS_TEMPLATE_VIRLISTCHANGE 	   1548	/* ERR_NUM(  6,  12) */
#define IDS_TEMPLATE_INOCHANGE 	   1549	/* ERR_NUM(  6,  13) */
#define IDS_TEMPLATE_KNOWNVIRUS 	   1550	/* ERR_NUM(  6,  14) */
#define IDS_TEMPLATE_UNKNOWNVIRUS 	   1551	/* ERR_NUM(  6,  15) */
#define IDS_TEMPLATE_STARTEND 	   1552	/* ERR_NUM(  6,  16) */
#define IDS_TEMPLATE_VIRUSLIKE 	   1553	/* ERR_NUM(  6,  17) */
#define IDS_TEMPLATE_QUARANTINE	   1554	/* ERR_NUM(  6,  18) */
#define IDS_TEMPLATE_ACCESSDENIED  1555	/* ERR_NUM(  6,  19) */

#define IDS_STATUS_DEFAULT   	   1793	/* ERR_NUM(  7,   1) */

#define IDS_NAVNET_APPCAPTIONFMT 	   2048	/* ERR_NUM(  8,   0) */
#define IDS_NAVNET_FILEUNTITLED 	   2049	/* ERR_NUM(  8,   1) */
#define IDS_NAVNET_OPENFILETITLE           2050 /* ERR_NUM(  8,   2) */
#define IDS_NAVNET_SAVEFILEASTITLE 	   2051	/* ERR_NUM(  8,   3) */
#define IDS_NAVNET_SAVECHANGES 	   2052	/* ERR_NUM(  8,   4) */
#define IDS_NAVNET_SAVECHANGESNOINI 	   2053	/* ERR_NUM(  8,   5) */
#define IDS_NAVNET_CREATEINI 	   2054	/* ERR_NUM(  8,   6) */
#define IDS_NAVNET_SETUP_FILTER 	   2055	/* ERR_NUM(  8,   7) */
#define IDS_NAVNET_SETUP_EXTENSION 	   2056	/* ERR_NUM(  8,   8) */

#define IDS_NAVOPTS_CATEGORY               2304 /* ERR_NUM(  9,   0) */
#define IDS_NAVOPTS_CPL_PRODUCT 	   2305	/* ERR_NUM(  9,   1) */
#define IDS_NAVOPTS_CPL_TOPIC              2306 /* ERR_NUM(  9,   2) */
#define IDS_NAVOPTS_CPL_DLL_FILTER         2308 /* ERR_NUM(  9,   4) */

#define IDS_NAVOPTS_CPL_NAVSTART_ERR    2310
#define IDS_NAVOPTS_CPL_NETWORKTOPIC_3  2311

#define IDS_NAVOPTS_UPDATE_FAILED       2312

#define IDS_STATUS_AP_ENABLED                   2400
#define IDS_STATUS_AP_DISABLED                  2401
#define IDS_STATUS_AP_BUTTON_ENABLE             2402
#define IDS_STATUS_AP_BUTTON_DISABLE            2403
#define IDS_STATUS_QUARANTINE_ONE               2404
#define IDS_STATUS_QUARANTINE_MULTIPLE          2405
#define IDS_STATUS_DEFINITIONS                  2406
#define IDS_STATUS_NEW_DEFS                     2407
#define IDS_INFOTITLE_DEFINITIONS               2408
#define IDS_INFOTITLE_QUARANTINE                2409
#define IDS_STATUS_AP_UNAVAILABLE               2410

#define IDS_A                	   2560	/* ERR_NUM( 10,   0) */
#define IDS_L                	   2561	/* ERR_NUM( 10,   1) */
#define IDS_BO               	   2562	/* ERR_NUM( 10,   2) */
#define IDS_MEM              	   2563	/* ERR_NUM( 10,   3) */
#define IDS_M                	   2564	/* ERR_NUM( 10,   4) */
#define IDS_S                	   2565	/* ERR_NUM( 10,   5) */
#define IDS_B                	   2566	/* ERR_NUM( 10,   6) */
#define IDS_O                	   2567	/* ERR_NUM( 10,   7) */
#define IDS_DEFAULT          	   2568	/* ERR_NUM( 10,   8) */
#define IDS_NOSUBS           	   2569	/* ERR_NUM( 10,   9) */
#define IDS_MEMHANDLE        	   2571	/* ERR_NUM( 10,  10) */
#define IDS_DOWNLOAD         	   2572	/* ERR_NUM( 10,  11) */
#define IDS_CPULOAD          	   2573	/* ERR_NUM( 10,  12) */
#define IDS_SAGERUN          	   2574	/* ERR_NUM( 10,  13) */
#define IDS_SAGESET          	   2575	/* ERR_NUM( 10,  14) */
#define IDS_UILAUNCH         	   2576	/* ERR_NUM( 10,  15) */
#define IDS_HIMEM                  2577 /* ERR_NUM( 10,  16) */
#define IDS_NORESULTS              2578
#define IDS_VISIBLE                2579

#define IDS_NONAVBIN         	   2816	/* ERR_NUM( 11,   0) */
#define IDS_NODEFS           	   2817	/* ERR_NUM( 11,   1) */
#define IDS_PROTECTEDOPTIONS 	   2818	/* ERR_NUM( 11,   2) */
#define IDS_NOTFORWINNT            2819	/* ERR_NUM( 11,   3) */

#define IDS_NAVWNT_SCHEDCMDLINE    2821 /* ERR_NUM( 11,   5) */
#define IDS_NAVWNT_SCHEDCMDFMT     2822 /* ERR_NUM( 11,   6) */
#define IDS_NAVWNT_LOGOUTPROMPT    2823 /* ERR_NUM( 11,   7) */
#define IDS_NAVWNT_LOGOUTDURINGSCAN 2824    /* ERR_NUM( 11,   8) */
#define IDS_NAVWNT_LOGOUTAFTERSCAN  2825    /* ERR_NUM( 11,   9) */

#define IDS_SCHEDULERNT             2827 /* ERR_NUM( 12,   1) */
#define IDS_LIVEUPDATE              2828
#define IDS_QUARANTINE_CONSOLE      2829

#define IDS_NAVW_SCANNER_NETSCAPE	2830
#define IDS_NAVW_SCANNER_NETSCAPE_TAG	2831
#define IDS_NAVW_INTERNET           2832

#define IDS_RUNENVIRONMENT          2835

#ifdef NAV_PREVIEW
  #define IDS_LICENSEEXPIRED          2860
#endif

#define IDS_SUPPORT_HELP_FILE       2861
