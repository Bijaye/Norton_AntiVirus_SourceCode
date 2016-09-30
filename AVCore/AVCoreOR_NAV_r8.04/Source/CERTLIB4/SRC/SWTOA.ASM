include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_WORD_TO_ASCII                   1/2/93                          CAG
;
;       Convert a WORD to an ASCIIZ string.
;
; VOID WINAPI STR4_WORD_TO_ASCII(WORD,LPSTR);
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
STR4_WORD_TO_ASCII PROC USES SDS SES XSI XDI,
        MyWord:WORD,
        Storage:LPSTR

        LES_DI  Storage
        cld

        mov     bx,10
        mov     ax,MyWord

J_0:
        xor     dx, dx
        div     bx
        xchg    ax, dx
        add     al, '0'              ; convert to decimal digit

        stosb
        mov     ax, dx
        or      ax, ax               ; if zero then done
        jnz     J_0

        xor     al,al
        stosb   

        LDS_SI  Storage
        dec     XDI
        dec     XDI
J_2:
        mov     al,[XSI]
        mov     ah,XES[XDI]
        mov     XES[XDI],al
        mov     [XSI],ah
        cmp     XSI,XDI
        je      J_1
        inc     XSI
        dec     XDI

J_1:

        ret

STR4_WORD_TO_ASCII ENDP
;-----------------------------------------------------------------------------
END
