// Copyright 1998 Symantec, Peter Norton Product Group
//*****************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navoptx.h_v   1.1   06 Oct 1998 15:36:04   tcashin  $
//
// Description:
//      This include file strings used when manipulating NAVOPTX.DAT.
//      
//      NAVOPTX.DAT is a file to save and read extended options.
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/navoptx.h_v  $
// 
//    Rev 1.1   06 Oct 1998 15:36:04   tcashin
// Added do not translate comments.
// 
//    Rev 1.0   06 Oct 1998 13:52:52   tcashin
// Initial revision.
//*****************************************************************************

//*****************************************************************************
// Extended Options File Name
//*****************************************************************************
#define NAVOPTX_FILE                    "\\NAVOPTX.DAT"     // Do Not Translate


//*****************************************************************************
// Defines for section name strings
//*****************************************************************************
#define NAVOPTX_SECT_ACTLOG             "Activity Log"      // Do Not Translate
#define NAVOPTX_SECT_SCANOPTS           "Scan Options"      // Do Not Translate
#define NAVOPTX_SECT_EXTENSIONS		"File Extensions"   // Do Not Translate

//*****************************************************************************
// Defines for key strings
//*****************************************************************************
#define NAVOPTX_ACTLOG_ACCESSDENIED     "LogAccessDenied"   // Do Not Translate
#define NAVOPTX_SCAN_NOEXT_FILES        "ScanNoExtFiles"    // Do Not Translate
#define NAVOPTX_USER_DEFINED_EXTENSIONS "UserExtensions"    // Do Not Translate
#define NAVOPTX_DEFAULT_EXTENSIONS      "DefaultExtensions" // Do Not Translate

//*****************************************************************************
// Extension defines
//*****************************************************************************
#define MAX_EXTENSION_LIST_SIZE		257    // Size of buffer, not # of exts

