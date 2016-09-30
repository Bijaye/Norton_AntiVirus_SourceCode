// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __SAVROAM_H_
#define __SAVROAM_H_

#include "SendComHints.h"

// decide whether to declare or extern globals

#if defined(SAVROAM_DEFINE) | defined(SAVROAM_SHIM)

#define SR_EXTERN 
#define SR_DEFINED_AS(a) = a

#else

#define SR_EXTERN extern 
#define SR_DEFINED_AS(a)

#endif

// these are functions exported by savroam.c(pp) - they are also exported
// modules in rtvscan.

#ifdef __cplusplus
extern "C" {
#endif

LRESULT GetHomeDir(LPTSTR lpHomeDir, DWORD dwHomeSize);
DWORD GetAppDataDirectory(DWORD dwFlags, LPTSTR lpDataDir, DWORD nDataDirBytes);

// these aren't exported by rtvscan - these are supplied for rtvscan by RoamShim.cpp

void ReportLastError( char *szMsg, FILE *fFile, BOOL bGetLastError = TRUE);
BOOL KeepSCMHappyOnStop( int nWaitHint );

#ifndef SAVROAM_SHIM

// define flags for GetAppDataDirectory() - these match the values in 
// pscan.h
#define SAV_COMMON_APP_DATA     1
#define SAV_USER_APP_DATA       2

// name of the executable
#ifdef ADMIN_ONLY
#define SZAPPNAME            "RoamAdmn.exe"
// internal name of the service
#define SZSERVICENAME        "RoamAdmn"
// displayed name of the service
#define SZSERVICEDISPLAYNAME "RoamAdmn"
// list of service dependencies - "dep1\0dep2\0\0"
#else
#define SZAPPNAME            "SAVRoam.exe"
// internal name of the service
#define SZSERVICENAME        "SAVRoam"
// displayed name of the service
#define SZSERVICEDISPLAYNAME "SAVRoam"
// list of service dependencies - "dep1\0dep2\0\0"
#endif
#define SZDEPENDENCIES       ""

// forward declarations

struct  tagDOMAIN;
struct  tagSERVERM;
struct  tagCLIENT;

typedef struct tagDOMAIN *PDOMAIN;
typedef struct tagSERVERM *PSERVERM;
typedef struct tagCLIENT *PCLIENT;

typedef struct tagCLIENT
{
    PCLIENT     pNext;
    PCLIENT     pPrev;

    CBA_Addr    Address_0;
    CBA_Addr    Address_1;

    DWORD       dwLastCheckInTime;

    PSERVERM    pParentServer;

    BOOL        bAddress_0;
    BOOL        bAddress_1;

    TCHAR       szClientName[1];    // variable size

} CLIENT;


typedef struct tagSERVERM
{
    PSERVERM     pNext;
    PSERVERM     pPrev;

    CBA_Addr    Address_0;
    CBA_Addr    Address_1;

    PDOMAIN     pOwningDomain;

    DWORD       dwTotalChildServers;
    DWORD       dwTotalClients;

    BOOL        bMasterServer;
    BOOL        bOffline;

    CLIENT      ClientList;

    BOOL        bAddress_0;
    BOOL        bAddress_1;

    TCHAR       szParent[512];

    TCHAR       szServerName[1];    // variable size

} SERVERM;



typedef struct tagDOMAIN
{
    PDOMAIN     pNext;
    PDOMAIN     pPrev;

    DWORD       dwTotalServers;
    DWORD       dwTotalClients;

    PSERVERM     pMasterServer;

    SERVERM      ServerList;

    BOOL        bNoMaster;

    TCHAR       szDomainName[1];    // variable size

} DOMAIN;

#endif			// ifndef(SAVROAM_SHIM)		

// globals

// EXTERN is either nothing or extern depending on whether this is the main file or not
// DEFINED_AS is either = %VALUE% or nothing, complementing the setting of EXTERN

// these are all exported by rtvscan - so only supply if any main savroam build

#ifndef SAVROAM_SHIM

SR_EXTERN HKEY     hMainKey;
SR_EXTERN HKEY     hProductControlKey;

SR_EXTERN char HomeDir[524] SR_DEFINED_AS( "" );

// end rtvscan common exports

#else

// these are provided by RoamShim.cpp

#endif			// defined(SAVROAM_DEFINE)

//  these are supplied for all builds

// small quantity of global data

#define MAX_BUFFER		1024

SR_EXTERN TCHAR   szBuffer[MAX_BUFFER];
SR_EXTERN TCHAR   szBuffer2[MAX_BUFFER];

#ifdef __cplusplus
}
#endif

#endif // __SAVROAM_H_
