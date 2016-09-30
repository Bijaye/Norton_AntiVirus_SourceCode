; Copyright 1997 - 1998 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:/NAVAP/VCS/VXDENTRY.ASv   1.7   14 Sep 1998 16:40:58   RStanev  $
;
; Description:
;       This file contains the main entry points of NAVAP.VXD as well as
;       some VxD primitive ASM functions.
;
; Contains:
;       NAVAP_Control
;
;       NAVAPDIOCHandler
;
;       Hook_V86_Int_Chain()
;       Unhook_V86_Int_Chain()
;       SHELL_Get_Version()
;       _SHELL_ShellExecute()
;
;       IsRealModeAPRunning()
;       DisableRealModeAP()
;       EnableRealModeAP()
;
;       InstallInt2FAPI()
;       UninstallInt2FAPI()
;       Int2FHookCallback
;
;       NAVAPV86CriticalErrorHandlerDispatcher
;
;****************************************************************************
; $Log:   S:/NAVAP/VCS/VXDENTRY.ASv  $
;  
;     Rev 1.7   14 Sep 1998 16:40:58   RStanev
;  Added handling of Destroy_VM and Destroy_Thread messages.
;
;     Rev 1.6   02 Jun 1998 01:07:16   RStanev
;  Merged Roswell fixes.
;
;     Rev 1.5   17 Feb 1998 22:16:48   RStanev
;  Updated copyright.
;
;     Rev 1.4   09 Aug 1997 01:00:38   RStanev
;  Patch for support of old NAVAP APIs.
;
;     Rev 1.3   27 Jun 1997 21:31:28   RStanev
;  Dispatching Terminate_Thread and VM_Terminate messages.
;
;     Rev 1.2   21 May 1997 05:01:22   RStanev
;  Changed the definition segments of several wraps.
;
;     Rev 1.1   15 May 1997 03:01:30   RStanev
;  Too many changes to describe.
;
;     Rev 1.0   09 Apr 1997 12:21:52   RStanev
;  Initial revision.
;
;****************************************************************************

        .XLIST
        INCLUDE platform.inc
        INCLUDE shell.inc
        INCLUDE navap.inc
        .LIST

;****************************************************************************
;
; Define code and data required for building NAVAP VxD.
;
;****************************************************************************

;----------------------------------------------------------------------------
;
; The following is the declaration of NAVAP.
;
;----------------------------------------------------------------------------
Declare_Virtual_Device  NAVAP, NAVAP_MAJOR_VER, NAVAP_MINOR_VER,        \
                        NAVAP_Control, NAVAP_Device_ID,                 \
                        UNDEFINED_INIT_ORDER, NULL, NULL

;----------------------------------------------------------------------------
;
; NAVAP_Control
;
; This is the control procedure for NAVAP.  It services the control
; messages sent to NAVAP.VXD.  Each control message has its own function
; for processing.  Below is a list of control messages and the names of
; NAVAP functions servicing them:
;
; ========== Global messages ================================================
;
; Init_Complete                 BOOL WINAPI NAVAP_Init_Complete ( VOID );
;
; Sys_VM_Terminate              BOOL WINAPI NAVAP_Sys_VM_Terminate ( VOID );
;
; Terminate_Thread              BOOL WINAPI NAVAP_Terminate_Thread ( PTCB pTCB );
;
; VM_Terminate                  BOOL WINAPI NAVAP_VM_Terminate ( HVM hVM );
;
; Destroy_Thread                BOOL WINAPI NAVAP_Destroy_Thread ( PTCB pTCB );
;
; Destroy_VM                    BOOL WINAPI NAVAP_Destroy_Thread ( PTCB pTCB );
;
; ========== Win95 specific control messages ================================
;
; W32_DEVICEIOCONTROL           NAVAPDIOCHandler
;
; SYS_DYNAMIC_DEVICE_INIT       BOOL WINAPI NAVAP_Dynamic_Init ( VOID );
;
; SYS_DYNAMIC_DEVICE_EXIT       BOOL WINAPI NAVAP_Dynamic_Exit ( VOID );
;
; ===========================================================================
;
; Entry:
;       eax - control message
;
; Exit:
;       cf - 0 if successful
;       cf - 1 if error
;
; Uses:
;       eax, ebx, ecx, edx, esi, edi, flags
;
;----------------------------------------------------------------------------
VxD_LOCKED_CODE_SEG

        EXTRN   SYMKrnl_DDB:VxD_Desc_Block

