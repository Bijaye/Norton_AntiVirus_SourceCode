// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a library simpler. All files within this library are compiled with the SCSAUDIT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this library. This way any other project whose source files include this file see 
// SCSAUDIT_API functions as being imported from a library, wheras this library sees symbols
// defined with this macro as being exported.

#ifdef SCSAUDIT_EXPORTS
#define SCSAUDIT_API
#else
#define SCSAUDIT_API extern
#endif

struct  tagDESIRED_STATE;
struct  tagACTUAL_STATE;

typedef struct tagDESIRED_STATE *PDESIRED_STATE;
typedef struct tagACTUAL_STATE *PACTUAL_STATE;

typedef struct tagDESIRED_STATE
{

DWORD dwExpectedOnOff;                  // = 1;    A/P enabled                   // A/P enabled
DWORD dwExpectedWrites;                 // = 1;    scan on modify                // scan on modify
DWORD dwExpectedExecs;                  // = 1;    scan on exec                  // scan on exec
DWORD dwExpectedReads;                  // = 1;    scan on open                  // scan on open
DWORD dwExpectedFileType;               // = 0;    scan all extensions           // scan all extensions
DWORD dwExpectedHeuristics;             // = 1;    heuristic engine enabled      // heuristic engine enabled
DWORD dwMinHeuristicsLevel;             // = 2;    default level                      // default level

int nMinPattVerYear;                    // = 2001;
int nMinPattVerMonth;                   // = 11;  
int nMinPattVerDay;                     // = 6;   
int nMinPattVerRev;                     // = 1;   
                                             
int nMaxPattVerYear;                    // = 2001;
int nMaxPattVerMonth;                   // = 11;  
int nMaxPattVerDay;                     // = 8;   
int nMaxPattVerRev;                     // = 1;   

DWORD dwMinEngineVersion;               // = 0x04010006;
DWORD dwMaxEngineVersion;               // = 0x04010103;

int nMinProductVersion;                 // = 7;
int nMinProductSubver;                  // = 50;
int nMinProductBuild;                   // = 948;
                                              
int nMaxProductVersion;                 // = 7;
int nMaxProductSubver;                  // = 61;
int nMaxProductBuild;                   // = 926;

BOOL bRunLiveUpdate;                    // = TRUE run LiveUpdate and verify current
BOOL bAuditSymantecClientFirewall;      // = TRUE - check SCF

} DESIRED_STATE;

typedef struct tagACTUAL_STATE
{

DWORD dwOnOff;                  // A/P enabled
DWORD dwWrites;                 // scan on modify
DWORD dwExecs;                  // scan on exec
DWORD dwReads;                  // scan on open
DWORD dwFileType;               // scan all extensions
DWORD dwHeuristics;             // heuristic engine enabled
DWORD dwHeuristicsLevel;        // default level

TCHAR *szExts;					// extension list

int nPattVerYear;         
int nPattVerMonth;       
int nPattVerDay;        
int nPattVerRev;         

DWORD dwEngineVersion;              

int nProductVersion;     
int nProductSubver;       
int nProductBuild;      

DWORD dwLiveUpdateStatus;
DWORD dwSCSServiceStatus;
DWORD dwSCSSettingStatus;

BOOL  bSCFInstalledAndManaged;
BOOL  bSCFEnabled;
BOOL  bSCFLastPolicyOK;

} ACTUAL_STATE;

// prototypes

SCSAUDIT_API BOOL AuditSCS( PDESIRED_STATE pDesiredState, PACTUAL_STATE *ppActualState, LPDWORD lpdwResult );
SCSAUDIT_API BOOL FreeActualState( PACTUAL_STATE pActualState );

// here are the Result code returned by AuditSCS on failure (FALSE)

// success returns TRUE, with a result code of ERROR_SUCCESS

enum _RUN_AUDIT_CHECK_RETURNS
{
    FAILED_SETTINGS_AUDIT = -100, 
    FAILED_NAVAP_AUDIT, 
    FAILED_SAV_SERVICE_NOT_RUNNING,
    FAILED_SAV_DRIVER_NOT_RUNNING,
    FAILED_SCF_NOT_RUNNING,
    FAILED_LIVE_UPDATE_FAILED,

    // 'non-errors start here' - these mean that I got a response that told me something

    PASSED_SETTINGS_AUDIT = 1, 
    PASSED_NAVAP_AUDIT, 
    PASSED_SAVAP_RUNNING, 
    PASSED_SCF_RUNNING,
    PASSED_LIVE_UPDATE_AUDIT
};


