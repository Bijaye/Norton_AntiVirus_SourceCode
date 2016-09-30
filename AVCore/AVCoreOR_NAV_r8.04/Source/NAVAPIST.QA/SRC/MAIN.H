// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPIST.QA/VCS/main.h_v   1.6   17 Sep 1998 15:22:22   NMILLER  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVAPIST.QA/VCS/main.h_v  $
// 
//    Rev 1.6   17 Sep 1998 15:22:22   NMILLER
// More CARLIN changes due to API change.
// 
//    Rev 1.5   10 Sep 1998 15:20:10   mmendon
// Consolidated multithread functions.
//
//    Rev 1.4   02 Sep 1998 16:07:52   NMILLER
// Fixed "unexpected end of file" compilation error.
//
//    Rev 1.3   01 Sep 1998 08:06:08   mmendon
// Added multithreading for BR, MBR and memory
//
//    Rev 1.2   18 Aug 1998 17:24:46   mmendon
// Added multithreading for file processing
//
//    Rev 1.1   17 Aug 1998 16:33:26   NMILLER
// Ripped out callback DLL stuff
//
//    Rev 1.0   14 Aug 1998 16:43:00   NMILLER
// Initial revision.
//
//************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

#include "platform.h"
#include "navapi.h"

#ifdef SYM_WIN

//  NAVAPI DLL

#if defined(SYM_WIN32)
    #define NAVAPI_DLL "NAVAPI32.DLL"
#else
    #define NAVAPI_DLL "NAVAPI16.DLL"
#endif

typedef HNAVENGINE          NAVCALLAPI FUNC_ENGINE_INIT(LPTSTR, LPTSTR, LPNAVFILEIO, WORD, WORD, NAVSTATUS *);
typedef NAVSTATUS           NAVCALLAPI FUNC_ENGINE_CLOSE(HNAVENGINE);
#ifdef SYM_WIN32
typedef NAVSTATUS           NAVCALLAPI FUNC_IS_INFECTABLE_FILE(HNAVENGINE, LPVOID, LPTSTR, BOOL, LPBOOL);
#endif
typedef NAVSTATUS           NAVCALLAPI FUNC_SCAN_FILE(HNAVENGINE, LPVOID, LPTSTR, BOOL, HNAVVIRUS*);
typedef NAVSTATUS           NAVCALLAPI FUNC_SCAN_BOOT(HNAVENGINE, char, HNAVVIRUS*);
typedef NAVSTATUS           NAVCALLAPI FUNC_SCAN_MASTER_BOOT(HNAVENGINE, UINT, HNAVVIRUS*);
typedef NAVSTATUS           NAVCALLAPI FUNC_SCAN_MEMORY(HNAVENGINE, HNAVVIRUS*);
typedef NAVSTATUS           NAVCALLAPI FUNC_REPAIR_FILE(HNAVENGINE, LPVOID, LPTSTR);
typedef NAVSTATUS           NAVCALLAPI FUNC_DELETE_FILE(HNAVENGINE, LPVOID, LPTSTR);
typedef NAVSTATUS           NAVCALLAPI FUNC_REPAIR_BOOT(HNAVENGINE, char);
typedef NAVSTATUS           NAVCALLAPI FUNC_REPAIR_MASTER_BOOT(HNAVENGINE, UINT);
typedef NAVSTATUS           NAVCALLAPI FUNC_FREE_VIRUS_HANDLE(HNAVVIRUS);
typedef DWORD               NAVCALLAPI FUNC_GET_VIRUS_INFO(HNAVVIRUS, NAVVIRINFO, LPTSTR, LPDWORD);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIRUS_DEF_COUNT(HNAVENGINE, DWORD*);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIRUS_DEF_MAX_COUNT(HNAVENGINE, DWORD*);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIRUS_DEF_DETECT_COUNT(HNAVENGINE, DWORD*);
typedef HNAVVIRUSDEF        NAVCALLAPI FUNC_LOAD_VIRUS_DEF(HNAVENGINE, DWORD);
typedef VOID                NAVCALLAPI FUNC_RELEASE_VIRUS_DEF(HNAVVIRUSDEF);
typedef DWORD               NAVCALLAPI FUNC_GET_VIRUS_DEF_INFO(HNAVVIRUSDEF, NAVVIRINFO, LPTSTR, LPDWORD);
typedef HNAVVIRUSDEFTABLE   NAVCALLAPI FUNC_LOAD_VIRUS_DEF_TABLE(HNAVENGINE);
typedef VOID                NAVCALLAPI FUNC_RELEASE_VIRUS_DEF_TABLE(HNAVVIRUSDEFTABLE);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIRUS_DEF_TABLE_COUNT(HNAVVIRUSDEFTABLE, DWORD*);
typedef DWORD               NAVCALLAPI FUNC_GET_VIRUS_DEF_TABLE_INFO(HNAVVIRUSDEFTABLE, DWORD, NAVVIRINFO, LPTSTR, LPDWORD);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_NAVAPI_VER_STRING(LPTSTR, UINT);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIR_DB_INFO(HNAVENGINE, LPNAVVIRUSDBINFO);
typedef NAVSTATUS           NAVCALLAPI FUNC_GET_VIR_DB_INFO_EX(HNAVENGINE, LPNAVVIRUSDBINFOEX);

