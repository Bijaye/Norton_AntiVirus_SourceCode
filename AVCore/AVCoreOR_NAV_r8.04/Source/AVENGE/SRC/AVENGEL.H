// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/AVENGEL.H_v   1.5   13 Aug 1997 13:56:34   CNACHEN  $
//
// Description:
//  Contains internal prototypes for functions exported by AVENGE.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/AVENGEL.H_v  $
// 
//    Rev 1.5   13 Aug 1997 13:56:34   CNACHEN
// Added new function EngScanBooBuffer to do old style boot scanning like
// certlib.
// 
//    Rev 1.4   08 Jul 1997 17:21:20   MKEATIN
// No longer pass a default NULL in any prototype.  This won't compile for
// the NLM C compiler.
// 
//    Rev 1.3   15 May 1997 12:20:06   MKEATIN
// Added a lpvProvidedFileInfo pamameter to some info getting routines.
// 
//    Rev 1.2   13 May 1997 18:27:22   MKEATIN
// Ported Carey's boot caching code.
// 
//    Rev 1.1   02 May 1997 16:43:50   MKEATIN
// Changed prototype from LPTSTR to LPSTR in EngGetInfoHVIRUS().
// 
//    Rev 1.0   18 Apr 1997 13:39:08   MKEATIN
// Initial revision.
// 
//    Rev 1.4   23 Oct 1996 11:18:42   DCHI
// Added IPC callback support.
// 
//    Rev 1.3   28 May 1996 14:29:08   DCHI
// Updated EngScanMemAux() prototype to support memory object.
// 
//    Rev 1.2   21 May 1996 16:51:12   RAY
// Changed from Physical to Linear Objects and Callbacks.
// 
//    Rev 1.1   17 May 1996 13:35:30   RAY
// Added prototypes for boot repair.
// 
//    Rev 1.0   16 May 1996 14:13:48   CNACHEN
// Initial revision.
// 
// 
//************************************************************************


#ifndef _AVENGEL_H

#define _AVENGEL_H

#include "avenge.h"

///////////////////////////////////////////////////////////////////////////
//
// Internal prototype for algorithmic scanner
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS AlgApplyAllSigs
(
    LPFILEOBJECT        lpFileObject,
    LPALGSCANDATA       lpAlgScanThis,
    LPFSTARTINFO        lpFStartInfo,
    LPALG_SCAN_INFO     lpAlgScanData,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
);


///////////////////////////////////////////////////////////////////////////
//
// Internal prototype for FSTART function
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS GetFStart
(
    LPFILEOBJECT    lpFileObject,
    WORD            wEntryPointNum,
    LPFSTARTDATA    lpFStartData,
    LPFSTARTINFO    lpFStartInfo
);

///////////////////////////////////////////////////////////////////////////
//
// Internal prototype for algorithmic data loader
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngFreeAlgAux
(
    LPGENERALCALLBACKS  lpGeneralCallBacks,
    LPALG_SCAN_INFO     lpAlgScanInfo
);

ENGSTATUS EngLoadAlgScanData
(
    LPDATAFILECALLBACKS    lpDataFileCallBacks,
    LPGENERALCALLBACKS     lpGeneralCallBacks,
    DWORD                  dwFileSection,
    LPALG_SCAN_INFO        lpAlgScanInfo
);


///////////////////////////////////////////////////////////////////////////
//
// Info caching prototypes
//
///////////////////////////////////////////////////////////////////////////


ENGSTATUS EngCacheInit
(
    LPVIRUS_INFO_CACHE  lpInfoCache,
    LPIPCCALLBACKS      lpIPCCallBacks
);

ENGSTATUS EngCacheClose
(
    LPVIRUS_INFO_CACHE  lpInfoCache
);

ENGSTATUS EngCacheLocateInfoHVIRUS
(
    HVIRUS              hVirus,
    LPVIRUS_INFO_CACHE  lpInfoCache,
    LPVIRUS_INFO        lpVirusInfo,
    LPHVIRUS            lphVirus,
    LPSTR               lpszVirusName,
    LPBOOL              lpFoundEntry
);

ENGSTATUS EngCacheLocateInfoVID
(
    WORD                wVirusID,
    LPVIRUS_INFO_CACHE  lpInfoCache,
    LPVIRUS_INFO        lpVirusInfo,
    LPHVIRUS            lphVirus,
    LPSTR               lpszVirusName,
    LPBOOL              lpFoundEntry
);

ENGSTATUS EngCacheInsertItem
(
    LPVIRUS_INFO_CACHE  lpInfoCache,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpszVirusName
);


///////////////////////////////////////////////////////////////////////////
//
// Boot scanner loading prototypes
//
///////////////////////////////////////////////////////////////////////////


ENGSTATUS EngBootLoad
(
    HGENG       hGEng
);

ENGSTATUS EngBootUnload
(
    HGENG       hGEng
);

ENGSTATUS EngLoadResidentBootInfo
(
    HGENG       hGEng
);

ENGSTATUS EngFreeResidentBootInfo
(
    HGENG       hGEng
);


///////////////////////////////////////////////////////////////////////////
//
// CRC scanner loading prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngCRCLoad
(
    HGENG       hGEng
);

ENGSTATUS EngCRCUnload
(
	HGENG   hGEng
);


///////////////////////////////////////////////////////////////////////////
//
// CRC scanner prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS CRCApplySigs
(
    LPFSTARTDATA        lpFStartData,
    LPFSTARTINFO        lpFStartInfo,
    LPCRC_SCAN_INFO     lpCRCScanInfo,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
);

///////////////////////////////////////////////////////////////////////////
//
// Delete virus record prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngDeleteVirusEntry
(
    HGENG       hGEng,
    HVIRUS      hVirus
);


