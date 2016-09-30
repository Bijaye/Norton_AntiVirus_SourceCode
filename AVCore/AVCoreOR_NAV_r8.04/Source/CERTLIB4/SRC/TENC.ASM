include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; TIM4_ENCODE                          12/30/92                        CAG
;
;       Get TODAY's time in ENCODED form.
;
; UINT WINAPI TIM4_ENCODE(void);
;
; Output:
;       1)  Time = Encoded time HHHHHMMMMMMSSSSS
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
TIM4_ENCODE PROC

        mov     ah,2Ch
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

; DH = Seconds (0-59)
        xor     ax,ax
        mov     al,dh
        shr     al,1                    ; Divide seconds by two

; CL = Minute (0-59)
        xor     dx,dx
        mov     dl,cl
        mov     cl,5
        shl     dx,cl
        or      ax,dx

; CH = Hour (0-23)
        xor     dx,dx
        mov     dl,ch
        mov     cl,11
        shl     dx,cl
        or      ax,dx

        ret

TIM4_ENCODE ENDP
;-----------------------------------------------------------------------------
END

