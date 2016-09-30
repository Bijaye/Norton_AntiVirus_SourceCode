;=============================================================================
;
; $Header:   S:/SYMKRNL/VCS/IOSSRV.ASv   1.4   10 Aug 1998 08:25:40   MARKK  $
;
; Description:
;       This is the main source module for IOSSrv.
;
;=============================================================================
; $Log:   S:/SYMKRNL/VCS/IOSSRV.ASv  $
;  
;     Rev 1.4   10 Aug 1998 08:25:40   MARKK
;  Place all INT 1's in ifdef DEBUG bracket
;  
;     Rev 1.3   18 May 1998 17:30:26   DALLEE
;  Updated VxD DiskAbsOperation() to work like "C" W32/NTK/DOS version.
;  ABSDISKREC structure now matches "C" version with large drive
;  support -- DWORD fields for Cyl,Head,Sector.
;  
;     Rev 1.2   18 Feb 1998 10:02:06   mdunn
;  Ported ROSWELL and ROSNEC changes from the K branch.
;  
;     Rev 1.1   19 Sep 1997 18:21:12   BHELLST
;  Checked back in good revision due to vcs logfile corruption
;  
;     Rev 1.0   19 Sep 1997 16:42:38   BHELLST
;  Initial revision.
;  
;     Rev 1.0   26 Jan 1996 20:21:24   JREARDON
;  Initial revision.
;  
;     Rev 1.18   27 Jul 1995 15:08:42   RSTANEV
;  Merged Quake 8 branch.
;
;     Rev 1.17   18 Jul 1995 01:15:26   LCOHEN
;  Added code to get IOS Linkage Block service routine address from SYMEvent is available.
;
;     Rev 1.16   01 Jul 1995 05:23:54   LCOHEN
;  Marked read buffer as dirty.  This prevents pager from discarding data if it was bu-mastered.
;
;     Rev 1.15   13 Jun 1995 16:23:54   RSTANEV
;  Duh.
;
;     Rev 1.14   09 Jun 1995 21:14:48   RSTANEV
;  All functions can now access drives without 32-bit support.
;
;     Rev 1.13   09 Mar 1995 17:19:14   RSTANEV
;  Moved code and data to locked segments.
;
;     Rev 1.12   08 Mar 1995 16:23:02   RSTANEV
;  Added _IOSSrv_GetInfo.
;
;     Rev 1.11   07 Mar 1995 18:42:08   HENRI
;  Fixed CountDrives to identify PHYSICAL AND INT13 drives (not just PHYSICAL)
;
;     Rev 1.10   05 Mar 1995 12:31:38   HENRI
;  Implemented (with the fixes from branch) GetPhysicalInfo VxD function
;
;     Rev 1.9   27 Feb 1995 14:22:50   LCOHEN
;  Added internal errors so that SYMKrnl.DLL can process appropriately
;
;     Rev 1.8   07 Feb 1995 22:49:02   LCOHEN
;  Registered with IOS without an AEP routine
;
;
;=============================================================================
.386p
	page 58,132
;=============================================================================
        title   I O S S R V  V x D
;=============================================================================
;==
;== (C) Copyright SYMANTEC Corp. 1994
;==
;==     Title:   IOSSrv.VxD - Symantec IOS VxD IO Service
;==
;==     Version: 1.00
;==
;==     Date:    October 31, 1994
;==
;==     Author:  Leo Cohen
;==
;=============================================================================
;==
;==	Change Log:
;==
;==     DATE     REVISION DESCRIPTION
;==     -------- -------- ----------------------------------------------------
;==     10/31/94 1.00.01X Original
;==
;=============================================================================
;==
;=============================================================================
IOSSrv               equ TRUE
SYM_VXD_NONEWSTRUCTS equ TRUE
;=============================================================================
;==	I N C L U D E S
;=============================================================================
        .xlist
        include platform.inc
        include v86mmgr.inc
        include blockdev.inc
        include ior.inc
        include iop.inc
        include aep.inc
        include isp.inc
        include ios.inc
        include drp.inc
        include ilb.inc
        include dcb.inc
        include sgd.inc
        include ida.inc
        include SYMEvent.inc
        include IOSSrv.inc
        include symkvxd.inc
        .list
;=============================================================================
;==     P R O T E C T E D   M O D E
;=============================================================================
;=============================================================================
;==	P R O T E C T E D   M O D E   D A T A
;=============================================================================
VxD_LOCKED_DATA_SEG

GeneralFlags    dd      fFirst  ; general flags
  fFirstBit     equ     0       ; indicates we have initialized
  fVMM4Bit      equ     1       ; indicates VMM version 4
   fFirst       equ     1 shl fFirstBit
   fVMM4        equ     1 shl fVMM4Bit

DRPx DRP<EyeCatcher, DRP_MISC_PD, 0, offset32 ILBx, 'IOS Srv Provider'>
ILBx ILB<>

