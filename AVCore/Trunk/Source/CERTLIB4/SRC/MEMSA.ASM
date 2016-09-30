include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SRCH_ALPHA                     1/8/93                          CAG
;
;       Search thru memory for any alpha char, avoiding NULLs.
;
; UINT WINAPI MEM4_SRCH_ALPHA(LPSTR Buffer,UINT Size);
;
; Input:
;       1)  Buffer to look in
;       2)  Size of buffer
; Output:
;       1)  0 Not found in buffer
;       2)  ? Offset of where found
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_SRCH_ALPHA PROC USES SDS XSI,
        lpBuffer:LPSTR,
        uSize:UINT

        cld
        LDS_SI  lpBuffer

        mov     XCX,uSize
DoMore:
        lodsb
        cmp     al,'A'
        jb      More
        cmp     al,'z'
        ja      More
        cmp     al,'a'
        jae     Found
        cmp     al,'Z'
        jbe     Found
More:
        loop    DoMore

        xor     ax,ax
        jmp     NotFound
Found:
        dec     XSI
        mov     XAX,XSI
NotFound:
        ret
MEM4_SRCH_ALPHA ENDP
;-----------------------------------------------------------------------------
END
