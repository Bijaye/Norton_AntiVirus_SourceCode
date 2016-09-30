// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   N:/NAVAP/VCS/APSTRS.CPv   1.12   29 Sep 1998 18:30:34   RStanev  $
//
// Description:
//      This file defines translatable strings.
//
// Contains:
//      ...
//
// See Also:
//      evstrs.cpp, krnlstrs.cpp, uistrs.cpp
//
// Notes:
//
//***************************************************************************
// $Log:   N:/NAVAP/VCS/APSTRS.CPv  $
// 
//    Rev 1.12   29 Sep 1998 18:30:34   RStanev
// dded szNAVAPCFGdwFastCacheResetTimeout.
//
//    Rev 1.11   02 Jun 1998 00:15:22   RStanev
// Made szNAVAPCFGbFileSharing[] global.
//
//    Rev 1.10   17 Feb 1998 22:55:24   RStanev
// Merged Roswell and Tesla enhancements.
//
//    Rev 1.9   17 Feb 1998 22:53:12   RStanev
// Updated copyright.  Cleaned up.
//
//    Rev 1.8   12 Dec 1997 17:10:20   CEATON
// Added registry keys for file sharing and debug log.
//
//    Rev 1.7   13 Aug 1997 18:29:30   MKEATIN
// Added szNavexInfFileName[].
//
//    Rev 1.6   02 Jul 1997 13:24:58   JBRENNA
// Change the NAV??_30_AP1 strings to NAV??_50_AP1. The version was bumped to
// match our internal version number.
//
//    Rev 1.5   06 Jun 1997 16:42:04   RStanev
// Added szNAVAPDefPathRegSection[] and szNAVAPDefPathRegKeyname[].
//
//    Rev 1.4   02 Jun 1997 15:11:18   RStanev
// Updated szExitWindowsName[] and szExitWindowsDLL[].
//
//    Rev 1.3   22 May 1997 16:19:40   RStanev
// Added szCUSTOMALERTFileName[].
//
//    Rev 1.2   21 May 1997 04:39:30   RStanev
// Removed unreferenced strings and added a few new strings.
//
//    Rev 1.1   15 May 1997 02:52:40   RStanev
// Minor changes.
//
//    Rev 1.0   09 Apr 1997 12:23:22   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apstrs.h"

//---------------------------------------------------------------------------
//
// Names of registry keys and internal registry settings.  None of these
// strings should be translated.
//
//---------------------------------------------------------------------------
TCHAR szNAVAPInstallPathRegSection[] = _T("InstalledApps");
#if defined(SYM_NTK)
TCHAR szNAVAPInstallPathRegKeyname[] = _T("NAVNT");
#elif defined(SYM_VXD)
TCHAR szNAVAPInstallPathRegKeyname[] = _T("NAV95");
#endif

TCHAR szNAVAPDefPathRegSection[] = _T("SharedDefs");
#if defined(SYM_NTK)
TCHAR szNAVAPDefPathRegKeyname[] = _T("NAVNT_50_AP1");
#elif defined(SYM_VXD)
TCHAR szNAVAPDefPathRegKeyname[] = _T("NAV95_50_AP1");
#endif

#if defined(SYM_NTK)
TCHAR szNAVAPCFGProductName[]    = _T("Norton AntiVirus NT");
#elif defined(SYM_VXD)
TCHAR szNAVAPCFGProductName[]    = _T("Norton AntiVirus");
#endif
TCHAR szNAVAPCFGProjectName[]    = _T("Auto-Protect");
TCHAR szNAVAPCFGSectionName[]    = _T("InternalSettings");

#if defined(SYM_NTK)
TCHAR szNAVAPStackSwappingLevelRegKeyname[] = _T("StackSwappingLevel");
#endif

#if defined(SYM_VXD)

