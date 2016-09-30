// Copyright 1993 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/INCLUDE/VCS/navw_hlp.h_v   1.2   26 Jun 1998 17:41:42   mdunn  $
// 
// Description:                                                         
//      Contains the help ids for NAVW and NAVNETW.
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/navw_hlp.h_v  $
// 
//    Rev 1.2   26 Jun 1998 17:41:42   mdunn
// Now using navwhlp5.h for help IDs.
// 
//    Rev 1.1   24 Apr 1997 18:39:40   JBRENNA
// Tesla port: KSACKIN r1.1
// Added HowTo links for the Password change and verify dialogs.  These were
// taken from the Hydra codebase.
// 
//    Rev 1.0   06 Feb 1997 20:51:16   RFULLER
// Initial revision
// 
//    Rev 1.2   11 Jul 1996 12:46:50   PVENKAT
// Removed 5 'How To' help Ids for NT platform.
// 
// 
//    Rev 1.1   26 Jun 1996 19:18:34   DALLEE
// LuigiPlus port.
// 
//    Rev 1.0   30 Jan 1996 15:47:26   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:50   BARRY
// Initial revision.
// 
//    Rev 1.25   06 Nov 1995 17:49:12   GWESTER
// Removed nnt_xhlp.h and Gargoyle-specific header; we will instead split
// the help functionality by using conditional compilation inside the
// navwhlp.h file itself.
// 
//    Rev 1.24   06 Nov 1995 17:22:34   GWESTER
// Added nnt_xhlp.h to NT Gargoyle.
// 
//    Rev 1.23   06 Nov 1995 14:56:52   GWESTER
// For NT Gargolye, changed generated IDs file from navwhlp.h to navwnt.h .
// 
//    Rev 1.22   25 Oct 1995 15:47:02   YVANG
// Added 'HowTo' ID for password.
// 
//    Rev 1.21   07 Jul 1995 17:12:32   BARRY
// Added HowTo... IDs. (Gary)
// 
//    Rev 1.20   16 Mar 1995 19:55:34   AMATIC
// Added "Howto" for Virlist
// navw_hlp.h -> S:\include\vcs\navw_hlp.h_v
// Enter description of changes made.
// 
//    Rev 1.19   16 Mar 1995 13:11:28   AMATIC
// More "HowTo's".
// navw_hlp.h -> S:\include\vcs\navw_hlp.h_v
// Enter description of changes made.
// 
//    Rev 1.18   16 Mar 1995 12:47:12   AMATIC
// Added more "HowTo" string defs.
// navw_hlp.h -> S:\include\vcs\navw_hlp.h_v
// Enter description of changes made.
// 
//    Rev 1.17   15 Mar 1995 21:12:40   AMATIC
// Added new "howTo" CH ids.
// 
// navw_hlp.h -> S:\include\vcs\navw_hlp.h_v
// Enter description of changes made.
// 
//    Rev 1.16   14 Mar 1995 18:33:32   AMATIC
// Added "HowTo" headers.
// 
// navw_hlp.h -> S:\include\vcs\navw_hlp.h_v
// Enter description of changes made.
// 
//    Rev 1.15   14 Mar 1995 12:51:44   amatic
// 
//    Rev 1.14   14 Mar 1995 11:35:46   AMATIC
// Included "HowtoHlp.H"
// 
//    Rev 1.13   10 Feb 1995 14:18:00   AMATIC
// Added default help id's for missing help strings.
// 
//    Rev 1.12   07 Feb 1995 14:54:54   AMATIC
// No change.
// 
//    Rev 1.11   31 Jan 1995 18:55:44   mfallen
// 
//    Rev 1.10   31 Jan 1995 12:26:22   MFALLEN
// No change.
// 
//    Rev 1.9   31 Jan 1995 12:23:14   MFALLEN
// 
//    Rev 1.8   20 Jan 1995 15:28:06   MFALLEN
// Help fixes.
// 
//    Rev 1.7   20 Jan 1995 14:36:58   MFALLEN
// Scanner options prompt.
// 
//    Rev 1.6   16 Jan 1995 17:35:58   DALLEE
// Changed OPTONS typo to OPTIONS.
// (Hopefully) No one was using this ID yet.
//***************************************************************************