BeginProc  NAVAP_Control
                                        ; Allow SymKrnl to process all
                                        ; messages before we see them.

        pushad
        call    [SYMKrnl_DDB.DDB_Control_Proc]
        popad

                                        ; Dispatch global control functions.

        Control_Dispatch  Init_Complete, NAVAP_Init_Complete, sCall
        Control_Dispatch  Sys_VM_Terminate, NAVAP_Sys_VM_Terminate, sCall
        Control_Dispatch  Terminate_Thread, NAVAP_Terminate_Thread, sCall, <edi>
        Control_Dispatch  VM_Terminate, NAVAP_VM_Terminate, sCall, <ebx>
        Control_Dispatch  Destroy_Thread, NAVAP_Destroy_Thread, sCall, <edi>
        Control_Dispatch  Destroy_VM, NAVAP_Destroy_Thread, sCall, <edi>

                                        ; Dispatch Win95-specific control
                                        ; functions.

        Control_Dispatch  W32_DEVICEIOCONTROL, NAVAPDIOCHandler
        Control_Dispatch  SYS_DYNAMIC_DEVICE_INIT, NAVAP_Dynamic_Init, sCall
        Control_Dispatch  SYS_DYNAMIC_DEVICE_EXIT, NAVAP_Dynamic_Exit, sCall

                                        ; Dispatch control function for
                                        ; making us compatible with Win98.

        Control_Dispatch  Device_Reboot_Notify, NAVAP_Sys_VM_Terminate, sCall
        cmp     eax,36h                 ; SYS_POWER_DOWN from Win98's DDK.
        jne     NotAPMShutdown
        sCall   NAVAP_Sys_VM_Terminate
NotAPMShutdown:
        clc
        ret
EndProc  NAVAP_Control

VxD_LOCKED_CODE_ENDS

;----------------------------------------------------------------------------
;
; NAVAPDIOCHandler
;
; This procedure is called during the W32_DEVICEIOCONTROL message.  It
; dispatches the call to DriverCommW95DispatchDIOC().
;
; Entry:
;       eax - W32_DEVICEIOCONTROL
;       esi - pointer to DIOCParams structure
;
; Exit:
;       Varies for different messages.
;
; Uses:
;       All registers
;
;----------------------------------------------------------------------------
VxD_PAGEABLE_CODE_SEG

BeginProc  NAVAPDIOCHandler
        mov     ebx,[esi].DIOCParams.dwIoControlCode
        cmp     ebx,DIOC_GETVERSION
        jne     NAVAPDIOCHandler_CheckClose
        mov     ecx,( (NAVAP_MAJOR_VER SHL 8) + NAVAP_MINOR_VER )
        jmp     NAVAPDIOCHandler_Return
  NAVAPDIOCHandler_CheckClose:
        cmp     ebx,DIOC_CLOSEHANDLE
        je      NAVAPDIOCHandler_Return
        cld
;------------------------------------------------------------------- CUT HERE
; This is a patch for supporting the process protection APIs
; provided by older versions of NAVAP (Luigi and Kirin).  This
; patch should be removed as soon as all products using the old
; APIs are updated and distributed.  The patch function
; EmulateOldDIOC() is located in NAVAPVXD.CPP.
;
        sCall   EmulateOldDIOC, <esi>
        test    eax,eax
        jz      NAVAPDIOCHandler_End
;------------------------------------------------------------------- CUT HERE
        sCall   DriverCommW95DispatchDIOC, <esi>
        clc
        jmp     NAVAPDIOCHandler_End
  NAVAPDIOCHandler_Return:
        xor     eax,eax
  NAVAPDIOCHandler_End:
        ret
EndProc  NAVAPDIOCHandler

VxD_PAGEABLE_CODE_ENDS

;****************************************************************************
;
; Define VxD wraps missing from the Win95 DDK.
;
;****************************************************************************
VxD_INIT_CODE_SEG

                                        ; Define Hook_V86_Int_Chain()

        OPTION LANGUAGE:C

        PUBLIC Hook_V86_Int_Chain
Hook_V86_Int_Chain  PROC,  Interrupt:DWORD, pfnCallBack:DWORD
        xor     eax,eax
        pushad
        mov     eax,Interrupt
        mov     esi,pfnCallback
        VxDCall Hook_V86_Int_Chain
        popad
        cmc
        setc    al
        ret
Hook_V86_Int_Chain  ENDP

VxD_INIT_CODE_ENDS

VxD_PAGEABLE_CODE_SEG

                                        ; Define Unhook_V86_Int_Chain()

        PUBLIC Unhook_V86_Int_Chain
Unhook_V86_Int_Chain  PROC,  Interrupt:DWORD, pfnCallBack:DWORD
        xor     eax,eax
        pushad
        mov     eax,Interrupt
        mov     esi,pfnCallback
        VxDCall Unhook_V86_Int_Chain
        popad
        cmc
        setc    al
        ret
