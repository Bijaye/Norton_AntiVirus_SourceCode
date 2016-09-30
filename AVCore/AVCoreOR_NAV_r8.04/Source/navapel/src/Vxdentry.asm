        .XLIST
        INCLUDE platform.inc
        .LIST

;****************************************************************************
;
; Define code and data required for building NAVAPEL VxD.
;
;****************************************************************************

		EXTRN _NAVAPELQueryInterface:PROC

;----------------------------------------------------------------------------
;
; The following is the declaration of NAVAPEL.
;
;----------------------------------------------------------------------------
Declare_Virtual_Device  NAVAPEL, 1, 0, NAVAPEL_Control, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER, NULL, NULL

;----------------------------------------------------------------------------
;
; NAVAPEL_Control
;
; This is the control procedure for NAVAPEL.  It services the control
; messages sent to NAVAPEL.VXD.  Each control message has its own function
; for processing.  Below is a list of control messages and the names of
; NAVAPEL functions servicing them:
;
; ========== Win95 specific control messages ================================
;
; W32_DEVICEIOCONTROL           NAVAPDIOCHandler
;
; SYS_DYNAMIC_DEVICE_INIT       BOOL WINAPI NAVAPEL_Dynamic_Init ( VOID );
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

BeginProc  NAVAPEL_Control
                                        ; Allow SymKrnl to process all
                                        ; messages before we see them.

        pushad
        call    [SYMKrnl_DDB.DDB_Control_Proc]
        popad

                                        ; Dispatch Win95-specific control
                                        ; functions.

        Control_Dispatch  W32_DEVICEIOCONTROL, NAVAPDIOCHandler
        Control_Dispatch  SYS_DYNAMIC_DEVICE_INIT, NAVAPEL_Dynamic_Init, sCall

        clc
        ret
EndProc  NAVAPEL_Control

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
        mov     ecx,100h
        jmp     NAVAPDIOCHandler_Return
  NAVAPDIOCHandler_CheckClose:
        cmp     ebx,DIOC_CLOSEHANDLE
        je      NAVAPDIOCHandler_Return
        cmp		ebx,222AD7h ; DIOCTL_SYM_USER01
        jne		NAVAPDIOCHandler_CheckDrvComm
        mov		ebx,[esi].DIOCParams.lpvOutBuffer
        mov		[ebx],OFFSET32 _NAVAPELQueryInterface
        jmp		NAVAPDIOCHandler_Return
  NAVAPDIOCHandler_CheckDrvComm:
        cld
        sCall   DriverCommW95DispatchDIOC, <esi>
        clc
        jmp     NAVAPDIOCHandler_End
  NAVAPDIOCHandler_Return:
        xor     eax,eax
  NAVAPDIOCHandler_End:
        ret
EndProc  NAVAPDIOCHandler

VxD_PAGEABLE_CODE_ENDS

        END