//#ifndef _NAVW_HLP_H
//#define _NAVW_HLP_H
                                        //------------------------------
                                        // Required for missing help id's
                                        //------------------------------

  /*  Common constant for Help string which has not been defined yet: */                                

#define IDH_MISSING_HELP IDH_NAVW_AP_HELP_BTN 
#define IDH_COMBOBOX     0 

                                        //------------------------------
                                        // Menu structure
                                        //------------------------------
#define HELP_MENU_SCAN_DRIVES           1
#define HELP_MENU_SCAN_DIR              2
#define HELP_MENU_SCAN_FILE             3
#define HELP_MENU_SCAN_EXIT             4

#define HELP_MENU_TOOLS_OPTIONS         5
#define HELP_MENU_TOOLS_SCHEDULER       6
#define HELP_MENU_TOOLS_INOCULATION     7
#define HELP_MENU_TOOLS_VIRUS_LIST      8

#define HELP_MENU_REPORTS_ACT_LOG       9
#define HELP_MENU_REPORTS_INF_REP       10

#define HELP_MENU_HELP_CONTENTS         11

#ifdef NDW_SPECIFIC_HELP
    #define HELP_MENU_HELP_SEARCH       17
#else
    #define HELP_MENU_HELP_COMMANDS     12
    #define HELP_MENU_HELP_PROCEDURES   13
#endif

#define HELP_MENU_HELP_QUICKHELP        14
#define HELP_MENU_HELP_USING_HELP       15
#define HELP_MENU_HELP_ABOUT            16
                
                                        //------------------------------
                                        // Menu structure (NAVNETW)
                                        //------------------------------
#define HELP_MENU_FILE_NEW              20
#define HELP_MENU_FILE_OPEN             21
#define HELP_MENU_FILE_SAVE             22
#define HELP_MENU_FILE_SAVEAS           23
#define HELP_MENU_FILE_EXIT             24

                                        //------------------------------
                                        // Main Dialog
                                        //------------------------------
#define HELP_DLG_MAIN                   100

                                        //------------------------------
                                        // Control Panel Dlgs
                                        //------------------------------
#define HELP_DLG_CPL_ACTIVITY           200
#define HELP_DLG_LOGFILE_BROWSE         201

#define HELP_DLG_CPL_ALERTS             205
#define HELP_DLG_ALERT_OTHERS           206
#define HELP_DLG_ALERT_NETWORK_BROWSE   207

#define HELP_DLG_CPL_EXCLUSIONS         210
#define HELP_DLG_ADD_EXCLUSION          211
#define HELP_DLG_ADD_EXCLUSION_BROWSE   212

#define HELP_DLG_CPL_GENERAL            215

#define HELP_DLG_CPL_INOCULATION        220
#define HELP_DLG_INOC_BROWSE            221

#define HELP_DLG_CPL_PASSWORD           225
#define HELP_DLG_SET_PASSWORD           226
#define HELP_DLG_VERIFY_PASSWORD        227

#define HELP_DLG_CPL_ADMIN              230
#define HELP_DLG_NETDIR_BROWSE          231
#define HELP_DLG_BEFORE                 232
#define HELP_DLG_BEFORE_BROWSE          233
#define HELP_DLG_USER                   234
#define HELP_DLG_WINDOWS                235

#define HELP_DLG_CPL_SYSMONITOR         240
#define HELP_DLG_SYSMON_ADVANCED        241
#define HELP_DLG_STARTUP                242
#define HELP_DLG_SENSOR                 243

#define HELP_DLG_CPL_SCANNER            250
#define HELP_DLG_CPL_SCANNER_ADVANCED   251

                                        //------------------------------
                                        // Additional help hooks for
                                        // the Bankers Trust mods.
                                        //------------------------------
#define HELP_DLG_CPL_MONITOR                252
#define HELP_DLG_REGISTERED_FILES_BROWSE    253
#define HELP_DLG_REGISTERED_FILES           254
#define HELP_DLG_ADD_FILE_TEMPLATE          255
#define HELP_DLG_EDIT_FILE_TEMPLATE         256

                                        //------------------------------
                                        // Activity Log dialogs
                                        //------------------------------
#define HELP_DLG_ACTLOG                 260
#define HELP_DLG_ACTLOG_CLEAR           261
#define HELP_DLG_ACTLOG_FILTER          262

                                        //------------------------------
                                        // Infection Report dlgs
                                        //------------------------------
