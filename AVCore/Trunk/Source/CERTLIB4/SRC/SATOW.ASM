include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_ASCII_TO_WORD                   1/2/93                          CAG
;
;       Convert a HEX ASCIIZ string to a WORD.
;
; WORD WINAPI STR4_ASCII_TO_WORD(LPSTR);
;
; Input:
;       1)  What to convert.
; Output:
;       1)  Word returned.
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       'AB',0 is converted to 00ABh
;
;-----------------------------------------------------------------------------
STR4_ASCII_TO_WORD PROC USES SDS XSI,
        Storage:LPSTR

        LOCAL   wWord:WORD

        LDS_SI  Storage

        cld
        xor     cx,cx

SeekEnd:
        lodsb
        inc     cx
        or      al,al
        jnz     SeekEnd
        dec     si
        dec     si
        dec     cx
        mov     bx,1
        std
        mov     wWord,0
        jcxz    DoneIt


TillDone:
        xor     ax,ax
        lodsb                           ; Load "ones"
        cmp     al,'A'
        jae     IsAlpha
        sub     al,30h
        jmp     DoIt
IsAlpha:
        sub     al,'A'
        add     al,0Ah
DoIt:
        mul     bx
        add     wWord,ax
        mov     ax,10h
        mul     bx
        xchg    ax,bx
        loop    TillDone

DoneIt:
        mov     ax,wWord
        ret

STR4_ASCII_TO_WORD ENDP
;-----------------------------------------------------------------------------
END
