; Copyright 1995 Symantec, Peter Norton Product Group
;************************************************************************
;
; $Header:   S:/NAVEX/VCS/vavex15.asv   1.4   11 May 1998 20:17:50   MCANDELA  $
;
; Description: Modified from VAVEX.ASM, this file contains DeviceIOControl
;              routines for NAVEX15.VXD
;
; Contains:
;
; See Also:
;************************************************************************
; $Log:   S:/NAVEX/VCS/vavex15.asv  $
;  
;     Rev 1.4   11 May 1998 20:17:50   MCANDELA
;  Final decision is to implement a patch to unpatch Win98's patch to
;  
;     Rev 1.3   08 May 1998 02:44:50   MCANDELA
;  One more time - we DO need the NAVEX15 name.
;  
;     Rev 1.2   07 May 1998 13:06:42   MCANDELA
;  Restored back the NAVEX name.  Win98 specifically looks for this VxD in
;  order to work properly :-(((
;  
;     Rev 1.1   04 May 1998 10:28:10   CNACHEN
;  Made changes for rad: navex->navex15
;  
;     Rev 1.0   21 May 1997 16:46:32   MKEATIN
;  Initial revision.
;************************************************************************

        .XLIST
        INCLUDE platform.inc
        .LIST

;----------------------------------------------------------------------------
;
; Global VAVEX definitions.
;
;----------------------------------------------------------------------------
NAVEX_MAJOR_VER            equ     1
NAVEX_MINOR_VER            equ     18
NAVEX_GET_QUERY_INFERFACE  equ     2
ERROR_BAD_COMMAND          equ     22

;----------------------------------------------------------------------------
;
; External declarations.
;
; export_table is a structure, whose first member is a UINT containing
; the length of the structure.
;
;----------------------------------------------------------------------------
extern _export_table           :UINT
extern _EXTQueryInterface@8    :PROC

;----------------------------------------------------------------------------
;
; NAVEX declaration.
;
;----------------------------------------------------------------------------
VXD_LOCKED_CODE_SEG

Declare_Virtual_Device  NAVEX, NAVEX_MAJOR_VER, NAVEX_MINOR_VER,        \
                        NAVEX_Control, Undefined_Device_ID,             \
                        Undefined_Init_Order, 0, 0

        EXTRN   SYMKrnl_DDB:VxD_Desc_Block

;----------------------------------------------------------------------------
;
; NAVEX_Control procedure
;
; This procedure services control messages.
;
; Entry:
;       As per VMM's docs.
;
; Exit:
;       As per VMM's docs.
;
;----------------------------------------------------------------------------
BeginProc  NAVEX_Control
        pushad
        call    [SYMKrnl_DDB.DDB_Control_Proc]
        popad
        Control_Dispatch  W32_DEVICEIOCONTROL, NAVEX_lDeviceIOControl
        Control_Dispatch  SYS_DYNAMIC_DEVICE_INIT, NAVEXVXD_Dynamic_Init, sCall
        Control_Dispatch  SYS_DYNAMIC_DEVICE_EXIT, NAVEXVXD_Dynamic_Exit, sCall
        clc
        ret
EndProc  NAVEX_Control

;----------------------------------------------------------------------------
;
; NAVEX_lDeviceIOControl procedure
;
; This procedure is called during the W32_DEVICEIOCONTROL message.
;
; Entry:
;       eax - W32_DEVICEIOCONTROL
;       esi - pointer to DIOCParams structure
;
; Exit:
;       Varies for different messages.
;
;       Calls processed:
;               DIOC_GETVERSION
;                       returns version of VxD
;               DIOC_CLOSEHANDLE
;                       does nothing
;               NAVEX_GET_EXPORT_TABLE
;                       returns callbacks in [esi].lpvOutBuffer
;
; Uses:
;       All registers
;----------------------------------------------------------------------------
BeginProc  NAVEX_lDeviceIOControl
                                        ; Make sure pointer to rDIOC is valid
        test    esi,esi
        jz      short IOCTL_Return
                                        ; Get command
        mov     ebx,[esi].DIOCParams.dwIoControlCode
                                        ; Process DIOC_GETVERSION
        cmp     ebx,DIOC_GETVERSION
        jne     short IOCTL_NotGetVersion
        mov     ecx, ( (NAVEX_MAJOR_VER SHL 8) + NAVEX_MINOR_VER )
        jmp     short IOCTL_Return
                                        ; Process DIOC_CLOSEHANDLE
IOCTL_NotGetVersion:
                                        ; Process NAVEX_GET_QUERY_INTERFACE
        cmp     ebx,NAVEX_GET_QUERY_INFERFACE
        jne     short IOCTL_Return

        push    edi

        mov     eax,ERROR_BAD_COMMAND

        mov     edi,[esi].DIOCParams.lpvOutBuffer
        test    edi,edi
        jz      short IOCTL_GetCallbacksEnd
        mov     [edi], _EXTQueryInterface@8

        xor     eax,eax

IOCTL_GetCallbacksEnd:
        pop     edi
        jmp     short IOCTL_End
IOCTL_Return:
        xor     eax,eax
IOCTL_End:
        ret

EndProc  NAVEX_lDeviceIOControl

VXD_LOCKED_CODE_ENDS

END

