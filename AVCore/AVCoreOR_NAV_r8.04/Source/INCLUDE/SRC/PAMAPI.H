// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/PAMAPI.H_v   1.15   06 Sep 1996 14:04:22   CNACHEN  $
//
// Description:
//
//  This file contains the prototypes for the PAM API functions as well as
//  PAMGHANDLE and PAMLHANDLE data structure definitions required for proper
//  PAM use.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/PAMAPI.H_v  $
// 
//    Rev 1.15   06 Sep 1996 14:04:22   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.15   06 Sep 1996 14:00:50   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.14   16 May 1996 13:30:48   CNACHEN
// Changed PAM to only use the CPU cache in auto-protect (VXD/NTK)
// 
//    Rev 1.13   25 Mar 1996 12:33:10   CNACHEN
// Added CACHE support for ALL platforms.
// 
// 
//    Rev 1.12   08 Mar 1996 10:51:24   CNACHEN
// Added NOT_IN_TSR stuff..
// 
//    Rev 1.11   04 Mar 1996 16:21:50   CNACHEN
// Changed cache.h to pamcache.h
// 
//    Rev 1.10   04 Mar 1996 16:02:00   CNACHEN
// Added CACHE support.
// 
//    Rev 1.9   27 Feb 1996 18:45:02   CNACHEN
// Updated to include COVER.H
// 
//    Rev 1.8   20 Feb 1996 13:48:36   CNACHEN
// Changed bitmap type to bitmap_t for Windows NT driver...
// 
// 
//    Rev 1.7   20 Feb 1996 11:27:12   CNACHEN
// Changed all LPSTRs to LPTSTRs.
// 
//    Rev 1.6   13 Feb 1996 14:39:04   CNACHEN
// Updated to fix problems with prototypes on WIN16 platform.
// 
//    Rev 1.5   12 Feb 1996 17:46:50   CNACHEN
// Added support for Quake2, Win16.
// 
//    Rev 1.4   12 Feb 1996 13:44:16   CNACHEN
// Added extern "C" directives..
// 
//    Rev 1.3   12 Feb 1996 13:33:14   CNACHEN
// One more * to LP...
// 
//    Rev 1.2   12 Feb 1996 13:30:06   CNACHEN
// Changed *'s to LP's.
// 
// 
//    Rev 1.1   02 Feb 1996 11:45:18   CNACHEN
// Added new dwFlags and exclusion fields...
// 
//    Rev 1.0   01 Feb 1996 10:17:36   CNACHEN
// Initial revision.
// 
//    Rev 1.18   01 Feb 1996 10:15:36   CNACHEN
// Changed char * to LPTSTR...
// 
//    Rev 1.17   23 Jan 1996 14:50:18   DCHI
// Addition of ARPL GetCSIPQ, ARPL InRepair, and CSIP queue.
// 
//    Rev 1.16   22 Jan 1996 17:26:04   DCHI
// Added ulMinNoExcludeCount and byFiller fields to PAMConfigType structure.
// 
//    Rev 1.15   22 Jan 1996 13:40:44   CNACHEN
// Added new fault support.
// 
// 
//    Rev 1.14   04 Jan 1996 10:16:08   CNACHEN
// Added two new PAM opcodes for PrefetchQ->VM and VM->PrefetchQ...
// 
//    Rev 1.13   19 Dec 1995 19:08:08   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.12   15 Dec 1995 19:03:16   CNACHEN
// Added LOW_IN_RAM stuff...
// 
//    Rev 1.11   13 Dec 1995 11:57:30   CNACHEN
// PAMFile* -> File* (change made for NLM compatbility)
// 
// 
//    Rev 1.10   26 Oct 1995 14:09:36   CNACHEN
// Added memory swapping instead of disk swapping if NULL is passed in for
// the swap-file filename.
// 
//    Rev 1.9   23 Oct 1995 12:51:30   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.8   19 Oct 1995 18:39:20   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************

#ifndef _PAMAPI_H
#define _PAMAPI_H

#if defined(SYM_VXD) || defined(SYM_NTK)

#define IN_AUTOPROTECT
#define USE_CACHE                       // only use the cache in autoprotects

#endif  // SYM_VXD || SYM_NTK

