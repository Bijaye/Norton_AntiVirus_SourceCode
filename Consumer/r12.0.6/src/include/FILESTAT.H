// Copyright 1993-1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/filestat.h_v   1.3   04 Jun 1998 10:57:18   mdunn  $
//
// Description:
//      Lookup table based on filestatus for message strings, and buttons
//      available.
//
// Contains:
//      FILESTATUSSTRUCT FileStatus[];
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/filestat.h_v  $
// 
//    Rev 1.3   04 Jun 1998 10:57:18   mdunn
// Fixed to build for DOS.
// 
//    Rev 1.2   01 Jun 1998 15:58:20   tcashin
// New file status for quarantined zip files.
// 
//    Rev 1.1   02 May 1997 11:23:24   TIVANOV
// new entries for the quarantine functionality; removed some legasy code; empty
// entires to keep in sync nonav enum for the filestatus entries
//
//    Rev 1.0   06 Feb 1997 20:51:32   RFULLER
// Initial revision
//
//    Rev 1.0   30 Jan 1996 15:47:40   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:12:04   BARRY
// Initial revision.
//
//    Rev 1.1   07 Jun 1995 10:28:40   BARRY
// Removed ZIP dependencies for DOS
//
//    Rev 1.0   31 Oct 1994 12:48:28   MFALLEN
// Initial revision.
//
//    Rev 1.11   18 Mar 1994 16:32:00   DALLEE
// Changed #ifdef BANKERS to #if defined(BANKERS) && defined(SYM_WIN)
// on registration file statuses since DOS scanner does not use these.
//************************************************************************

