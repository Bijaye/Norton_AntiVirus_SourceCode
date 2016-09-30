include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; TIM4_ASCII_TO_ENCODE                 12/30/92                        CAG
;
;       Convert the ASCII string to the ENCODED word time.
;
; void WINAPI TIM4_ASCII_TO_ENCODE(UINT FAR *Time,LPSTR Storage);
;
; Input:
;       1)  Time = Encoded time HHHHHMMMMMMSSSSS
;       2)  Storage = 9 bytes "00:00:00"
; Output:
;       1)  Storage changed
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
TIM4_ASCII_TO_ENCODE PROC USES SDS XSI,
        lpwTime:LPSTR,
        lpTime:LPSTR

        LOCAL   wTime:WORD

        LDS_SI  lpTime

        lodsw                           ; Get the HOUR
        sub     ax,3030h
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        mov     cl,11
        shl     ax,cl
        mov     wTime,ax
        lodsb                           ; Skip over :

        lodsw                           ; Get the Minutes
        sub     ax,3030h
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        mov     cl,5
        shl     ax,cl
        or      wTime,ax
        lodsb                           ; Skip over :

        lodsw                           ; Get the Seconds
        sub     ax,3030h
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        shr     al,1                    ; Divide by two
        or      ax,wTime

        LDS_SI  lpwTime
        mov     [XSI],ax

        ret

TIM4_ASCII_TO_ENCODE ENDP
;-----------------------------------------------------------------------------
END
