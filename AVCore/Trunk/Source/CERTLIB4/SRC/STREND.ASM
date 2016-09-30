include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_SRCH_END                 2/5/93                                  CAG
;
;       Search to the end of a null terminated string.
;
; LPSTR WINAPI STR4_SRCH_END(LPSTR lpString);
;
; Input:
;       1)  String to search to end of.
; Output:
;       2)  Address of the null character
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_SRCH_END PROC USES SDS XSI,
        lpString:LPSTR

        cld
        LDS_SI  lpString
NotEnd:
        lodsb
        or      al,al
        jnz     NotEnd
        dec     XSI
        mov     dx,ds
        mov     XAX,XSI
        ret

STR4_SRCH_END ENDP
;-----------------------------------------------------------------------------
END
