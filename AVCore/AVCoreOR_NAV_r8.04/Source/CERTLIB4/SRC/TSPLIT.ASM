include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; TIM4_SPLIT_ENCODE                    1/2/93                          CAG
;
;       Split the ENCODED bits to each compondent.
;
; VOID WINAPI TIM4_SPLIT_ENCODE(UINT Time, UINT FAR *H, 
;                               UINT FAR *M, UINT FAR *S);
;
; Input:
;       1)  Time = Encoded time HHHHHMMMMMMSSSSS
;       2)  Pointer to store hour (0-23)
;       3)  Pointer to store minutes (0-59)
;       4)  Pointer to store seconds (0-59)
; Ouput:
;       1)  Pointers filled.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
TIM4_SPLIT_ENCODE PROC USES SES XDI,
        wTime:WORD,
        Hour:LPSTR,
        Minute:LPSTR,
        Second:LPSTR

        mov     ax,wTime
        mov     cl,11
        shr     ax,cl
        and     ax,001Fh                ; Strip to HOUR
        LES_DI  Hour
        stosw                           ; Store HOUR

        mov     ax,wTime
        mov     cl,5
        shr     ax,cl
        and     ax,3Fh                  ; Strip MINUTE
        LES_DI  Minute
        stosw                           ; Store MINUTE

        mov     ax,wTime
        and     ax,001Fh                ; Strip to SECOND
        shl     ax,1                    ; Multiply by two
        LES_DI  Second
        stosw                           ; Store SECOND

        ret

TIM4_SPLIT_ENCODE ENDP
;-----------------------------------------------------------------------------
END

