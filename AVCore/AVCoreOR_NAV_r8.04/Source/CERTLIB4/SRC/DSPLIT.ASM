include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DAT4_SPLIT_ENCODE                    1/2/93                          CAG
;
;       Split the ENCODED bits to each compondent.
;
; VOID WINAPI DAT4_SPLIT_ENCODE(UINT Date, UINT FAR *M, 
;                               UINT FAR *D, UINT FAR *Y);
;
; Input:
;       1)  Date = Encoded date YYYYYYYMMMMDDDDD
;       2)  Pointer to store month, (1-12)
;       3)  Pointer to store day, (1-31)
;       4)  Pointer to store year, (0-99)
; Ouput:
;       1)  Pointers filled.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DAT4_SPLIT_ENCODE PROC USES SES XDI,
        wDate:WORD,
        Month:LPSTR,
        Day:LPSTR,
        Year:LPSTR

        mov     ax,wDate

        mov     cl,5
        shr     ax,cl
        and     ax,000Fh                ; Strip to MONTH
        LES_DI  Month
        stosw

        mov     ax,wDate
        and     ax,001Fh                ; Strip to DAY
        LES_DI  Day
        stosw

        mov     ax,wDate
        mov     cl,9
        shr     ax,cl
        and     ax,7Fh                  ; Strip year
        add     ax,80                   ; Convert to proper year
        LES_DI  Year
        stosw

        ret

DAT4_SPLIT_ENCODE ENDP
;-----------------------------------------------------------------------------
END
