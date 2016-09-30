include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_WORD_TO_ASCIIHEX                 2/11/93                        bAd.CAG
;
;       Convert a WORD to an ASCIIZ string.
;
; VOID WINAPI STR4_WORD_TO_ASCIIHEX(WORD,LPSTR);
;
; Input:
;       1)  Word to convert (UNSIGNED)
;       2)  Where to store
; Output:
;       1)  LPSTR Filled
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_WORD_TO_ASCIIHEX PROC USES SES XAX XBX XCX XDX XSI XDI,
        MyWord:WORD,
        Storage:LPSTR

        cld
        xor     cx,cx

        mov     bx,10h
        mov     ax,MyWord

J_0:
        xor     dx, dx
        div     bx
        xchg    ax, dx
        cmp     al, 0Ah
        jae     IsAlpha
        add     al, '0'              ; convert to decimal digit
        jmp     DoIt

IsAlpha:
        add     al, 37h                 ; convert to hex
DoIt:
        push    ax
        inc     cx
        mov     ax, dx
        or      ax, ax               ; if zero then done
        jnz     J_0

        LES_DI  Storage

J_2:
        pop     ax
        stosb   
        loop     j_2
        xor     al,al
        stosb
J_1:

        ret

STR4_WORD_TO_ASCIIHEX ENDP
;-----------------------------------------------------------------------------
END
