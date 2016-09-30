include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CHR4_LOWER                           1/2/93                          CAG
;
;       Convert a CHARACTER to uppercase.
;
; BYTE WINAPI CHR4_LOWER (BYTE);
;
; Input:
;       1)  Character to be converted.
; Output:
;       2)  Character converted.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CHR4_LOWER PROC USES SDS XSI,
        MyChar:BYTE


        INVOKE  CHR4_TO_ALT_CASE, MyChar, CHR_LOWER

;        mov     al,MyChar
;        cmp     al,'z'
;        ja      no_LOWER1
;        cmp     al,'a'
;        jb      no_LOWER1
;        add     al,20h
;no_LOWER1:
        ret

CHR4_LOWER ENDP
;-----------------------------------------------------------------------------
END

