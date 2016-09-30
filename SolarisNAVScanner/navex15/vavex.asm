; Copyright 1995 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:/NAVEX/VCS/vavex.asv   1.25   15 Jan 1998 18:43:48   JWILBER  $
;
; Description:
;       This file contains VAVEX's main entry points.  This file has to
;       be linked to VAVEX.VXD.
;
;****************************************************************************
; $Log:   S:/NAVEX/VCS/vavex.asv  $
;  
;     Rev 1.25   15 Jan 1998 18:43:48   JWILBER
;  Updated version number for Feb 98 build.
;
;     Rev 1.24   11 Dec 1997 15:22:34   JWILBER
;  Updated version number for Dec 97C build.
;
;     Rev 1.23   13 Nov 1997 16:54:14   JWILBER
;  Incremented version number for Dec 97 def build.
;
;     Rev 1.22   20 Oct 1997 18:29:52   JWILBER
;  Modified for Nov 97 build.
;
;     Rev 1.21   17 Sep 1997 16:02:50   JWILBER
;  Modified for Oct 97 build.
;
;     Rev 1.20   18 Aug 1997 12:20:50   JWILBER
;  Modified for Sep97 Build.
;
;     Rev 1.19   16 Jul 1997 17:45:18   AOONWAL
;  Modified version number for AUG update
;
;     Rev 1.18   17 Jun 1997 11:03:56   AOONWAL
;  Modified minor revision number for JULY update
;
;     Rev 1.17   19 May 1997 10:18:38   AOONWAL
;  Modified for JUNE 97 update
;
;     Rev 1.16   18 Apr 1997 19:03:56   AOONWAL
;  Modified during DEV1 malfunction
;
;     Rev ABID   16 Apr 1997 11:21:12   AOONWAL
;  Modified for MAY97
;
;     Rev 1.15   07 Apr 1997 18:11:08   DCHI
;  Added MVP support.
;
;     Rev 1.14   14 Mar 1997 17:23:24   AOONWAL
;  Updated for APRIL 97 def set
;
;     Rev 1.13   18 Feb 1997 13:16:38   AOONWAL
;  Modified for MARCH97 update
;
;     Rev 1.12   20 Jan 1997 11:57:34   AOONWAL
;  Modified version number for FEB97 update
;
;     Rev 1.11   09 Jan 1997 18:10:54   RSTANEV
;  Added pushad/popad around the call to SYMKrnl's control procedure.
;
;     Rev 1.10   23 Dec 1996 12:11:24   AOONWAL
;  Modified for JAN97 update
;
;     Rev 1.9   18 Nov 1996 10:35:50   AOONWAL
;  Changed NAVEX_MINOR_VER equate to 12 for December update
;
;     Rev 1.8   18 Oct 1996 18:52:54   AOONWAL
;  Modified for NOV96 update
;
;     Rev 1.7   17 Sep 1996 15:59:14   AOONWAL
;  Changed equate value for October build
;
;     Rev 1.6   20 Aug 1996 17:53:00   SCOURSE
;  Mods for Sep96 update
;
;     Rev 1.5   14 Jul 1996 22:21:58   SCOURSE
;  Revved for August 96
;
;     Rev 1.4   14 Jun 1996 16:53:52   SCOURSE
;  Modifications for July update
;
;     Rev 1.3   20 May 1996 11:38:26   SCOURSE
;  Revved for Jun96 update
;
;     Rev 1.2   21 Apr 1996 23:33:54   SCOURSE
;  Revved for May96 update
;
;     Rev 1.1   17 Nov 1995 10:24:12   CNACHEN
;  Vixed VAVEX->NAVEX references.
;
;     Rev 1.0   13 Oct 1995 13:06:28   DCHI
;  Initial revision.
;
;     Rev 1.0.1.0   11 Aug 1995 14:48:02   RSTANEV
;  Branch base for version QAK8
;
;     Rev 1.0   11 Aug 1995 14:29:18   RSTANEV
;  Initial revision.
;
;****************************************************************************

        .XLIST
        INCLUDE platform.inc
        .LIST

;----------------------------------------------------------------------------
;
; Global VAVEX definitions.
;
;----------------------------------------------------------------------------
NAVEX_MAJOR_VER         equ     1
NAVEX_MINOR_VER         equ     26
NAVEX_GET_EXPORT_TABLE  equ     2
ERROR_BAD_COMMAND       equ     22

;----------------------------------------------------------------------------
;
; External declarations.
;
; export_table is a structure, whose first member is a UINT containing
; the length of the structure.
;
;----------------------------------------------------------------------------
extern _export_table:UINT

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
;        cmp     ebx,DIOC_CLOSEHANDLE
;        je      short IOCTL_Return
                                        ; Process NAVEX_GET_EXPORT_TABLE
;  IOCTL_NotCloseHandle:
        cmp     ebx,NAVEX_GET_EXPORT_TABLE
        jne     short IOCTL_Return

        push    ecx
        push    esi
        push    edi

        mov     eax,ERROR_BAD_COMMAND

        mov     ecx,[esi].DIOCParams.cbOutBuffer
        cmp     ecx,_export_table
        jne     short IOCTL_GetCallbacksEnd

        mov     ebx,[esi].DIOCParams.lpcbBytesReturned
        test    ebx,ebx
        jz      short IOCTL_GetCallbacksEnd
        mov     [ebx],ecx

        mov     edi,[esi].DIOCParams.lpvOutBuffer
        test    edi,edi
        jz      short IOCTL_GetCallbacksEnd
        lea     esi,_export_table
        cld
        rep movsb

        xor     eax,eax

  IOCTL_GetCallbacksEnd:
        pop     edi
        pop     esi
        pop     ecx
        jmp     short IOCTL_End
  IOCTL_Return:
        xor     eax,eax
  IOCTL_End:
        ret

EndProc  NAVEX_lDeviceIOControl

VXD_LOCKED_CODE_ENDS

END