#define HELP_DLG_INF_REPORT             310
#define HELP_DLG_INF_REPORT_FILTER      311

                                        //------------------------------
                                        // Virus List dlgs
                                        //------------------------------
#define HELP_DLG_VIRUS_LIST             320
#define HELP_DLG_VIRUS_DELETE           321
#define HELP_DLG_VIRUS_INFO             322

                                        //------------------------------
                                        // Inoculation dlg
                                        //------------------------------
#define HELP_DLG_INOCULATE              330

                                        //------------------------------
                                        // Extensions dlg
                                        //------------------------------
#define HELP_DLG_EXTENSIONS             340
#define HELP_DLG_ADD_EXTENSIONS         341

                                        //------------------------------
                                        // Print dlg
                                        //------------------------------
#define HELP_DLG_PRINT                  350
#define HELP_DLG_OVERWRITE              351
#define HELP_DLG_PRINT_BROWSE           352

                                        //------------------------------
                                        // NAVWORK dlgs
                                        //------------------------------

#define HELP_DLG_INFO                   360
#define HELP_DLG_VIRUS_FOUND            361
#define HELP_DLG_ASK_ACTION             362  // Generic action (repair/delete..)
//#define HELP_DLG_ASK_REPAIR             362
//#define HELP_DLG_ASK_DELETE             363
//#define HELP_DLG_ASK_INOC               364
#define HELP_DLG_SCAN_RESULTS           365
#define HELP_DLG_SCAN_SUMMARY           366

                                        //------------------------------
                                        // Scan Dir/File dlgs
                                        //------------------------------
#define HELP_DLG_SCAN_DIR               400
#define HELP_DLG_SCAN_FILE              405

                                        //------------------------------
                                        // Open/Save Setup files (NAVNETW)
                                        //------------------------------
#define HELP_DLG_OPEN_SETUP             410
#define HELP_DLG_SAVE_SETUP             411

// #########################################################################
// ##
// ##               RIGHT CLICK HELP ID'S
// ##
// #########################################################################



#include "navwhlp5.h"                   // Generated Help IDs for W95 Luigi.
                                        // With conditional compilation,
                                        // this is also the header file
                                        // for NT Gargoyle.  

//
// Fname     : NAVWHLP.H -  HOWTO extension
//

// -------------------------------------------------------------------------
// For all IDHs and strings, not defined Yet by Help authors:
    #define IDH_HT_MISSING   312345
    #define MISSING_STRING   "HERE - STRING IS MISSING"
// -------------------------------------------------------------------------

// *****************************************************************************
//
//
//           All Help Compiler NAVW  "HowTo" - Entries - APP level
//
//
// *****************************************************************************


// *****************************************************************************
// PROJ - NAVW
// *****************************************************************************


    #define      IDH_HT_NAVW_MAINWIN_1            IDH_NAVW_PROC_USE_HELP 
    #define      IDH_HT_NAVW_MAINWIN_2            IDH_NAVW_PROC_USE_NAV
    #define      IDH_HT_NAVW_MAINWIN_3            IDH_NAVW_PROC_FIND_VIRUSES_ON_YOUR_COMPUTER
    #define      IDH_HT_NAVW_MAINWIN_4            IDH_NAVW_PROC_SET_UP_MAXIMUM_PROTECTION
    #define      IDH_HT_NAVW_MAINWIN_5            IDH_NAVW_PROC_INITIATE_SCANS 
    #define      IDH_HT_NAVW_MAINWIN_6            IDH_NAVW_PROC_RESPOND_TO_ALERTS
    #define      IDH_HT_NAVW_MAINWIN_7            IDH_NAVW_PROC_ALWAYS_SCAN_FLOPPIES 
    #define      IDH_HT_NAVW_MAINWIN_8            IDH_NAVW_PROC_KEEP_UP_WITH_NEW_VIRUSES
    #define      IDH_HT_NAVW_MAINWIN_9            IDH_NAVW_PROC_SCHEDULE_SCANS

