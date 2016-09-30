// Copyright 1994-1997 Symantec Corporation
//***************************************************************************
//
// $Header: /Atomic/NAVXUtil/stbexts.h 2     12/30/98 11:07a Cbenson $
//
// Description:
//
//***************************************************************************
// $Log: /Atomic/NAVXUtil/stbexts.h $
// 
// 2     12/30/98 11:07a Cbenson
// updated in 33g
// 
//    Rev 1.7   22 Sep 1998 22:22:34   tcashin
// Added IDs for additional activity log events.
// 
//    Rev 1.6   17 Sep 1998 14:41:10   tcashin
// Added a define for IDS_LOG_SCANEND_STATS.
// 
//    Rev 1.5   31 May 1997 15:42:28   KSACKIN
// Changed IDS_SCHEDULEDLLNAME to IDS_SCHEDULERNT.
//
//    Rev 1.4   21 May 1997 16:30:02   TIVANOV
// new defines for the long msg filename and the messagebox title
//
//    Rev 1.3   19 May 1997 14:56:52   TIVANOV
// new define for the quarantine file name
//
//    Rev 1.2   09 May 1997 16:57:42   TIVANOV
// new defines for the quarantine log
//
//    Rev 1.1   24 Apr 1997 16:20:22   JBRENNA
// Tesla port: Add IDS_ERR_NAVAP_OPEN_EX.
//
//***************************************************************************

#ifndef STBEXTS_H_INCLUDED
#define STBEXTS_H_INCLUDED

