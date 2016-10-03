// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/PAGE.H_v   1.0   01 Feb 1996 10:17:46   CNACHEN  $
//
// Description:
//
//  This source file contains data structures and constants used in the
//  CPU emulator paging system.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/PAGE.H_v  $
// 
//    Rev 1.0   01 Feb 1996 10:17:46   CNACHEN
// Initial revision.
// 
//    Rev 1.6   26 Oct 1995 14:09:34   CNACHEN
// Added memory swapping instead of disk swapping if NULL is passed in for
// the swap-file filename.
// 
//    Rev 1.5   23 Oct 1995 12:51:30   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.4   19 Oct 1995 18:39:22   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************

#ifndef _PAGE_H_
#define _PAGE_H_


#define NUM_RESIDENT_PAGES      50      // 1 page = 1 sector

#define MEMORY_SIZE_IN_PAGES    2048    // 1 MB / 512 bytes = # of sectors

#define PAGE_DIRTY              1
#define PAGE_IN_MEMORY          2
#define PAGE_ON_DISK            4
#define PAGE_EXEC               8       // code was executed in this page

#define SECTOR_SHIFT            9
#define SECTOR_MASK             0x01FF
#define ADDRESS_MASK            0xFFFFFE00L
#define DISK_NOT_USED           0xFFFFFFFFUL

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    BYTE        data[SECTOR_SIZE];
    DWORD       page_index_num;
    DWORD       last_reference;
} page_type;


typedef struct
{
    BYTE        flags;
    BYTE        location;       // resident page #
    DWORD       disk_addr;      // sector # in swap file. FFFFFFFF = not on disk
} index_type;


typedef struct
{
    page_type           page_data[NUM_RESIDENT_PAGES];
    index_type          page_index[MEMORY_SIZE_IN_PAGES];
    HFILE               swap_file;
    DWORD               last_reference;
    BYTE                executable_fetch;   // is the current memory reference
                                            // due to code execution
    WORD                next_disk_addr;     // sector # of next page swapout
} page_info_type;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif
