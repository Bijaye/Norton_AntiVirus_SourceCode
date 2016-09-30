include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DAT4_ENCODE_TO_ASCII                 12/30/92                        CAG
;
;       Convert the ENCODED word date to a ASCII string.
;
; void WINAPI DAT4_ENCODE_TO_ASCII(UINT Date,LPSTR Storage);
;
; Input:
;       1)  Date = Encoded date YYYYYYYMMMMDDDDD
;       2)  Storage = 9 bytes "00/00/00"
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DAT4_ENCODE_TO_ASCII PROC USES SES XDI,
        wDate:WORD,
        lpDate:LPSTR

        LES_DI  lpDate

        mov     ax,wDate
        mov     cl,5
        shr     ax,cl
        and     ax,000Fh                ; Strip to MONTH
        mov     cl,0Ah
        div     cl
        add     ax,3030h                ; Convert to ASCII
        stosw                           ; Store MONTH
        mov     al,'-'
        stosb                           ; Store MM-

        mov     ax,wDate
        and     ax,001Fh                ; Strip to DAY
        mov     cl,0Ah
        div     cl
        add     ax,3030h                ; Convert to ASCII
        stosw                           ; Store DAY
        mov     al,'-'
        stosb                           ; Store MM-DD-

        mov     ax,wDate
        mov     cl,9
        shr     ax,cl
        and     ax,7Fh                  ; Strip year
        add     ax,80                   ; Convert to proper year
        mov     cl,0Ah
        div     cl
        add     ax,3030h
        stosw                           ; Store YEAR
        xor     al,al
        stosb                           ; Store NULL

        ret

DAT4_ENCODE_TO_ASCII ENDP
;-----------------------------------------------------------------------------
END
