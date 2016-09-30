/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/NAVBNHDR.H_v   1.3   28 Aug 1998 17:48:58   JBRENNA  $
 *                                                                      *
 * Description:                                                         *
 *      Defines header of NAV.BIN file.  This file contains current     *
 *      settings for NAV.                                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/NAVBNHDR.H_v  $
// 
//    Rev 1.3   28 Aug 1998 17:48:58   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.2.2.1   31 Jul 1998 09:48:20   tcashin
//    Added a define for Atomic/Maxwell NAVOPTS version.
//
//    Rev 1.2   20 Jun 1997 11:09:10   DBUCHES
// Added new define for Roswell/Tesla NAVOPTS version.
// 
//    Rev 1.1   18 Jun 1997 15:36:14   DBUCHES
// Added 1byte packing for Win32.
// 
//    Rev 1.0   26 Jan 1996 20:15:36   RJACKSO
// Initial revision.
// 
//    Rev 1.5   02 May 1994 09:13:32   MFALLEN
// BANKERS version number.
// 
//    Rev 1.4   28 Apr 1994 16:53:08   MFALLEN
// Added nav version #define
// 
//    Rev 1.3   26 Apr 1994 16:41:24   BRAD
// Removed PVCS Logs
 ************************************************************************/

#ifndef _INC_NAVBNHDR
#define _INC_NAVBNHDR

                                        //-------------------------------
                                        // Defines for dwVersion field.
                                        //-------------------------------
#define NAV_VERSION_30          0
#define NAV_VERSION_30_BANKERS  1
#define NAV_VERSION_40          2
#define NAV_VERSION_97			3		// For Tesla/Roswell '97 project
#define NAV_VERSION_50          4       // Atomic

                                        //-------------------------------
                                        // Header for NAV.BIN file
                                        //-------------------------------

#ifdef SYM_WIN32
#pragma pack(push,1)
#endif

typedef struct tagNAVBINHEADER
    {
    char        szSignature[15];        // Signature "NAV.BIN"
    DWORD       dwVersion;              // Numeric version number
    BYTE        byFiller[64-15-4];      // Filler to add extra 'stuff'
    } NAVBINHEADER;

#ifdef SYM_WIN32
#pragma pack(pop)
#endif

    
    
#endif