I13ErrorCode2IORSErrorCode  LABEL  DWORD
  dd  IORS_NO_DEVICE                    ; 00h successful completion (don't change to IORS_SUCCESS!)
  dd  IORS_INVALID_COMMAND              ; 01h invalid function in AH or invalid parameter
  dd  IORS_DEVICE_ERROR                 ; 02h address mark not found
  dd  IORS_WRITE_PROTECT                ; 03h disk write-protected
  dd  IORS_INVALID_SECTOR               ; 04h sector not found/read error
  dd  IORS_DEVICE_ERROR                 ; 05h reset failed (hard disk)
  dd  IORS_DEVICE_ERROR                 ; 06h disk changed (floppy)
  dd  IORS_DEVICE_ERROR                 ; 07h drive parameter activity failed (hard disk)
  dd  IORS_MEMORY_ERROR                 ; 08h DMA overrun
  dd  IORS_MEMORY_ERROR                 ; 09h attempted DMA across 64K boundary
  dd  IORS_MEDIA_ERROR                  ; 0Ah bad sector detected (hard disk)
  dd  IORS_MEDIA_ERROR                  ; 0Bh bad track detected (hard disk)
  dd  IORS_WRONG_MEDIA                  ; 0Ch unsupported track or invalid media
  dd  IORS_INVALID_PARM                 ; 0Dh invalid number of sectors on format (hard disk)
  dd  IORS_DEVICE_ERROR                 ; 0Eh control data address mark detected (hard disk)
  dd  IORS_MEMORY_ERROR                 ; 0Fh DMA arbitration level out of range (hard disk)
  dd  IORS_UNREC_ERROR                  ; 10h uncorrectable CRC or ECC error on read
  dd  IORS_SUCCESS_WITH_ECC             ; 11h data ECC corrected (hard disk)
I13ErrorCode2IORSErrorCodeTableLen  equ  ($-I13ErrorCode2IORSErrorCode) / 4

VxD_LOCKED_DATA_ENDS
;=============================================================================
;==     P R O T E C T E D   M O D E   C O D E
;=============================================================================
VxD_LOCKED_CODE_SEG
;=============================================================================
;==     E X T E R N A L   I N T E R F A C E
;=============================================================================

;=============================================================================
;==
;==  IOSSrv_GetInfo:
;==
;==  Entry:
;==      DL = int 13 drive number
;==     EBX = pointer to a DCB
;==     ECX = pointer to a BDD
;==
;==  Exit:
;==     EAX =  0: successful
;==            No32BitDriver: could not find Int 13h drive DCB
;==     ECX =  Number of drives of the same type
;==
;=============================================================================
CreateBDDForRealModeDrive  PROTO  STDCALL, uDrive:DWORD, prBDD:DWORD
NumOfInt13Drives  dd   0
BeginProc IOSSrv_GetInfo
        cld
        pushad
        xor     edi, edi
        mov     al, dl                  ; Int 13h drive number
        VxDcall IOS_Find_Int13_Drive    ; Q: BDD for drive found?
        jc      GetInfo_NoDCB           ;   N: continue
        test    edi, edi                ; Q: EDI -> Valid BDD?
        jz      GetInfo_NoDCB           ;   N: continue

        push    edi                     ; Save BDD
        mov     esi,edi                 ; Copy BDD to BDD buffer
        mov     edi, ecx
        mov     ecx, SIZE DCB_BLOCKDEV
        rep     movsb

        pop     edi                     ; Restore BDD pointer
        sub     edi, DCB_bdd            ;   Y: EDI -> DCB
        mov     esi, edi                ; EDI -> ESI
        mov     edi, ebx                ; pDCB -> EDI
        mov     ecx, SIZE DCB
        rep movsb                       ; Copy DCB pointed by ESI to struct in EDI
        mov     bl, DCB_type_disk      ; Assume hard disk type
        test    dl,80h                  ; Is this a hard drive
        jnz     @f                      ;   Y: go count the drives
        mov     bl, DCB_type_floppy    ;   N: must be a floppy
@@:
        call    CountDrives             ; get the number of drives of type specified
                                        ; in EBX
GetInfo_ExitOk:
        popad
        mov     ecx, NumOfInt13Drives
        xor     eax, eax
        ret                             ; Exit!

GetInfo_NoDCB:
                                        ; Create a BDD through V86 int 13
        movzx   edx,dl
        xor     eax,eax
        pushad

        mov     edi,ecx
        mov     esi,ecx
        mov     ecx,size DCB_BLOCKDEV
        rep stosb

        INVOKE  CreateBDDForRealModeDrive, edx, esi
        mov     [esp.Pushad_EDI],ecx
        test    eax,eax
        popad
        jz      GetInfo_AbsolutelyNoDCB
                                        ; Save drive count
        mov     NumOfInt13Drives,edi
                                        ; Create DCB
        mov     esi,ecx
        mov     ecx,size DCB
        mov     edi,ebx
        rep stosb

        test    dl,dl
;        mov     [ebx.DCB.DCB_cmn.DCB_physical_dcb]        DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_expansion_length]    DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_ptr_cd]              DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_next_dcb]            DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_next_logical_dcb]    DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_drive_lttr_equiv]    DB      ?
        mov     [ebx.DCB.DCB_cmn.DCB_unit_number],dl
        mov     [ebx.DCB.DCB_cmn.DCB_TSD_Flags],DCB_TSD_USER_SET
;        mov     [ebx.DCB.DCB_cmn.DCB_vrp_ptr]             DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_dmd_flags]           DD      ?
DEVICE_FLAGS = DCB_DEV_INT13_CONFIGURED OR DCB_DEV_RMM_INT13 OR DCB_DEV_REAL_MODE OR DCB_DEV_PHYSICAL OR DCB_DEV_RMM OR DCB_DEV_WRITEABLE OR DCB_DEV_INT13_DRIVE
        mov     [ebx.DCB.DCB_cmn.DCB_device_flags], DEVICE_FLAGS
        js      @f
        mov     [ebx.DCB.DCB_cmn.DCB_device_flags], DEVICE_FLAGS OR DCB_DEV_REMOVABLE
  @@:
;        mov     [ebx.DCB.DCB_cmn.DCB_device_flags2]       DD      ?
        mov     [ebx.DCB.DCB_cmn.DCB_Partition_Start],1
;        mov     [ebx.DCB.DCB_cmn.DCB_track_table_ptr]     DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_bds_ptr]             DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_Reserved1]           DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_Reserved2]           DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_apparent_blk_shift]  DB      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_partition_type]      DB      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_sig]                 DW      ?
        mov     [ebx.DCB.DCB_cmn.DCB_device_type],DCB_type_disk
        js      @f
        mov     [ebx.DCB.DCB_cmn.DCB_device_type],DCB_type_floppy
  @@:
;        mov     [ebx.DCB.DCB_cmn.DCB_Exclusive_VM]        DD      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_disk_bpb_flags]      DB      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_cAssoc]              DB      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_Sstor_Host]          DB      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_user_drvlet]         DW      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_Reserved3]           DW      ?
;        mov     [ebx.DCB.DCB_cmn.DCB_Reserved4]           DD      ?
;        mov     [ebx.DCB_max_xfer_len]                DD      ?
;        mov     [ebx.DCB_actual_sector_cnt]           DD      2 DUP (?)
        mov     eax,[esi.DCB_apparent_blk_size]
        mov     [ebx.DCB_actual_blk_size],eax
        mov     eax,[esi.DCB_apparent_head_cnt]
        mov     [ebx.DCB_actual_head_cnt],eax
        mov     eax,[esi.DCB_apparent_cyl_cnt]
        mov     [ebx.DCB_actual_cyl_cnt],eax
        mov     eax,[esi.DCB_apparent_spt]
        mov     [ebx.DCB_actual_spt],eax
;        mov     [ebx.DCB_next_ddb_dcb]                DD      ?
;        mov     [ebx.DCB_dev_node]                    DD      ?
;        mov     [ebx.DCB_bus_type]                    DB      ?
;        mov     [ebx.DCB_bus_number]                  DB      ?
;        mov     [ebx.DCB_queue_freeze]                DB      ?
;        mov     [ebx.DCB_max_sg_elements]             DB      ?
;        mov     [ebx.DCB_io_pend_count]               DB      ?
;        mov     [ebx.DCB_lock_count]                  DB      ?
;        mov     [ebx.DCB_SCSI_VSD_FLAGS]              DW      ?
;        mov     [ebx.DCB_scsi_target_id]              DB      ?
;        mov     [ebx.DCB_scsi_lun]                    DB      ?
;        mov     [ebx.DCB_scsi_hba]                    DB      ?
;        mov     [ebx.DCB_max_sense_data_len]          DB      ?
;        mov     [ebx.DCB_srb_ext_size]                DW      ?
;        mov     [ebx.DCB_inquiry_flags]               DB      8 DUP (?)
;        mov     [ebx.DCB_vendor_id]                   DB      8 DUP (?)
;        mov     [ebx.DCB_product_id]                  DB      16 DUP (?)
;        mov     [ebx.DCB_rev_level]                   DB      4 DUP (?)
;        mov     [ebx.DCB_port_name]                   DB      8 DUP (?)
;        mov     [ebx.DCB_current_unit]                DB      ?
;        mov     [ebx.DCB_blocked_iop]                 DD      ?
;        mov     [ebx.DCB_vol_unlock_timer]            DD      ?
;        mov     [ebx.DCB_access_timer]                DB      ?
;        mov     [ebx.DCB_Vol_Flags]                   DB      ?
;        mov     [ebx.DCB_q_algo]                      DB      ?
;        mov     [ebx.DCB_unit_on_ctl]                 DB      ?
;        mov     [ebx.DCB_Port_Specific]               DD      ?
;        mov     [ebx.DCB_spindown_timer]              DD      ?

        jmp     GetInfo_ExitOk

