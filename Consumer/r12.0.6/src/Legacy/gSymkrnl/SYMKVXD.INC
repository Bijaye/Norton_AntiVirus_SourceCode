;=============================================================================
;
; $Header:   S:/SYMKRNL/VCS/SYMKVXD.INv   1.0   26 Jan 1996 20:21:36   JREARDON  
;
; Description:
;       This is the header file for SYMKVxD.VxD.
;
;=============================================================================
; $Log:   S:/SYMKRNL/VCS/SYMKVXD.INv  $
;  
;     Rev 1.0   26 Jan 1996 20:21:36   JREARDON
;  Initial revision.
;  
;     Rev 1.5   28 Jul 1995 15:44:48   BILL
;  Merged to trunk
;  
;     Rev 1.4.2.1   27 Jul 1995 03:14:20   LCOHEN
;  Added a service which patches VWin32 and fixes a deadlock situation when entering a level 3 lock.
;
;     Rev 1.4.2.0   27 Jun 1995 18:49:16   RJACKSO
;  Branch base for version QAK8
;
;     Rev 1.4   05 Mar 1995 12:31:38   HENRI
;  Implemented (with the fixes from branch) GetPhysicalInfo VxD function
;
;     Rev 1.3   27 Feb 1995 14:22:48   LCOHEN
;  Added internal errors so that SYMKrnl.DLL can process appropriately
;
;     Rev 1.2   02 Feb 1995 21:52:12   LCOHEN
;  Updated structures
;
;=============================================================================
;=============================================================================
;==
;== (C) Copyright SYMANTEC Corp. 1995
;==
;==     Title:   SYMKVxD.VxD - Symantec SYMKVxD VxD
;==
;==     Version: 1.00
;==
;==     Date:    February 1, 1995
;==
;==     Author:  Leo Cohen
;==
;=============================================================================
;==
;==	Change Log:
;==
;==     DATE     REVISION DESCRIPTION
;==     -------- -------- ----------------------------------------------------
;==     02/01/95 1.00     Original
;==
;=============================================================================
;=============================================================================
;==     I N I T   O R D E R
;=============================================================================
SYMKVxD_Init_Order      equ     Undefined_Init_Order
SYMKVxD_Device_ID       equ     Undefined_Device_ID
;=============================================================================
;==     D R I V E R  V E R S I O N  N U M B E R
;=============================================================================
SYMKVxD_MAJOR_VER       equ     1
SYMKVxD_MINOR_VER       equ     0

Version         equ     <"1.00">

ifndef FINAL
TestVersion     equ     <".01X">
else
TestVersion	equ	<"    ">
endif
;=============================================================================
;==     F U N C T I O N   N U M B E R S
;=============================================================================
SYMKVxD_FN_GETVERSION      equ 0
SYMKVxD_FN_REGBASED        equ 1
SYMKVxD_FN_STKBASED        equ 2
SYMKVxD_FN_VXDINT          equ 3
SYMKVxD_FN_IOSSRV          equ 4
SYMKVxD_FN_GETPHYSICALINFO equ 5
SYMKVxD_FN_PATCHVWIN32     equ 6
;=============================================================================
;==  Structure used with RegBasedCall and ExecVxDInt (REGBASED and VXDINT)
;=============================================================================
VxDParams       struct
  CallNum       dword   ?
  InEAX         dword   ?
  InEBX         dword   ?
  InECX         dword   ?
  InEDX         dword   ?
  InEBP         dword   ?
  InESI         dword   ?
  InEDI         dword   ?
  OutEAX        dword   ?
  OutEBX        dword   ?
  OutECX        dword   ?
  OutEDX        dword   ?
  OutEBP        dword   ?
  OutESI        dword   ?
  OutEDI        dword   ?
  OutFS         dword   ?
  OutGS         dword   ?
  OutEFLAGS     dword   ?
VxDParams       ends

;=============================================================================
;==  Structure used with StkBasedCall (STKBASED)
;=============================================================================
VxDPushParams   struct
  S_CallNum     dword   ?
  S_NumP        dword   ?
  S_P           dword 10 dup (?)
  S_OutEAX      dword   ?
  S_OutEDX      dword   ?
VxDPushParams   ends

;=============================================================================
;==  Structure used with IOSOperation
;=============================================================================
IOSOp           struct
  IOSCmd        dw      ?       ; IOR.H: IOR_READ or IOR_WRITE
  Int13Drv      dw      ?       ; 80h, 81h, 82h... or 00h, 01h,
  Sector        dd      ?       ; starting sector number
  SectorCnt     dd      ?       ; number of sectors to read/write
  Buffer        dd      ?       ; address of buffer
  Result        dd      ?       ; error codes from IOR.H: IORS_???
IOSOp           ends

;=============================================================================
;==  Structure used with GetPhysicalInfo
;=============================================================================
GetDCB          struct
  GetDCB_Int13Drv      dw      ?       ; 80h, 81h, 82h... or 00h, 01h,
  GetDCB_pDCB          dd      ?       ; address of a DCB to copy info to
  GetDCB_pBDD          dd      ?       ; address of a BDD to copy info to
  GetDCB_numDrives     dd      ?       ; number of drives
  GetDCB_Result        dd      ?       ; error codes from IOR.H: IORS_???
GetDCB          ends


CARRYFLAG       equ     000000001h
ZEROFLAG        equ     000000040h
OVERFLOWFLAG    equ     000000800h

LF	equ	0Ah
CR	equ	0Dh
