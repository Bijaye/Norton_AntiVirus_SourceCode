// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/apstrs.h_v   1.10   29 Sep 1998 18:30:04   RStanev  $
//
// Description:
//      This file declares translatable strings.
//
// Contains:
//      -
//
// See Also:
//      apstrs.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/apstrs.h_v  $
// 
//    Rev 1.10   29 Sep 1998 18:30:04   RStanev
// Added szFastCacheResetTimeout.
//
//    Rev 1.9   02 Jun 1998 00:16:22   RStanev
// Made szNAVAPCFGbFileSharing[] global.
//
//    Rev 1.8   17 Feb 1998 22:55:56   RStanev
// Merged Roswell and Tesla enhancements.
//
//    Rev 1.7   17 Feb 1998 22:44:52   RStanev
// Updated copyright.  Cleaned up.
//
//    Rev 1.6   12 Dec 1997 17:10:38   CEATON
// Added registry keys for file sharing and debug log.
//
//    Rev 1.5   13 Aug 1997 18:28:16   MKEATIN
// Added szNavexInfFileName[].
//
//    Rev 1.4   06 Jun 1997 16:41:30   RStanev
// Added szNAVAPDefPathRegSection[] and szNAVAPDefPathRegKeyname[].
//
//    Rev 1.3   22 May 1997 16:20:26   RStanev
// Added szCUSTOMALERTFileName[].
//
//    Rev 1.2   21 May 1997 04:40:46   RStanev
// Removed unreferenced strings and added a few new strings.
//
//    Rev 1.1   15 May 1997 02:53:14   RStanev
// Minor changes.
//
//    Rev 1.0   09 Apr 1997 12:21:32   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _APSTRS_H_
#define _APSTRS_H_

//---------------------------------------------------------------------------
//
// Names of registry keys and internal registry settings.  None of these
// strings should be translated.
//
//---------------------------------------------------------------------------
extern TCHAR szNAVAPInstallPathRegSection[];
extern TCHAR szNAVAPInstallPathRegKeyname[];

extern TCHAR szNAVAPDefPathRegSection[];
extern TCHAR szNAVAPDefPathRegKeyname[];

extern TCHAR szNAVAPCFGProductName[];
extern TCHAR szNAVAPCFGProjectName[];
extern TCHAR szNAVAPCFGSectionName[];

#if defined(SYM_NTK)
extern TCHAR szNAVAPStackSwappingLevelRegKeyname[];
#endif

#if defined(SYM_VXD)

extern TCHAR szNAVAPCFGbRealModeAPCommunication[];
extern TCHAR szNAVAPCFGbRealModeAPEmulation[];
extern TCHAR szNAVAPCFGbRealModeCriticalErrorHandler[];
extern TCHAR szNAVAPCFGbControlBreakHandler[];
extern TCHAR szNAVAPCFGbRegisterAppyTimeForAgent[];
extern TCHAR szNAVAPCFGbExecuteAgent[];
extern TCHAR szNAVAPCFGbShowUIWhileScanningBoot[];
extern TCHAR szNAVAPCFGdwDeviceShutdownDelayTime[];

#elif defined(SYM_NTK)

extern TCHAR szNAVAPCFGdwSIDLength[];

#endif

extern TCHAR szNAVAPCFGbFileSharing[];
extern TCHAR szNAVAPCFGdwFileCacheEntries[];
extern TCHAR szNAVAPCFGulFileCacheFilterFlags[];
extern TCHAR szNAVAPCFGbIgnoreInt13DeviceCount[];
extern TCHAR szNAVAPCFGabyForcedInt13DeviceList[];
extern TCHAR szNAVAPCFGabyHostileInt13DeviceList[];
extern TCHAR szNAVAPCFGbUsePAMEngine[];
extern TCHAR szNAVAPCFGbExclusionsEnabled[];
extern TCHAR szNAVAPCFGbSendInfoPackets[];
extern TCHAR szNAVAPCFGbTruncateProgramFileExtensions[];
extern TCHAR szNAVAPCFGdwFastCacheResetTimeout[];

extern TCHAR szNAVAPCFGbDeleteInfectedOnCreate[];

//---------------------------------------------------------------------------
//
// Names of external NAV files.  No need of translation.
//
//---------------------------------------------------------------------------
extern TCHAR szNAVOPTSFileName[];
extern TCHAR szNAVDEFOPTSFileName[];
extern TCHAR szNAVSTARTFileName[];
extern TCHAR szNAVDEFSTARTFileName[];
extern TCHAR szCUSTOMALERTFileName[];

extern TCHAR szFileInocFileName[];
extern TCHAR szFileInocLFileName[];

extern TCHAR szVirusDatabaseFileName[];
extern TCHAR szVirusInfoFileName[];
extern TCHAR szNavexInfFileName[];

#if defined(SYM_VXD)

extern TCHAR szNAVAPFileName[];
extern TCHAR szNAVAPWFileName[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Other internal strings.  These strings shouldn't be changed without
// consulting a developer.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

extern WCHAR szImagePathValueName[];
extern WCHAR szSystemRoot[];
extern WCHAR szSystem32Drivers[];
extern WCHAR szDriverExtension[];

extern WCHAR szSYMEVENTDeviceName[];

extern WCHAR szNAVAPNTDeviceName[];
extern WCHAR szNAVAPNTDeviceCommName[];

#elif defined(SYM_VXD)

extern TCHAR szExitWindowsName[];
extern TCHAR szExitWindowsDLL[];

#endif

#endif // #ifndef _APSTRS_H_
