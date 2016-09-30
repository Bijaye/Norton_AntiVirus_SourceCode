// Copyright 1996 Symantec Corporation
// ----------------------------------------------------------------------------
//
// $Header:   S:/AVENGE/VCS/BOOTREP.H_v   1.0   18 Apr 1997 13:39:14   MKEATIN  $
//
// Description:
//  This file contains Boot and MBR Repair prototypes, structures, and macros.
//
// Contains:
//
// See Also:
//
// ----------------------------------------------------------------------------
// $Log:   S:/AVENGE/VCS/BOOTREP.H_v  $
//
//    Rev 1.0   18 Apr 1997 13:39:14   MKEATIN
// Initial revision.
//
//    Rev 1.7   17 Jun 1996 15:09:24   RAY
// Various bug fixes to Boot Repair stuff.
//
//    Rev 1.6   04 Jun 1996 16:36:42   RAY
// Fixed data file logic in EngRepairBootAux.
//
//    Rev 1.5   04 Jun 1996 13:45:04   RAY
// Reorganized most of BootRep and added support for SECTORs.
//
//    Rev 1.4   21 May 1996 17:10:28   RAY
// Fixed prototype and call to EngRepairBootEOFD
//
//    Rev 1.3   21 May 1996 16:53:34   RAY
// Changed from Physical to Linear Objects and Callbacks.
//
//    Rev 1.2   17 May 1996 18:18:00   RAY
// Added LO/HIBYTE macros for Borland's Fisher-Price compiler.
//
//    Rev 1.1   17 May 1996 13:42:32   RAY
// moved proto for EngRepairBootAux to avengel.h
//
//    Rev 1.0   16 May 1996 18:27:42   RAY
// Initial revision.
//
// ----------------------------------------------------------------------------

#if !defined ( BOOTREP_H )
#   define BOOTREP_H

#include "bootdefs.h"
#include "avenge.h"

// -----------------------------------
// BOOTREPAIRDEF type
// -----------------------------------
//  *** MAKE SURE THIS IS KEPT IN SYNC
//  *** WITH VIRUS_REPAIR_T
//
// This is the Boot Repair view of the
// VIRUS_REPAIR_T structure.
// -----------------------------------

typedef struct
{                                       // ----------------------------------
    WORD    wCRC;                       // not used for Boot repair
    WORD    wFloppyRepairType;          // was wTagSize
    WORD    wHDLocation1;               // was wHostJmp
    WORD    wHDLocation2;               // was wJmpLen
    WORD    wFloppy360;                 // was SS_reg
    WORD    wFloppy720;                 // was SP_reg
    WORD    wFloppy12;                  // was CS_reg
    WORD    wFloppy144;                 // was IP_reg
    WORD    wFloppy288;                 // was Header
    WORD    wFloppyAll;                 // was HeadLen
    WORD    wRestoreFlags;              // was BookLocation
    WORD    wSpecialFlags;              // was BookMark
    DWORD   dwReserved;                 // not used for Boot repair
    DWORD   dwRepairFlags;              // was wControl2 and wControl
    WORD    wMemSigIndex;               // not used for Boot repair
    WORD    wHDRepairType;              // was wInfSize
                                        // ----------------------------------
} BOOTREPAIRDEF, FAR * LPBOOTREPAIRDEF;


// -----------------------------
// wRestoreFlags
// -----------------------------
                                        // -----------------------------
#define BIG_SECTOR_OFFSET       0x0001  // Add 0x0100 to Sector Offset
#define BIG_SIDE_OFFSET         0x0002  // Add 0x0100 to Side Offset
#define BIG_CYLINDER_OFFSET     0x0004  // Add 0x0100 to Cylinder Offset
                                        // -----------------------------
// ---------------------
// wSpecialFlags
// ---------------------
                                        // --------------------------------------------------------
#define WORD_CYLINDER_OFFSET    0x0001  // Read Cylinder as WORD
#define AFTER_ENDING_SECTOR     0x0001  // Original MBR at MBR EndingSector +
                                        // LOBYTE(HardLocation1)
