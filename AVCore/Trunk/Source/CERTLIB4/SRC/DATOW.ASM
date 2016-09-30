include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DAT4_ASCII_TO_ENCODE                 12/30/92                        CAG
;
;       Convert the ASCII date to the ENCODED word date.
;
; void WINAPI DAT4_ASCII_TO_ENCODE(UINT FAR *Date,LPSTR Storage);
;
; Input:
;       1)  Date = Encoded date YYYYYYYMMMMDDDDD
;       2)  Storage = 9 bytes "00/00/00"
; Output:
;       1)  Storage filled
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DAT4_ASCII_TO_ENCODE PROC PASCAL USES SDS XSI,
        lpwDate:LPSTR,
        lpDate:LPSTR

        LOCAL   wDate:WORD

        LDS_SI  lpDate

        lodsw                           ; Month
        sub     ax,3030h                ; Convert to BCD
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        mov     cl,5
        shl     ax,cl
        mov     wDate,ax
        lodsb                           ; Skip over /

        lodsw                           ; Day
        sub     ax,3030h
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        or      wDate,ax
        lodsb                           ; Skip over /

        lodsw                           ; Year
        sub     ax,3030h
        mov     bx,ax
        mov     cl,0Ah
        mul     cl
        add     al,bh
        sub     al,80
        mov     cl,9
        shl     ax,cl
        or      ax,wDate

        LDS_SI  lpwDate
        mov     [XSI],ax

        ret

DAT4_ASCII_TO_ENCODE ENDP
;-----------------------------------------------------------------------------
END