#if defined(SYM_WIN32)
    #define PROC_ADDR_ENG_INIT              "_NAVEngineInit@24"
    #define PROC_ADDR_ENG_CLOSE             "_NAVEngineClose@4"
#ifdef SYM_WIN32
	#define PROC_ADDR_IS_INFECTABLE_FILE    "_NAVIsInfectableFile@20"
#endif
    #define PROC_ADDR_SCAN_FILE             "_NAVScanFile@20"
    #define PROC_ADDR_SCAN_BOOT             "_NAVScanBoot@12"
    #define PROC_ADDR_SCAN_MAST_BOOT        "_NAVScanMasterBoot@12"
    #define PROC_ADDR_SCAN_MEM              "_NAVScanMemory@8"
    #define PROC_ADDR_REP_FILE              "_NAVRepairFile@12"
    #define PROC_ADDR_DEL_FILE              "_NAVDeleteFile@12"
    #define PROC_ADDR_REP_BOOT              "_NAVRepairBoot@8"
    #define PROC_ADDR_REP_MAST_BOOT         "_NAVRepairMasterBoot@8"
    #define PROC_ADDR_FREE_VIR_HANDLE       "_NAVFreeVirusHandle@4"
    #define PROC_ADDR_GET_VIR_INFO          "_NAVGetVirusInfo@16"
    #define PROC_ADDR_GET_VIR_CNT           "_NAVGetVirusDefCount@8"
	#define PROC_ADDR_GET_VIR_MAX_CNT       "_NAVGetVirusDefMaxCount@8"
	#define PROC_ADDR_GET_VIR_DETECT_CNT    "_NAVGetVirusDefDetectCount@8"
    #define PROC_ADDR_LOAD_VIR_DEF          "_NAVLoadVirusDef@8"
    #define PROC_ADDR_REL_VIR_DEF           "_NAVReleaseVirusDef@4"
    #define PROC_ADDR_GET_VIR_DEF_INFO      "_NAVGetVirusDefInfo@16"
    #define PROC_ADDR_LOAD_VIR_DEF_TBL      "_NAVLoadVirusDefTable@4"
    #define PROC_ADDR_REL_VIR_DEF_TBL       "_NAVReleaseVirusDefTable@4"
    #define PROC_ADDR_GET_VIR_DEF_TBL_CNT   "_NAVGetVirusDefTableCount@8"
    #define PROC_ADDR_GET_VIR_DEF_TBL_INFO  "_NAVGetVirusDefTableInfo@20"
    #define PROC_ADDR_GET_NAVAPI_VER_STR    "_NAVGetNavapiVersionString@8"
    #define PROC_ADDR_GET_VIR_DB_INFO       "_NAVGetVirusDBInfo@8"
	#define PROC_ADDR_GET_VIR_DB_INFO_EX    "_NAVGetVirusDBInfoEx@8"
#else
    #define PROC_ADDR_ENG_INIT              "NAVEngineInit"
    #define PROC_ADDR_ENG_CLOSE             "NAVEngineClose"
#ifdef SYM_WIN32
	#define PROC_ADDR_IS_INFECTABLE_FILE    "NAVIsInfectableFile"
#endif
    #define PROC_ADDR_SCAN_FILE             "NAVScanFile"
    #define PROC_ADDR_SCAN_BOOT             "NAVScanBoot"
    #define PROC_ADDR_SCAN_MAST_BOOT        "NAVScanMasterBoot"
    #define PROC_ADDR_SCAN_MEM              "NAVScanMemory"
    #define PROC_ADDR_REP_FILE              "NAVRepairFile"
    #define PROC_ADDR_DEL_FILE              "NAVDeleteFile"
    #define PROC_ADDR_REP_BOOT              "NAVRepairBoot"
    #define PROC_ADDR_REP_MAST_BOOT         "NAVRepairMasterBoot"
    #define PROC_ADDR_FREE_VIR_HANDLE       "NAVFreeVirusHandle"
    #define PROC_ADDR_GET_VIR_INFO          "NAVGetVirusInfo"
    #define PROC_ADDR_GET_VIR_CNT           "NAVGetVirusDefCount"
	#define PROC_ADDR_GET_VIR_MAX_CNT       "NAVGetVirusDefMaxCount"
	#define PROC_ADDR_GET_VIR_DETECT_CNT    "NAVGetVirusDefDetectCount"
    #define PROC_ADDR_LOAD_VIR_DEF          "NAVLoadVirusDef"
    #define PROC_ADDR_REL_VIR_DEF           "NAVReleaseVirusDef"
    #define PROC_ADDR_GET_VIR_DEF_INFO      "NAVGetVirusDefInfo"
    #define PROC_ADDR_LOAD_VIR_DEF_TBL      "NAVLoadVirusDefTable"
    #define PROC_ADDR_REL_VIR_DEF_TBL       "NAVReleaseVirusDefTable"
    #define PROC_ADDR_GET_VIR_DEF_TBL_CNT   "NAVGetVirusDefTableCount"
    #define PROC_ADDR_GET_VIR_DEF_TBL_INFO  "NAVGetVirusDefTableInfo"
    #define PROC_ADDR_GET_NAVAPI_VER_STR    "NAVGetNavapiVersionString"
    #define PROC_ADDR_GET_VIR_DB_INFO       "NAVGetVirusDBInfo"
	#define PROC_ADDR_GET_VIR_DB_INFO_EX    "NAVGetVirusDBInfoEx"
