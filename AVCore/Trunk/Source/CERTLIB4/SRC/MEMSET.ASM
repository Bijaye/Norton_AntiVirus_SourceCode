include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SET                            1/8/93                          CAG
;
;       Set the memory buffer to this character.
;
; UINT WINAPI MEM4_SET(LPSTR Buffer,BYTE Char,UINT Size);
;
; Input:
;       1)  Buffer to set
;       2)  Char to set to
;       3)  Number of bytes to set
; Output:
;       1)  0 Not found in buffer
;       2)  ? Offset of where found
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_SET PROC USES XCX SES XDI,
        lpBuffer:LPSTR,
        byChar:BYTE,
        uSize:UINT

        cld
        LES_DI  lpBuffer
        mov     XCX,uSize
        mov     al,byChar
        rep     stosb
        ret

MEM4_SET ENDP
;-----------------------------------------------------------------------------
END
