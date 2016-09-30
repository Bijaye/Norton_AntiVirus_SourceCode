include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_SRCH_CHAR                       2/7/93                          CAG
;
;       Search for a matching character in a string.
;
; WORD WINAPI STR4_SRCH_CHAR(LPSTR String,BYTE Char);
;
; Input:
;       1)  String to look in
;       2)  Look for this character
; Output:
;       1)  Offset where found
;       2)  Character not found (FALSE 0)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_SRCH_CHAR PROC USES SDS XSI,
        lpString:LPSTR,
        byChar:BYTE

        LDS_SI  lpString
        mov     al,byChar

MoreString:
        cmp     [XSI],byte ptr 0        ; Not found
        je      NotFound
        cmp     [XSI],al
        je      Found
        inc     XSI
        jmp     MoreString
Found:
        mov     XAX,XSI
        jmp     J_Done
NotFound:
        mov     XAX,FALSE
J_Done:
        ret

STR4_SRCH_CHAR ENDP
;-----------------------------------------------------------------------------
END
