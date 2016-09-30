/////////////////////////////////////////////////////////////////////////////
//
// nlm_nav.h
// Map NAVAPI function names to NLM specific names.
// Copyright 1998 by Symantec Corporation.  All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPI/VCS/nlm_nav.h_v   1.3   02 Dec 1998 19:46:08   MKEATIN  $
// $Log:   S:/NAVAPI/VCS/nlm_nav.h_v  $
// 
//    Rev 1.3   02 Dec 1998 19:46:08   MKEATIN
// reverted back to revision 1.1
// 
//    Rev 1.1   13 Aug 1998 17:18:38   DHERTEL
// Changed the name of NAVLIB.NLM to NAVAPI.NLM, and renamed NAVLIB.* source
// files to NAVAPI (where possible) or NAVAPINW (where necessary)
// 
//    Rev 1.0   30 Jul 1998 21:53:20   DHERTEL
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined(__NLM_NAV_H)
#define __NLM_NAV_H

// NLM_NAVAPI is defined only for the compilation of the NLM version
// of NAVAPI.LIB.
//
// Originally, we were going to ship the NAVAPI as a static library
// for the NLM platform.  However, Intel wanted us to package it 
// as an NLM.  Thus, we invented NAVAPI.NLM to server as a glue layer
// above the static library.  NAVAPI.NLM must export the original
// function names (NAVEngineInit, etc.), so we use this preprocessor
// trick to change the names in the NLM static library.

#ifdef NLM_NAVAPI

#define NAVEngineInit             NLM_NAVEngineInit
#define NAVEngineClose            NLM_NAVEngineClose
#define NAVScanFile               NLM_NAVScanFile
#define NAVScanBoot               NLM_NAVScanBoot
#define NAVScanMasterBoot         NLM_NAVScanMasterBoot
#define NAVScanMemory             NLM_NAVScanMemory
#define NAVRepairFile             NLM_NAVRepairFile
#define NAVDeleteFile             NLM_NAVDeleteFile
#define NAVRepairBoot             NLM_NAVRepairBoot
#define NAVRepairMasterBoot       NLM_NAVRepairMasterBoot
#define NAVFreeVirusHandle        NLM_NAVFreeVirusHandle
#define NAVGetVirusInfo           NLM_NAVGetVirusInfo
#define NAVGetVirusDefCount       NLM_NAVGetVirusDefCount
#define NAVLoadVirusDef           NLM_NAVLoadVirusDef
#define NAVReleaseVirusDef        NLM_NAVReleaseVirusDef
#define NAVGetVirusDefInfo        NLM_NAVGetVirusDefInfo
#define NAVLoadVirusDefTable      NLM_NAVLoadVirusDefTable
#define NAVReleaseVirusDefTable   NLM_NAVReleaseVirusDefTable
#define NAVGetVirusDefTableCount  NLM_NAVGetVirusDefTableCount
#define NAVGetVirusDefTableInfo   NLM_NAVGetVirusDefTableInfo
#define NAVGetNavapiVersionString NLM_NAVGetNavapiVersionString
#define NAVGetVirusDBInfo         NLM_NAVGetVirusDBInfo

#endif

#endif // if !defined(__NLM_NAV_H)
