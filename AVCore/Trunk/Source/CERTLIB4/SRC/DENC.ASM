include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; DAT4_ENCODE                          12/30/92                        CAG
;
;       Get TODAY's date in ENCODED form.
;
; UINT WINAPI DAT4_ENCODE(void);
;
; Output:
;       1)  Date = Encoded date YYYYYYYMMMMDDDDD
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
DAT4_ENCODE PROC

        mov     ah,2Ah                  ; Get Date
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

; DL = Day (1-31)                       ; Place bits of DAY
        mov     ax,dx
        and     ax,1Fh
        mov     bx,cx

; DH = Month (1-12)
        xchg    dh,dl
        and     dx,0Fh
        mov     cl,5
        shl     dx,cl
        or      ax,dx

; CX = Year 1980 - 2099
        sub     bx,1980                 ; Place bits of YEAR
        mov     cl,9
        shl     bx,cl
        or      ax,bx

        ret

DAT4_ENCODE ENDP
;-----------------------------------------------------------------------------
END

