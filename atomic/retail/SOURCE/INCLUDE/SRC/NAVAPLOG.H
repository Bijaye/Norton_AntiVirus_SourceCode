// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navaplog.h_v   1.11   21 May 1998 16:31:36   rpulint  $
//
// Description:
//      NAVAPLOG.H - String table file for Norton AntiVirus Auto-Protect
//      Agent containing the strings that go in the activity log for each
//      event that gets logged.  Use GENIDS utility to create the
//      global NAVAPLOG.H string table include file.
//
// NOTE (MUI IMPORTANTE):
//
//      DO ***NOT*** RUN GENIDS ***UNTIL*** YOU DELETE THE LINES AFTER
//      "DELETE BELOW THIS SECTION BEFORE RUNNING GENIDS"
//
//      ***AND***
//
//      TELL GENIDS TO ***APPEND*** THE EXISTING FILE.  ***DO NOT***
//      TELL IT TO OVERWRITE...
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/navaplog.h_v  $
// 
//    Rev 1.11   21 May 1998 16:31:36   rpulint
// Increased the size of szSystem[] in NAVAP_LOG from 15 to 15+2 to hold
// 15 chars + null char - Computer Name can contain 15 chars
// 
//    Rev 1.10   18 Mar 1998 17:40:56   DALLEE
// Argh! Put back defines for unknown and inoculation.
// Alert service is using these - may be getting messages from
// old guys. Will someone who is familiar/responsible for this
// mess clean up these interdependencies? Thank you.
// 
//    Rev 1.9   18 Mar 1998 16:35:32   DALLEE
// Regenerated string ID's after removing unknown and inoculation 
// auto-protect log messages.
// 
//    Rev 1.8   21 Aug 1997 14:59:36   DALLEE
// Last round of fixes to make old stuff compile for DOS again.
// 
//    Rev 1.7   21 Aug 1997 13:07:06   DALLEE
// Put back old log code alongside new for Alert Service compatibility.
// 
//    Rev 1.6   21 Aug 1997 00:04:36   DALLEE
// Re-worked NavApLogSpoolFileProcess() to work w/ new spool file written
// by driver. Also removed some old defines and prototypes used by the
// old scheme.
// 
//    Rev 1.5   19 May 1997 15:33:00   TIVANOV
// remove the function prototype from my last log
// 
//    Rev 1.4   19 May 1997 14:57:24   TIVANOV
// new function prototype to support the quarantine log file
// 
//    Rev 1.3   30 Apr 1997 12:48:36   JBRENNA
// Use N30_SIZE_VIRUSNAME instead of SIZE_VIRUSNAME because of AVENGE work.
// 
//    Rev 1.2   25 Apr 1997 15:56:08   JBRENNA
// Add define for MAX_COMPUTERNAME_LENGTH for non-Win32 platforms.
// 
//    Rev 1.1   24 Apr 1997 17:05:20   JBRENNA
// Tesla port:
//  TSmith r1.1:
// Added szSystem to NAVAP_LOG struct to facilitate having the system name in
// the NAV activity log.
// 
//  TSmith r1.2:
// Bracketed file with #if/#endif block.
// 
//  TSmith r1.1:
// Entered additional IDS_VIR_ACCT... string IDs via GENIDS.
// 
//    Rev 1.0   06 Feb 1997 20:51:54   RFULLER
// Initial revision
//***************************************************************************

#ifndef NAVAPLOG_H_INCLUDED
#define NAVAPLOG_H_INCLUDED

#include "n30type.h"


#define SPOOL_FILE_NAME         "~APSPOOL.TMP"

//***************************************************************************
// Struct is old!! Should remove after updating Alert service which still uses this.
// Activity Log Reporting Structure for VxD -> Agent Reporting
//***************************************************************************

// Ensure that this symbol is defined for the DOS platforms. Normally, this
// symbol is defined by MSVC in WINBASE.H. If you get this error, make sure
// our fake MAX_COMPUTERNAME_LENGTH is the same length as real one.

#ifdef MAX_COMPUTERNAME_LENGTH
    #if MAX_COMPUTERNAME_LENGTH != 15
        #error MAX_COMPUTERNAME_LENGTH is not 15 characters. See comment.
    #endif
#else
    #define MAX_COMPUTERNAME_LENGTH 15
#endif

typedef struct
    {
    WORD        wSize;                    // Size of this structure
    BYTE        byRecType;                // One of ALOG_FI_* in symalog.h
    DWORD       dwEncodedDate;            // Encoded date for entry
    DWORD       dwEncodedTime;            // Encoded time for entry
    WORD        wVirMessageID;            // Message ID to write to log
    WORD        wVirActionID;             // Action ID to write to log
    char        szVirus[N30_SIZE_VIRUSNAME];  // Name of virus
    char        szFile[SYM_MAX_PATH+1];     // File or drive for substitutions
    char        szProgram[SYM_MAX_PATH+1];  // Program doing bad things
    char        szUser[SYM_MAX_PATH];       // User associated with event
                                            // System event occurred at
//    char        szSystem[MAX_COMPUTERNAME_LENGTH];
    char        szSystem[MAX_COMPUTERNAME_LENGTH+2];

    } NAVAP_LOG, FAR * LPNAVAP_LOG, *PNAVAP_LOG;


//***************************************************************************
// Auto-Protect Activity Log APIs
//***************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

VOID SYM_EXPORT WINAPI NavApLogSpoolFileProcess ( // Spool file -> ActLog
    LPSTR       lpszSpoolFile);         // [IN] Spool file to update log with


// Old!! Should remove after updating Alert service which still uses this.
UINT SYM_EXPORT WINAPI NavApDoCommandActivityLog ( // Write act log for VxD
    LPNAVAP_LOG lpNavApLog);            // [IN] Log entry to write

#ifdef __cplusplus
}
#endif


//***************************************************************************
// DELETE BELOW THIS SECTION BEFORE RUNNING GENIDS!!!!!!!!!!!
//
// DON'T FORGET TO REPLACE THE #endif AT THE END OF THE FILE!!!
//
// NOTE: THESE IDs ARE USED IN BOTH THE NAVXUTIL AND ALERTSVC PROJECTS
// AND  M U S T  REMAIN THE SAME FOR BOTH IN ORDER FOR THE MESSAGE
// STRING RESOLUTION TO WORK PROPERLY!!!!!
//***************************************************************************

