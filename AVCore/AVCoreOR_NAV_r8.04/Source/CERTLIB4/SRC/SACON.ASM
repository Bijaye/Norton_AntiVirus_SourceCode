include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_ASCII_CONV                   2/21/93                          bAd
;
;       Converts a LPSTR to both a DWORD and LPSTR in a different base.
;
; DWORD WINAPI STR4_ASCII_CONV(LPSTR,WORD,LPSTR,WORD);
;
; Input:
;       1)  Source string to convert from.
;       2)  Source number base (0Ah or 10h).
;       3)  Destination string.
;       4)  Destination number base (0Ah or 10h).
; Output:
;       1)  Converted DWORD returned.
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;
;-----------------------------------------------------------------------------
STR4_ASCII_CONV PROC USES SES XBX XCX XSI,
        Storage:LPSTR,
        FromBase:WORD,
        wTo:LPSTR,
        ToBase:WORD

        LOCAL   dwWord:DWORD            ; destination dword

        mov     word ptr dwWord+2,00h
        mov     word ptr dwWord,00h
        mov     es, word ptr Storage+2  ; es:di - source dword
        mov     di, word ptr Storage

        xor     bx,bx                   ; index for strings > 4 chars

        mov     cl, es:[di]             ; save first character

        INVOKE  STR4_LENGTH,vSES XDI    ; string fit in a word?
        cmp     ax,4
        jle     J_1                     ; yep
        mov     bx,ax                   ; find end of high word
        sub     bx,04h                  ;

        add     di,bx
        INVOKE  STR4_ASCIIHEX_TO_WORD,vSES XDI
        mov     word ptr dwWord,ax
        sub     di,bx                   ; return to beginning of string
        mov     cl,es:[di+bx]           ; save first character, lo word
        mov     byte ptr es:[di+bx],00h ; make hi word ASCIIZ

J_1:
        INVOKE  STR4_ASCIIHEX_TO_WORD,vSES XDI
        mov     word ptr dwWord+2,ax
        mov     es:[di+bx],cl              ; put character back

        INVOKE  STR4_DWORD_CONV,dwWord,FromBase,wTo,ToBase

        ret

STR4_ASCII_CONV ENDP
;-----------------------------------------------------------------------------
END