// Virlist - is in a TASK instance:
    #define      IDH_HT_NAVW_VIRLIST_1            IDH_NAVW_PROC_UPDATE_THE_VIRUS_LIST
    #define      IDH_HT_NAVW_VIRLIST_2            IDH_NAVW_PROC_USE_THE_VIRUS_LIST
    #define      IDH_HT_NAVW_VIRLIST_3            IDH_NAVW_PROC_SEARCH_THE_VIRUS_LIST
    #define      IDH_HT_NAVW_VIRLIST_4            IDH_NAVW_PROC_DISPLAY_INFORMATION_ABOUT_A_VIRUS
    #define      IDH_HT_NAVW_VIRLIST_5            IDH_NAVW_PROC_REMOVE_VIRUS_DEFINITIONS

// VIRUS LIST - VIRUS INFORMATION DLG//
    #define      IDH_HT_NAVW_VIRLIST_VIRINFO_1    IDH_NAVW_PROC_USE_VIRUS_FOUND_INFORMATION



// *****************************************************************************
// PROJ - NAVOPTS
// *****************************************************************************
//                Help ID definition               HELP ENTRY ID
// -----------------------------------------------------------------------------
// SCANNER SETTINGS ....................................................
    #define      IDH_HT_OPTS_SCANNER_1            IDH_NAVW_PROC_USE_HELP                
    #define      IDH_HT_OPTS_SCANNER_2            IDH_NAVW_PROC_SELECT_WHICH_FILES_TO_SCAN
    #define      IDH_HT_OPTS_SCANNER_3            IDH_NAVW_PROC_CHOOSE_PROGRAM_FILES_TO_SCAN
    #define      IDH_HT_OPTS_SCANNER_4            IDH_NAVW_PROC_SELECT_PROC_RESPOND      
    #define      IDH_HT_OPTS_SCANNER_5            IDH_NAVW_PROC_SET_GENERAL_SCANNING_OPTIONS
    #define      IDH_HT_OPTS_SCANNER_6            IDH_NAVW_PROC_SCHEDULE_SCANS
    #define      IDH_HT_OPTS_SCANNER_7            IDH_NAVW_PROC_ALWAYS_SCAN_FLOPPIES               
    #define      IDH_HT_OPTS_SCANNER_8            IDH_NAVW_PROC_SET_UP_MAXIMUM_PROTECTION               
    #define      IDH_HT_OPTS_SCANNER_9            IDH_NAVW_PROC_USE_NAV


// SCANNER - ADVANCED ..................................................    
    #define      IDH_HT_OPTS_SCANADV_1            IDH_NAVW_PROC_SCAN_NETWORKS
    #define      IDH_HT_OPTS_SCANADV_2            IDH_NAVW_PROC_ALWAYS_SCAN_FLOPPIES
    #define      IDH_HT_OPTS_SCANADV_3            IDH_HT_MISSING

                                                  
// SCANNER -  FILE EXTENSIONS ADD/EDIT .................................  
    #define      IDH_HT_OPTS_FILEEXT_1            IDH_NAVW_PROC_CHOOSE_PROGRAM_FILES_TO_SCAN           
    #define      IDH_HT_OPTS_FILEEXT_2            IDH_HT_MISSING
    #define      IDH_HT_OPTS_FILEEXT_3            IDH_HT_MISSING


// AUTOPROTECT .........................................................
// Removed the following 5 cases.
#ifndef  NAVWNT
    #define      IDH_HT_OPTS_APROTCT_1            IDH_NAVW_PROC_DETERMINE_HOW_AUOTMATIC_PROTECTION_WORKS_AT_STARTUP         
    #define      IDH_HT_OPTS_APROTCT_2            IDH_NAVW_PROC_AP_SELECT_WHICH_FILES_TO_SCAN
    #define      IDH_HT_OPTS_APROTCT_3            IDH_NAVW_PROC_AP_SELECT_WHAT_YOUR_OPTIONS_ARE_WHEN_A_VIRUS_IS_FOUND
    #define      IDH_HT_OPTS_APROTCT_4            IDH_NAVW_PROC_MONITOR_FOR_VIRUS_LIKE_ACTIVITIES
    #define      IDH_HT_OPTS_APROTCT_5            IDH_NAVW_PROC_MONITOR_FOR_UNKNOWN_VIRUSES
#endif
    #define      IDH_HT_OPTS_APROTCT_6            IDH_NAVW_PROC_SET_UP_MAXIMUM_PROTECTION        