#define LOW_IN_RAM

#ifdef BORLAND
#include <stdio.h>
#include <string.h>
#include <mem.h>
#endif

#ifdef BORLAND
#include "types.h"		// temporary!
#include "callfake.h"	// temporary!
#else
#include "platform.h"
#include "file.h"
#endif

#define NUM_FAULTS      32              // 16 different faults... (CPU.H)

#include "datafile.h"
#include "cpu.h"
#include "exclude.h"
#include "opcode.h"
#include "profile.h"
#include "page.h"
#include "search.h"
#include "sigex.h"
#include "prepair.h"
#include "pamcache.h"
#include "cover.h"

/* the PAMConfigType structure contains various configuration options */

#define IMM_SIZE        3               // size of the immunity sig

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD        wFillWord;              // fill unused memory with this
    DWORD       dwMaxIter;              // max iter to emulate for ALL viri
    WORD        wMinWritesForScan;      // min # of writes to scan mod. buffer

    BYTE        byImmuneSig[IMM_SIZE];  // if we find this sig after an int
                                        // then we want to go into no exclude
                                        // mode

    BYTE        byNoFaultSig[IMM_SIZE]; // if we find this sig, then exclude
                                        // the next non no_excluded instruction
                                        // from being faulted

    BYTE        byFretSig[IMM_SIZE];    // if we find this sig, then do a special
                                        // IRET

    BYTE        byLoadPreSig[IMM_SIZE]; // if we find this sig, then do a reload
                                        // of the prefetch queue as soon as the
                                        // prefetch queue is re-enabled

    BYTE        byPreToVMSig[IMM_SIZE]; // if we find this sig, then store the
                                        // contents of the prefetch queue to
                                        // virtual memory at ES:DI

    BYTE        byVMToPreSig[IMM_SIZE]; // if we find this sig, then retrieve
                                        // the contents of the prefetch queue
                                        // from VM back into the actual queue

    BYTE        byFaultOff[IMM_SIZE];   // Turn a fault off for a # of iters

    BYTE        byGetCSIPQ[IMM_SIZE];   // Get a CSIP from the queue

    BYTE        byInRepair[IMM_SIZE];   // In repair?  CY if yes.

    WORD        wFaultSS;               // stack segment for fault
    WORD        wFaultSP;               // stack pointer for fault

    DWORD       ulMaxImmuneIter;        // how many instructions can our
                                        // fault code execute before timing
                                        // out. (to stop crashes)

    DWORD       ulMinNoExcludeCount;    // minimum number of real
                                        // instruction fetches before
                                        // dynamic exclusions begin
                                        //  (0 = always exclude)

    // fault information...

    BYTE        byInstrFaultTable[256]; // FF means no fault, !FF means fault
    DWORD       dwFaultSegOffArray[NUM_FAULTS]; // 0-based table
    DWORD       dwFaultIterArray[NUM_FAULTS];   // min # of iterations before
                                                // fault becomes active...

    DWORD       dwCacheCheckIter;       // how many iterations before we
                                        // check our CPU cache?

    DWORD       dwCacheStoreIter;       // Only store in the cache if
                                        // >= this number of iterations

    DWORD       dwAPMaxIter;            // Maximum # iterations for AP

    BYTE        byFiller[256];          // filler for expansion

} PAMConfigType;

/* the GlobalDataType structure contains all data which is not specific to
   a current scanning session.  IE the static data which is the same over all
   threads
*/

typedef struct
{
    /* global exclusion data */

    exclude_type        exclude_info;

    /* string scanning data */

    sig_type            **sig_ptr_array;                /* array of sig ptrs */
    table_type          table_array[NUM_TOP_SETS];      /* top lvl set array */
    BYTE                index_array[256];               /* indexs into table */
    BYTE                top_set[NUM_TOP_SETS];          /* list of top sets  */
    WORD                num_sigs;                       /* # of sigs         */
    PAMConfigType       config_info;                    /* config options    */
    ExcludeContext      sig_exclude_info;               /* exclude info      */

#ifdef LOW_IN_RAM
    BYTE                low_mem_area[LOW_MEM_SIZE];     /* read only swap area*/
#endif

    CACHE_T             stCache;                        /* our cache */

} GlobalPAMType;