Unhook_V86_Int_Chain  ENDP

                                        ; Define SHELL_Get_Version()

        OPTION LANGUAGE:SYSCALL

        PUBLIC _SHELL_Get_Version
  _SHELL_Get_Version:
        VxDJmp  SHELL_Get_Version

                                        ; Define _SHELL_ShellExecute()

        PUBLIC __SHELL_ShellExecute
  __SHELL_ShellExecute:
        VxDJmp _SHELL_ShellExecute

VxD_PAGEABLE_CODE_ENDS

;****************************************************************************
;
; Define DOS communication functions and necessary data.
;
;****************************************************************************
VxD_PAGEABLE_DATA_SEG

  pwRMAPReentranceCount  DD  0

VxD_PAGEABLE_DATA_ENDS

;----------------------------------------------------------------------------
;
; BOOL WINAPI IsRealModeAPRunning ( VOID );
;
; Checks if the real mode auto-protect is loaded.  If it is loaded, then
; get a pointer to its global reentrance counter and save it at
; pwRMAPReentranceCount.  If the auto-protect is an old version, then this
; function will still return TRUE, but the reentrance counter pointer will
; be set to NULL.
;
; Note:
;       The strings in this function should not be translated.
;
;----------------------------------------------------------------------------
VxD_INIT_CODE_SEG

        OPTION LANGUAGE:STDCALL

        PUBLIC  IsRealModeAPRunning
IsRealModeAPRunning  PROC

        LOCAL   bResult:BOOL

        push    ebx
        push    esi
        push    edi

                                        ; Assume TSR is not loaded
        mov     bResult, 0

                                        ; Check if loaded
        Push_Client_State

        VMMCall Begin_Nest_V86_Exec

        VMMCall Get_Cur_VM_Handle
        mov     edx,[ebx].cb_s.CB_Client_Pointer

                                        ; Get TSR status
        mov     [edx].Client_Reg_Struc.Client_EAX,0FE00h
        mov     [edx].Client_Reg_Struc.Client_ESI,'NA'
        mov     [edx].Client_Reg_Struc.Client_EDI,'NU'

        mov     eax,2Fh
        VMMCall Exec_Int

        VMMCall Get_Cur_VM_Handle
        mov     edx,[ebx].cb_s.CB_Client_Pointer

                                        ; Check if TSR loaded
        cmp     [edx].Client_Word_Reg_Struc.Client_SI,'na'
        jne     no_rmap
        cmp     [edx].Client_Word_Reg_Struc.Client_DI,'nu'
        jne     no_rmap

                                        ; Set result to TRUE if RMAP present
        inc     bResult

                                        ; Get pointer to reentrance count
        mov     [edx].Client_Reg_Struc.Client_EAX,0FE05h
        mov     [edx].Client_Reg_Struc.Client_ESI,'NA'
        mov     [edx].Client_Reg_Struc.Client_EDI,'NU'
        mov     [edx].Client_Reg_Struc.Client_EDX,0

        mov     eax,2Fh
        VMMCall Exec_Int

        VMMCall Get_Cur_VM_Handle
        mov     edx,[ebx].cb_s.CB_Client_Pointer

                                        ; Check if a pointer was returned
        movzx   eax,[edx].Client_Word_Reg_Struc.Client_DX
        movzx   edx,[edx].Client_Word_Reg_Struc.Client_AX
        test    eax,eax
        jz      no_rmap
        shl     eax,4
        add     eax,edx
        push    eax
        VMMCall _TestGlobalV86Mem, <eax, 2, 0>
        cmp     eax,1
        pop     eax
        jne     no_rmap
        mov     pwRMAPReentranceCount,eax

  no_rmap:

        VMMCall End_Nest_Exec

        Pop_Client_State

        pop     edi
        pop     esi
        pop     ebx
        cld

        mov     eax,bResult

        ret

IsRealModeAPRunning  ENDP

;----------------------------------------------------------------------------
;
; BOOL WINAPI DisableRealModeAP ( VOID );
;
; Disables the real mode auto-protect.
;
; Returns TRUE if successfull, FALSE if error.
;
; Note:
;       This function can be called only if IsRealModeAPRunning()
;       returned TRUE.
;
;----------------------------------------------------------------------------
        PUBLIC  DisableRealModeAP
DisableRealModeAP  PROC

        mov     eax,[pwRMAPReentranceCount]
        test    eax,eax
        jz      cant_disable
        inc     WORD PTR [eax]
        mov     eax,1
  cant_disable:
        ret