/*********************************************************************
** String IDs for file: navxutil.stb
*********************************************************************/
#define IDS_ERR_NAVTSR       	      0	/* ERR_NUM(  0,   0) */
#define IDS_RELOAD_TSR       	      1	/* ERR_NUM(  0,   1) */
#define IDS_UPDATINGTSR      	      2	/* ERR_NUM(  0,   2) */
#define IDS_NAV              	    256	/* ERR_NUM(  1,   0) */
#define IDS_DOS_EXECUTABLE   	    257	/* ERR_NUM(  1,   1) */
#define IDS_ERR_INVALIDEXT   	    258	/* ERR_NUM(  1,   2) */
#define IDS_ERR_EXTEXISTS    	    259	/* ERR_NUM(  1,   3) */
#define IDS_DEF_ACTIFILE     	    260	/* ERR_NUM(  1,   4) */
#define IDS_CONSOLETEMPLATE  	    261	/* ERR_NUM(  1,   5) */
#define IDS_ALERT_VIRUSALERT 	    262	/* ERR_NUM(  1,   6) */
#define IDS_ALERT_INOC_CHANGE 	    263	/* ERR_NUM(  1,   7) */
#define IDS_ALERT_END_SCAN   	    264	/* ERR_NUM(  1,   8) */
#define IDS_ALERT_VIRLIST_CHANGE 	    265	/* ERR_NUM(  1,   9) */
#define IDS_ALERT_BOOT_INOC_CHANGE 	    266	/* ERR_NUM(  1,  10) */
#define IDS_ALERT_MBOOT_INOC_CHANGE 	    267	/* ERR_NUM(  1,  11) */
#define IDS_LOG_SCANNER      	    268	/* ERR_NUM(  1,  12) */
#define IDS_LOG_MEMORYISINFECTED 	    269	/* ERR_NUM(  1,  13) */
#define IDS_LOG_FILEISINFECTED 	    270	/* ERR_NUM(  1,  14) */
#define IDS_LOG_FILEWASINFECTED 	    271	/* ERR_NUM(  1,  15) */
#define IDS_LOG_BOOTISINFECTED 	    272	/* ERR_NUM(  1,  16) */
#define IDS_LOG_BOOTWASINFECTED 	    273	/* ERR_NUM(  1,  17) */
#define IDS_LOG_ZIPFILEISINFECTED 	    274	/* ERR_NUM(  1,  18) */
#define IDS_LOG_FILEEXCLUDE  	    275	/* ERR_NUM(  1,  19) */
#define IDS_LOG_FILEINOC_CHANGE 	    276	/* ERR_NUM(  1,  20) */
#define IDS_LOG_FILEINOC_NEW 	    277	/* ERR_NUM(  1,  21) */
#define IDS_LOG_VIRLISTCHANGED 	    278	/* ERR_NUM(  1,  22) */
#define IDS_LOG_UNINOCULATED 	    279	/* ERR_NUM(  1,  23) */
#define IDS_LOG_UNINOCULATEFAILED 	    280	/* ERR_NUM(  1,  24) */
#define IDS_LOG_SCANEND      	    281	/* ERR_NUM(  1,  25) */
#define IDS_LOG_SCANEND_NONE 	    282	/* ERR_NUM(  1,  26) */
#define IDS_LOG_SCANINTERRUPTED 	    283	/* ERR_NUM(  1,  27) */
#define IDS_LOG_SCANINTERRUPTED_NONE 	    284	/* ERR_NUM(  1,  28) */
#define IDS_LOG_INTEG_CREATEFAILED 	    285	/* ERR_NUM(  1,  29) */
#define IDS_LOG_INTEG_MBCHANGED 	    286	/* ERR_NUM(  1,  30) */
#define IDS_LOG_INTEG_BOOTCHANGED 	    287	/* ERR_NUM(  1,  31) */
#define IDS_LOG_INTEG_SYSFILECHANGED 	    288	/* ERR_NUM(  1,  32) */
#define IDS_LOG_INTEG_NOTEXIST 	    289	/* ERR_NUM(  1,  33) */
#define IDS_LOG_INTEG_REINOCULATED 	    290	/* ERR_NUM(  1,  34) */
#define IDS_LOGACTION_INTEG_MBREPAIRED 	    291	/* ERR_NUM(  1,  35) */
#define IDS_LOGACTION_INTEG_MBREPAIRFAILED 	    292	/* ERR_NUM(  1,  36) */
#define IDS_LOGACTION_INTEG_BOOTREPAIRED 	    293	/* ERR_NUM(  1,  37) */
#define IDS_LOGACTION_INTEG_BOOTREPAIRFAILED 	    294	/* ERR_NUM(  1,  38) */
#define IDS_LOGACTION_INTEG_SYSFILEREPAIRED 	    295	/* ERR_NUM(  1,  39) */
#define IDS_LOGACTION_INTEG_SYSFILEREPAIRFAILED 	    296	/* ERR_NUM(  1,  40) */
#define IDS_LOGACTION_NOACTION 	    297	/* ERR_NUM(  1,  41) */
#define IDS_LOGACTION_BOOTISINFECTED 	    298	/* ERR_NUM(  1,  42) */
#define IDS_LOGACTION_FILEREPAIRED 	    299	/* ERR_NUM(  1,  43) */
#define IDS_LOGACTION_REPAIRFAILED 	    300	/* ERR_NUM(  1,  44) */
#define IDS_LOGACTION_DELETED 	    301	/* ERR_NUM(  1,  45) */
#define IDS_LOGACTION_DELETEFAILED 	    302	/* ERR_NUM(  1,  46) */
#define IDS_LOGACTION_BOOTREPAIRED 	    303	/* ERR_NUM(  1,  47) */
#define IDS_LOGACTION_BOOTNOTREPAIRED 	    304	/* ERR_NUM(  1,  48) */
#define IDS_LOGACTION_EXCLUDE_VIRUS 	    305	/* ERR_NUM(  1,  49) */
#define IDS_LOGACTION_EXCLUDE_INOC 	    306	/* ERR_NUM(  1,  50) */
#define IDS_LOGACTION_REINOCULATED 	    307	/* ERR_NUM(  1,  51) */
#define IDS_LOGACTION_REINOCULATEFAILED 	    308	/* ERR_NUM(  1,  52) */
#define IDS_LOGACTION_INOCULATED 	    309	/* ERR_NUM(  1,  53) */
#define IDS_LOGACTION_INOCULATEFAILED 	    310	/* ERR_NUM(  1,  54) */
#define IDS_LOGACTION_INOCULATEREPAIRED 	    311	/* ERR_NUM(  1,  55) */
#define IDS_LOGACTION_INOCULATEREPAIRFAILED 	    312	/* ERR_NUM(  1,  56) */
#define IDS_LOGACTION_NOTINOCULATED 	    313	/* ERR_NUM(  1,  57) */
#define IDS_LOGACTION_HALTCPU 	    314	/* ERR_NUM(  1,  58) */
#define IDS_DEFAULT          	    315	/* ERR_NUM(  1,  59) */
#define IDS_BOOT             	    316	/* ERR_NUM(  1,  60) */
#define IDS_FILE             	    317	/* ERR_NUM(  1,  61) */
#define IDS_BOOTRECORD       	    318	/* ERR_NUM(  1,  62) */
#define IDS_MEMORY           	    319	/* ERR_NUM(  1,  63) */
#define IDS_UNKNOWNVIRUS     	    320	/* ERR_NUM(  1,  64) */
#define IDS_BOOTACCESS       	    321	/* ERR_NUM(  1,  65) */
#define IDS_DRIVEACCESS      	    322	/* ERR_NUM(  1,  66) */
#define IDS_DRIVELOCKED      	    323	/* ERR_NUM(  1,  67) */
#define IDS_VXDAUTOLOAD_CAPTION 	    324	/* ERR_NUM(  1,  68) */
#define IDS_VXDAUTOLOAD_YES  	    325	/* ERR_NUM(  1,  69) */
#define IDS_VXDAUTOLOAD_YES_ASKLOADNOW 	    326	/* ERR_NUM(  1,  70) */
#define IDS_VXDAUTOLOAD_NO   	    327	/* ERR_NUM(  1,  71) */
#define IDS_VXDAUTOLOAD_NO_NOWLOADED 	    328	/* ERR_NUM(  1,  72) */
#define IDS_AUTOPROTECT      	    329	/* ERR_NUM(  1,  73) */
#define IDS_DRIVELOCKED_UNKNOWN_APP  333 	/* ERR_NUM(  1,  77) */
#define IDS_BOOTACCESS_PHYS               334 /* ERR_NUM(  1,  78) */
#define IDS_DRIVELOCKED_PHYS              335 /* ERR_NUM(  1,  79) */
#define IDS_DRIVELOCKED_PHYS_SYMSTAT_MBR  336 /* ERR_NUM(  1,  80) */