GetInfo_AbsolutelyNoDCB:
        popad
        mov     eax, No32BitDriver
        ret                             ; Exit!

EndProc   IOSSrv_GetInfo


;=============================================================================
;==
;==  CountDrives:
;==
;==  Entry:
;==      DL = int 13 drive number
;==      BL = device type: DCB_type_disk or DCB_type_floppy
;==  Exit:
;==     ECX =  Number of drives of the same type
;=============================================================================
BeginProc CountDrives
        pushad
;
;  Check to see if we have intialized
;
        btr     [GeneralFlags], fFirstBit       ;Q: First time called?
        jnc     short @f                        ; N: continue
        call    IOSSrvInit                      ; Y: initialize
@@:
;
;  Traverse Int 13h DCBs using the ISP_GET_FRST_NXT_DCB service
;
        sub     esp, size ISP_GET_FRST_NXT_DCB  ; allocate storage for ISP
        mov     esi, esp
        mov     [NumOfInt13Drives], 0
;
;  Init ISP structure
;
        mov     [esi].ISP_func, ISP_GET_FIRST_NEXT_DCB  ; set function
        mov     [esi].ISP_gfnd_dcb_type, bl             ; DCB_type_disk or DCB_type_floppy
        mov     [esi].ISP_gfnd_dcb_offset, 0            ; first DCB

NextDrive:
        cCall   [ILBx].ILB_service_rtn, <esi>           ; get next DCB
        mov     edi, [esi].ISP_gfnd_found_dcb           ; EDI -> DCB
        mov     [esi].ISP_gfnd_dcb_offset, edi          ; setup for next DCB
        test    edi, edi                                ;Q: Valid DCB?
        jz      short CountDone                         ; N: we're done!
                                                        ; Q: Is this physical device?
        test    [edi].DCB_device_flags, DCB_DEV_PHYSICAL
        jz      short NextDrive                         ;   N: try next drive
                                                        ; Q: Is it also an INT13 device?
        test    [edi].DCB_device_flags, DCB_DEV_INT13_DRIVE
        jz      short NextDrive                         ;   N: try next drive
        inc     [NumOfInt13Drives]                      ;   Y: we have a valid type
        jmp     NextDrive                               ; try next drive

CountDone:
        add     esp, size ISP_GET_FRST_NXT_DCB  ; deallocate storage for ISP
        popad
        mov     ecx, [NumOfInt13Drives]
        ret

EndProc   CountDrives
;=============================================================================
;==
;==  IOSSrv_DoIO:
;==
;==  Entry:
;==      AH = operation (0 read, 1 write)
;==      AL = flags
;==           fPageable: indicates buffer is pageable and must be LinLocked
fPageableBit    equ     0       ; buffer passed needs to be locked
fPageable       equ 1 shl fPageableBit
;==      DL = int 13 drive number
;==     EBX = starting sector number
;==     ECX = number of sectors
;==     ESI = read/write buffer
;==
;==  Exit:
;==     EAX =  0: successful
;==            1: could not find Int 13h drive DCB
;==            2: device driven by RMM
;==            3: buffer failed to page lock
;==          >10: IORS error codes (see IOR.H)
;==
;=============================================================================
StackPtr   equ dword ptr [esp]    ; current stack pointer
hIOP       equ StackPtr           ; handle for allocated IOP
LocalVars  equ 4
Regs       equ hIOP+4             ; saved registers
OpDW       equ Regs.Pushad_EAX    ; entry EAX
OpType     equ Regs.Pushad_EAX[1] ; operation type (read=0/write=1)
OpFlags    equ Regs.Pushad_EAX    ; operation flags
DrvNum     equ Regs.Pushad_EDX    ; int 13 drive number
Sec        equ Regs.Pushad_EBX    ; starting sector number
nSec       equ Regs.Pushad_ECX    ; number of sectors
pBuff      equ Regs.Pushad_ESI    ; pointer to buffer
Status     equ OpDW               ; status of operation

DCBAdjust  dd   0

BeginProc IOSSrv_DoIO
        pushad
        sub     esp, LocalVars  ; allocate local storage for hIOP
;
;  Check to see if we have intialized
;
        btr     [GeneralFlags], fFirstBit       ;Q: First time called?
        jnc     short @f                        ; N: continue
        call    IOSSrvInit                      ; Y: initialize
@@:
;
;  Check to see if we are running on Win'95 or greater
;
        mov     eax, 1                          ; assume we cannot find drive
        test    [GeneralFlags], fVMM4           ;Q: Win'95 or greater?
        jz      DIOexit                         ; N: all done

;
;  Find the appropriate DCB by calling the IOS_Find_Int13_Drive service
;  or Find_Int1B_Drive (local) on NECs
;

