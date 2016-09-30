// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/APCOMM.H_v   1.13   02 Jun 1998 01:05:36   RStanev  $
//
// Description:
//      This file defines shared structures and constants needed for
//      communication between the NAVAP driver and user-mode applications.
//
//      External communications are based on the exported functions by
//      NAVAP32.DLL.  Internal communications are based on the DriverComm
//      module in SymKrnl.
//
// Contains:
//      NAVAPGetEnabledDisabled             (SYM_WIN32)
//      NAVAPSetEnabledDisabled             (SYM_WIN32)
//      NAVAPProtectProcess                 (SYM_WIN32)
//      NAVAPUnprotectProcess               (SYM_WIN32)
//
//      NAVAPiGetEnabledDisabled            (SYM_WIN32)
//      NAVAPiSetEnabledDisabled            (SYM_WIN32)
//      NAVAPiProtectProcess                (SYM_WIN32)
//      NAVAPiUnprotectProcess              (SYM_WIN32)
//
//      NAVAPiDecodeUserInfoSubPacket()     (SYM_WIN32)
//
// See Also:
//      APCOMM project
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/APCOMM.H_v  $
// 
//    Rev 1.13   02 Jun 1998 01:05:36   RStanev
// Merged Roswell fixes.
//
//    Rev 1.12   21 May 1998 22:20:32   jtaylor
// Added support for Dec Aplha.  It does not mangle the function names in this dll.
//
//    Rev 1.11   16 Mar 1998 19:46:54   DALLEE
// Updated the defines for exported function names.
// Added the underscores and @paramsize mangling.
//
//    Rev 1.10   19 Feb 1998 16:40:52   RStanev
// Removed unknown virus detection.
//
//    Rev 1.9   17 Feb 1998 22:58:00   RStanev
// Updated copyright.
//
//    Rev 1.8   28 Jul 1997 19:39:34   RStanev
// Enhanced NAVAP_STATUSPACKET to contain information about initialized
// modules.
//
//    Rev 1.7   10 Jun 1997 14:43:14   RStanev
// Added dwSize to NAVAP_STATUSPACKET.
//
//    Rev 1.6   10 Jun 1997 14:26:42   RStanev
// Added dwDriverImageUnloadable to NAVAP_STATUSPACKET.
//
//    Rev 1.5   04 Jun 1997 16:01:34   RStanev
// Fixed a typo.
//
//    Rev 1.4   04 Jun 1997 15:54:42   RStanev
// Added dwInInocDB to NAVAP_EVENTPROGRESSREPORTPACKET.
//
//    Rev 1.3   29 May 1997 11:00:14   RStanev
// Added dwConfigurationPathIndex to NAVAP_EVENTPROGRESSREPORTPACKET.
//
//    Rev 1.2   15 May 1997 00:32:12   RStanev
// Added more APCOMM constants.
//
//    Rev 1.1   16 Apr 1997 11:04:38   RStanev
// Cleaned up.  Now supports the new APCOMM project.
//
//    Rev 1.0   09 Apr 1997 12:29:48   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef __cplusplus
#error Need a C++ compiler!
#endif  // __cplusplus

#ifndef _APCOMM_H_
#define _APCOMM_H_

//---------------------------------------------------------------------------
//
// NAVAP_COMMUNICATION_NAME is a constant which should be used by user
// mode applications when creating a DriverComm class.
//
//---------------------------------------------------------------------------
#define NAVAP_COMMUNICATION_NAME    _T("NAVAP")

//---------------------------------------------------------------------------
//
// Define DriverComm service request codes to be sent by user-mode
// applications to NAVAP.
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_GETENABLEDDISABLED   0x01
#define NAVAP_COMM_SETENABLEDDISABLED   0x02
#define NAVAP_COMM_PROTECTPROCESS       0x03
#define NAVAP_COMM_UNPROTECTPROCESS     0x04
#define NAVAP_COMM_RELOADCONFIG         0x05
#define NAVAP_COMM_LOADAVAPI            0x06
#define NAVAP_COMM_UNLOADAVAPI          0x07
#define NAVAP_COMM_GETSTATUS            0x08
#define NAVAP_COMM_CONTROLLEDINIT       0x09
#define NAVAP_COMM_CONTROLLEDDEINIT     0x0A
#define NAVAP_COMM_WIN95AGENTINIT       0x0B
#define NAVAP_COMM_WIN95AGENTDEINIT     0x0C

//---------------------------------------------------------------------------
//
// Define DriverComm service request codes to be sent by NAVAP to an
// application (its user mode SO).
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_EVENTPROGRESSREPORT  0x01
#define NAVAP_COMM_SCANMOUNTEDDEVICE    0x02
#define NAVAP_COMM_SHUTDOWN             0x03

