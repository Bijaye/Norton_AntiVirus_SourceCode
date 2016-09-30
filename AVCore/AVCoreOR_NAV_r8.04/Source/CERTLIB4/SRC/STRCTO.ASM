include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_COPY_TO_CHAR                  1/8/93                          CAG
;
;       Copy a string upto a character or a null, not including.
;
; UINT WINAPI STR4_COPY_TO_CHAR(LPSTR FromBuffer,LPSTR ToString,BYTE Char);
;
; Input:
;       1)  Copy from here
;       2)  Copy to here
;       3)  Look for this or NULL
; Output:
;       1)  0 Not found in buffer
;       2)  ? Number of bytes copied
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_COPY_TO_CHAR PROC USES XCX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR,
        byChar:BYTE


        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        xor     XCX,XCX

MoreToDo:
        lodsb
        cmp     al,byChar
        je      Done
        or      al,al
        jz      DoneError
        stosb
        inc     XCX
        jmp     MoreToDo
DoneError:
        xor     XCX,XCX
Done:
        mov     XAX,XCX

        ret

STR4_COPY_TO_CHAR ENDP
;-----------------------------------------------------------------------------
END
