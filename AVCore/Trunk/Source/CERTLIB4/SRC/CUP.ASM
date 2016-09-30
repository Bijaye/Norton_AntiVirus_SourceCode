include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CHR4_UPPER                           1/2/93                          CAG
;
;       Convert a CHARACTER to uppercase.
;
; BYTE WINAPI CHR4_UPPER (BYTE);
;
; Input:
;       1)  Character to be converted.
; Output:
;       2)  Character converted.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CHR4_UPPER PROC USES SDS XSI,
        MyChar:BYTE

        INVOKE  CHR4_TO_ALT_CASE, MyChar, CHR_UPPER            

;        mov     al,MyChar
;        cmp    al,'z'
;        ja      no_upper1
;        cmp     al,'a'
;        jb      no_upper1
;        sub     al,20h
;no_upper:

        ret

CHR4_UPPER ENDP
;-----------------------------------------------------------------------------
END

