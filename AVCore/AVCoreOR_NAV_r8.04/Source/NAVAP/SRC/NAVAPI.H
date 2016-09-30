// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/NAVAPI.H_v   1.6   14 Sep 1998 16:34:48   RStanev  $
//
// Description:
//      This file defines general NAVAP structures, constants, data, and
//      function prototypes.  Some of these definitions may be exported to
//      other NAVAP modules.
//
// Contains:
//      NAVAPGlobalInitEx()
//      NAVAPGlobalDeInitEx()
//      NAVAPControlledInit()
//      NAVAPControlledDeInit()
//
//      NAVAP_Init_Complete()                     (SYM_VXD only)
//      NAVAP_Dynamic_Init()                      (SYM_VXD only)
//      NAVAP_Sys_VM_Terminate()                  (SYM_VXD only)
//      NAVAP_Dynamic_Exit()                      (SYM_VXD only)
//      NAVAP_Terminate_Thread()                  (SYM_VXD only)
//      NAVAP_VM_Terminate()                      (SYM_VXD only)
//      NAVAP_Destroy_Thread()                    (SYM_VXD only)
//
//      NAVAPStartupTasks()
//      NAVAPExitTasks()
//
//      CheckIfNAVAPIntact()
//      CheckIfMemoryInfected()
//
//      LoadNAVAPInternalConfiguration()
//      InitNAVAPConfiguration()
//      DeInitNAVAPConfiguration()
//      ReloadNAVAPConfiguration()
//      LoadNAVAPScanConfiguration()
//      UnloadNAVAPScanConfiguration()
//      LockNAVAPConfiguration()
//      UnlockNAVAPConfiguration()
//      LockNAVAPScanConfiguration()
//      UnlockNAVAPScanConfiguration()
//
//      NAVAPNTWriteStandardEventLog()            (SYM_NTK only)
//
//      Hook_V86_Int_Chain()                      (SYM_VXD only)
//      Unhook_V86_Int_Chain()                    (SYM_VXD only)
//      SHELL_Get_Version()                       (SYM_VXD only)
//      _SHELL_ShellExecute()                     (SYM_VXD only)
//
//      NAVAPV86CriticalErrorHandlerDispatcher()  (SYM_VXD only)
//      NAVAPV86CriticalErrorHandler()            (SYM_VXD only)
//      IsRealModeAPRunning()                     (SYM_VXD only)
//      DisableRealModeAP()                       (SYM_VXD only)
//      EnableRealModeAP()                        (SYM_VXD only)
//      InstallInt2FAPI()                         (SYM_VXD only)
//      UninstallInt2FAPI()                       (SYM_VXD only)
//      NAVAPInitBreakHandler()                   (SYM_VXD only, inline stub)
//      NAVAPDeInitBreakHandler()                 (SYM_VXD only, inline stub)
//
//      StackCleanupForThread()
//
// See Also:
//      navap.h, apcfg.h
//
//***************************************************************************

#ifndef _NAVAPI_H_
#define _NAVAPI_H_

//---------------------------------------------------------------------------
//
// Define the SYMEvent version required by NAVAP.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define SYMEVENT_VERSION_REQUIRED_BY_NAVAP 0x501


#elif defined(SYM_VXD)

#define SYMEVENT_VERSION_REQUIRED_BY_NAVAP 0x20B

#endif

//---------------------------------------------------------------------------
//
// Declare public variables representing NAVAP's status and its startup
// configuration.
//
//---------------------------------------------------------------------------
extern BOOL bNAVAPIsEnabled;

extern BOOL bNAVAPIsInShutdownMode;

#if defined(SYM_VXD)

extern BOOL bNAVTSRIsLoaded;

extern BOOL bNAVAPCfgStartupScanMemory;
extern BOOL bNAVAPCfgStartupScanUpperMemory;
extern BOOL bNAVAPCfgStartupRunDOSAutoProtect;

#endif // #if defined(SYM_VXD)

extern PTSTR pszNAVAPStartupName;

#if defined(SYM_NTK)

extern PWSTR pszNAVAPSYSStartupName;

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// prNAVAPPersistentData is a variable containing a pointer to the current
// NAVAP persistent data.  This variable is initialized when NAVAP loads,
// and is used by all modules which need to use the persistent data area.
// NAVAP will not run without prNAVAPPersistentData initialized.
//
//---------------------------------------------------------------------------
extern PERSISTENTDATA *prNAVAPPersistentData;

//---------------------------------------------------------------------------
//
// Define startup/exit function prototypes.
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalInitEx (
    VOID
    );

BOOL NAVAPGlobalDeInitEx (
    VOID
    );

BOOL NAVAPControlledInit (
    VOID
    );

