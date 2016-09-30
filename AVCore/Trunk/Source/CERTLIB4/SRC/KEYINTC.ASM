include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; KEY4_INTERCEPT                        4/19/93                         CAG
;
;       Determine if a keyboard allows for the INT 15h intercept
;       command.
;
; BOOL KEY4_INTERCEPT(void);
;
; Output:
;       1)  TRUE - Yes
;       2)  FALSE - No
;
;-----------------------------------------------------------------------------
KEY4_INTERCEPT PROC USES SES XBX

        mov     ax,0C000h
        int     15h
        sti
        jc      Error
        
        mov     al,es:[bx+4]            ; Feature information
        test    al,00010000b
        jz      NoIntercept
        mov     ax,TRUE
        jmp     Done

NoIntercept:
Error:
        xor     ax,ax
Done:
        ret

KEY4_INTERCEPT ENDP
;-----------------------------------------------------------------------------
END
