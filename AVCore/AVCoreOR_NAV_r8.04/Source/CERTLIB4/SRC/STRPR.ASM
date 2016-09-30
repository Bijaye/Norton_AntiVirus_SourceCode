include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_PRINT                           1/9/93                          CAG
;
;       Print a ASCIIZ string.
;
; VOID WINAPI STR4_PRINT(LPSTR String);
;
; Input:
;       1)  String to print
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
STR4_PRINT PROC USES SDS XSI,
        lpString:LPSTR

        cld
        LDS_SI  lpString

More:
        lodsb
        or      al,al
        jz      Done

        mov     dl,al
        mov     ah,02h
        int     21h
        jmp     More
Done:
        ret

STR4_PRINT ENDP
;-----------------------------------------------------------------------------
END
