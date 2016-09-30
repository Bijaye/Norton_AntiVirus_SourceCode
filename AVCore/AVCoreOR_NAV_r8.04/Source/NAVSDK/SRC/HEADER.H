// Copyright 1998 Symantec Corporation, Peter Norton Product Group
//*********************************************************************
//
//  $Header:   S:/NAVSDK/VCS/HEADER.H_v   1.0   04 Mar 1998 13:17:14   PVENKAT  $
//
//
//*********************************************************************
//  $Log:   S:/NAVSDK/VCS/HEADER.H_v  $
// 
//    Rev 1.0   04 Mar 1998 13:17:14   PVENKAT
// Initial revision.
//*********************************************************************

//*********************************************************************
//  NOTE:   
//      This file same as in CORE/NAVDEFS project.  We need to read
//      VIRSCAN1.DAT file to get the VirusDefinition date if the defs
//      directory is given and no AppID is specified.  This file has to
//      be in sync with CORE/NAVDEFS always.  The main reason why a 
//      second copy is created is: whenever there is a change there it
//      may not immediately visible in this project and can be noticed 
//      only when that change gets shipped and our product in the field
//      may have a problem in getting the date.
//*********************************************************************


#ifndef     __HEADER_H__
#define     __HEADER_H__

const char VIRSCAN_FILE[] = "VIRSCAN1.DAT";

// save the current packing alignment
#pragma pack(push, defutils_virscan_header)

// pack on one-byte boundaries
#pragma pack(1)

typedef struct
{
    DWORD       dwSignature;                /* signature for data file */
    DWORD       dwVersionNumber;
    BYTE        byMon, byDay, byYear;       /* last revision */
    BYTE        byFiller;
    WORD        wNumSections;
    DWORD       dwTableOffset;
    DWORD       dwCRC;                      /* integrity CRC */
    BYTE        byPadding[10];             /* pad to 32 bytes */
} DATA_FILE_HEADER_T, FAR *LPDATA_FILE_HEADER;

// restore old packing alignment
#pragma pack(pop, defutils_virscan_header)

#endif      // __HEADER_H__