// AutoProtect  ADVANCED ...............................................
    #define      IDH_HT_OPTS_APADVCD_1            IDH_NAVW_PROC_ALWAYS_SCAN_FLOPPIES
    #define      IDH_HT_OPTS_APADVCD_2            IDH_NAVW_PROC_SET_UP_MAXIMUM_PROTECTION 
    #define      IDH_HT_OPTS_APADVCD_3            IDH_HT_MISSING  
    #define      IDH_HT_OPTS_APADVCD_4            IDH_HT_MISSING 

//  SENSOR ............................................................. 
    #define      IDH_HT_OPTS_SENSORx_1            IDH_NAVW_PROC_MONITOR_FOR_VIRUS_LIKE_ACTIVITIES
    #define      IDH_HT_OPTS_SENSORx_2            IDH_NAVW_PROC_SET_UP_MAXIMUM_PROTECTION        


// EXLUSIONS LIST - SETTINGS......................22070................... 
    #define      IDH_HT_OPTS_EXCLUDE_1            IDH_NAVW_PROC_EDIT_THE_EXCLUSION_LIST           
    #define      IDH_HT_OPTS_EXCLUDE_2            IDH_NAVW_PROC_DELETE_AN_EXCLUSION           

// Exclusions - ADD AXCLUSION - Same as Exlusion List Dlg................ 
    #define      IDH_HT_OPTS_EXCLADD_1            IDH_HT_OPTS_EXCLUDE_1          
    #define      IDH_HT_OPTS_EXCLADD_2            IDH_HT_OPTS_EXCLUDE_2           

// GENERAL SETTINGS ..............................22130................... 
    #define      IDH_HT_OPTS_GENERAL_1            IDH_NAVW_PROC_DECIDE_WHAT_TO_SCAN
    #define      IDH_HT_OPTS_GENERAL_2            IDH_NAVW_PROC_DECIDE_WHETHER_TO_BACK_UP


// INOCULATION....................................22100................... 
    #define      IDH_HT_OPTS_INOCULS_1            IDH_NAVW_PROC_SELECT_WHICH_FILES_TO_INOCULATE
    #define      IDH_HT_OPTS_INOCULS_2            IDH_NAVW_PROC_SELECT_PROC_RESPOND_TO_INOC           
    #define      IDH_HT_OPTS_INOCULS_3            IDH_NAVW_PROC_USE_INOCULATION
    #define      IDH_HT_OPTS_INOCULS_4            IDH_HT_MISSING

// STARTUP - Settings ..... ......................22090................... 
    #define      IDH_HT_OPTS_STARTUP_1            IDH_NAVW_PROC_CHOOSE_WHAT_TO_SCAN_AT_STARTUP
    #define      IDH_HT_OPTS_STARTUP_2            IDH_NAVW_PROC_SELECT_WHICH_FILES_TO_INOCULATE           
    #define      IDH_HT_OPTS_STARTUP_3            IDH_NAVW_PROC_SELECT_PROC_RESPOND_TO_INOC
    #define      IDH_HT_OPTS_STARTUP_4            IDH_NAVW_PROC_USE_INOCULATION
    #define      IDH_HT_OPTS_STARTUP_5            IDH_HT_MISSING

                                                 
// ALERTS.........................................22110................... 
    #define      IDH_HT_OPTS_ALERTSx_1            IDH_HT_MISSING // Not defined in  help header: IDH_NAVW_PROC_MAKE_DECISIONS_ABOUT_ALERTS           
    #define      IDH_HT_OPTS_ALERTSx_2            IDH_HT_MISSING

// ACTIVITY LOG ..................................22120................... 
    #define      IDH_HT_OPTS_ACTYLOG_1            IDH_NAVW_PROC_DECIDE_WHAT_TO_LOG
    #define      IDH_HT_OPTS_ACTYLOG_2            IDH_HT_MISSING

// PASSWORD ......................................12240................... 
    #define      IDH_HT_OPTS_PASSWORD_1           IDH_NAVW_PROC_CHANGE_HOW_PASSWORD_WORKS
    #define      IDH_HT_OPTS_PASSCHAG_1           12256 //prs temporary IDH_NAVW_CHANGE_PASSWORD
    #define      IDH_HT_OPTS_PASSVERF_1           12257 // IDH_NAVW_VERIFY_PASSWORD



// *****************************************************************************
// PROJ - NAVWORK
// *****************************************************************************