FILESTATUSSTRUCT FileStatus[] =
{
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_INFECTED
    IDS_FILESTATUS_INFECTED,            // File status string ID
    IDS_RAP_INFECTED,                   // Follow help string ID
    YES,                                // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_REPAIRED
    IDS_FILESTATUS_REPAIRED,            // File status string ID
    IDS_RAP_REPAIRED,                   // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_NOTREPAIRED
    IDS_FILESTATUS_NOTREPAIRED,         // File status string ID
    IDS_RAP_NOTREPAIRED,                // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_DELETED
    IDS_FILESTATUS_DELETED,             // File status string ID
    IDS_RAP_DELETED,                    // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_NOTDELETED
    IDS_FILESTATUS_NOTDELETED,          // File status string ID
    IDS_RAP_NOTDELETED,                 // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_EXCLUDE_INOCULATION
    IDS_FILESTATUS_EXCLUDED,            // File status string ID
    IDS_RAP_EXCLUDED_INOCULATION,       // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_EXCLUDE_VIRUSDETECTION
    IDS_FILESTATUS_EXCLUDED,            // File status string ID
    IDS_RAP_EXCLUDED_VIRUSDETECT,       // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_EXCLUDE_FAILED
    IDS_FILESTATUS_EXCLUDEFAILED,       // File status string ID
    IDS_RAP_EXCLUDE_FAILED,             // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATED
    IDS_FILESTATUS_INOCULATED,          // File status string ID
    IDS_RAP_INOCULATED,                 // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATIONFAILED
    IDS_FILESTATUS_INOCULATIONFAILED,   // File status string ID
    IDS_RAP_INOCULATEFAILED,            // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATEREPAIRED
    IDS_FILESTATUS_INOCULATEREPAIRED,   // File status string ID
    IDS_RAP_INOCREPAIRED,               // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATEREPAIR_FAILED
    IDS_FILESTATUS_INOCULATEREPAIRFAILED,// File status string ID
    IDS_RAP_INOCREPAIRFAILED,           // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATEDELETED
    IDS_FILESTATUS_DELETED,             // File status string ID
    IDS_RAP_INOCDELETED,                // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATEDELETE_FAILED
    IDS_FILESTATUS_NOTDELETED,          // File status string ID
    IDS_RAP_INOCNOTDELETED,             // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOC_CHANGED_DELETED
    IDS_FILESTATUS_DELETED,             // File status string ID
    IDS_RAP_INOCDELETED,                // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOC_CHANGED_NOTDELETED
    IDS_FILESTATUS_NOTDELETED,          // File status string ID
    IDS_RAP_INOCNOTDELETED,             // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_REINOCULATED
    IDS_FILESTATUS_REINOCULATED,        // File status string ID
    IDS_RAP_REINOCULATED,               // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_REINOCULATIONFAILED
    IDS_FILESTATUS_INOCULATIONFAILED,   // File status string ID
    IDS_RAP_INOCULATEFAILED,            // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_NOTINOCULATED
    IDS_FILESTATUS_NOTINOCULATED,       // File status string ID
    IDS_RAP_NOTINOCULATED,              // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    YES,                                // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_INOCULATEDCHANGED
    IDS_FILESTATUS_INOCULATECHANGED,    // File status string ID
    IDS_RAP_INOCULATEDCHANGED,          // Follow help string ID
    YES,                                // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    YES,                                // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_BOOTINFECTED
    IDS_FILESTATUS_BOOTINFECTED,        // File status string ID
    IDS_RAP_BOOTINFECTED,               // Follow help string ID
    YES,                                // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_BOOTREPAIRED
    IDS_FILESTATUS_BOOTREPAIRED,        // File status string ID
    IDS_RAP_BOOTREPAIRED,               // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_BOOTNOTREPAIRED
    IDS_FILESTATUS_BOOTNOTREPAIRED,     // File status string ID
    IDS_RAP_BOOTNOTREPAIRED,            // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_MASTERBOOTINFECTED
    IDS_FILESTATUS_MASTERBOOTINFECTED,  // File status string ID
    IDS_RAP_MASTERBOOTINFECTED,         // Follow help string ID
    YES,                                // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_MASTERBOOTREPAIRED
    IDS_FILESTATUS_MASTERBOOTREPAIRED,  // File status string ID
    IDS_RAP_MASTERBOOTREPAIRED,         // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_MASTERBOOTNOTREPAIRED
    IDS_FILESTATUS_MASTERBOOTNOTREPAIRED,// File status string ID
    IDS_RAP_MASTERBOOTNOTREPAIRED,      // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },


    {                                   // FILESTATUS_ERROR
    IDS_FILESTATUS_ERROR,               // File status string ID
    IDS_RAP_ERROR,                      // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_ERRORDELETED
    IDS_FILESTATUS_ERRORDELETED,        // File status string ID
    IDS_RAP_ERRORDELETED,               // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_ERRORNOTDELETED
    IDS_FILESTATUS_ERRORNOTDELETED,     // File status string ID
    IDS_RAP_ERRORNOTDELETED,            // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_ZIPINFECTED
    IDS_FILESTATUS_INFECTED,            // File status string ID
#ifdef SYM_WIN
    IDS_RAP_ZIPINFECTED,                // Follow help string ID
#else
    IDS_RAP_ERROR,                      // DOS Doesn't do ZIP
#endif
    YES,                                // Show Repair Button
    YES,                                // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    YES,                                // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },
    {                                   // FILESTATUS_NOT_USED
    0, 0, NO, NO, NO, NO, NO, NO
    },

    {                                   // FILESTATUS_QUARANTINE_INFECTED
    IDS_FILESTATUS_QUARANTINE_INFECTED, // File status string ID
    IDS_RAP_QUARANTINE,                 // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_INFECTED_FAILED
    IDS_FILESTATUS_QUARANTINE_INFECTED_FAILED, // File status string ID
    IDS_RAP_NOTQUARANTINE,              // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_NOTINOCULATED
    IDS_FILESTATUS_QUARANTINE_INFECTED,         // File status string ID
    IDS_RAP_QUARANTINE,                 // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_NOTINOCULATED_FAILED
    IDS_FILESTATUS_QUARANTINE_INFECTED_FAILED,  // File status string ID
    IDS_RAP_NOTQUARANTINE,              // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_INOCULATEDCHANGED
    IDS_FILESTATUS_QUARANTINE_INFECTED,         // File status string ID
    IDS_RAP_QUARANTINE,                 // Follow help string ID
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED
    IDS_FILESTATUS_QUARANTINE_INFECTED_FAILED,  // File status string ID
    IDS_RAP_NOTQUARANTINE,              // Follow help string ID
    NO,                                 // Show Repair Button
    YES,                                // Show Delete Button
    YES,                                // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    NO                                  // Show Information Button
    },

    {                                   // FILESTATUS_QUARANTINE_INFECTED_ZIP
    IDS_FILESTATUS_QUARANTINE_INFECTED, // File status string ID
#ifdef SYM_WIN
    IDS_RAP_QUARANTINE_ZIP,             // Follow help string ID
#else
    IDS_RAP_ERROR,                      // DOS doesn't do ZIPs
#endif
    NO,                                 // Show Repair Button
    NO,                                 // Show Delete Button
    NO,                                 // Show Exclude Button
    NO,                                 // Show Inoculate Button
    NO,                                 // show quarantine button
    YES                                 // Show Information Button
    }

};
