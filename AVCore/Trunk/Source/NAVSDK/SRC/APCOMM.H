// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/APCOMM.H_v   1.0   02 Dec 1997 14:23:24   DALLEE  $
//
// Description:
//      This file defines shared structures and constants needed for
//      communication between NAVAP driver and user-mode applications.
//      The communication is based on the DriverComm library in SymKrnl.
//
// Contains:
//      DecodeNAVAPUserInfoSubPacket()
//
// See Also:
//      drvcomm.h, actions.h
//
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/APCOMM.H_v  $
// 
//    Rev 1.0   02 Dec 1997 14:23:24   DALLEE
// Initial revision.
// 
//    Rev 1.2   04 Nov 1997 17:09:34   tcashin
// This is the right APCOMM.H. It combines the one from 
// Gryphon/Marconi with the strings from the exports in the
// new NAVAP32.DLL method. Talking to both the old and new
// VxDs works now.
// 
//    Rev 1.0   08 Nov 1996 11:35:02   DKEEFER
// Initial revision
// 
//    Rev 1.8   27 Oct 1996 18:37:04   RSTANEV
// Added dwMovedRenamedFileIndex and united with dwFileCausingTroubleIndex.
//
//    Rev 1.7   22 Oct 1996 00:24:20   RSTANEV
// Prototyped DecodeNAVAPUserInfoSubPacket().  Enforced C++ usage of this
// file when compiling for SYM_WIN32.
//
//    Rev 1.6   05 Aug 1996 22:50:06   RSTANEV
// Removed the communication IDs and structures for NAVAP's log and
// alert packets.  Added new IDs and structures for the latest and
// greatest configuration reloading architecture.
//
//    Rev 1.5   12 Jul 1996 01:58:54   RSTANEV
// Added NAVAP_INFOPACKET.
//
//    Rev 1.4   24 May 1996 08:24:50   RSTANEV
// Changed the x in NAVAP_COMM_PROTECTx and NAVAP_COMM_UNPROTECTx from
// THREAD to PROCESS.
//
//    Rev 1.3   16 May 1996 13:22:16   RSTANEV
// Changed the way NAVAP packets carry user IDs.
//
//    Rev 1.2   11 May 1996 14:30:04   RSTANEV
// Added structures and IDs needed for scanning mounted devices.
//
//    Rev 1.1   23 Apr 1996 14:09:16   RSTANEV
// Changed constants and structures used for reloading the configuration.
//
//    Rev 1.0   01 Apr 1996 21:44:08   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef _APCOMM_H_
#define _APCOMM_H_

//---------------------------------------------------------------------------
//
// Include ctsn.h.  This is needed for SIZE_VIRUSNAME.
//
//---------------------------------------------------------------------------
#include "ctsn.h"

//---------------------------------------------------------------------------
//
// NAVAP_COMMUNICATION_NAME is a constant which should be used by user
// mode applications when creating a DriverComm class.
//
//---------------------------------------------------------------------------
#define NAVAP_COMMUNICATION_NAME    _T("NAVAP")

//---------------------------------------------------------------------------
//
// Define commands to be sent by NAVAP to user-mode applications, and
// commands to be sent by user-mode applications to NAVAP.  Both
// communication directions should share the DIOCTL_SYM_USER?? constants.
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_GETVERSION        DIOCTL_SYM_USER00  // app   -> NAVAP
#define NAVAP_COMM_GETSTATUS         DIOCTL_SYM_USER01  // app   -> NAVAP
#define NAVAP_COMM_SETSTATUS         DIOCTL_SYM_USER02  // app   -> NAVAP
#define NAVAP_COMM_CONTROLLEDINIT    DIOCTL_SYM_USER03  // app   -> NAVAP
#define NAVAP_COMM_CONTROLLEDDEINIT  DIOCTL_SYM_USER04  // app   -> NAVAP
#define NAVAP_COMM_RELOADCONFIG      DIOCTL_SYM_USER05  // app   -> NAVAP
#define NAVAP_COMM_LOADAVAPI         DIOCTL_SYM_USER06  // app   -> NAVAP
#define NAVAP_COMM_UNLOADAVAPI       DIOCTL_SYM_USER07  // app   -> NAVAP
#define NAVAP_COMM_UNPROTECTPROCESS  DIOCTL_SYM_USER08  // app   -> NAVAP
#define NAVAP_COMM_PROTECTPROCESS    DIOCTL_SYM_USER09  // app   -> NAVAP
#define NAVAP_COMM_INFO              DIOCTL_SYM_USER10  // NAVAP -> app
#define NAVAP_COMM_SCANMOUNTEDDEVICE DIOCTL_SYM_USER11  // NAVAP -> app

