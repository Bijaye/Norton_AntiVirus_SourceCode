;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\nmod.asv   1.3   16 Dec 1994 21:46:30   BRAD  $
;;
;; Description: Defines NUM4_MODULUS function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\nmod.asv  $
;  
;     Rev 1.3   16 Dec 1994 21:46:30   BRAD
;  Don't allow divide by 0
;  
;     Rev 1.2   16 Dec 1994 18:30:46   BRAD
;  Converted to 32-bit
;  
;     Rev 1.1   17 Aug 1994 13:26:02   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   31 May 1994 13:36:26   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; NUM4_MODULUS
;
; Return the MODULUS of a number.
;
; DX:AX WINAPI NUM4_MODULUS ( DWORD Number, DWORD Divider );
;
; Input:
;       1)  Number - the large number to get modulus of
;       2)  Divider - small number to use to get modulus
; Output:
;       1)  UINT = Number % Divider
;----------------------------------------------------------------------------
NUM4_MODULUS  PROC  CODEDISTANCE  USES XBX,
        Number:DWORD,
        Divider:DWORD

IF OS_16
        mov     ax,WORD PTR Divider     ; Check divide by 0
        or      ax,WORD PTR Divider+2
        jnz     J_Start

        mov     dx,ax                   ; Zero DX:AX
        jmp     J_5

J_Start:
        mov     ax,WORD PTR Divider+2
        or      ax,ax
        jnz     J_1
        mov     cx,WORD PTR Divider
        mov     ax,WORD PTR Number+2
        xor     dx,dx
        div     cx
        mov     ax,WORD PTR Number
        div     cx
        mov     ax,dx
        xor     dx,dx
        jmp     J_5
J_1:
        mov     cx,ax
        mov     bx,WORD PTR Divider
        mov     dx,WORD PTR Number+2
        mov     ax,WORD PTR Number
J_2:
        shr     cx,1
        rcr     bx,1
        shr     dx,1
        rcr     ax,1
        or      cx,cx
        jnz     J_2
        div     bx
        mov     cx,ax
        mul     WORD PTR Divider+2
        xchg    ax,cx
        mul     WORD PTR Divider
        add     dx,cx
        jc      J_3
        cmp     dx,WORD PTR Number+2
        ja      J_3
        jc      J_4
        cmp     ax,WORD PTR Number
        jbe     J_4
J_3:
        sub     ax,WORD PTR Divider
        sbb     dx,WORD PTR Divider+2

J_4:
        sub     ax,WORD PTR Number
        sbb     dx,WORD PTR Number+2
        neg     dx
        neg     ax
        sbb     dx,0

ELSE
        mov     eax, Divider            ; Trying to divide by 0?
        or      eax, eax                ; Yes, just return 0
        jz      J_5
        
        xor     edx, edx                ; Clear high DWORD
        mov     eax, Number
        div     Divider
        mov     eax, edx                ; Get the remainder
ENDIF

J_5:
        ret

NUM4_MODULUS  ENDP

UniqueSegmentEnd
END
