include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_LENGTH                         1/8/93                          CAG
;
;       Return the length of a string, not including NULL.
;
; UINT WINAPI STR4_LENGTH(LPSTR Buffer);
;
; Input:
;       1)  String
; Output:
;       1)  0 Not found in buffer
;       2)  ? Number of bytes copied
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_LENGTH PROC USES XCX SDS XSI,
        lpFrom:LPSTR

        cld
        LDS_SI  lpFrom
        xor     XCX,XCX

MoreToDo:
        lodsb
        or      al,al
        jz      Done
        inc     XCX
        jmp     MoreToDo
Done:
        mov     XAX,XCX

        ret

STR4_LENGTH ENDP
;-----------------------------------------------------------------------------
END
