// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/options.h_v   1.8   26 May 1997 14:19:42   BGERHAR  $
//
// Description:
//      This include file defines the options structure.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/options.h_v  $
// 
//    Rev 1.8   26 May 1997 14:19:42   BGERHAR
// Fixed NAVOPT_DAT_CONFIG macro declaration
// 
//    Rev 1.7   23 May 1997 19:51:44   DBUCHES
// Moved quarantine and custom response stuff to NAVOPTS_DAT_CONFIG_BINARY.
// 
//    Rev 1.6   23 May 1997 15:40:00   RStanev
// Created NAVOPTS_DAT_CONFIGURATION_BINARY.
//
//    Rev 1.5   01 May 1997 15:40:14   JBRENNA
// Add include for Custom.h for CUSTOMRESPONSE.
//
//    Rev 1.4   01 May 1997 15:09:52   JBRENNA
// Add customAP and customScan for Custom Response. Checked in Tchavdar.
//
//    Rev 1.3   25 Apr 1997 20:42:42   JBRENNA
// 1. Removed obsolete BANKERS and NAVNETW #ifdefs.
// 2. Added "quarantineset" to the non-CREATE_NAVBIN options structure.
//
//    Rev 1.2   25 Apr 1997 12:04:16   JBRENNA
// #ifdef out the #pragma pack... statement for non-WIN32 platforms. The DOS
// platform specifically does not support this statement. In talking with
// TSmith, the #pragma was added for WinNT.
//
//    Rev 1.1   24 Apr 1997 18:52:42   JBRENNA
// Tesla port: TSmith r1.1:
// Added pragma to ensure struct packing is set to 1.
//
//    Rev 1.0   06 Feb 1997 20:51:22   RFULLER
// Initial revision
//
//    Rev 1.0   30 Jan 1996 15:47:34   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:58   BARRY
// Initial revision.
//
//    Rev 1.12   16 Oct 1995 18:24:02   JWORDEN
// Replace SectionSuffix structure with new elements
//
//    Rev 1.11   14 Sep 1995 09:31:24   JWORDEN
// Add typedef for SECTIONSUFFIX structure from navutil.h
//
//    Rev 1.10   05 Sep 1995 07:19:28   JWORDEN
// Change ifdef for NAVNET to NAVNETW
//
//    Rev 1.9   23 Jan 1995 17:39:54   DALLEE
//
//    Rev 1.8   15 Dec 1994 20:31:44   MFALLEN
// Removed previous change.
//
//    Rev 1.7   15 Dec 1994 20:29:16   MFALLEN
// Commented out netobj.h since none of this stuff works.
//
//    Rev 1.6   28 Nov 1994 13:48:44   RSTANEV
// Added PNAVOPTIONS type.
//
//    Rev 1.5   21 Nov 1994 17:13:32   MFALLEN
//
//    Rev 1.4   21 Nov 1994 17:12:18   MFALLEN
//
//    Rev 1.3   03 Nov 1994 12:00:52   MFALLEN
// Now using EXCLUDE structure once again.
//
//    Rev 1.2   31 Oct 1994 13:10:42   MFALLEN
// Commented out EXCLUDE structure so we can compile.
//
//    Rev 1.1   31 Oct 1994 13:09:14   MFALLEN
// Commented out exclude.h so we can compile.
//
//    Rev 1.0   31 Oct 1994 12:50:24   MFALLEN
// Initial revision.
//
//    Rev 1.58   14 Feb 1994 15:06:52   DALLEE
// Changed _NAVMON conditional to BANKERS.-
//
//    Rev 1.57   10 Feb 1994 16:13:32   DALLEE
// Added .registration field to NAVOPTIONS dependent on _NAVMON #define.
//************************************************************************
#ifndef _INC_OPTIONS
#define _INC_OPTIONS

#ifdef SYM_WIN32
    #pragma pack(push)
    #pragma pack(1)             // This #pragma does not work for DOS platforms.
#endif

#include        "navopts.h"
#include        "exclude.h"
#include        "netobj.h"
#include        "navstart.h"
#include        "quarset.h"
#include        "custom.h"

//**************************************************************************
//
// ATTENTION: Adding any NAV configuration structure members which you
// you would like to be written to NAVOPTS.DAT should be done through the
// NAVOPTS_DAT_CONFIGURATION_BINARY macro.
//
//**************************************************************************
#define NAVOPTS_DAT_CONFIGURATION_BINARY  \
    USEROPT     user;       \
    TSROPT      tsr;        \
    ACTOPT      activity;   \
    ALERTOPT    alert;      \
    INOCOPT     inoc;       \
    SCANOPT     scanner;    \
    PASWOPT     password;   \
    GENOPT      general;    \
    QUARANTINESET quarantineset;  \
    CUSTOMRESPONSE customAP; \
    CUSTOMRESPONSE customScan

//**************************************************************************
//
// ATTENTION: Adding any NAV configuration structure members which you
// you would like to be written to NAVOPTS.DAT should be done through the
// NAVOPTS_DAT_CONFIGURATION_BINARY macro.
//
//**************************************************************************
typedef struct tagNAVOPTIONS
    {
    NAVOPTS_DAT_CONFIGURATION_BINARY;
#ifndef CREATE_NAVBIN
    EXCLUDE       exclude;              // Exclusion List info
    NETUSER       netUser;              // Network User info
    STARTOPT      startup;              // NAVBOOT/NAVTSR/NAVAP startup opts.
#endif
    } NAVOPTIONS, *PNAVOPTIONS, FAR *LPNAVOPTIONS;

//**************************************************************************
//
// ATTENTION: Adding any NAV configuration structure members which you
// you would like to be written to NAVOPTS.DAT should be done through the
// NAVOPTS_DAT_CONFIGURATION_BINARY macro.
//
//**************************************************************************
typedef struct
    {
    NAVOPTS_DAT_CONFIGURATION_BINARY;
    } _NAVOPTIONS;

#ifdef __cplusplus
extern "C" {
#endif
extern  NAVOPTIONS      navOptions;             // Option Settings
#ifdef __cplusplus
}
#endif

// ********************************************** //
// Structure to hold section name modifiers       //
// ********************************************** //

#define MAX_SUFFIXES                  5

typedef struct tagSUFFIX
{
    LPSTR lpszSectionSuffix;

    WORD  wPlatform;

} SUFFIX, FAR * LPSUFFIX;

typedef struct tagSECTIONSUFFIX
{
    WORD        wPlatform[MAX_SUFFIXES];// At lease one platform is required
                                        // The array must be loaded contiguously
                                        // beginning with offset 0

    SUFFIX      Suffix[MAX_SUFFIXES];

} SECTIONSUFFIX, FAR *LPSECTIONSUFFIX;

#ifdef SYM_WIN32
    #pragma pack( pop )
#endif

#endif // _INC_OPTIONS
