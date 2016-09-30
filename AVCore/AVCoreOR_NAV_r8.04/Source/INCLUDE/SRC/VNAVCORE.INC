;; Copyright 1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\include\vcs\vnavcore.inv   1.7   31 Jan 1995 16:04:34   RSTANEV  $
;;
;; Description: Defines global NAVCore data
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\include\vcs\vnavcore.inv  $
;  
;     Rev 1.7   31 Jan 1995 16:04:34   RSTANEV
;  Oops...
;
;     Rev 1.6   31 Jan 1995 15:58:08   RSTANEV
;  NAVCORE is a static library.  We can't have services anymore.
;
;     Rev 1.5   09 Dec 1994 17:51:26   RSTANEV
;  No more Carey's inoculation library :-(.
;
;     Rev 1.4   15 Sep 1994 15:48:54   RSTANEV
;  Added inoculation services.
;
;     Rev 1.3   02 Sep 1994 08:53:28   RSTANEV
;  Added NAVCore_Is_Compatible().
;
;     Rev 1.2   30 Aug 1994 22:45:26   RSTANEV
;  Defining the service table in a specific segment.
;
;     Rev 1.1   25 Aug 1994 16:00:58   RSTANEV
;  Got rid of the VxD ID.
;
;     Rev 1.0   23 Aug 1994 22:37:04   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

;----------------------------------------------------------------------------
;
; Define NAVCore global data.
;
;----------------------------------------------------------------------------
NAVCore_MAJOR_VER   EQU  1
NAVCore_MINOR_VER   EQU  0

NAVCore_Init_Order  EQU  Undefined_Init_Order

;----------------------------------------------------------------------------
;
; Define NAVCore service table
;
;----------------------------------------------------------------------------
IFNDEF STATIC_NAVCORE

IFDEF Create_NAVCore_Service_Table
  VXD_LOCKED_CODE_SEG
ENDIF

Begin_Service_Table  NAVCore

  NAVCore_Service  NAVCore_Get_Version, LOCAL
  NAVCore_Service  NAVCore_Is_Compatible, LOCAL
  ;--------------  Virus Library services
  NAVCore_Service  VLScanInit, LOCAL
  NAVCore_Service  VLScanClose, LOCAL
  NAVCore_Service  VLReleaseVirusHandle, LOCAL
  NAVCore_Service  VLReleaseEntry, LOCAL
  NAVCore_Service  VLRegisterVirusDB, LOCAL
  NAVCore_Service  VLGetDBInfo, LOCAL
  NAVCore_Service  VLScanFile, LOCAL
  NAVCore_Service  VLScanBoot, LOCAL
  NAVCore_Service  VLScanPart, LOCAL
  NAVCore_Service  VLScanMemory, LOCAL
  NAVCore_Service  VLRepairItem, LOCAL
  NAVCore_Service  VLCanRepairItem, LOCAL
  NAVCore_Service  VLCanDeleteItem, LOCAL
  NAVCore_Service  VLRepairFile, LOCAL
  NAVCore_Service  VLRepairBootPart, LOCAL
  NAVCore_Service  VLDeleteFile, LOCAL
  NAVCore_Service  VLVirusInfo, LOCAL
  NAVCore_Service  VLNumVirusEntries, LOCAL
  NAVCore_Service  VLGetVirusEntry, LOCAL
  NAVCore_Service  VLFillListTable, LOCAL
  NAVCore_Service  VLDeleteEntry, LOCAL
  NAVCore_Service  VLDefInfo, LOCAL

End_Service_Table  NAVCore

IFDEF Create_NAVCore_Service_Table
  VXD_LOCKED_CODE_ENDS
ENDIF

ENDIF
