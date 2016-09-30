; Copyright 1994 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:\navfamly\vcs\vnavcore.asv   1.9   31 Jan 1995 16:07:48   RSTANEV  $
;
; Description:
;       This file contains NAVCore's VxD main pieces.
;
;****************************************************************************
; $Log:   S:\navfamly\vcs\vnavcore.asv  $
;  
;     Rev 1.9   31 Jan 1995 16:07:48   RSTANEV
;  IFDEF->IFNDEF...
;
;     Rev 1.8   31 Jan 1995 15:58:32   RSTANEV
;  IFNDEF->IFDEF...
;
;     Rev 1.7   03 Jan 1995 14:17:20   RSTANEV
;  Supporting NAVCORE as a static library.
;
;     Rev 1.6   09 Dec 1994 17:52:48   RSTANEV
;  No more Carey's inoculation library :-(.
;
;     Rev 1.5   27 Oct 1994 11:35:42   RSTANEV
;  Code reviewed.
;
;     Rev 1.4   15 Sep 1994 19:49:54   RSTANEV
;  Added inoculation library functions.
;
;     Rev 1.3   02 Sep 1994 08:53:00   RSTANEV
;  Added NAVCore_Is_Compatible().
;
;     Rev 1.2   23 Aug 1994 22:35:34   RSTANEV
;  Added new services and fixed a problem with exporting the DDB.
;
;     Rev 1.1   16 Aug 1994 21:45:48   RSTANEV
;  Added vnavcore.inc.
;
;     Rev 1.0   16 Aug 1994 21:42:50   RSTANEV
;  Initial revision.
;
;****************************************************************************

IFNDEF STATIC_NAVCORE

Create_NAVCore_Service_Table  EQU  1

ENDIF

        .XLIST
        INCLUDE platform.inc
        INCLUDE vnavcore.inc
        .LIST

VXD_LOCKED_CODE_SEG

IFNDEF STATIC_NAVCORE

;----------------------------------------------------------------------------
;
; Define general VxD stuff...
;
;----------------------------------------------------------------------------

Declare_Virtual_Device  NAVCore, NAVCore_MAJOR_VER, NAVCore_MINOR_VER,  \
                        NAVCore_Control, NAVCore_Device_ID,             \
                        NAVCore_Init_Order, 0, 0

Begin_Control_Dispatch  NAVCore
End_Control_Dispatch  NAVCore

;----------------------------------------------------------------------------
;
; Define support macros for NAVCore services.
;
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; DispatchService  MACRO  servicename, args, proclang
;
; Provides a bridge for connecting VxD service to the actual function
; which will handle the request.  This is used to solve problems with stack
; parameter passing and naming conventions between different languages.
;
; servicename is the name of the service (which should match the name of
;             the function which should be called.)
; args        list of arguments, as they would appear if the function would
;             be prototyped using PROTO directive.  The arguments should be
;             enclosed in angle brackets, eg. <,:DWORD,:DWORD>
;             !note the comma in front of the first parameter!
; proclang    calling convention of the function: scall or ccall.
;             (if not specified, scall will be assumed)
;----------------------------------------------------------------------------
DispatchService  MACRO  servicename, args, proclang

  set_&proclang&_calling_convention

  servicename  PROTO  NEAR  args

  OPTION LANGUAGE:SYSCALL

  OPTION Prologue:NONE
  OPTION Epilogue:NONE

  BeginProc  SYMSERV_&servicename, service
    jmp  servicename
  EndProc  SYMSERV_&servicename

ENDM

set_ccall_calling_convention  MACRO
  OPTION LANGUAGE:C
ENDM

set_scall_calling_convention  MACRO
  OPTION LANGUAGE:STDCALL
ENDM

set__calling_convention  MACRO
  set_scall_calling_convention
ENDM

;----------------------------------------------------------------------------
;
; Define the bridges for NAVCore services.
;
; The following table should be kept in sync with:
;       VNAVCORE.INC file
;       AVAPI.H
;       INOCAPI.H
;
;----------------------------------------------------------------------------
  DispatchService  NAVCore_Get_Version,<>
  DispatchService  NAVCore_Is_Compatible,<,:DWORD>
  ;--------------  Virus Library Services
  DispatchService  VLScanInit,<,:UINT,:DWORD,:WORD>
  DispatchService  VLScanClose,<,:DWORD>
  DispatchService  VLReleaseVirusHandle,<,:DWORD>
  DispatchService  VLReleaseEntry,<,:DWORD>
  DispatchService  VLRegisterVirusDB,<,:DWORD,:DWORD,:DWORD>
  DispatchService  VLGetDBInfo,<,:DWORD,:DWORD,:DWORD>
  DispatchService  VLScanFile,<,:DWORD,:DWORD,:DWORD>
  DispatchService  VLScanBoot,<,:DWORD,:BYTE,:BYTE,:DWORD,:DWORD>
  DispatchService  VLScanPart,<,:DWORD,:BYTE,:DWORD,:DWORD>
  DispatchService  VLScanMemory,<,:DWORD,:WORD,:DWORD>
  DispatchService  VLRepairItem,<,:DWORD>
  DispatchService  VLCanRepairItem,<,:DWORD>
  DispatchService  VLCanDeleteItem,<,:DWORD>
  DispatchService  VLRepairFile,<,:DWORD>
  DispatchService  VLRepairBootPart,<,:DWORD>
  DispatchService  VLDeleteFile,<,:DWORD>
  DispatchService  VLVirusInfo,<,:DWORD,:WORD,:UINT,:DWORD>
  DispatchService  VLNumVirusEntries,<,:DWORD>
  DispatchService  VLGetVirusEntry,<,:DWORD,:UINT>
  DispatchService  VLFillListTable,<,:DWORD,:DWORD>
  DispatchService  VLDeleteEntry,<,:DWORD>
  DispatchService  VLDefInfo,<,:DWORD,:UINT,:DWORD>

ENDIF

;----------------------------------------------------------------------------
;
; DWORD NAVCore_Get_Version ( VOID );
;
; Clears CF and returns NAVCore's version.
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:STDCALL

        PUBLIC  NAVCore_Get_Version

NAVCore_Get_Version  PROC  STDCALL

        mov     eax,(NAVCore_MAJOR_VER SHL 8) + NAVCore_MINOR_VER
        clc
        ret

NAVCore_Get_Version  ENDP

;----------------------------------------------------------------------------
;
; BOOL WINAPI NAVCore_Is_Compatible ( DWORD dwVersion );
;
; Checks if NAVCore is compatible with an older dwVersion of the engine.
;
; Returns TRUE if compatible, FALSE if not compatible.
;
;----------------------------------------------------------------------------
        PUBLIC  NAVCore_Is_Compatible

NAVCore_Is_Compatible  PROC  STDCALL,
        dwVersion:DWORD

        mov     eax,TRUE
        clc
        ret

NAVCore_Is_Compatible  ENDP

        OPTION  LANGUAGE:SYSCALL

VXD_LOCKED_CODE_ENDS

        END
