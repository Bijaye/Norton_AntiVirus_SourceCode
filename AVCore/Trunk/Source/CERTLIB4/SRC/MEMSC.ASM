include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SRCH_CHAR                       1/8/93                          CAG
;
;       Search thru memory for a matching character, avoiding NULLs.
;
; UINT WINAPI MEM4_SRCH_CHAR(LPSTR Buffer,BYTE Char,UINT Size);
;
; Input:
;       1)  Buffer to look in
;       2)  Character to look for
;       3)  Size of buffer
; Output:
;       1)  0 Not found in buffer
;       2)  ? Offset of where found
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_SRCH_CHAR PROC USES XCX SES XDI,
        lpBuffer:LPSTR,
        byChar:BYTE,
        uSize:UINT

        cld
        LES_DI  lpBuffer
        mov     al,byChar
        mov     XCX,uSize

        repne   scasb
        jne     NotThere
        dec     XDI
        mov     XAX,XDI
        jmp     There
NotThere:
        xor     XAX,XAX
        xor     XDX,XDX
There:
        ret

MEM4_SRCH_CHAR ENDP
;-----------------------------------------------------------------------------
END
