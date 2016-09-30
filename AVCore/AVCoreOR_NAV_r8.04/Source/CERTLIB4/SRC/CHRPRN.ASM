include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CHR4_PRINT                           1/9/93                          CAG
;
;       Print a ASCII character.
;
; VOID WINAPI CHR4_PRINT(BYTE Char);
;
; Input:
;       1)  Character to print
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
CHR4_PRINT PROC USES SDS XSI,
        byChar:BYTE

        mov     dl,byChar
        mov     ah,02h
        int     21h
        ret

CHR4_PRINT ENDP
;-----------------------------------------------------------------------------
END