//---------------------------------------------------------------------------
//
// Define common data structures used when sending packets.
//
// NAVAP_USERINFOSUBPACKET is included in several of the packets which are
// send by NAVAP to its SO.  When such packet is received, the SO should
// examine the value of bUseAsSIDInsteadOfString, and if it is FALSE, it
// should retrieve the user information as a string from rUserInfo.szUserID.
// The max length in characters of the string will be sizeof(rUserInfo) /
// sizeof(WCHAR) - 1.  Whe bUseAsSIDInsteadOfString is TRUE, then the user
// should use the LookupAccountSid() function to retrieve user information,
// with the SID located at rUserInfo.Sid.
//
// The function NAVAPiDecodeUserInfoSubPacket() should be used to decode
// NAVAP_USERINFOSUBPACKET structures.
//
// NAVAP_SYSTEMTIME is a user-to-kernel and kernel-to-user mode portable
// structure containing the same data as SYSTEMTIME.
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
        BYTE  fixed_union_size_padding[512 * sizeof(WCHAR)];
        } rUserInfo;

    } NAVAP_USERINFOSUBPACKET, *PNAVAP_USERINFOSUBPACKET;

typedef struct
    {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    } NAVAP_SYSTEMTIME, *PNAVAP_SYSTEMTIME;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define NAVAP status packet.  This packet is returned by the NAVAP driver
// when an application has requested to get information on driver's status
// by requesting service NAVAP_COMM_GETSTATUS.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    DWORD dwErrorStatus;

    DWORD dwSize;

    NAVAP_SYSTEMTIME rSystemTimeAtStartup;

    DWORD dwDriverImageUnloadable;

    DWORD dwInitializedModules;

    DWORD dwFilesScannedSinceStartup;

    DWORD dwLastFileScannedIndex;
    DWORD dwLastInfectedFileFoundIndex;
    DWORD dwLastVirusFoundIndex;

    WCHAR szStringArray[1];

    } NAVAP_STATUSPACKET, *PNAVAP_STATUSPACKET;

#include <poppack.h>

#define NAVAP_INITIALIZEDMODULES_AVAPI 0x00000001

//---------------------------------------------------------------------------
//
// Define NAVAP event progress report packet.  This packet is used by the
// driver to notify its SO of the actions that it is taking while processing
// events.  The service code for the notification is
// NAVAP_COMM_EVENTPROGRESSREPORT.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    NAVAP_SYSTEMTIME rSystemTime;

    NAVAP_USERINFOSUBPACKET rUser;

    DWORD dwSequenceRecognizer;
    DWORD dwLastSequencePacket;

    DWORD dwAVContext;
    DWORD dwAction;
    DWORD dwActionCompleted;

    DWORD dwConfigurationPathIndex;
    DWORD dwVirusNameIndex;
    DWORD dwFileDeviceInTroubleIndex;
    union
        {
        DWORD dwFileCausingTroubleIndex;
        DWORD dwMovedRenamedFileIndex;
        };

    WCHAR szStringArray[1];

    } NAVAP_EVENTPROGRESSREPORTPACKET, *PNAVAP_EVENTPROGRESSREPORTPACKET;

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
    NAVAP_SYSTEMTIME rSystemTime;

    NAVAP_USERINFOSUBPACKET rUser;

    WCHAR szDevice[3];

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

#ifdef _M_ALPHA
    #define szNAVAPGetEnabledDisabled   "gretz1"
    #define szNAVAPSetEnabledDisabled   "gretz2"
    #define szNAVAPProtectProcess       "gretz3"
    #define szNAVAPUnprotectProcess     "gretz4"
#else
    #define szNAVAPGetEnabledDisabled   "_gretz1@4"
    #define szNAVAPSetEnabledDisabled   "_gretz2@4"
    #define szNAVAPProtectProcess       "_gretz3@0"
    #define szNAVAPUnprotectProcess     "_gretz4@0"
#endif

extern "C" {

BOOL SYM_EXPORT WINAPI NAVAPGetEnabledDisabled (
    PDWORD pdwStatus
    );

BOOL SYM_EXPORT WINAPI NAVAPSetEnabledDisabled (
    DWORD dwStatus
    );

BOOL SYM_EXPORT WINAPI NAVAPProtectProcess (
    VOID
    );

BOOL SYM_EXPORT WINAPI NAVAPUnprotectProcess (
    VOID
    );

}

//---------------------------------------------------------------------------
//
// Prototype APCOMM Win32 functions internal to NAV modules.
//
//---------------------------------------------------------------------------
BOOL NAVAPiGetEnabledDisabled (
    PDWORD pdwStatus
    );

BOOL NAVAPiSetEnabledDisabled (
    DWORD dwStatus
    );

BOOL NAVAPiProtectProcess (
    VOID
    );

BOOL NAVAPiUnprotectProcess (
    VOID
    );

BOOL NAVAPiDecodeUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    );

#endif // #if defined(SYM_WIN32)

#endif // #ifndef _APCOMM_H_
