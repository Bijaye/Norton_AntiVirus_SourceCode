;
; $Header:   S:/CERTLIBN/VCS/MEMSS.ASv   1.2   08 Mar 1996 14:57:14   DALLEE  $
;
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/MEMSS.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:57:14   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;-----------------------------------------------------------------------------

include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SRCH_STRING                     1/8/93                          CAG
;
;       Search thru memory for a matching string, avoiding NULLs.
;
; UINT WINAPI MEM4_SRCH_STRING(LPVOID Buffer,LPVOID String,UINT Size);
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
        lpBuffer:LPBYTE,
        lpString:LPBYTE,
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
        xor     XAX, XAX
FoundIt:

        ret


MEM4_SRCH_STRING ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