///////////////////////////////////////////////////////////////////////////
//
// Repair prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngRepairFileAux
(
    HLENG               hLEng,              // temporary buffers and callbacks
    LPVIRUS_INFO        lpVirusInfo,        // contains CRC/ALG info, etc...
    LPFILEOBJECT        lpFileObject,       // file object to repair
    LPBOOL              lpbFileRepaired     // OUT: was the file repaired?
);

ENGSTATUS EngRepairBootAux
(
    HLENG            hLocalEng,         // [in] Local engine context
    LPVIRUS_INFO     lprVirusInfo,      // [in] contains repair info
    LPLINEAROBJECT   lprLinear,         // [in] Linear object to repair
    BYTE             byPartNumber,      // [in] Partition to repair
    LPBOOL           lpbRepaired        // [out] Repaired?
);

ENGSTATUS GetRepairRecord
(
    HLENG                   hLEng,
    LPVIRUS_INFO            lpstVirusInfo,
    LPVIRUS_REPAIR          lpVirusRepairInfo,
    LPBYTE                  lpbyDecryptSig
);

ENGSTATUS GetRepairRecordAux
(
    LPDATAFILECALLBACKS     lpDataFileCallBacks,
    LPVOID                  lpvFileInfo,
    LPDATA_FILE_TABLE       lpRepSec,
    LPDATA_FILE_TABLE       lpMemSec,
    WORD                    wRepairIndex,
    LPVIRUS_REPAIR          lpVirusRepairInfo,
    LPBYTE                  lpbyDecryptData
);

ENGSTATUS GetRepairRecordAux
(
    LPDATAFILECALLBACKS     lpDataFileCallBacks,
    LPVOID                  lpvFileInfo,
    LPDATA_FILE_TABLE       lpRepSec,
    LPDATA_FILE_TABLE       lpMemSec,
    WORD                    wRepairIndex,
    LPVIRUS_REPAIR          lpVirusRepairInfo,
    LPBYTE                  lpbyDecryptData
);

ENGSTATUS GetBOORepairRecord
(
    HGENG               hGEng,
    WORD                wRepairIndex,
    LPVIRUS_REPAIR      lpVirusRepairInfo
);

ENGSTATUS EngLocateSections
(
    HGENG                   hLEng,
    LPVIRUS_INFO            lpVirusInfo,
    LPLPVOID                lplpvFileInfo,
    LPDATA_FILE_TABLE       lpRepSec,
    LPDATA_FILE_TABLE       lpMemSec,
    LPVOID                  lpvProvidedFileInfo
);

///////////////////////////////////////////////////////////////////////////
//
// Get information prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngGetInfoHVIRUS
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpsVirusName,
    LPVOID              lpvProvidedFileInfo
);

ENGSTATUS EngGetVirusInfoAux
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPVOID              lpvProvidedFileInfo
);

ENGSTATUS EngPutVirusInfoAux
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo
);

ENGSTATUS EngGetVirusInfoVIDAux
(
    HGENG               hGEng,
    WORD                wVID,
    LPHVIRUS            lphVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPBOOL              lpbFound
);

///////////////////////////////////////////////////////////////////////////
//
// Memory loading prototypes
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngMemoryLoad
(
    HGENG       hGEng
);

ENGSTATUS EngMemoryUnload
(
    HGENG       hGEng
);

void EngMemorySigShellSort
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wSigLen
);

ENGSTATUS EngMemoryFreeSigGroup
(
    HGENG           hGEng,
    LPMEMSIGGROUP   lpMemSigGroup
);

ENGSTATUS EngMemoryAllocSigGroup
(
	HGENG           hGEng,
	WORD            wNumSigs,
	LPMEMSIGGROUP   lpMemSigGroup
);

ENGSTATUS EngMemoryReadSigGroup
(
    HGENG           hGEng,
    LPVOID          lpvFileInfo,
    WORD            wMemSigSize,
    BOOL            bQuickWordEliminate,
    LPMEMSIGGROUP   lpMemSigGroup
);

///////////////////////////////////////////////////////////////////////////
//
// Memory scanning prototypes
//
///////////////////////////////////////////////////////////////////////////


ENGSTATUS EngScanMemoryAux
(
    HLENG           hLEng,
    LPMEMORYOBJECT  lpMemoryObject,
    DWORD           dwStartAddress,
    DWORD           dwEndAddress,
    DWORD           dwFlags,
    DWORD           dwMemCookie,
    LPBOOL          lpbFoundSig,
    LPHVIRUS        lphVirus
);

BOOL EngScanBuffer
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    WORD            wSigLen,
    LPHVIRUS        lphVirus
);

BOOL EngScanBooBuffer
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    WORD            wSigLen,
    LPHVIRUS        lphVirus
);


// ---------------------------------------
// Miscellaneous Boot/MBR support routines
// ---------------------------------------

VOID EngDecodeSectorCylinder
(                                       // -------------------------------
    WORD    wEncoded,                   // [in] Encoded Sect/Cyl as in MBR
    LPBYTE  lpbySector,                 // [out] Decoded Sector
    LPWORD  lpwCylinder                 // [out] Decoded Cylinder
);                                      // -------------------------------

ENGSTATUS EngGetFloppyType
(                                       // -------------------------------
    HLENG               hLocalEng,      // [in] Local boot engine context
    LPLINEAROBJECT      lprLinear,      // [in] Linear object to analyze
    LPBYTE              lpbySector,     // [in] Boot sector to anaylze
    LPWORD              lpwDiskType     // [out] Disk type
);                                      // -------------------------------

#endif      // _AVENGEL_H