#define BEFORE_ENDING_SECTOR    0x0002  // Original MBR at MBR EndingSector -
                                        // LOBYTE(HardLocation1)
#define FIXUP_MBR_ENDINGSECTOR  0x0004  // Fixup MBR EndingSector +
                                        // LOBYTE(RestoreWord)
#define FIXUP_MBR_TOTALSECTORS  0x0008  // Fixup MBR TotalSectors +
                                        // LOBYTE(RestoreWord)
#define FIXUP_BOOTSECTOR        0x0010  // Fixup Boot Sector TotalSectors +
                                        // HIBYTE(RestoreWord)
#define FIXUP_RECOGNIZE_BIGFOOT 0x0020  // Recognize and use BigTotalSectors
#define FIXUP_ALWAYS_BIGFOOT    0x0040  // Always use BigtotalSectors if set;
                                        // always use old TotalSectors if clear;
                                        // only valid if FIXUP_RECOGNIZE_BIGFOOT
                                        //---------------------

typedef struct tagSECTOR
{
    BYTE        bySide;
    WORD        wCylinder;
    BYTE        bySector;
    DWORD       dwLinearLow;
    DWORD       dwLinearHigh;
    LPBYTE      lpbyBuffer;
} SECTOR, FAR * LPSECTOR;


// -------------------------
// AVENGE internal functions
// -------------------------

ENGSTATUS EngRepairBootPhysical
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair Def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Disk type
    LPSECTOR        lprInfection        // [in] Infected sector
);

ENGSTATUS EngRepairBootSavePhysical
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair Def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Infected Sector
);

ENGSTATUS EngRepairBootSaveLogical
(
    HLENG               hLocalEng,      // [in] Local boot engine context
    LPLINEAROBJECT      lprLinear,
    LPBOOTREPAIRDEF     lprBootRepairDef,
    WORD                wDiskType,
    BYTE                byPartion,
    BYTE                byRepairType,
    LPSECTOR            lprInfection
);

ENGSTATUS EngRepairBootEOFD
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Sector with partition data
);

ENGSTATUS EngRepairGeneric
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of repair def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Infected sector
);

ENGSTATUS EngRepairMBRGeneric
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Sector to fill
    LPSECTOR        lprInfection,       // [in] Sector with partition data
    WORD            wDiskType           // [in] Disk type
);

ENGSTATUS EngRepairBootGeneric
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPSECTOR        lprInfection,       // [in] Infected Sector
    WORD            wDiskType           // [in] Disk type
);

ENGSTATUS EngAnalyzeFloppy
(
    HLENG            hLocalEng,         // [in] Local boot engine context
    LPLINEAROBJECT   lprLinear,         // [in] Linear object to analyze
    LPBYTE           lpbySector,        // [in] Sector containing BPB
    LPWORD           lpwDiskType        // [out] Disk Type
);

VOID EngReplaceMBRPartition
(
    LPBYTE      lpbyMBR,                // [in/out] Entire MBR
    BYTE        byPartNumber,           // [in] Partition to parse (0 - 3)
    PARTENTRY   rPartEntry              // [in] filled out PARTENTRY structure
);

VOID EngEncodeSectorCylinder
(
    BYTE    bySector,                   // [in] Decoded Sector
    WORD    wCylinder,                  // [in] Decoded Cylinder
    LPWORD  lpwEncoded                  // [out] Encoded Sect/Cyl as in MBR
);

ENGSTATUS EngValidateMBR
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyMBR             // [in] Raw MBR to validate
);

ENGSTATUS EngValidateMBRPartition
(
    HLENG           hLocalEng,          // [in] Local boot engine context
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyMBR,            // [in] Raw MBR
    BYTE            byPartNumber        // [in] Partition to validate (0 - 3)
);

ENGSTATUS EngValidateBootSector
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyBootSector      // [in] Raw boot sector to validate
);

VOID EngUpdateMBR
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyInfectedMBR,    // [in] Original MBR
    LPBYTE          lpbyCleanMBR        // [in/out] New MBR
);

