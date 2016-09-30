;=============================================================================
;
; $Header:   S:/SYMKRNL/VCS/iossrv.inv   1.1   18 Feb 1998 10:02:18   mdunn  $
;
; Description:
;       This is the header file for IOSSrv.ASM
;
;=============================================================================
; $Log:   S:/SYMKRNL/VCS/iossrv.inv  $
;  
;     Rev 1.1   18 Feb 1998 10:02:18   mdunn
;  Ported ROSWELL and ROSNEC changes from the K branch.
;  
;     Rev 1.0   26 Jan 1996 20:22:42   JREARDON
;  Initial revision.
;  
;     Rev 1.6   09 Jun 1995 21:13:24   RSTANEV
;  Made CreateBDDForRealModeDrive public.
;
;     Rev 1.5   08 Mar 1995 16:22:40   RSTANEV
;  Added public _IOSSrv_GetInfo.
;
;     Rev 1.4   05 Mar 1995 12:31:36   HENRI
;  Implemented (with the fixes from branch) GetPhysicalInfo VxD function
;
;     Rev 1.3   27 Feb 1995 14:22:52   LCOHEN
;  Added internal errors so that SYMKrnl.DLL can process appropriately
;
;     Rev 1.2   02 Feb 1995 13:25:14   LCOHEN
;  One last change due to different versions of DCBs
;
;=============================================================================
;=============================================================================
;==
;== (C) Copyright SYMANTEC Corp. 1995
;==
;==     Title:   IOSSrv.inc - IOS Services for a VxD
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
ifndef IOSSrv
;=============================================================================
;==     E X T E R N A L   I N T E R F A C E
;=============================================================================
VxD_PAGEABLE_CODE_SEG
        extrn   IOSSrv_DoIO:near
        extrn   IOSSrv_GetInfo:near
ifndef NODISKABSOPERATION
;       extrn   DiskAbsOperation:near
endif
VxD_PAGEABLE_CODE_ENDS
else
;=============================================================================
;==	P U B L I C   D E C L A R A T I O N S
;=============================================================================
VxD_PAGEABLE_CODE_SEG
        public  IOSSrv_DoIO
        public  IOSSrv_GetInfo
        public  CreateBDDForRealModeDrive
        public  IOSSrv_Chk_NEC_Floppy
ifndef NODISKABSOPERATION
        public  DiskAbsOperation
        public  _IOSSrv_GetInfo
endif
VxD_PAGEABLE_CODE_ENDS
endif
;=============================================================================
;==     I N T E R N A L   E R R O R    N U M B E R S
;=============================================================================
InternalError   equ     0FFFFFFF0h
No32BitDriver   equ     InternalError+0
PageLockError   equ     InternalError+1
