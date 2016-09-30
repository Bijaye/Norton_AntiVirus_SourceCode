// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/avexload.h_v   1.2   21 Feb 1996 14:14:30   CRENERT  $
//
// Description:
//      Include for DOS AVAPI Extensions loader.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/avexload.h_v  $
// 
//    Rev 1.2   21 Feb 1996 14:14:30   CRENERT
// #ifdef'd out exeheader structure definition.
// 
//    Rev 1.1   12 Jan 1996 07:00:44   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.0.1.1   13 Oct 1995 14:37:18   DCHI
// Added DOS NAVEX external overlay code.
// 
//    Rev 1.0.1.0   07 Sep 1995 16:16:16   DALLEE
// Branch base for QAK8.
// 
//    Rev 1.0   07 Sep 1995 16:11:00   DALLEE
// Initial revision.
//************************************************************************

#if 0

typedef struct
{
    char exesig[2];                 /* MZ */
    WORD len_mod_512;       /* length of file % 512 */
    WORD page_size;         /* size of file in 512 byte pages */
    WORD reloc_table_items; /* # of relocation table items */
    WORD header_size;       /* size of header in paragraphs */
    WORD min_par_above;     /* min # of pars needed above program */
    WORD max_par_above;     /* max " */
    WORD ss;
    WORD sp;
    WORD checksum;
    WORD ip;
    WORD cs;
    WORD first_rel_offset;  /* offset of first relocation table */
} exeheader;

#endif

#define LOADER_STACK_SIZE      1024

typedef struct
{
    WORD    wImageSize;
    DWORD   dwOffset;
} OVERLAY_INFO;

//************************************************************************
// Constants for indicating which module is resident.
// Used to fill in wResidentModule field of GLOBAL_LOAD_INFO_TYPE
//************************************************************************

#define EXTRESIDENT_NONE                0
#define EXTRESIDENT_SCANFILE            1
#define EXTRESIDENT_SCANBOOT            2
#define EXTRESIDENT_SCANPART            3
#define EXTRESIDENT_SCANMEMORY          4
#define EXTRESIDENT_REPAIRFILE          5
#define EXTRESIDENT_REPAIRBOOT          6
#define EXTRESIDENT_REPAIRPART          7

typedef struct
{
    WORD                    wSS, wSP;
    WORD                    wExportSP;
    WORD                    wExportSS;
    BOOL                    bInitialized;

    WORD                    wResidentModule;    // Currently resident module
    OVERLAY_INFO            oiEXTScanFile;
    OVERLAY_INFO            oiEXTScanBoot;
    OVERLAY_INFO            oiEXTScanPart;
    OVERLAY_INFO            oiEXTScanMemory;
    OVERLAY_INFO            oiEXTRepairFile;
    OVERLAY_INFO            oiEXTRepairBoot;
    OVERLAY_INFO            oiEXTRepairPart;

    EXPORT_TABLE_TYPE       ExportTable;
    EXTSTATUS               result;
    LPVOID                  lpOverlayPtr;
    LPVOID                  lpSwapArea;
} GLOBAL_LOADER_INFO_TYPE;