typedef GlobalPAMType FAR   *PAMGHANDLE;
typedef PAMGHANDLE FAR      *LPPAMGHANDLE;


/* Circular queue containing CS:IP values */

#define CSIP_QUEUE_SIZE     64

typedef struct
{
    WORD    wIndex;
    DWORD   dwCSIP[CSIP_QUEUE_SIZE];
} csip_queue_type;

// HLOCAL flags for proper operation

#define LOCAL_FLAG_IMMUNE                   0x00000001
#define LOCAL_FLAG_IMMUNE_EXCLUSION_PERM    0x00000002
#define LOCAL_FLAG_REPAIR_DECRYPT           0x00000004
#define LOCAL_FLAG_REPAIR_REPAIR            0x00000008
#define LOCAL_FLAG_NO_FAULT                 0x00000010
#define LOCAL_FLAG_WRITE_THRU               0x00000020
#define LOCAL_FLAG_FILE_ERROR               0x00000040
#define LOCAL_FLAG_MEM_ERROR                0x00000080
#define LOCAL_FLAG_OTHER_ERROR              0x00000100
#define LOCAL_FLAG_NO_CAND_LEFT             0x00000200

#define LOCAL_FLAG_ERROR                    (LOCAL_FLAG_FILE_ERROR | \
                                             LOCAL_FLAG_MEM_ERROR |  \
                                             LOCAL_FLAG_OTHER_ERROR)

typedef struct
{
    PAMGHANDLE      hGPAM;

    /* candidates is exclusion data which is local to the current thread.
       it contains the current list of candidate mutation engines which could
       be infecting the current sample
    */

    bitmap_t        candidates;
    BYTE            excluded[MAX_EXCLUDE_CAT];

    /* CPU state for this thread */

    CPU_TYPE        CPU;

    /* page table information for this thread. */

    page_info_type  page_global;

    DWORD           dwFlags;                    /* exclusion and other flags */
    WORD            repair_result;              /* result of repair op */
    WORD            instr_run;                  /* run-count of same opcodes */
    HFILE           hRepairFile;                /* handle to our temporary
                                                   repair file */
    csip_queue_type csip_queue;                 /* queue of CS:IPs of last
                                                   n instructions */

    CACHE_ENTRY_T   stTempEntry;


} LocalPAMType;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif


typedef LocalPAMType FAR    *PAMLHANDLE;
typedef PAMLHANDLE FAR      *LPPAMLHANDLE;


typedef unsigned int    PAMSTATUS;

#define PAMSTATUS_OK            0
#define PAMSTATUS_FILE_ERROR    1
#define PAMSTATUS_MEM_ERROR     2
#define PAMSTATUS_VIRUS_FOUND   3
#define PAMSTATUS_NO_SCAN       4
#define PAMSTATUS_NEED_TO_SCAN  5
#define PAMSTATUS_NO_REPAIR     6

/* pam protos */

#ifdef __cplusplus
extern "C" {
#endif

PAMSTATUS PAMGlobalInit(LPTSTR szDataFile,LPPAMGHANDLE hPtr);

PAMSTATUS PAMLocalInit(LPTSTR szDataFile,
                       LPTSTR szSwapFile,
                       PAMGHANDLE hGPAM,
                       LPPAMLHANDLE hLPtr);

PAMSTATUS PAMLocalClose(PAMLHANDLE hLocal);

PAMSTATUS PAMGlobalClose(PAMGHANDLE hGlobal);

PAMSTATUS PAMScanFile(PAMLHANDLE hLocal,
                      HFILE hFile,
                      WORD wFlags,
                      LPWORD lpVirusID);

PAMSTATUS PAMRepairFile(PAMLHANDLE hLocal,
                        LPTSTR szDataFile,
                        LPTSTR lpszFileName,
                        LPTSTR lpszWorkFileName,
						WORD wVirusID,
                        WORD wFlags);

#ifdef __cplusplus
}
#endif


#include "proto.h"

#ifdef BORLAND
#define MEMSET memset
#define MEMCMP memcmp
#define MEMCPY memcpy
#define STRCPY strcpy
#endif

#endif