#define IDS_LOG_MBRISINFECTED             337 /* ERR_NUM(  1,  81) */
#define IDS_LOG_MBRWASINFECTED            338 /* ERR_NUM(  1,  82) */
#define IDS_LOGACTION_MBRISINFECTED       339 /* ERR_NUM(  1,  83) */
#define IDS_LOGACTION_MBRREPAIRED         340 /* ERR_NUM(  1,  84) */
#define IDS_LOGACTION_MBRNOTREPAIRED      341 /* ERR_NUM(  1,  85) */
#define IDS_HARD_DRIVE_LC                 342 /* ERR_NUM(  1,  86) */
#define IDS_NAVAPSVC_NAME                 343 /* ERR_NUM(  1,  87) */
#define IDS_HKEY_APPPATH                  344 /* ERR_NUM(  1,  88) */

                                     // These sysmtat IDs must remain together
                                     // The "unknown" id is calculated by
                                     // adding 1 to the regular id
#define IDS_DRIVELOCKED_SYMSTAT               345 /* ERR_NUM(  1,  89) */
#define IDS_DRIVELOCKED_SYMSTAT_UNKNOWN       346 /* ERR_NUM(  1,  90) */
#define IDS_DRIVELOCKED_SYMSTAT_MBR           347 /* ERR_NUM(  1,  91) */
#define IDS_DRIVELOCKED_SYMSTAT_MBR_UNKNOWN   348 /* ERR_NUM(  1,  92) */
#define IDS_DRIVELOCKED_SYMSTAT_BOOT          349 /* ERR_NUM(  1,  93) */
#define IDS_DRIVELOCKED_SYMSTAT_BOOT_UNKNOWN  350 /* ERR_NUM(  1,  94) */

#define IDS_DRIVELOCKED_REPAIR_SYMSTAT               351 /* ERR_NUM(  1,  95) */
#define IDS_DRIVELOCKED_REPAIR_SYMSTAT_UNKNOWN       352 /* ERR_NUM(  1,  96) */
#define IDS_DRIVELOCKED_REPAIR_SYMSTAT_MBR           353 /* ERR_NUM(  1,  97) */
#define IDS_DRIVELOCKED_REPAIR_SYMSTAT_MBR_UNKNOWN   354 /* ERR_NUM(  1,  98) */
#define IDS_DRIVELOCKED_REPAIR_SYMSTAT_BOOT          355 /* ERR_NUM(  1,  99) */
#define IDS_DRIVELOCKED_REPAIR_SYMSTAT_BOOT_UNKNOWN  356 /* ERR_NUM(  1, 100) */

#define IDS_SCHEDULER                     357 /* ERR_NUM(  1,  101) */
#define IDS_SCHEDULERNT                   358 /* ERR_NUM(  1,  102) */

#define IDS_NAVAP_SERVICE_NAME            359
#define IDS_ERR_NAVAP_OPEN                360
#define	IDS_ERR_NAVAP_START               361
#define	IDS_ERR_NAVAP_STOP                362
#define IDS_ERR_NAVAP_OPEN_EX             363

#define IDS_LOGACTION_QUARANTINED         364
#define IDS_LOGACTION_QUARANTINEFAILED    365
#define IDS_DEF_QUARANRINEFILE            366
#define IDS_DEF_LONGCUSTOMMESSAGE         367

#define IDS_LOG_SCANEND_STATS             368
#define IDS_LOG_SCANSTART                 369
#define IDS_LOG_ACCESSDENIED_DIR          370
#define IDS_LOG_ACCESSDENIED_FILE         371
#define IDS_LOG_SKIPDRIVE                 372


#endif // STBEXTS_H_INCLUDED