#endif
struct tagNAVAPIDLLFUNC
{
    FUNC_ENGINE_INIT*               lpfnEngineInit;
    FUNC_ENGINE_CLOSE*              lpfnEngineClose;
#ifdef SYM_WIN32
	FUNC_IS_INFECTABLE_FILE*       lpfnIsInfectableFile;
#endif
    FUNC_SCAN_FILE*                lpfnScanFile;
    FUNC_SCAN_BOOT*                lpfnScanBoot;
    FUNC_SCAN_MASTER_BOOT*         lpfnScanMasterBoot;
    FUNC_SCAN_MEMORY*              lpfnScanMemory;
    FUNC_REPAIR_FILE*              lpfnRepairFile;
    FUNC_DELETE_FILE*              lpfnDeleteFile;
    FUNC_REPAIR_BOOT*              lpfnRepairBoot;
    FUNC_REPAIR_MASTER_BOOT*       lpfnRepairMasterBoot;
    FUNC_FREE_VIRUS_HANDLE*        lpfnFreeVirusHandle;
    FUNC_GET_VIRUS_INFO*           lpfnGetVirusInfo;
    FUNC_GET_VIRUS_DEF_COUNT*      lpfnGetVirusDefCount;
	FUNC_GET_VIRUS_DEF_MAX_COUNT*      lpfnGetVirusDefMaxCount;
	FUNC_GET_VIRUS_DEF_DETECT_COUNT*   lpfnGetVirusDefDetectCount;
    FUNC_LOAD_VIRUS_DEF*           lpfnLoadVirusDef;
    FUNC_RELEASE_VIRUS_DEF*        lpfnReleaseVirusDef;
    FUNC_GET_VIRUS_DEF_INFO*       lpfnGetVirusDefInfo;
    FUNC_LOAD_VIRUS_DEF_TABLE*    lpfnLoadVirusDefTable;
    FUNC_RELEASE_VIRUS_DEF_TABLE*  lpfnReleaseVirusDefTable;
    FUNC_GET_VIRUS_DEF_TABLE_COUNT* lpfnGetVirusDefTableCount;
    FUNC_GET_VIRUS_DEF_TABLE_INFO* lpfnGetVirusDefTableInfo;
    FUNC_GET_NAVAPI_VER_STRING*    lpfnGetNavapiVerString;
    FUNC_GET_VIR_DB_INFO*          lpfnGetVirDbInfo;
	FUNC_GET_VIR_DB_INFO_EX*       lpfnGetVirDbInfoEx;
};

typedef struct tagNAVAPIDLLFUNC NAVAPI_FUNC, FAR *LPNAVAPI_FUNC;

#ifdef SYM_WIN32
    struct tagMT_FUNC_PARAMS
    {
        HNAVENGINE      hEngine;
        CLog *          pLog;
        CFileServer *   lpFileServer;
        LPNAVAPI_FUNC   lpNavapiFuncs;

    };

#define MAX_DRIVES 20
UINT  gDriveNumbers[MAX_DRIVES];
TCHAR gDriveLetters[MAX_DRIVES];
WORD gwDriveLetterCounter;
WORD gwDriveNumberCounter;
WORD gwMaxDriveLetters;
WORD gwMaxDriveNumbers;
cCriticalSection CritSectionDLetter;
cCriticalSection CritSectionDNumber;

typedef struct tagMT_FUNC_PARAMS MT_FUNC_PARAMS, FAR *LPMT_FUNC_PARAMS;

#define SCAN_FILES      0
#define REPAIR_FILES    1
#define DELETE_FILES    2
#define SCAN_BR         3
#define REPAIR_BR       4
#define SCAN_MBR        5
#define REPAIR_MBR      7
#define SCAN_MEMORY     8

void WINAPI MTScanFile(LPVOID lpFuncParams);
void WINAPI MTRepairFile(LPVOID lpFuncParams);
void WINAPI MTDeleteFile(LPVOID lpFuncParams);
void WINAPI MTScanBR(LPVOID lpFuncParams);
void WINAPI MTScanMBR(LPVOID lpFuncParams);
void WINAPI MTRepairBR(LPVOID lpFuncParams);
void WINAPI MTRepairMBR(LPVOID lpFuncParams);
void WINAPI MTScanMemory(LPVOID lpFuncParams);


HANDLE hMTSignal;
#endif


#endif //SYM_WIN


#endif
