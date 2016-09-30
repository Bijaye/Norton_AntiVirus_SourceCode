;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fseek.asv   1.13   02 Feb 1996 21:26:20   RSTANEV  $
;;
;; Description: Defines FIL4_SEEK function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fseek.asv  $
;  
;     Rev 1.13   02 Feb 1996 21:26:20   RSTANEV
;  Added support for SYM_NTK.
;
;     Rev 1.12   06 Feb 1995 10:53:14   DALLEE
;  Replaced buggy, impossible-to-read code to negate the bytes to seek number
;  with sub 1, neg.
;
;     Rev 1.11   20 Dec 1994 14:26:36   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.10   19 Aug 1994 21:47:56   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.9   17 Aug 1994 13:26:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.8   08 Aug 1994 18:55:58   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.7   01 Aug 1994 18:08:48   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.6   01 Aug 1994 17:17:36   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.5   01 Aug 1994 14:49:02   CAREY
;
;     Rev 1.4   29 Jul 1994 13:20:40   CAREY
;
;     Rev 1.3   27 Jul 1994 13:09:04   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.2   26 May 1994 15:20:48   RSTANEV
;  Minor changes.
;
;     Rev 1.1   05 May 1994 12:44:02   RSTANEV
;  Now calling _llseek instead of _LLSEEK.
;
;     Rev 1.0   02 May 1994 18:15:06   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

;----------------------------------------------------------------------------
; FIL4_SEEK
;
; Seek to a new place in file.
;
; DWORD WINAPI FIL4_SEEK ( UINT Handle, DWORD Number, WORD Flag );
;
; Input:
;       1)  Handle
;       2)  Bytes to seek
;       3)  Seek flag
;               fSEEK_BEG (1)  Seek from beginning of file
;               fSEEK_FOR (2)  Seek forward from current position
;               fSEEK_BAK (4)  Seek backward from current position
;               fSEEK_END (8)  Seek backward from end of file
; Output:
;       1)  New location of handle.
;       2)  -1 if error
;
;----------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE AVAPI.INC

UniqueDataSegment

EXTERN          C lpgCallBack:DWORD

UniqueDataSegmentEnd

UniqueSegment


SEEK_SET                equ             0
SEEK_CUR                equ             1
SEEK_END                equ             2


IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_SEEK  PROC  CODEDISTANCE  USES XBX XCX XSI XDI SDS,
        Handle:UINT,
        Number:DWORD,
        Flags:WORD

        ; set up CX:DX to point to seek area

        mov     cx,WORD PTR Number + 2
        mov     dx,WORD PTR Number

        cmp     Flags, fSEEK_BEG
        jne     try_seek_for

        mov     di, SEEK_SET

        jmp     J_Seek

try_seek_for:

        cmp     Flags, fSEEK_FOR
        jne     try_seek_bak

        mov     di, SEEK_CUR

        jmp     J_Seek

try_seek_bak:

        cmp     Flags, fSEEK_BAK
        jne     try_seek_end

        mov     di, SEEK_CUR
        jmp     Neg_Offset

try_seek_end:

        cmp     Flags, fSEEK_END
        jne     J_Error

        mov     di, SEEK_END
        jmp     Neg_Offset

Neg_Offset:
        sub     dx,1
        sbb     cx,0
        not     dx
        not     cx
        jmp     J_Seek

J_Error:

        mov     ax, -1
        mov     dx, ax
        jmp     J_Done

J_Seek:

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF

        LDS_SI  DWORD PTR lpgCallBack
        INVOKE  [XSI].CALLBACKREV1.FileSeek, Handle, CX::DX, DI

J_Done:
        ret

  FIL4_SEEK  ENDP

ELSEIF bSYM_WIN32 or bSYM_VXD OR bSYM_NLM OR bSYM_NTK

  FIL4_SEEK  PROC  CODEDISTANCE  USES XBX XCX XSI XDI,
        Handle:UINT,
        Number:DWORD,
        Flags:WORD



        ; set up CX:DX to point to seek area

        mov     XAX, Number

        cmp     Flags, fSEEK_BEG
        jne     try_seek_for

        mov     XDI, SEEK_SET

        jmp     J_Seek

try_seek_for:

        cmp     Flags, fSEEK_FOR
        jne     try_seek_bak

        mov     XDI, SEEK_CUR

        jmp     J_Seek

try_seek_bak:

        cmp     Flags, fSEEK_BAK
        jne     try_seek_end

        mov     XDI, SEEK_CUR

        neg     XAX
        jmp     J_Seek

try_seek_end:

        cmp     Flags, fSEEK_END
        jne     J_Error

        mov     XDI, SEEK_END
        neg     XAX
        jmp     J_Seek

J_Error:

        mov     XAX, -1
        jmp     J_Done

J_Seek:

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF

        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileSeek, Handle, XAX, XDI

J_Done:
        ret

  FIL4_SEEK  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_SEEK.
ENDIF

UniqueSegmentEnd

END
