; Copyright 1997 - 1998 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:/APUI/VCS/VXDUI.ASv   1.1   17 Feb 1998 20:54:06   RStanev  $
;
; Description:
;       This file defines VxD specific ASM functions for APUI.
;
; Contains:
;       ScheduleSoundAlert()
;       SoundAlert()            (local function)
;       SoundAlertNEC()         (local function)
;       SoundAlertCallback()    (local function)
;       _Begin_Message_Mode()
;       _End_Message_Mode()
;       _VKD_Get_Kbd_Owner()
;       _VKD_Set_Kbd_Owner()
;       _VMD_Get_Mouse_Owner()
;       _VMD_Set_Mouse_Owner()
;       _VDD_Msg_SetCursPos()
;       _VDD_Msg_ClrScrn()
;       _VDD_Msg_BakColor()
;       _VDD_Msg_ForColor()
;       _VDD_Msg_TextOut()
;       ClearKeyboardQueue()
;       GetKeyPressed()
;       SleepDead()
;
;****************************************************************************
; $Log:   S:/APUI/VCS/VXDUI.ASv  $
;  
;     Rev 1.1   17 Feb 1998 20:54:06   RStanev
;  Updated copyright.
;
;     Rev 1.0   09 Apr 1997 12:16:08   RStanev
;  Initial revision.
;
;****************************************************************************

MINIVDD = 1

        .XLIST
        INCLUDE platform.inc
        INCLUDE minivdd.inc
        INCLUDE vkd.inc
        INCLUDE vmd.inc
        .LIST

;----------------------------------------------------------------------------
;
; Miscellaneous constants.
;
;----------------------------------------------------------------------------
NEC_SPKR_MASK  EQU  00001000b

;----------------------------------------------------------------------------
;
; bHWIsNEC allows us to check for NEC hardware.
;
; dwSoundAlertScheduled is a global flag
;
;----------------------------------------------------------------------------
VXD_LOCKED_DATA_SEG

        EXTRN _bHWIsNEC:BOOL

dwSoundAlertScheduled  DD  0

VXD_LOCKED_DATA_ENDS

VXD_LOCKED_CODE_SEG

        PUBLIC  ScheduleSoundAlert
        PUBLIC  _Begin_Message_Mode
        PUBLIC  _End_Message_Mode
        PUBLIC  _VKD_Get_Kbd_Owner
        PUBLIC  _VKD_Set_Kbd_Owner
        PUBLIC  _VMD_Get_Mouse_Owner
        PUBLIC  _VMD_Set_Mouse_Owner
        PUBLIC  _VDD_Msg_SetCursPos
        PUBLIC  _VDD_Msg_ClrScrn
        PUBLIC  _VDD_Msg_BakColor
        PUBLIC  _VDD_Msg_ForColor
        PUBLIC  _VDD_Msg_TextOut
        PUBLIC  ClearKeyboardQueue
        PUBLIC  GetKeyPressed
        PUBLIC  SleepDead

;----------------------------------------------------------------------------
;
; VOID WINAPI ScheduleSoundAlert ( UINT uSoundType, UINT uTime );
;
; Schedules to play NAVAP's sound alert after uTime milliseconds.  Note,
; that if a sound alert is pending, calls to ScheduleSoundAlert() will be
; ignored!
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:STDCALL

ScheduleSoundAlert  PROC ,
        uSoundType:UINT,
        uTime:UINT

        mov     eax,1
        xchg    dwSoundAlertScheduled,eax
        test    eax,eax
        jnz     SkipSoundAlertSchedule

        mov     eax,uTime
        mov     edx,uSoundType
        lea     esi,SoundAlertCallback
        VMMCall Set_Global_Time_Out

SkipSoundAlertSchedule:
        ret

ScheduleSoundAlert  ENDP

;----------------------------------------------------------------------------
;
; SoundAlert  PROC, uSoundTye:UINT
;
; Plays NAVAP's sound alert uSoundType on IBM/AT compatible hardware.
;
;----------------------------------------------------------------------------
SoundAlert  PROC  ,
        uSoundType:UINT

        push    edi
        pushf
        cli
                                        ; Turn the speaker on
        in      al,61h
        or      al,3
        jmp     $+2
        out     61h,al
                                        ; Start main beep loop
        mov     ecx,3

RepeatBeep:
        push    ecx
                                        ; Start single beep loop
        mov     ecx,400d
        mov     edi,200d
ChangeTone:
                                        ; Play a tone
        mov     eax,144F38h
        mov     edx,0
        div     edi
        out     42h,al
        mov     al,ah
        jmp     $+2
        out     42h,al
                                        ; A small delay
        push    ecx
        mov     ecx,0B00h
        loop    $
        pop     ecx
                                        ; Calculate new sound
        add     edi,uSoundType
        loop    ChangeTone
                                        ; Repeat the beep several times
        pop     ecx
        loop    RepeatBeep
                                        ; Turn speaker off
        in      al,61h
        and     al,0FCh
        jmp     $+2
        out     61h,al
                                        ; Return to caller
        popf
        pop     edi
        ret

