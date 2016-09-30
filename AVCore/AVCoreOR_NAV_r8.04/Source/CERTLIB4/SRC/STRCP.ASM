include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_COPY                           1/8/93                          CAG
;
;       Copy a string, including NULL.
;
; UINT WINAPI STR4_COPY(LPSTR FromBuffer,LPSTR ToString);
;
; Input:
;       1)  Copy from here
;       2)  Copy to here
; Output:
;       1)  0 Not found in buffer
;       2)  ? Number of bytes copied
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_COPY PROC USES XCX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        xor     XCX,XCX

MoreToDo:
        lodsb
        stosb
        or      al,al
        jz      Done
        inc     XCX
        jmp     MoreToDo
Done:
        mov     XAX,XCX

        ret

STR4_COPY ENDP
;-----------------------------------------------------------------------------
END