/*********************************************************************
** String IDs for file: navaplog.stb
*********************************************************************/
#define IDS_ALOG_APP_NAME    	  10240	/* ERR_NUM( 40,   0) */
#define IDS_VXD_MSG_LOADED   	  10241	/* ERR_NUM( 40,   1) */
#define IDS_VXD_MSG_UNLOADED 	  10242	/* ERR_NUM( 40,   2) */
#define IDS_VXD_MSG_ENABLED  	  10243	/* ERR_NUM( 40,   3) */
#define IDS_VXD_MSG_DISABLED 	  10244	/* ERR_NUM( 40,   4) */
#define IDS_VIR_MSG_IS_INFECTED 	  10496	/* ERR_NUM( 41,   0) */
#define IDS_VIR_MSG_WAS_INFECTED 	  10497	/* ERR_NUM( 41,   1) */
#define IDS_VIR_MSG_IS_UNKNOWN 	  10498	/* ERR_NUM( 41,   2) */
#define IDS_VIR_MSG_WAS_UNKNOWN 	  10499	/* ERR_NUM( 41,   3) */
#define IDS_VIR_MSG_INOC_CHANGED 	  10500	/* ERR_NUM( 41,   4) */
#define IDS_VIR_MSG_ATTRIB_CHANGED 	  10501	/* ERR_NUM( 41,   5) */
#define IDS_VIR_MSG_WRITEPROG 	  10502	/* ERR_NUM( 41,   6) */
#define IDS_VIR_MSG_WRITE_BOOT 	  10503	/* ERR_NUM( 41,   7) */
#define IDS_VIR_MSG_WRITE_MBR 	  10504	/* ERR_NUM( 41,   8) */
#define IDS_VIR_MSG_BOOT_IS_INFECTED 	  10505	/* ERR_NUM( 41,   9) */
#define IDS_VIR_MSG_BOOT_WAS_INFECTED 	  10506	/* ERR_NUM( 41,  10) */
#define IDS_VIR_MSG_MBR_IS_INFECTED 	  10507	/* ERR_NUM( 41,  11) */
#define IDS_VIR_MSG_MBR_WAS_INFECTED 	  10508	/* ERR_NUM( 41,  12) */
#define IDS_VIR_MSG_BOOT_INOC_CHANGE 	  10509	/* ERR_NUM( 41,  13) */
#define IDS_VIR_MSG_MBR_INOC_CHANGE 	  10510	/* ERR_NUM( 41,  14) */
#define IDS_VIR_MSG_FILE_NOT_INOC 	  10511	/* ERR_NUM( 41,  15) */
#define IDS_VIR_MSG_VIRUS_IN_MEMORY 	  10512	/* ERR_NUM( 41,  16) */
#define IDS_VIR_MSG_LOW_FORMAT 	  10513	/* ERR_NUM( 41,  17) */
#define IDS_VIR_ACT_NONE     	  10752	/* ERR_NUM( 42,   0) */
#define IDS_VIR_ACT_COMPUTER_SHUTDOWN 	  10753	/* ERR_NUM( 42,   1) */
#define IDS_VIR_ACT_DELETED  	  10754	/* ERR_NUM( 42,   2) */
#define IDS_VIR_ACT_NOT_DELETED 	  10755	/* ERR_NUM( 42,   3) */
#define IDS_VIR_ACT_REPAIRED 	  10756	/* ERR_NUM( 42,   4) */
#define IDS_VIR_ACT_NOT_REPAIRED 	  10757	/* ERR_NUM( 42,   5) */
#define IDS_VIR_ACT_QUARANTINED 	  10758	/* ERR_NUM( 42,   6) */
#define IDS_VIR_ACT_NOT_QUARANTINED 	  10759	/* ERR_NUM( 42,   7) */
#define IDS_VIR_ACT_ACCESS_DENIED 	  10760	/* ERR_NUM( 42,   8) */
#define IDS_VIR_ACT_KNOWN_EXCL 	  10761	/* ERR_NUM( 42,   9) */
#define IDS_VIR_ACT_KNOWN_NOT_EXCL 	  10762	/* ERR_NUM( 42,  10) */
#define IDS_VIR_ACT_UNKNOWN_EXCL 	  10763	/* ERR_NUM( 42,  11) */
#define IDS_VIR_ACT_UNKNOWN_NOT_EXCL 	  10764	/* ERR_NUM( 42,  12) */
#define IDS_VIR_ACT_INOCULATE_LIST_ADD 	  10765	/* ERR_NUM( 42,  13) */
#define IDS_VIR_ACT_REINOCULATED 	  10766	/* ERR_NUM( 42,  14) */
#define IDS_VIR_ACT_NOT_INOCULATED 	  10767	/* ERR_NUM( 42,  15) */
#define IDS_VIR_ACT_INOC_EXCLUDE 	  10768	/* ERR_NUM( 42,  16) */
#define IDS_VIR_ACT_INOC_NOT_EXCL 	  10769	/* ERR_NUM( 42,  17) */
#define IDS_VIR_ACT_ATTRIB_ALLOWED 	  10770	/* ERR_NUM( 42,  18) */
#define IDS_VIR_ACT_ATTRIB_NOT_ALLOWED 	  10771	/* ERR_NUM( 42,  19) */
#define IDS_VIR_ACT_ATTRIB_EXCL 	  10772	/* ERR_NUM( 42,  20) */
#define IDS_VIR_ACT_NOT_ATTRIB_EXCL 	  10773	/* ERR_NUM( 42,  21) */
#define IDS_VIR_ACT_ACTION_ALLOWED 	  10774	/* ERR_NUM( 42,  22) */
#define IDS_VIR_ACT_ACTION_NOT_ALLOWED 	  10775	/* ERR_NUM( 42,  23) */
#define IDS_VIR_ACT_WRITEPROG_EXCL 	  10776	/* ERR_NUM( 42,  24) */
#define IDS_VIR_ACT_WRITEPROG_NOT_EXCL 	  10777	/* ERR_NUM( 42,  25) */
#define IDS_VIR_ACT_WRITEBOOT_EXCL 	  10778	/* ERR_NUM( 42,  26) */
#define IDS_VIR_ACT_WRITEBOOT_NOT_EXCL 	  10779	/* ERR_NUM( 42,  27) */
#define IDS_VIR_ACT_WRITEMBR_EXCL 	  10780	/* ERR_NUM( 42,  28) */
#define IDS_VIR_ACT_WRITEMBR_NOT_EXCL 	  10781	/* ERR_NUM( 42,  29) */
#define IDS_VIR_ACT_BOOTREC_REPAIRED 	  10782	/* ERR_NUM( 42,  30) */
#define IDS_VIR_ACT_BOOTREC_NOT_REPAIRED 	  10783	/* ERR_NUM( 42,  31) */
#define IDS_VIR_ACT_MBR_REPAIRED 	  10784	/* ERR_NUM( 42,  32) */
#define IDS_VIR_ACT_MBR_NOT_REPAIRED 	  10785	/* ERR_NUM( 42,  33) */
#define IDS_VIR_ACT_LOW_FMT_EXCL 	  10786	/* ERR_NUM( 42,  34) */
#define IDS_VIR_ACT_LOW_FMT_NOT_EXCL 	  10787	/* ERR_NUM( 42,  35) */
#define IDS_VIR_ACT_SHUTDOWN_FAIL 	  10788	/* ERR_NUM( 42,  36) */
#define IDS_VIR_ACT_MOVE     	  10789	/* ERR_NUM( 42,  37) */
#define IDS_VIR_ACT_MOVE_FAIL 	  10790	/* ERR_NUM( 42,  38) */
#define IDS_VIR_ACT_RENAME   	  10791	/* ERR_NUM( 42,  39) */
#define IDS_VIR_ACT_RENAME_FAIL 	  10792	/* ERR_NUM( 42,  40) */
#define IDS_VIR_ACT_UNDEFINED 	  10793	/* ERR_NUM( 42,  41) */
#define IDS_VIR_ACT_BR_KNOWN_EXCL 	  10794	/* ERR_NUM( 42,  42) */
#define IDS_VIR_ACT_BR_KNOWN_NOT_EXCL 	  10795	/* ERR_NUM( 42,  43) */
#define IDS_VIR_ACT_MBR_KNOWN_EXCL 	  10796	/* ERR_NUM( 42,  44) */
#define IDS_VIR_ACT_MBR_KNOWN_NOT_EXCL 	  10797	/* ERR_NUM( 42,  45) */
#define IDS_USER_MSG_KNOWN_FILE_INFECTOR 	  11008	/* ERR_NUM( 43,   0) */
#define IDS_USER_MSG_KNOWN_MBR_INFECTOR 	  11009	/* ERR_NUM( 43,   1) */
#define IDS_USER_MSG_KNOWN_BR_INFECTOR 	  11010	/* ERR_NUM( 43,   2) */
#define IDS_ADMIN_MSG_KNOWN_FILE_INFECTOR 	  11011	/* ERR_NUM( 43,   3) */
#define IDS_ADMIN_MSG_KNOWN_MBR_INFECTOR 	  11012	/* ERR_NUM( 43,   4) */
#define IDS_ADMIN_MSG_KNOWN_BR_INFECTOR 	  11013	/* ERR_NUM( 43,   5) */

#endif // NAVAPLOG_H_INCLUDED
