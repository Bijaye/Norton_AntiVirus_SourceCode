include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_LENGTH_TO_CHAR                 1/8/93                          CAG
;
;       Return the length of a string up to char or NULL, not including.
;
; UINT WINAPI STR4_LENGTH_TO_CHAR(LPSTR Buffer,BYTE Char);
;
; Input:
;       1)  String
;       2)  Look for this or NULL
; Output:
;       1)  0 Not found in buffer
;       2)  ? Number of bytes copied
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_LENGTH_TO_CHAR PROC USES XCX SDS XSI,
        lpFrom:LPSTR,
        byChar:BYTE


        cld
        LDS_SI  lpFrom
        xor     XCX,XCX

MoreToDo:
        lodsb
        cmp     al,byChar
        je      Done
        or      al,al
        jz      Done
        inc     XCX
        jmp     MoreToDo
Done:
        mov     XAX,XCX

        ret

STR4_LENGTH_TO_CHAR ENDP
;-----------------------------------------------------------------------------
END