TCHAR szNAVAPCFGbRealModeAPCommunication[]       = _T("RealModeAPCommunication");
TCHAR szNAVAPCFGbRealModeAPEmulation[]           = _T("RealModeAPEmulation");
TCHAR szNAVAPCFGbRealModeCriticalErrorHandler[]  = _T("RealModeCriticalErrorHandler");
TCHAR szNAVAPCFGbControlBreakHandler[]           = _T("ControlBreakHandler");
TCHAR szNAVAPCFGbRegisterAppyTimeForAgent[]      = _T("RegisterAppyTimeForAgent");
TCHAR szNAVAPCFGbExecuteAgent[]                  = _T("ExecuteAgent");
TCHAR szNAVAPCFGbShowUIWhileScanningBoot[]       = _T("ShowUIWhileScanningBoot");
TCHAR szNAVAPCFGdwDeviceShutdownDelayTime[]      = _T("DeviceShutdownDelayTime");

#elif defined(SYM_NTK)

TCHAR szNAVAPCFGdwSIDLength[]                    = _T("SIDLength");

#endif

TCHAR szNAVAPCFGbFileSharing[]                   = _T("FileSharing");
TCHAR szNAVAPCFGdwFileCacheEntries[]             = _T("FileCacheEntries");
TCHAR szNAVAPCFGulFileCacheFilterFlags[]         = _T("FileCacheFilterFlags");
TCHAR szNAVAPCFGbIgnoreInt13DeviceCount[]        = _T("IgnoreInt13DeviceCount");
TCHAR szNAVAPCFGabyForcedInt13DeviceList[]       = _T("ForcedInt13DeviceList");
TCHAR szNAVAPCFGabyHostileInt13DeviceList[]      = _T("HostileInt13DeviceList");
TCHAR szNAVAPCFGbUsePAMEngine[]                  = _T("UsePAMEngine");
TCHAR szNAVAPCFGbExclusionsEnabled[]             = _T("ExclusionsEnabled");
TCHAR szNAVAPCFGbSendInfoPackets[]               = _T("SendInfoPackets");
TCHAR szNAVAPCFGbTruncateProgramFileExtensions[] = _T("TruncateProgramFileExtensions");
TCHAR szNAVAPCFGdwFastCacheResetTimeout[]        = _T("FastCacheResetTimeout");

TCHAR szNAVAPCFGbDeleteInfectedOnCreate[]        = _T("DeleteInfectedOnCreate");

//---------------------------------------------------------------------------
//
// Names of external NAV files.  No need of translation.
//
//---------------------------------------------------------------------------
TCHAR szNAVOPTSFileName[]       = _T("NAVOPTS.DAT");
TCHAR szNAVDEFOPTSFileName[]    = _T("NAVDEF.DAT");
TCHAR szNAVSTARTFileName[]      = _T("NAVSTART.DAT");
TCHAR szNAVDEFSTARTFileName[]   = _T("NAVSTDEF.DAT");
TCHAR szCUSTOMALERTFileName[]   = _T("LONGMSG.TXT");

TCHAR szFileInocFileName[]      = _T("NAVINOC.DAT");
TCHAR szFileInocLFileName[]     = _T("NAVINOCL.DAT");

TCHAR szVirusDatabaseFileName[] = _T("VIRSCAN1.DAT");
TCHAR szVirusInfoFileName[]     = _T("VIRSCAN.INF");
TCHAR szNavexInfFileName[]      = _T("NAVEX15.INF");

#if defined(SYM_VXD)

TCHAR szNAVAPFileName[]         = _T("NAVAP.VXD");
TCHAR szNAVAPWFileName[]        = _T("NAVAPW32.EXE");

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Other internal strings.  These strings shouldn't be changed without
// consulting a developer.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

WCHAR szImagePathValueName[]    = L"ImagePath";
WCHAR szSystemRoot[]            = L"\\SystemRoot\\";
WCHAR szSystem32Drivers[]       = L"\\SystemRoot\\System32\\Drivers\\";
WCHAR szDriverExtension[]       = L".SYS";

WCHAR szSYMEVENTDeviceName[]    = L"\\Device\\SymEvent";

WCHAR szNAVAPNTDeviceName[]     = L"\\Device\\NAVAP";
WCHAR szNAVAPNTDeviceCommName[] = L"\\DosDevices\\NAVAP";

#elif defined(SYM_VXD)

TCHAR szExitWindowsName[]       = _T("EXITWINDOWS");
TCHAR szExitWindowsDLL[]        = _T("USER");

#endif