//---------------------------------------------------------------------------
//
// Define common data structures used when sending packets.
//
// NAVAP_USERINFOSUBPACKET is included in several of the packets which are
// send by NAVAP to app.  When such packet is received, the app should
// examine the value of bUseAsSIDInsteadOfString, and if it is FALSE, it
// should retrieve the user information as a string from rUserInfo.szUserID.
// The max length in characters of the string will be sizeof(rUserInfo) /
// sizeof(WCHAR) - 1.  Whe bUseAsSIDInsteadOfString is TRUE, then the user
// should use the LookupAccountSid() function to retrieve user information,
// with the SID located at rUserInfo.Sid.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    DWORD bUseAsSIDInsteadOfString;

    union
        {
        WCHAR szUserID[1];
        SID   Sid;
        BYTE  fixed_union_size_padding[SYM_MAX_PATH * sizeof(WCHAR)];
        } rUserInfo;

    } NAVAP_USERINFOSUBPACKET, *PNAVAP_USERINFOSUBPACKET;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define data and structures exchanged with the NAVAP_COMM_RELOADCONFIG
// command.
//
// Note: Currently NAVAP_RELOADCONFIGPACKET is ignored by NAVAP.  Whenever
//       the configuration is reloaded, NAVAP_RELOADCONFIG_ALL is used, and
//       the location of the currently loaded configuration is used.
//
//---------------------------------------------------------------------------
#define NAVAP_RELOADCONFIG_OPTIONS      0x00000001
#define NAVAP_RELOADCONFIG_EXCLUSIONS   0x00000002
#define NAVAP_RELOADCONFIG_ALL          0xFFFFFFFF

#include <pshpack1.h>

typedef struct
    {
    DWORD dwReloadFlags;
    WCHAR szConfigurationPath[SYM_MAX_PATH];
    } NAVAP_RELOADCONFIGPACKET, *PNAVAP_RELOADCONFIGPACKET;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define NAVAP information packet.  This packet is used by the NAVAP driver
// to notify its user mode counterpart of the actions that it is taking.
// The notification command is NAVAP_COMM_INFO.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    struct {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
    } rSystemTime;

    NAVAP_USERINFOSUBPACKET rUser;

    DWORD dwSequenceRecognizer;
    DWORD dwLastSequencePacket;

    DWORD dwAVContext;
    DWORD dwAction;
    DWORD dwActionCompleted;

    DWORD dwVirusNameIndex;
    DWORD dwFileDeviceInTroubleIndex;
    union
        {
        DWORD dwFileCausingTroubleIndex;
        DWORD dwMovedRenamedFileIndex;
        };

    WCHAR szStringArray[1];

    } NAVAP_INFOPACKET, *PNAVAP_INFOPACKET;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define device mount notification packet.  This packet is used by
// NAVAP_COMM_SCANMOUNTEDDEVICE.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    struct {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
    } rSystemTime;

    WCHAR szDevice[3];

    NAVAP_USERINFOSUBPACKET rUser;

    } NAVAP_SCANMOUNTEDDEVICEPACKET, *PNAVAP_SCANMOUNTEDDEVICEPACKET;

#include <poppack.h>

#if defined(SYM_WIN32)

//---------------------------------------------------------------------------
//
// Prototype APCOMM Win32 functions exported by NAVAP32.DLL.
//
//---------------------------------------------------------------------------
#define NAVAPGetEnabledDisabled     gretz1
#define NAVAPSetEnabledDisabled     gretz2
#define NAVAPProtectProcess         gretz3
#define NAVAPUnprotectProcess       gretz4

#define szNAVAPGetEnabledDisabled   "_gretz1@4"
#define szNAVAPSetEnabledDisabled   "_gretz2@4"
#define szNAVAPProtectProcess       "_gretz3@0"
#define szNAVAPUnprotectProcess     "_gretz4@0"

#ifndef __cplusplus
#error Need a C++ compiler!
#endif  // __cplusplus

BOOL DecodeNAVAPUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    );

#endif

#endif // #ifndef _APCOMM_H_