ENGSTATUS EngReadSectorLinear
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in] Sector ro read
);

ENGSTATUS EngWriteSectorLinear
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in] Sector to write
);

ENGSTATUS EngLinearizeSector
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in/out] Sector to linearize
);


// ----------------------------------
// Repair Types
// (from virlib/certlibn/repairn.inc)
// ----------------------------------

#define REP_MBR_PHYSICAL        01
#define REP_MBR_SAVE_PHYSICAL   02
#define REP_MBR_EOFD            03
#define REP_MBR_GENERIC         04
#define REP_MBR_UNIQUE          05

#define REP_BOOT                10

#define REP_BOOT_PHYSICAL       11
#define REP_BOOT_SAVE_PHYSICAL  12
#define REP_BOOT_SAVE_LOGICAL   13
#define REP_BOOT_GENERIC        14
#define REP_BOOT_UNIQUE         15

#define REP_EXEC_CODE           20


// -------------
// Request Flags
// -------------
#define REQUEST_READ            0x01
#define REQUEST_WRITE           0x02
#define REQUEST_GENERIC         0x04
#define REQUEST_MBRUPDATE       0x08


// ---------------------
// Miscellaneous defines
// ---------------------

#ifndef LOBYTE
    #define LOBYTE(w)           ((BYTE)(w))
#endif
#ifndef HIBYTE
    #define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

typedef struct tagBPB32FLAGS
{
    WORD        btActiveFAT:4;          // Number of the active FAT
    WORD        btReserved1:3;
    WORD        btMirrored:1;           // True if FATs are mirrored
    WORD        btReserved2:8;
} BPB32FLAGS;

typedef struct tagHARDBPB
{
    WORD        wBytesPerSector;        // Bytes per sector
    BYTE        bySectorsPerCluster;    // Sectors per cluster
    WORD        wReservedSectors;       // First sector of Fat
    BYTE        byNumberOfFats;         // FAT Copies
    WORD        wMaxRootEntries;        // Maximum entries in Root dir
    WORD        wTotalSectors;          // Total number of sectors
    BYTE        byMediaDescriptor;      // Media descriptor byte
    WORD        wSectorsPerFat;         // Sectors per Fat, 0 = 32 bit FAT
    WORD        wSectorsPerTrack;       // Sectors per Track
    BYTE        bySides;                // Number of sides

    DWORD       dwHiddenSectors;        // Number of hidden sectors
    DWORD       dwBigTotalSectors;      // Total sectors for BigFoot volumes

    DWORD       dwSectorsPerFAT32;      // Number of sectors in a 32 bit FAT
    BPB32FLAGS  rBPB32Flags;            // See structure above
    BYTE        byFSMinorVersion;       // Version for file system
    BYTE        byFSMajorVersion;       // Version for file system
    DWORD       dwRootSCN;              // Starting cluster of Root dir
    WORD        wFSInfoSector;          // Sector number of FS Info struct
    WORD        wBackupBootSector;      // If non zero, indicates the sector
                                        // number of a copy of the boot
                                        // record.  Usually 6.
    BYTE        wReserved1[12];         // Reserved

} HARDBPB, FAR * LPHARDBPB;

typedef struct tagFLOPPYBPB
{
    WORD        wBytesPerSector;        // Bytes per sector
    BYTE        bySectorsPerCluster;    // Sectors per cluster
    WORD        wReservedSectors;       // First sector of Fat
    BYTE        byNumberOfFats;         // FAT Copies
    WORD        wMaxRootEntries;        // Maximum entries in Root dir
    WORD        wTotalSectors;          // Total number of sectors
    BYTE        byMediaDescriptor;      // Media descriptor byte
    WORD        wSectorsPerFat;         // Sectors per Fat, 0 = 32 bit FAT
    WORD        wSectorsPerTrack;       // Sectors per Track
    BYTE        bySides;                // Number of sides
} FLOPPYBPB, FAR * LPFLOPPYBPB;


                                        // ---------
#endif                                  // BOOTREP_H
                                        // ---------
