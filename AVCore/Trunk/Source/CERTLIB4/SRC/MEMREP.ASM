include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_REPLACE                         1/9/93                          CAG
;
;       Search for characters in memory and replace with value.
;
; UINT WINAPI MEM4_REPLACE(LPSTR Buffer,BYTE LookFor, BYTE Replace,UINT Len);
;
; Input:
;       1)  Buffer to look in.
;       2)  Look for this character
;       3)  Replace with this character
;       4)  For this length of the buffer
; Output:
;       1)  Number of characters replaced
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_REPLACE PROC USES XCX XDX SES XDI,
        lpBuffer:LPSTR,
        byLook:BYTE,
        byReplace:BYTE,
        uLength:UINT

        cld
        xor     XDX,XDX
        LES_DI  lpBuffer
        mov     al,byLook
        mov     ah,byReplace
        mov     XCX,uLength

FindMore:
        repne   scasb
        jne     NotFound
        mov     XES[XDI-1],ah
        inc     XDX
        cmp     XCX,0
        jne     FindMore


NotFound:
        mov     XAX,XDX

        ret

MEM4_REPLACE ENDP
;-----------------------------------------------------------------------------
END
