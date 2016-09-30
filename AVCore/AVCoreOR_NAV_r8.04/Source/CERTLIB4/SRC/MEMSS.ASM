include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SRCH_STRING                     1/8/93                          CAG
;
;       Search thru memory for a matching string, avoiding NULLs.
;
; UINT WINAPI MEM4_SRCH_STRING(LPSTR Buffer,LPSTR String,UINT Size);
;
; Input:
;       1)  Buffer to look in
;       2)  String to look for
;       3)  Size of buffer
; Output:
;       1)  0 Not found in buffer
;       2)  ? Offset of where found
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_SRCH_STRING PROC USES XCX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        lpString:LPSTR,
        uSize:UINT

        LOCAL   uLength:UINT

        cld
        mov     uLength,0

        LDS_SI  lpString
MoreLength:
        lodsb
        or      al,al
        jz      DoneLength
        inc     uLength
        jmp     MoreLength
DoneLength:
        cmp     uLength,0
        jz      NotThere

        LES_DI  lpBuffer
        mov     XCX,uSize

LookMore:
        LDS_SI  lpString

        mov     al,[XSI]
        repne   scasb
        jne     NotThere

        mov     XAX,XDI
        push    XCX
        dec     XDI
        mov     XCX,uLength
        repe    cmpsb
        pop     XCX
        jne     LookMore
        dec     XAX
        jmp     FoundIt

NotThere:
        xor     dx,dx
        xor     ax,ax
FoundIt:

        ret


MEM4_SRCH_STRING ENDP
;-----------------------------------------------------------------------------
END