// PROBLEMS FOUND  ...............................23020................... 
    #define      IDH_HT_WORK_PROBLMF_1            IDH_NAVW_PROC_WORK_WITH_SCAN_RESULTS
    #define      IDH_HT_WORK_PROBLMF_2            IDH_NAVW_PROC_RESPOND_IF_A_FILE_CANNOT_BE_REPAIRED
    #define      IDH_HT_WORK_PROBLMF_3            IDH_NAVW_PROC_RESPOND_TO_INOCULATION_CHANGES      
    #define      IDH_HT_WORK_PROBLMF_4            IDH_NAVW_PROC_CUSTOMIZE_RESPONSE_TO_PROBLEMS_FOUND

// SYSTEM INTEGRITY...............................23010................... 
    #define      IDH_HT_WORK_SYSINTG_1            IDH_NAVW_PROC_RESPOND_TO_THIS_ALERT


// VIRUS FOUND   .................................23030................... 
    #define      IDH_HT_WORK_VIRUSFD_1            IDH_NAVW_PROC_USE_VIRUS_FOUND_INFORMATION
    #define      IDH_HT_WORK_VIRUSFD_2            NAVW_PROC_REMOVE_VIRUSES_FROM_DOWNLOADED_FILES
    #define      IDH_HT_WORK_VIRUSFD_3            NAVW_PROC_REMOVE_VIRUSES_FROM_DOWNLOADED_COMPRESSED_FILES

// VIRUS FOUND INFO ..............................23040................... 
    #define      IDH_HT_WORK_VIRINFO_1            IDH_NAVW_PROC_USE_VIRUS_FOUND_INFORMATION

// SCAN RESULTS DLG ..............................23060................... 
    #define      IDH_HT_WORK_SCANRES_1            IDH_NAVW_PROC_WORK_WITH_SCAN_RESULTS

// DETAILS OF SCAN  ..............................23070................... 
    #define      IDH_HT_WORK_RESDTLS_1            IDH_NAVW_PROC_USE_SCAN_DETAILS_INFORMATION

// NAV QUESTIONS (n_questi.cpp)  .................230??................... 
    #define      IDH_HT_WORK_NQUESTI_1            IDH_NAVW_PROC_INOCULATE_ALL
    #define      IDH_HT_WORK_NQUESTI_2            IDH_NAVW_PROC_EXCLUDE_A_FILE


// *****************************************************************************
// PROJ - NAVALOG
// *****************************************************************************
// ACTIVITY LOG - main dlg .......................23010................... 
    #define      IDH_HT_ALOG_MAINDLG_1            IDH_NAVW_PROC_USE_THE_ACTIVITY_LOG
    #define      IDH_HT_ALOG_MAINDLG_2            IDH_NAVW_PROC_FILTER_THE_ACTIVITY_LOG
    #define      IDH_HT_ALOG_MAINDLG_3            IDH_NAVW_PROC_CLEAR_THE_ACTIVITY_LOG

// ACTIVITY LOG - CLEAR dlg ......................23010................... 
    #define      IDH_HT_ALOG_CLEARxx_1            IDH_NAVW_PROC_USE_THE_ACTIVITY_LOG
    #define      IDH_HT_ALOG_CLEARxx_2            IDH_NAVW_PROC_CLEAR_THE_ACTIVITY_LOG


// *****************************************************************************
// PROJ - NAVINOC
// *****************************************************************************

// INOCULATE dlg .........................................................
    #define      IDH_HT_INOC_INOCULT_1            IDH_NAVW_PROC_USE_INOCULATION
    #define      IDH_HT_INOC_INOCULT_2            IDH_NAVW_PROC_INOCULATE_FILES
    #define      IDH_HT_INOC_INOCULT_3            IDH_NAVW_PROC_UNINOCULATE_FILES
    #define      IDH_HT_INOC_INOCULT_4            IDH_NAVW_PROC_RESPOND_TO_AN_INOCULATION_ALERT
    
  

// *****************************************************************************
// PROJ - VIRLIST
// *****************************************************************************
// DELETE VIRUS DEFINITION........................23050................... 
    #define      IDH_HT_WORK_DELVDEF_1            23051       // comment           


// *****************************************************************************
// Windows Common Dialogs
// *****************************************************************************


// .EOF.
  


//#endif