SoundAlert  ENDP

;----------------------------------------------------------------------------
;
; SoundAlertNEC  PROC
;
; Plays NAVAP's sound alert on NEC hardware.
;
;----------------------------------------------------------------------------
SoundAlertNEC  PROC

        pushf
        pushad
        cli

        in       al, 035h		; Get current state of speaker control
        and      al, not NEC_SPKR_MASK  ; Turn on speaker
        out      035h, al		; by clearing bit 4
        jmp      $+2                    ; small delay

        mov      al, 076h               ; Prepare timer 1 (speaker)
        mov      dx,3fdfh               ; for frequency code
        out      dx,al
        jmp      $+2
                                        ; Start main beep loop
        mov      ecx,3
RepeatBeepNEC:
        push     ecx
                                        ; Start single beep loop
        mov      ecx,400d
        mov      di, 200d

ChangeToneNEC:
        in	 al, 42h		; Read signal
        test     al, 00100000B
        jnz      is8m
        mov      dx, 25h		; MSW
        mov      ax, 8000h	        ; LSW  (2,457,600)
        jmp	 short countset
is8m:
        mov      dx, 1Eh		; MSW
        mov      ax, 7800h              ; LSW  (1,996,800)
countset:
        div	 di
        mov      dx, 3fdbh
        out      dx, al
        jmp      $+2
	mov	 al, ah                 ; Get high byte
	out	 dx,al
        jmp      $+2

        push     ecx
        mov      ecx, 0000B00h          ; Delay
        loop     $
        pop      ecx

        add      di, 5                  ; next tone
        loop     ChangeToneNEC

        pop      ecx
        loop     RepeatBeepNEC

        in       al, 035h
        or       al, NEC_SPKR_MASK      ; Set bit 4 (to turn speaker off)
        out      035h,al                ; Turn speaker off

        popad
        popf
        ret

SoundAlertNEC  ENDP

;----------------------------------------------------------------------------
;
; SoundAlertCallback  PROC
;
; This procedure is called by the system VM when the sound alert uTime
; timeout had expired.
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:SYSCALL

SoundAlertCallback  PROC

        cmp     _bHWIsNEC,0
        jne     @f
        push    edx
        call    SoundAlert
        mov     dwSoundAlertScheduled,0
        ret
  @@:
        call    SoundAlertNEC
        mov     dwSoundAlertScheduled,0
        ret

SoundAlertCallback  ENDP

;----------------------------------------------------------------------------
;
; All functions below this line will be called from C++ code.
;
;----------------------------------------------------------------------------
        OPTION  LANGUAGE:STDCALL