BOOL NAVAPControlledDeInit (
    VOID
    );

#if defined(SYM_VXD)

extern "C" {

BOOL WINAPI NAVAP_Init_Complete (
    VOID
    );

BOOL WINAPI NAVAP_Dynamic_Init (
    VOID
    );

BOOL WINAPI NAVAP_Sys_VM_Terminate (
    VOID
    );

BOOL WINAPI NAVAP_Dynamic_Exit (
    VOID
    );

BOOL WINAPI NAVAP_Terminate_Thread (
    PTCB pTCB
    );

BOOL WINAPI NAVAP_VM_Terminate (
    HVM hVM
    );

BOOL WINAPI NAVAP_Destroy_Thread (
    PTCB pTCB
    );
}

#endif // #if defined(SYM_VXD)

BOOL NAVAPStartupTasks (
    VOID
    );

BOOL NAVAPExitTasks (
    VOID
    );

BOOL CheckIfNAVAPIntact(
    VOID
    );

BOOL CheckIfMemoryInfected (
    VOID
    );

//---------------------------------------------------------------------------
//
// Define configuration function prototypes.
//
//---------------------------------------------------------------------------
VOID LoadNAVAPInternalConfiguration (
    VOID
    );

BOOL InitNAVAPConfiguration (
    PNAVAP_CONFIG prConfig
    );

BOOL DeInitNAVAPConfiguration (
    VOID
    );

BOOL ReloadNAVAPConfiguration (
    PNAVAP_CONFIG prConfig
    );

BOOL LoadNAVAPScanConfiguration (
    PSTR pszPath
    );

BOOL UnloadNAVAPScanConfiguration (
    VOID
    );

PNAVAPCFG LockNAVAPConfiguration (
    VOID
    );

VOID UnlockNAVAPConfiguration (
    PNAVAPCFG prCfg
    );

HVCONTEXT LockNAVAPScanConfiguration (
    VOID
    );

VOID UnlockNAVAPScanConfiguration (
    HVCONTEXT hVContext
    );

//---------------------------------------------------------------------------
//
// Prototype NAVAP NT helper functions and data.  They are defined in
// NAVAPNT.CPP.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

extern const NTSTATUS aUIINITERRORTranslator[];

extern const NTSTATUS aUICONTROLLEDINITMESSAGETranslator[];

extern const NTSTATUS aOutOfMemoryErrorMessageTranslator[];

VOID NAVAPNTWriteStandardEventLog (
    NTSTATUS Status
    );

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// Prototype the VxD wrappers which we define on our own because Window 95's
// DDK doesn't do it for us.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" {

BOOL __cdecl Hook_V86_Int_Chain (
    UINT  uInterrupt,
    PVOID pfnCallback
    );

BOOL __cdecl Unhook_V86_Int_Chain (
    UINT  uInterrupt,
    PVOID pfnCallback
    );

WORD __cdecl SHELL_Get_Version (
    VOID
    );

DWORD __cdecl _SHELL_ShellExecute (
    PVOID lpShexPacket
    );

}

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Prototype NAVAP VxD helper functions.  These functions are defined
// either in VXDENTRY.ASM or NAVAPVXD.CPP.
//
// Note: Currently NAVAPInitBreakHandler() and NAVAPDeInitBreakHandler()
// are stubs implemented as inline functions right in this file.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" {

BOOL WINAPI NAVAPV86CriticalErrorHandlerDispatcher (
    VOID
    );

BOOL WINAPI NAVAPV86CriticalErrorHandler (
    PCRS prClientRegs
    );

BOOL WINAPI IsRealModeAPRunning (
    VOID
    );

BOOL WINAPI DisableRealModeAP (
    VOID
    );

BOOL WINAPI EnableRealModeAP (
    VOID
    );

BOOL WINAPI InstallInt2FAPI (
    VOID
    );

BOOL WINAPI UninstallInt2FAPI (
    VOID
    );

}

inline BOOL NAVAPInitBreakHandler (
    VOID
    )
    {
    return ( TRUE );
    }

inline BOOL NAVAPDeInitBreakHandler (
    VOID
    )
    {
    return ( TRUE );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Stack functions.
//
//---------------------------------------------------------------------------

VOID StackCleanupForThread (
    PVOID pvThread
    );

//---------------------------------------------------------------------------
//
// This callback structure is required by the AVAPI initialization
// functions.
//
//---------------------------------------------------------------------------
extern CALLBACKREV1 rVLCallbacksRev1;

//---------------------------------------------------------------------------
//
// lpgCallBack is a SYM_VXD hack needed only by the inoculation engine.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" CALLBACKREV1 FAR *lpgCallBack;

#endif // #if defined(SYM_VXD)

#endif // #ifndef _NAVAPI_H_
