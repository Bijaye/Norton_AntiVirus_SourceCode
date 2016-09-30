;; Copyright 1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\navfamly\vcs\vxdwraps.asv   1.5   15 Feb 1995 14:49:14   RSTANEV  $
;;
;; Description: Defines NAVCore's VxD wraps.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\navfamly\vcs\vxdwraps.asv  $
;  
;     Rev 1.5   15 Feb 1995 14:49:14   RSTANEV
;  No change.
;
;     Rev 1.4   09 Dec 1994 17:53:30   RSTANEV
;  No more Carey's inoculation library :-(.
;
;     Rev 1.3   15 Sep 1994 19:49:16   RSTANEV
;  Added inoculation library functions.
;
;     Rev 1.2   02 Sep 1994 08:53:14   RSTANEV
;  Added NAVCore_Is_Compatible().
;
;     Rev 1.1   23 Aug 1994 22:52:16   RSTANEV
;  Updated the services.
;
;     Rev 1.0   16 Aug 1994 22:03:28   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

        .XLIST
        INCLUDE platform.inc
        INCLUDE vnavcore.inc
        .LIST

;----------------------------------------------------------------------------
;
; Define necessary macros for the VxD wraps.
;
;----------------------------------------------------------------------------
WrapSTDCALL  MACRO  func, bcount

  PUBLIC  _&func&@&bcount

  _&func&@&bcount:
    VxDJmp  &func

ENDM

;----------------------------------------------------------------------------
;
; Define wraps.
;
;----------------------------------------------------------------------------
VXD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
; NAVCore_Get_Version is a special case, because service 0 cannot be
; called with a VxDJmp.  It should be VxDCall.
;
;----------------------------------------------------------------------------
PUBLIC  _NAVCore_Get_Version@0

_NAVCore_Get_Version@0:
        stc
        VxDCall NAVCore_Get_Version
        jnc     navcoreloaded
        mov     eax,0
  navcoreloaded:
        retn

;----------------------------------------------------------------------------
;
; Wrap the rest of the services.
;
;----------------------------------------------------------------------------
WrapSTDCALL  NAVCore_Is_Compatible,4
;----------  Virus Library Services
WrapSTDCALL  VLScanInit,12
WrapSTDCALL  VLScanClose,4
WrapSTDCALL  VLReleaseVirusHandle,4
WrapSTDCALL  VLReleaseEntry,4
WrapSTDCALL  VLRegisterVirusDB,12
WrapSTDCALL  VLGetDBInfo,12
WrapSTDCALL  VLScanFile,12
WrapSTDCALL  VLScanBoot,20
WrapSTDCALL  VLScanPart,16
WrapSTDCALL  VLScanMemory,12
WrapSTDCALL  VLRepairItem,4
WrapSTDCALL  VLCanRepairItem,4
WrapSTDCALL  VLCanDeleteItem,4
WrapSTDCALL  VLRepairFile,4
WrapSTDCALL  VLRepairBootPart,4
WrapSTDCALL  VLDeleteFile,4
WrapSTDCALL  VLVirusInfo,16
WrapSTDCALL  VLNumVirusEntries,4
WrapSTDCALL  VLGetVirusEntry,8
WrapSTDCALL  VLFillListTable,8
WrapSTDCALL  VLDeleteEntry,4
WrapSTDCALL  VLDefInfo,12

VXD_LOCKED_CODE_ENDS

        END