DisableRealModeAP  ENDP

VxD_INIT_CODE_ENDS

;----------------------------------------------------------------------------
;
; BOOL WINAPI EnableRealModeAP ( VOID );
;
; Enables the real mode auto-protect.
;
; Returns TRUE if successfull, FALSE if error.
;
; Note:
;       This function can be called only if IsRealModeAPRunning()
;       returned TRUE.
;
;----------------------------------------------------------------------------
VxD_PAGEABLE_CODE_SEG

        PUBLIC  EnableRealModeAP
EnableRealModeAP  PROC

        mov     eax,[pwRMAPReentranceCount]
        test    eax,eax
        jz      cant_enable
        dec     WORD PTR [eax]
        mov     eax,1
  cant_enable:
        ret

EnableRealModeAP  ENDP

VxD_PAGEABLE_CODE_ENDS

;----------------------------------------------------------------------------
;
; BOOL WINAPI InstallInt2FAPI ( VOID );
;
; Installs a real-mode interrupt 2F API hook.
;
; Returns TRUE if successfull, FALSE if error.
;
;----------------------------------------------------------------------------
VxD_INIT_CODE_SEG

        PUBLIC  InstallInt2FAPI
InstallInt2FAPI  PROC

        push    esi

        mov     eax,2Fh
        lea     esi,[Int2FHookCallback]
        VMMCall Hook_V86_Int_Chain
        sbb     eax,eax
        inc     eax

        pop     esi

        ret

InstallInt2FAPI  ENDP

VxD_INIT_CODE_ENDS

;----------------------------------------------------------------------------
;
; BOOL WINAPI UninstallInt2FAPI ( VOID );
;
; Uninstalls the real-mode interrupt 2F API hook.
;
; Returns TRUE if successfull, FALSE if error.
;
;----------------------------------------------------------------------------
VxD_PAGEABLE_CODE_SEG

        PUBLIC  UninstallInt2FAPI
UninstallInt2FAPI  PROC

        push    esi

        mov     eax,2Fh
        lea     esi,[Int2FHookCallback]
        VMMCall Unhook_V86_Int_Chain
        sbb     eax,eax
        inc     eax

        pop     esi

        ret

UninstallInt2FAPI  ENDP

;----------------------------------------------------------------------------
;
; Int2FHookCallback callback function
;
; This function is called every time a real-mode interrupt 2Fh occurs.  It
; dispatches the call to the more flexible Int2FChainCallback() function.
;
; Entry:
;       eax - interrupt number (should be 2Fh)
;       ebx - current VM
;       ebp - pointer to client's registers
;
; Exit:
;       cf cleared if function processed
;               client registers set appropriately
;       cf set if function not processed
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:SYSCALL
Int2FHookCallback  PROC

        push    ebx
        push    esi
        push    edi

        cmp     eax,2Fh
        stc
        jne     no_int2f_processing

        cld
        sCall   Int2FChainCallback, <ebp>
        cmp     eax,1

  no_int2f_processing:
        pop     edi
        pop     esi
        pop     ebx

        ret

Int2FHookCallback  ENDP

;----------------------------------------------------------------------------
;
; NAVAPV86CriticalErrorHandlerDispatcher
;
; This procedure is called by the VMM upon a V86 interrupt 24h.  It
; dispatches the call to the C function NAVAPV86CriticalErrorHandler for
; further processing.  The critical error handler is installed by
; NAVAPGlobalInitPlatformSpecific().
;
; Entry:
;       eax = 24h
;       ebx = current VM handle (or the so called "Magic number")
;       ebp = pointer to client's registers
;
; Exit:
;       cf set or cleared, depending on the return status of
;       NAVAPV86CriticalErrorHandler().
;
; Uses:
;       flags
;
; Note:
;       Since the handler is installed by a C function, it is important that
;       NAVAPV86CriticalErrorHandlerDispatcher's naming convention is
;       accessible from C and at the same time its prolog/epilog code are
;       compatible with Windows 95 callbacks.  That's the reason we have
;       the OPTION directives around BeginProc and EndProc.
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:STDCALL
BeginProc  NAVAPV86CriticalErrorHandlerDispatcher
        OPTION  LANGUAGE:SYSCALL
        pushad
        sCall   NAVAPV86CriticalErrorHandler, <ebp>
        cmp     eax,1
        popad
        ret
        OPTION  LANGUAGE:STDCALL
EndProc  NAVAPV86CriticalErrorHandlerDispatcher
        OPTION  LANGUAGE:SYSCALL

VxD_PAGEABLE_CODE_ENDS

        END