;
; Substitute for HWIsNEC call, since that call is crashing.
; (normally we'd just call SYMEvent_HWIsNEC)
; The way it works is: on ATs, the 8 bytes at 0xFFFF5 are a date stamp,
; but are unused (zero) on NECs.  So if the 8 bytes are all 0, we are
; on a NEC.
; After this code snippet is done, eax is 1 if the machine is a NEC,
; or 0 if not.

	push ecx

	mov     ecx, 8
	mov     eax, 0FFFF5h
NextByte:
	cmp     byte ptr [eax], 0
	jnz     IsNotNEC
	inc     eax
	loop    NextByte

	mov     eax, 1
	jmp     End_HWIsNEC_Replacement

IsNotNEC:
	mov     eax, 0

End_HWIsNEC_Replacement:
	pop ecx

;
; End of substituted code
;

	or      ax, ax
	jz      DIO_Find_Int13_Drive
; NEC
DIO_Find_Int1b_Drive:
	xor     edi, edi
	mov     al, dl                  ; Int 13 drive number
					; Finds floppies only!
	call    Find_Int1B_Drive
	jc      DIOnoDCB
	jmp     DIOAdjustDCB
; PC
DIO_Find_Int13_Drive:

        xor     edi, edi
        mov     al, dl                  ; Int 13h drive number
        VxDcall IOS_Find_Int13_Drive    ;Q: BDD for drive found?
        jc      DIOnoDCB                ; N: continue
        test    edi, edi                ;Q: EDI -> Valid BDD?
        jz      DIOnoDCB                ; N: continue
        sub     edi, DCB_bdd            ; Y: EDI -> DCB

DIOAdjustDCB:

ifndef DCBISSTABLE  ; temporary code to dynamically choose correct DCB version
        mov     [DCBAdjust], 0          ; DCBISSTABLE
        cmp     edi, [edi].DCB_physical_dcb
        je      short @f
.errnz DCB_bdd-0C3h ; used for Build 314 DDK DCB.INC file
        mov     [DCBAdjust], 8                  ; DCBISSTABLE
        add     edi, [DCBAdjust]                ; DCBISSTABLE
        cmp     edi, [edi].DCB_physical_dcb     ; DCBISSTABLE
        je      short @f                        ; DCBISSTABLE
ifdef DEBUG
        int 1;debug
endif
@@:
endif ; DCBISSTABLE

;
;  Check to see that this drive is not serviced by RMM
;  Make sure DCB_DEV_PHYSICAL is set.   It should always!!!!
;
        test    [edi].DCB_Device_Flags, DCB_DEV_RMM     ;Q: Serviced by RMM?
        jnz     DIOrmm                                  ; Y: device driven!!
;
;  Gather information from DCB so we can create an appropriate IOP
;
        sub     edi, [DCBAdjust]                ; DCBISSTABLE
        movzx   ecx, [edi].DCB_max_sg_elements  ; number of SGDs this DCB can support
        add     edi, [DCBAdjust]                ; DCBISSTABLE
        shl     ecx, 3                          ; each SGD is 8 bytes long
        .errnz  (size SGD) - 8  ; insure previous SHL 3 is correct
        mov     edx, [edi].DCB_expansion_length ; length of IOP extension
        add     ecx, edx                        ; SGDs plus extension
        add     cx, size IOP                    ; add IOP size
        add     edx, IOP_ior                    ; add offset to IOR
;
;  Create an IOP via the ISP_IOP_CREATE service
;
        sub     esp, size ISP_IOP_create        ; allocate space for ISP
        mov     esi, esp
        mov     [esi].ISP_func, ISP_CREATE_IOP  ; create IOP ISP function
        mov     [esi].ISP_i_c_flags, ISP_M_FL_MUST_SUCCEED
        mov     [esi].ISP_IOP_size, cx          ; size of IOP to create
        mov     [esi].ISP_delta_to_ior, edx     ; offset to IOR within IOP
        cCall   [ILBx].ILB_service_rtn, <esi>   ; call IOS ISP_Create_IOP routine
        mov     esi, [esi].ISP_IOP_ptr          ; ESI -> allocated IOP
        add     esp, (size ISP_IOP_create)      ; deallocate ISP structure
        mov     ecx, esi                        ; ECX -> IOP
        add     esi, edx                        ; ESI -> IOR
        mov     [esi].IOR_sgd_lin_phys, esi     ; -> IOR
        add     [esi].IOR_sgd_lin_phys, size IOR; -> first physical sgd
;
;  Save the I/O specfic information ptr in IOR for use during callback
;
        mov     [hIOP], ecx                     ; save hIOP
        mov     edx, esi                        ; EDX -> IOR
        sub     edx, IOP_ior                    ; EDX -> IOP
;
;  Indicate proper DCB
;
        mov     [edx].IOP_original_dcb, edi     ; EDI -> DCB
;
;  Initialize IOR
;
        xor     edx, edx
        mov     [esi].IOR_next, edx             ; indicate no more IORs
;
;  Indicate this is an IOS(ver 2) IOR, a phys dev IO, and hi priority operation
;
ifdef ASYNCOPERATION
        mov     [esi].IOR_flags, IORF_VERSION_002+IORF_PHYS_CMD
else
        mov     [esi].IOR_flags, IORF_VERSION_002+IORF_PHYS_CMD+IORF_SYNC_COMMAND
endif
        mov     [esi].IOR_start_addr[4], edx    ; high DWORD of starting sec = 0
        mov     [esi].IOR_req_vol_handle, edx   ; no VRP
ifdef ASYNCOPERATION
        mov     [esi].IOR_callback, offset32 IOCB
else
        mov     [esi].IOR_callback, edx
endif

        movzx   edx, byte ptr [OpType]          ; get operation type
        mov     [esi].IOR_func, dx              ; set ...

        mov     dl, byte ptr [DrvNum]           ; get drive unit number
        mov     [esi].IOR_vol_designtr, dl      ; set ...

        mov     edx, [Sec]                      ; get starting sector number
        mov     [esi].IOR_start_addr, edx       ; set ...

        mov     ecx, [nSec]                     ; get number of sectors
        mov     [esi].IOR_xfer_count, ecx       ; set ...

        mov     edx, [pBuff]                    ; get buffer address
        mov     [esi].IOR_buffer_ptr, edx       ; set ...

;
;  Check to see if we must page lock the buffer, and if so, make sure we have
;  a global linear address so that we can access it under any memory context.
;
        test    [OpFlags], fPageable
        jz      short @f
        shl     ecx, 9                          ;QLEO: assuming 512 byte sectors
        call    PageLockBuffer
        test    eax, eax                        ;Q: Did lock succeed?
        jz      DIOnoLock                       ; N: error
        and     edx, 0FFFh                      ; Y: byte offset into page
        add     eax, edx                        ; global address for buffer
        mov     [esi].IOR_buffer_ptr, eax       ; set global address for buffer
@@:
;
;  ILB_int_io_criteria_rtn:
;
;  Converts the contents of the IOP so that the associated drivers can carry
;  out the I/O request. If necessary, the service converts scatter/gather
;  descriptor addresses and scatter/gather buffer addresses from linear to
;  physical. It also converts the block counts in the scatter/gather
;  descriptors to byte counts. The service verifies the scatter/gather buffer
;  alignment and scatter/gather descriptor count maximums.
;
;  Returns zero if the IOP either meets the driver constraints or has been
;  modified to meet the constraints; otherwise, returns nonzero.
;  In cases, such as the Promise controller, the above call could fail
;  Therefore, check return code and switch to single sector mode if needed.
;
        mov     edx, esi                ; EDX -> IOR
        sub     edx, IOP_ior            ; EDX -> IOP
        cCall    [ILBx].ILB_int_io_criteria_rtn, <edx>
		or		eax, eax				; Did it succeed?
		jnz		DIOSwitchToInt13		; No, switch to single sector I/O mode
;
;  Need to allocate the semaphore for this operation
;
ifdef ASYNCOPERATION
        xor     ecx, ecx
        VMMcall Create_Semaphore
        mov     [esi].IOR_private_client, eax   ; hSem
endif
;
;  Finally, tell IOS to do IO
;
;       mov     esi, esi                ; ESI -> IOR
        add     edi, DCB_bdd            ; EDI -> BDD
        sub     edi, [DCBAdjust]        ; DCBISSTABLE
        VxDcall IOS_SendCommand
        add     edi, [DCBAdjust]        ; DCBISSTABLE
        sub     edi, DCB_bdd            ; EDI -> DCB , restore DCB

ifdef ASYNCOPERATION
        mov     eax, [esi].IOR_private_client
        mov     ecx, BLOCK_THREAD_IDLE
        VMMcall Wait_Semaphore
;
;  Need to deallocate the semaphore for this operation
;
        mov     eax, [esi].IOR_private_client
        VMMcall Destroy_Semaphore
endif

;
;  Check to see if we need to page unlock the buffer
;
        test    [OpFlags], fPageable
        jz      short @f
        mov     ecx, [nSec]                     ; get number of sectors
        mov     edx, [esi].IOR_buffer_ptr       ; get global linear address
        shl     ecx, 9                          ;QLEO: assuming 512 byte sectors
        call    PageUnLockBuffer
        shr     ecx, 9
        test    eax, eax                ;Q: Did unlock succeed?
        jnz     short @f                ; Y: continue
ifdef DEBUG
        int 1;debug
endif
@@:
;
;  Back-end of I/O
;
        xor     eax, eax                                ; assume success
        cmp     [esi].IOR_status, IORS_INVALID_SECTOR   ;Q: Did I/O succeed?
        jb      short @f                                ; Y: exit
        movzx   eax, [esi].IOR_status                   ; N: return error
@@:
;
;  Deallocate IOP
;
DIOdeallocIOP:
        mov     [Status], eax                   ; save status
        mov     esi, [hIOP]                     ; ESI -> IOP

        sub     esp, size ISP_mem_dealloc       ; alloc space for ISP
        mov     ecx, esp                        ; ECX -> ISP_mem_dealloc
        mov     [ecx].ISP_func, ISP_DEALLOC_MEM ; deallocate mem ISP service
        mov     [ecx].ISP_mem_ptr_da, esi       ; -> memory to deallocate
        cCall   [ILBx].ILB_service_rtn, <ecx>   ; call IOS ISP_DEALLOC_IOP routine
        add     esp, size ISP_mem_dealloc       ; dealloc space for ISP

        mov     eax, [Status]                   ; restore status
DIOexit:
        mov     [Status], eax   ; set status
        lea     esp, [Regs]     ; free local storage for hIOP
        popad
        ret

DIOnoDCB:
        lea     esp, [Regs]
        popad
        jmp     short I13Srv_DoIO
;
;  If a device is driven by RMM, we better not do operations through a
;  protected mode driver because we don't know what kind of added value the
;  real mode device driver might have implemented.
;
DIOrmm:
        mov     eax, No32BitDriver
        jmp     short DIOexit
DIOnoLock:
        mov     eax, PageLockError
        jmp     short DIOdeallocIOP

ifdef ASYNCOPERATION
IOCB:
ifdef DEBUG
        int 1;debug
endif
        mov     eax, [esi].IOR_private_client   ; hSem
        VMMcall Signal_Semaphore_No_Switch
        ret
endif

;
; Entered when ILB_int_io_criteria_rtn fails, it frees up the IOP, then jumps
; into I13Srv_DoIO to perform single sector I/O instead.
;

DIOSwitchToInt13:
;
;  Check to see if we need to page unlock the buffer
;
        test    [OpFlags], fPageable
        jz      short @f
        mov     ecx, [nSec]                     ; get number of sectors
        mov     edx, [esi].IOR_buffer_ptr       ; get global linear address
        shl     ecx, 9                          ;QLEO: assuming 512 byte sectors
        call    PageUnLockBuffer
        shr     ecx, 9
        test    eax, eax						;Q: Did unlock succeed?
        jnz     short @f						;Y: continue
ifdef DEBUG
		int 1
endif

@@:
;
; Free up the IOP
;
        mov     esi, [hIOP]                     ; ESI -> IOP
        sub     esp, size ISP_mem_dealloc       ; alloc space for ISP
        mov     ecx, esp                        ; ECX -> ISP_mem_dealloc
        mov     [ecx].ISP_func, ISP_DEALLOC_MEM ; deallocate mem ISP service
        mov     [ecx].ISP_mem_ptr_da, esi       ; -> memory to deallocate
        cCall   [ILBx].ILB_service_rtn, <ecx>   ; call IOS ISP_DEALLOC_IOP routine
        add     esp, size ISP_mem_dealloc       ; dealloc space for ISP
;
; Restore registers and jump into I13Srv_DoIO
;
		jmp		short DIOnoDCB


EndProc IOSSrv_DoIO

;============================================================================
;==
;==  I13Srv_DoIO:
;==
;==  Entry (same as IOSSrv_DoIO):
;==      AH = operation (0 read, 1 write)
;==      AL = flags
;==           fPageable: indicates buffer is pageable and must be LinLocked
;==      DL = int 13 drive number
;==     EBX = starting sector number
;==     ECX = number of sectors
;==     ESI = read/write buffer
;==
;==  Exit (same as IOSSrv_DoIO):
;==     EAX =  0: successful
;==            3: buffer failed to page lock
;==          >10: IORS error codes (see IOR.H)
;==
;============================================================================
i13_Local_Vars_Struc struc
  i13_wSector_Size  dw  ?
  i13_wCylinders    dw  ?
  i13_bySectors     db  ?
  i13_byHeads       db  ?
  i13_wReserved     dw  ?
i13_Local_Vars_Struc ends

i13_Local_Var  textequ  <ebp - size i13_Local_Vars_Struc>

BeginProc I13Srv_DoIO
;
;  Validate input parameters.
;
IFDEF IOSSRV_DEBUG_MESSAGES
        pushad
        Trace_Out "I13Srv_DoIO entry: AX #AX DL #DL EBX #EBX ECX #ECX ESI #ESI"
        popad
ENDIF
        cmp     ah,1
        jbe     I13Srv_CommandOk
        mov     eax,IORS_INVALID_COMMAND
        ret
  I13Srv_CommandOk:
;
;  Create stack frame and prepare current VM for nested execution.
;
        pushad
        mov     ebp,esp
        sub     esp,size i13_Local_Vars_Struc

        Push_Client_State
        VMMcall Begin_Nest_V86_Exec
;
;  Get drive parameters
;
        VMMcall Get_Cur_VM_Handle
        mov     edi,[ebx.CB_Client_Pointer]
        mov     ebx,[ebx.CB_High_Linear]
        mov     [edi.Client_AX],801h
        mov     [edi.Client_CX],0
        mov     [edi.Client_DL],dl
        mov     eax,13h
        VMMcall Exec_Int
IFDEF IOSSRV_DEBUG_MESSAGES
        pushad
        mov     si,[edi.Client_Flags]
        mov     ax,[edi.Client_AX]
        mov     cx,[edi.Client_CX]
        mov     bx,[edi.Client_DX]
        Trace_Out "I13Srv_DoIO get drive #DL parameters: FLAGS #SI AX #AX CX #CX DX #BX"
        popad
ENDIF
        test    [edi.Client_Flags],CF_Mask
        jnz     I13Srv_InvalidDrive
        push    edx
        and     dl,7Fh
        cmp     dl,[edi.Client_DL]
        pop     edx
        jae     I13Srv_InvalidDrive
        cmp     [edi.Client_DL],0
        je      I13Srv_InvalidDrive
        cmp     [edi.Client_CL],0
        je      I13Srv_InvalidDrive
        cmp     [edi.Client_CH],0
        je      I13Srv_InvalidDrive
;
;  Convert the parameters returned by Int 13h / ah = 8 into i13_rDiskInfoRec.
;
        test    dl,dl
        mov     ecx,200h
        js      I13Srv_UseHDSectorSize
        movzx   eax,[edi.Client_ES]
        shl     eax,4
        mov     cx,[edi.Client_DI]
        add     eax,ecx
        add     eax,ebx
        movzx   eax,byte ptr [eax + 3]
        xor     ecx,ecx
        bts     ecx,eax
        shl     ecx,7
  I13Srv_UseHDSectorSize:
        mov     [i13_Local_Var].i13_wSector_Size,cx
        movzx   eax,[edi.Client_CL]
        shl     eax,2
        shr     al,2
        mov     [i13_Local_Var].i13_bySectors,al
        mov     al,[edi.Client_CH]
        inc     eax
        mov     [i13_Local_Var].i13_wCylinders,ax
        mov     al,[edi.Client_DH]
        inc     al
        mov     [i13_Local_Var].i13_byHeads,al
IFDEF IOSSRV_DEBUG_MESSAGES
        pushad
        mov     ax,[i13_Local_Var].i13_wSector_Size
        mov     bl,[i13_Local_Var].i13_bySectors
        mov     cx,[i13_Local_Var].i13_wCylinders
        mov     dl,[i13_Local_Var].i13_byHeads
        Trace_Out "I13Srv_DoIO drive params: SectSize #AX Sectors #BL Cylinders #CX Heads #DL"
        popad
ENDIF
;
;  Allocate read/write V86 buffer.
;
        push    ebp
        mov     eax,ebx
        mov     edx,ecx
        mov     ebp,edi
        VMMcall Get_Cur_VM_Handle
        clc
        VxDcall V86MMGR_Allocate_Buffer
        mov     ebx,ebp
        pop     ebp
        jc      I13Srv_MemoryError
        cmp     ecx,edx
        jne     I13Srv_MemoryErrorFree
        mov     [ebx.Client_BX],di
        movzx   ecx,di
        add     eax,ecx
        shr     edi,10h
        mov     [ebx.Client_ES],di
        shl     edi,4
        add     edi,eax
;
;  Loop for reading / writing one sector at a time.
;
        xor     ecx,ecx
  I13Srv_ReadWriteMore:
        cmp     ecx,[ebp.Pushad_ECX]
        jae     I13Srv_DoneWithAllSectors
;
;  Setup V86 registers for int 13 read / write call.
;
        mov     ax,201h
        add     ah,byte ptr [ebp.Pushad_EAX + 1]
        mov     [ebx.Client_AX],ax
        mov     al,byte ptr [ebp.Pushad_EDX]
        mov     [ebx.Client_DL],al
        mov     eax,ecx
        add     eax,[ebp.Pushad_EBX]
        push    ecx
        movzx   ecx,[i13_Local_Var].i13_bySectors
        xor     edx,edx
        jecxz   I13Srv_InvalidDiskParametersFar
        div     ecx
        inc     edx
        mov     [ebx.Client_CL],dl
        movzx   ecx,[i13_Local_Var].i13_byHeads
        xor     edx,edx
        jecxz   I13Srv_InvalidDiskParametersFar
        div     ecx
        pop     ecx
        mov     [ebx.Client_DH],dl
        mov     [ebx.Client_CH],al
        shl     ah,6
        or      [ebx.Client_CL],ah
;
;  If write operation, then copy source buffer to write buffer
;
        cmp     byte ptr [ebp.Pushad_EAX + 1],1
        jne     I13Srv_SkipWriteMemoryOperations
        call    CopyReadWriteBuffer
  I13Srv_SkipWriteMemoryOperations:
;
;  Reflect int 13 in V86 mode.
;
        mov     eax,13h
        VMMcall Exec_Int
        test    [ebx.Client_Flags],CF_Mask
        jnz     I13Srv_IOError
;
;  If read operation, then copy read buffer to destination buffer.
;
        cmp     byte ptr [ebp.Pushad_EAX + 1],0
        jne     I13Srv_SkipReadMemoryOperations
        xchg    esi,edi
        call    CopyReadWriteBuffer
        xchg    esi,edi
  I13Srv_SkipReadMemoryOperations:
;
;  Increment sector counter, buffer pointer, and then loop to r/w next.
;
        inc     ecx
        movzx   eax,[i13_Local_Var].i13_wSector_Size
        add     esi,eax
        jmp     I13Srv_ReadWriteMore
;
;  This is just so that we can do a jecxz to a near location
;
  I13Srv_InvalidDiskParametersFar:
IFDEF IOSSRV_DEBUG_MESSAGES
        int     1
ENDIF
        jmp     short I13Srv_InvalidDiskParameters
;
;  Release the V86 buffer for read/write.
;
  I13Srv_DoneWithAllSectors:
        movzx   ecx,[i13_Local_Var].i13_wSector_Size
        push    ebp
        mov     ebp,ebx
        VMMcall Get_Cur_VM_Handle
        clc
        VxDcall V86MMGR_Free_Buffer
        pop     ebp
;
; Restore original VM's status and return to caller
;
        mov     [ebp.Pushad_EAX],IORS_SUCCESS
  I13Srv_Done:
        VMMcall End_Nest_Exec
        Pop_Client_State
        jmp     short I13Srv_End
;
; Subroutines for setting error codes.
;
  I13Srv_InvalidDrive:
        mov     dword ptr [ebp.Pushad_EAX],IORS_NO_DEVICE
        jmp     short I13Srv_Done
  I13Srv_MemoryErrorFree:
        push    ebp
        mov     ebp,ebx
        VMMcall Get_Cur_VM_Handle
        clc
        VxDcall V86MMGR_Free_Buffer
        pop     ebp
  I13Srv_MemoryError:
        mov     dword ptr [ebp.Pushad_EAX],IORS_MEMORY_ERROR
        jmp     short I13Srv_Done
  I13Srv_IOError:
        movzx   eax,[ebx.Client_AH]
        call    TranslateI13ErrorCode2IORSErrorCode
        mov     dword ptr [ebp.Pushad_EAX],eax
        jmp     short I13Srv_DoneFreeMemory
  I13Srv_InvalidDiskParameters:
        pop     ecx
        mov     dword ptr [ebp.Pushad_EAX],IORS_NO_DEVICE
  I13Srv_DoneFreeMemory:
        movzx   ecx,[i13_Local_Var].i13_wSector_Size
        push    ebp
        mov     ebp,ebx
        VMMcall Get_Cur_VM_Handle
        clc
        VxDcall V86MMGR_Free_Buffer
        pop     ebp
        jmp     short I13Srv_Done
;
; Restore original stack frame and return to caller
;
  I13Srv_End:
        mov     esp,ebp
        popad
IFDEF IOSSRV_DEBUG_MESSAGES
        pushad
        Trace_Out "I13Srv_DoIO exit: EAX #EAX"
        popad
ENDIF
        cld
        ret
EndProc I13Srv_DoIO

;=============================================================================
;==     L O C A L   R O U T I N E S
;=============================================================================
;=============================================================================
;==
;==  PageLockBuffer
;==
;==  Entry:
;==     ECX = size of buffer
;==     EDX = start of buffer
;==
;==  Exit:
;==     EAX = global address for buffer and buffer is page locked
;==
;=============================================================================
BeginProc PageLockBuffer
        push    ecx
        push    edx

        add     ecx, edx                ; address of byte after buffer
        dec     ecx                     ; address of last byte in buffer
        shr     edx, 12                 ; starting page number
        shr     ecx, 12                 ; last page in buffer
        sub     ecx, edx                ; number of pages buffer spans
        inc     ecx                     ; number of pages buffer occupies
;
;  Lock the page range.  We really only need to mark the pages dirty if this
;  is a read operation.  For now, we will always mark them dirty... a slight
;  performance degradation on write operations.
;
        VxDcall _LinPageLock, <edx, ecx, <PAGEMAPGLOBAL+PAGEMARKDIRTY>>
;        test    eax, eax                    ;Q: Did the lock succeed?
;        jz      short @f                    ; N: return error
        pop     edx
        pop     ecx
        ret
EndProc PageLockBuffer

;=============================================================================
;==
;==  PageUnLockBuffer
;==
;==  Entry:
;==     ECX = size of buffer
;==     EDX = start of buffer
;==
;==  Exit:
;==     buffer is unlocked
;==
;=============================================================================
BeginProc PageUnLockBuffer
        push    ecx
        push    edx

        add     ecx, edx                ; address of byte after buffer
        dec     ecx                     ; address of last byte in buffer
        shr     edx, 12                 ; starting page number
        shr     ecx, 12                 ; last page in buffer
        sub     ecx, edx                ; number of pages buffer spans
        inc     ecx                     ; number of pages buffer occupies

        VxDcall _LinPageUnLock, <edx, ecx, PAGEMAPGLOBAL>   ; unlock the pages
;        test    eax, eax                        ;Q: Did the unlock succeed?
;        jz      short @f                        ; N: return error
        pop     edx
        pop     ecx
        ret
EndProc PageUnLockBuffer

;=============================================================================
;==
;==  CopyReadWriteBuffer
;==
;==  Entry:
;==     ESI = source buffer to copy from
;==     EDI = destination buffer to copy to
;==     EBP = i13_Local_Var.i13_wSector_Size contains buffer length
;==
;==  Exit:
;==     buffer copied, registers not modified, DF cleared
;==
;=============================================================================
BeginProc CopyReadWriteBuffer
        push    ecx
        push    esi
        push    edi
        movzx   ecx,[i13_Local_Var].i13_wSector_Size
        cld
        rep movsb
        pop     edi
        pop     esi
        pop     ecx
        ret
EndProc CopyReadWriteBuffer

;=============================================================================
;==
;==  IOSSrvInit:
;==
;==  Entry:
;==
;==  Exit:
;==
;=============================================================================
BeginProc IOSSrvInit
        pushad

;
;  Get VMM version number
;
        VMMcall Get_VMM_Version
        cmp     ah, 4                   ;Q: Is this Windows '95 or greater
        jb      short ISIexit           ; N: we are done
        or      [GeneralFlags], fVMM4   ; Y: indicate Windows '95 or greater
;
;  If SYMEvent is available, grab the IOS Linkage Block Service Routine from it
;
        xor     eax, eax
        VxDcall SYMEvent_Get_Version    ; get SYMEvent version number
        cmp     eax, RequiredVersion    ;Q: SYMEvent correct version?
        jb      short ISIreg            ; N: register with IOS
;
;  Grab the ILB Service and Criteria Routine from SYMEvent ILB
;
        VxDcall SYMEvent_ILB            ; get IOS linkage block from SYMEvent
        mov     ecx, [eax].ILB_int_io_criteria_rtn
        mov     [ILBx].ILB_int_io_criteria_rtn, ecx
        mov     ecx, [eax].ILB_service_rtn
        mov     [ILBx].ILB_service_rtn, ecx
ISIexit:
        popad
        ret
;
;  Register ourselves with IOS
;
ISIreg:
        VxDcall IOS_Register, <offset32 DRPx>
        jmp     short ISIexit

EndProc IOSSrvInit

;=============================================================================
;==
;==  TranslateI13ErrorCode2IORSErrorCode:
;==
;==  Entry:
;==     eax = int 13 - type error code
;==
;==  Exit:
;==     eax = IORS - type error code
;==
;=============================================================================
BeginProc TranslateI13ErrorCode2IORSErrorCode

        cmp     eax,I13ErrorCode2IORSErrorCodeTableLen
        jae     short NotInTable
        mov     eax,[I13ErrorCode2IORSErrorCode + eax * 4]
        jmp     short EndTranslation
  NotInTable:
        push    ecx

        cmp     eax,0FFh                ; sense operation failed (hard disk)
        mov     ecx,IORS_DEVICE_ERROR
        je      short DoneWithNotInTable

        ja      short ErrorUnknown

        cmp     al,20h                  ; controller failure
        mov     ecx,IORS_HW_FAILURE
        je      short DoneWithNotInTable

        cmp     al,40h                  ; seek failed
        mov     ecx,IORS_INVALID_SECTOR
        je      short DoneWithNotInTable

        cmp     al,80h                  ; timeout (not ready)
        mov     ecx,IORS_TIME_OUT
        je      short DoneWithNotInTable

        cmp     al,0AAh                 ; drive not ready (hard disk)
        mov     ecx,IORS_NOT_READY
        je      short DoneWithNotInTable

        cmp     al,0BBh                 ; undefined error (hard disk)
        mov     ecx,IORS_DEVICE_ERROR
        je      short DoneWithNotInTable

        cmp     al,0CCh                 ; write fault (hard disk)
        mov     ecx,IORS_DEVICE_ERROR
        je      short DoneWithNotInTable

        cmp     al,0E0h                 ; status register error (hard disk)
        mov     ecx,IORS_DEVICE_ERROR
        je      short DoneWithNotInTable

  ErrorUnknown:
        mov     ecx,IORS_NO_DEVICE      ; if unknown, then return this...

  DoneWithNotInTable:
        mov     eax,ecx
        pop     ecx

  EndTranslation:
        ret

EndProc TranslateI13ErrorCode2IORSErrorCode

;----------------------------------------------------------------------------
;
; BOOL WINAPI CreateBDDForRealModeDrive (
;      UINT                        uDrive,
;      _BlockDev_Device_Descriptor *prBDD
;      );
;
; This function fills *prBDD with information about a real-mode int 13
; device.
;
; Returns TRUE if prBDD filled with information about the drive, FALSE if
; error.
;
; ASM callers: If TRUE, ECX returns the number of drives of that type.
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:STDCALL

CreateBDDForRealModeDrive  PROC, uDrive:DWORD, prBDD:DWORD

        push    ebx
        push    esi
        push    edi

        mov     ecx,uDrive
        mov     edx,prBDD
        mov     [edx.BDD_Int_13h_Number],cl
        mov     [edx.BDD_Flags],BDF_Int13_Drive
        test    cl,cl
        js      CreateBDD_NotRemovable
        or      [edx.BDD_Flags],BDF_Removable
  CreateBDD_NotRemovable:

        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        VMMcall Get_Cur_VM_Handle
        mov     edi,[ebx.CB_Client_Pointer]
        mov     [edi.Client_AX],801h
        mov     [edi.Client_CX],0
        mov     [edi.Client_DL],cl
        mov     eax,13h
        VMMcall Exec_Int
IFDEF IOSSRV_DEBUG_MESSAGES
        pushad
        mov     si,[edi.Client_Flags]
        mov     ax,[edi.Client_AX]
        mov     bx,[edi.Client_CX]
        mov     dx,[edi.Client_DX]
        Trace_Out "Create RM BDD for drive #CL: FLAGS #SI AX #AX CX #BX DX #DX"
        popad
ENDIF
        xor     eax,eax
        test    [edi.Client_Flags],CF_Mask
        jnz     CreateBDD_Exit
        push    ecx
        and     cl,7Fh
        cmp     cl,[edi.Client_DL]
        pop     ecx
        jae     CreateBDD_Exit
        cmp     [edi.Client_DL],0
        je      CreateBDD_Exit
        cmp     [edi.Client_CL],0
        je      CreateBDD_Exit
        cmp     [edi.Client_CH],0
        je      CreateBDD_Exit

        test    cl,cl
        mov     ecx,200h
        js      CreateDBB_UseHDSectorSize
        movzx   eax,[edi.Client_ES]
        shl     eax,4
        mov     cx,[edi.Client_DI]
        add     eax,ecx
        add     eax,[ebx.CB_High_Linear]
        movzx   eax,byte ptr [eax + 3]
        xor     ecx,ecx
        bts     ecx,eax
        shl     ecx,7
  CreateDBB_UseHDSectorSize:
        mov     [edx.BDD_Sector_Size],ecx
        movzx   eax,[edi.Client_CX]
        xchg    al,ah
        shr     ah,6
        inc     eax
        mov     [edx.BDD_Num_Cylinders],eax
        movzx   eax,[edi.Client_CL]
        and     al,3Fh
        mov     [edx.BDD_Num_Sec_Per_Track],eax
        mov     al,[edi.Client_DH]
        inc     eax
        mov     [edx.BDD_Num_Heads],eax

        movzx   ecx,[edi.Client_DL]

        mov     eax,1

  CreateBDD_Exit:

        VMMcall End_Nest_Exec
        Pop_Client_State

        pop     edi
        pop     esi
        pop     ebx

        cld
        ret

CreateBDDForRealModeDrive  ENDP

;=============================================================================
;==
;==  Find_Int1B_Drive:
;==
;==  Entry:
;==     AL = int13 drive number.
;==  Exit:
;==     Carry clear on success, edi points to DCB
;==     Carry set on fail.
;==
;=============================================================================
BeginProc Find_Int1B_Drive
	push    ebx
	push    edx
	push    esi

	mov     dl, al
;
;  Check to see if we have intialized
;
	btr     [GeneralFlags], fFirstBit       ;Q: First time called?
	jnc     short @f                        ; N: continue
	call    IOSSrvInit                      ; Y: initialize
@@:

	xor     edi, edi

	sub     esp, size ISP_GET_FRST_NXT_DCB  ; allocate space for ISP
	mov     esi, esp
  FID_Find_DCB:
						; ISP function
	mov     [esi].ISP_func, ISP_GET_FIRST_NEXT_DCB
	mov     [esi].ISP_result, 0ffh          ; set result = err
	mov     [esi].ISP_gfnd_dcb_offset, edi  ; find next DCB
	mov     [esi].ISP_gfnd_dcb_type, DCB_type_floppy

	cCall   [ILBx].ILB_service_rtn, <esi>   ; call IOS routine
	cmp     [esi].ISP_result, 0
	jnz     FID_No_DCB                      ; error
	mov     edi, [esi].ISP_gfnd_found_dcb   ; address of next DCB
	cmp     dl, [edi].DCB_drive_lttr_equiv  ;
	jz      FID_Found_DCB

						; find next
	jmp     FID_Find_DCB
						; deallocate ISP structure


FID_No_DCB:
	xor     edi, edi
	add     esp, (size ISP_GET_FRST_NXT_DCB)
	stc
	jmp     exit
FID_Found_DCB:

	add     esp, (size ISP_GET_FRST_NXT_DCB)
	clc
exit:
                                        ; ***NOTE*** that I moved the
                                        ; add esp,... from here in the
                                        ; exit: block to two places above,
                                        ; in order to preserve the carry
                                        ; flag (which would otherwise be
                                        ; clobbered by the _add_.
                                        ; You must never jump straight to
                                        ; exit: w/o doing the above _add_
                                        ; to pop the stack.
	pop     esi
	pop     edx
	pop     ebx

	ret


EndProc Find_Int1B_Drive

;----------------------------------------------------------------------------
;
; BOOL IOSSrv_Chk_NEC_Floppy ( UINT uDevice );
;
; This is a high-level wrapper for Find_Int1B_Drive, really just used to
; determine if we can get a DCB for an NEC drive.
;
; Returns TRUE if NEC floppy found.
;
;----------------------------------------------------------------------------
IOSSrv_Chk_NEC_Floppy  PROC, uDevice:UINT

    push    edi
    mov     al, byte ptr uDevice

    call    Find_Int1B_Drive
    
    mov     ax, FALSE
    jc      @f
    or      edi, edi
    jz      @f
    mov     ax, TRUE
@@:
    pop     edi
    ret

IOSSrv_Chk_NEC_Floppy ENDP


ifndef NODISKABSOPERATION
;----------------------------------------------------------------------------
;
; BYTE DiskAbsOperation ( BYTE byCommand, PABSDISKREC prAbsDisk );
;
; Performs a low-level disk operation.
;
; Returns 0 if OK, or an int 13 error level code.
;
; Note: Currently returns -1 if error.  This is not a standard error
;       code, but this is also being returned by the original
;       function in DSK_ABS.C!
;
;----------------------------------------------------------------------------
DiskAbsOperation  PROC, byCommand:BYTE, prAbsDisk:DWORD

        push    ebx
        push    esi
        push    edi

        mov     esi,prAbsDisk
        mov     ebx,[esi].ABSDISKREC.adr_dwSector
        dec     ebx
        mov     eax,[esi].ABSDISKREC.adr_dwHead
        mov     edx,[esi].ABSDISKREC.adr_dwSectorsPerTrack
        mul     edx
        add     ebx,eax
        mov     eax,[esi].ABSDISKREC.adr_dwTrack
        mov     edx,[esi].ABSDISKREC.adr_dwSectorsPerTrack
        mul     edx
        mov     edx,[esi].ABSDISKREC.adr_dwTotalHeads
        mul     edx
        add     ebx,eax

        movzx   ecx,[esi].ABSDISKREC.adr_numSectors

        mov     dl,[esi].ABSDISKREC.adr_dn
        mov     esi,[esi].ABSDISKREC.adr_buffer

        mov     eax,1                   ; al = fPageable; ah = 0
        cmp     byCommand,2             ; Read?
        je      diskabsoperation_goforit
        inc     ah
        cmp     byCommand,3             ; Write?
        jne     diskabsoperation_error

  diskabsoperation_goforit:
        call    IOSSrv_DoIO
        test    eax,eax
        jz      diskabsoperation_ok

  diskabsoperation_error:
        mov     al,-1

  diskabsoperation_ok:
        pop     edi
        pop     esi
        pop     ebx
        cld
        ret

DiskAbsOperation  ENDP

;----------------------------------------------------------------------------
;
; BOOL _IOSSrv_GetInfo ( VxDDiskGetPhysicalInfo* prInfo );
;
; This is a high-level wrapper for IOSSrv_GetInfo
;
; Returns TRUE if prInfo filled with information about the drive, FALSE
; if error.
;
;----------------------------------------------------------------------------
_IOSSrv_GetInfo  PROC, prInfo:DWORD

        push    ebx
        push    esi
        push    edi

        mov     edi, prInfo
        mov     al, fPageable
        mov     dl, byte ptr [edi].GetDCB_Int13Drv
        mov     ebx, [edi].GetDCB_pDCB
        mov     ecx, [edi].GetDCB_pBDD

        call    IOSSrv_GetInfo

        mov     [edi].GetDCB_Result, eax

        test    eax,eax
        mov     eax,0
        jnz     @f
        mov     [edi].GetDCB_numDrives, ecx
        inc     eax
  @@:
        pop     edi
        pop     esi
        pop     ebx
        cld
        ret

_IOSSrv_GetInfo  ENDP

endif

VxD_LOCKED_CODE_ENDS
end
