include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; TIM4_ENCODE_TO_ASCII                 12/30/92                        CAG
;
;       Convert the ENCODED word time to a ASCII string.
;
; void WINAPI TIM4_ENCODE_TO_ASCII(UINT Time,LPSTR Storage);
;
; Input:
;       1)  Time = Encoded time HHHHHMMMMMMSSSSS
;       2)  Storage = 9 bytes "00:00:00"
; Output:
;       1)  Storage changed.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
TIM4_ENCODE_TO_ASCII PROC USES SES XDI,
        wTime:WORD,
        lpTime:LPSTR

        LOCAL   bIsPM:BYTE

        mov     bIsPM, FALSE

        LES_DI  lpTime

        mov     ax,wTime
        mov     cl,11
        shr     ax,cl
        and     ax,001Fh                ; Strip to HOUR

        cmp     ax,000Ch
        jb      IsAM
        mov     bIsPM, TRUE
        cmp     ax,000Ch
        je      NoChange
        sub     ax,000Ch
NoChange:
IsAM:
        mov     cl,0Ah
        div     cl

        add     ax,3030h                ; Convert to ASCII
        cmp     al,'0'
        jne     StoreBoth
        xchg    ah,al
        stosb
        jmp     StoreOne
StoreBoth:
        stosw                           ; Store HOUR
StoreOne:
        mov     al,':'
        stosb                           ; Store HH:

        mov     ax,wTime
        mov     cl,5
        shr     ax,cl
        and     ax,3Fh                  ; Strip MINUTE
        mov     cl,0Ah
        div     cl
        add     ax,3030h
        stosw                           ; Store MINUTE
        mov     al,':'
        stosb                           ; Store HH:MM:


        mov     ax,wTime
        and     ax,001Fh                ; Strip to SECOND
        shl     ax,1                    ; Multiply by two
        mov     cl,0Ah
        div     cl
        add     ax,3030h                ; Convert to ASCII
        stosw                           ; Store SECOND

        mov     al, 20h
        stosb                           ; Store Space

        mov     al, 'p'
        cmp     bIsPM, TRUE
        je      SetPM
        mov     al, 'a'
SetPM:
        stosb
        mov     al, 'm'
        stosb


        xor     al,al
        stosb                           ; Store NULL

        ret

TIM4_ENCODE_TO_ASCII ENDP
;-----------------------------------------------------------------------------
END