;----------------------------------------------------------------------------
;
; VOID WINAPI _Begin_Message_Mode (
;       VOID
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_Begin_Message_Mode  PROC

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax,Begin_Message_Mode
        VMMCall System_Control
        pop     ebx
        cld
        ret

_Begin_Message_Mode  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _End_Message_Mode (
;       VOID
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_End_Message_Mode  PROC

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax,End_Message_Mode
        VMMCall System_Control
        pop     ebx
        cld
        ret

_End_Message_Mode  ENDP

;----------------------------------------------------------------------------
;
; UINT WINAPI _VKD_Get_Kbd_Owner ( VOID );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VKD_Get_Kbd_Owner  PROC

        push    ebx
        VxDCall VKD_Get_Kbd_Owner
        mov     eax,ebx
        pop     ebx
        cld
        ret

_VKD_Get_Kbd_Owner  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VKD_Set_Kbd_Owner ( UINT uVM );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VKD_Set_Kbd_Owner  PROC , uVM:UINT

        push    ebx
        clc
        mov     ebx,uVM
        mov     eax,Set_Device_Focus
        mov     edx,VKD_Device_ID
        VMMCall System_Control
        pop     ebx
        cld
        ret

_VKD_Set_Kbd_Owner  ENDP

;----------------------------------------------------------------------------
;
; UINT WINAPI _VMD_Get_Mouse_Owner ( VOID );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VMD_Get_Mouse_Owner  PROC

        push    ebx
        VxDCall VMD_Get_Version
        mov     eax,0
        jc      _VMD_Get_Mouse_Owner_no_mouse
        VxDCall VMD_Get_Mouse_Owner
        mov     eax,ebx
  _VMD_Get_Mouse_Owner_no_mouse:
        pop     ebx
        cld
        ret

_VMD_Get_Mouse_Owner  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VMD_Set_Mouse_Owner (
;       UINT uVM
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VMD_Set_Mouse_Owner  PROC , uVM:UINT

        push    ebx
        VxDCall VMD_Get_Version
        mov     eax,0
        jc      _VMD_Set_Mouse_Owner_no_mouse
        mov     ebx,uVM
        mov     eax,Set_Device_Focus
        mov     edx,VMD_Device_ID
        VMMCall System_Control
  _VMD_Set_Mouse_Owner_no_mouse:
        pop     ebx
        cld
        ret

_VMD_Set_Mouse_Owner  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VDD_Msg_SetCursPos (
;       UINT uPosX,
;       UINT uPosY
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VDD_Msg_SetCursPos  PROC , uPosX:UINT, uPosY:UINT

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     edx,uPosX
        mov     eax,uPosY
        VxDcall VDD_Msg_SetCursPos
        pop     ebx
        cld
        ret

_VDD_Msg_SetCursPos  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VDD_Msg_ClrScrn (
;       UINT  uScreenAttributes,
;       PUINT puSizeX,
;       PUINT puSizeY
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VDD_Msg_ClrScrn  PROC , uScreenAttributes:UINT, puSizeX:DWORD, puSizeY:DWORD

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax,uScreenAttributes
        VxDCall VDD_Msg_ClrScrn
        mov     ebx,puSizeX
        mov     [ebx],eax
        mov     ebx,puSizeY
        mov     [ebx],edx
        pop     ebx
        cld
        ret

_VDD_Msg_ClrScrn  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VDD_Msg_BakColor (
;       UINT uBackgroundColor
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VDD_Msg_BakColor  PROC , uBackgroundColor:UINT

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax,uBackgroundColor
        VxDCall VDD_Msg_BakColor
        pop     ebx
        cld
        ret

_VDD_Msg_BakColor  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VDD_Msg_ForColor (
;       UINT uForegroundColor
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VDD_Msg_ForColor  PROC , uForegroundColor:UINT

        push    ebx
        VMMCall Get_Cur_VM_Handle
        mov     eax,uForegroundColor
        VxDCall VDD_Msg_ForColor
        pop     ebx
        cld
        ret

_VDD_Msg_ForColor  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI _VDD_Msg_TextOut (
;       PSTR pszText,
;       UINT uTextLen,
;       UINT uPosX,
;       UINT uPosY
;       );
;
; This is a VxD wrap.
;
;----------------------------------------------------------------------------
_VDD_Msg_TextOut  PROC , pszText:DWORD, uTextLen:UINT, uPosX:UINT, uPosY:UINT

        push    ebx
        push    esi
        VMMCall Get_Cur_VM_Handle
        mov     esi,pszText
        mov     ecx,uTextLen
        mov     eax,uPosY
        mov     edx,uPosX
        VxDCall VDD_Msg_TextOut
        pop     esi
        pop     ebx
        cld
        ret

_VDD_Msg_TextOut  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI ClearKeyboardQueue (
;       VOID
;       );
;
; This functions clears all keystrokes from the message mode keyboard queue.
;
;----------------------------------------------------------------------------
ClearKeyboardQueue  PROC

        push    ebx
        VMMCall Get_Cur_VM_Handle
        VxDCall VKD_Flush_Msg_Key_Queue
        pop     ebx
        cld
        ret

ClearKeyboardQueue  ENDP

;----------------------------------------------------------------------------
;
; BOOL WINAPI GetKeyPressed (
;       PWORD pwKeyCode
;       );
;
; This function waits for a keystroke.
;
;----------------------------------------------------------------------------
GetKeyPressed  PROC  , pwKeyCode:DWORD

        push    ebx
        VMMCall Get_Cur_VM_Handle
        Push_Client_State
        VMMcall Begin_Nest_Exec
        VxDcall Resume_Exec
        VxDcall VKD_Get_Msg_Key
        mov     edx,0
        jz      nokey
        inc     edx
        mov     ebx,pwKeyCode
        mov     [ebx],ax
  nokey:
        VMMcall End_Nest_Exec
        Pop_Client_State
        mov     eax,edx
        pop     ebx
        cld
        ret

GetKeyPressed  ENDP

;----------------------------------------------------------------------------
;
; VOID WINAPI SleepDead (
;       UINT uMilliseconds
;       );
;
; I completely forgot why this function was implemented.  It is used only
; for a delay after shutdown scan until the floppies recover.
;
;----------------------------------------------------------------------------
SleepDead  PROC  , uMilliseconds:UINT

        pushad

        VMMCall Get_Cur_VM_Handle
        Push_Client_State
        VMMCall Begin_Nest_Exec

        VMMCall Get_System_Time
        add     uMilliseconds,eax

  loopmore:
        VxDCall Resume_Exec
        VMMCall Get_System_Time
        cmp     eax,uMilliseconds
        jb      loopmore

        VMMCall End_Nest_Exec
        Pop_Client_State

        popad
        cld
        ret

SleepDead  ENDP

VXD_LOCKED_CODE_ENDS

        END
