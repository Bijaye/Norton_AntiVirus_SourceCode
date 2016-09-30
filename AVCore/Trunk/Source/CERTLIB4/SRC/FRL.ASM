include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_READLINE                        2/16/93                         CAG
;
;       Read a line from a file, till it find CRLF, or EOF.
;
; UINT WINAPI FIL4_READLINE(UINT Handle,LPSTR lpBuffer,UINT uBuffer);
;
; Input:
;       1)  Handle of file to read from
;       2)  Buffer to store in
;       3)  Size of buffer
; Output:
;       1)  Bytes read
;       2)  -1 Error or End of file
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_READLINE PROC USES XCX XDX SDS XSI,
        uHandle:UINT,
        lpBuffer:LPSTR,
        uBuffer:UINT

        LOCAL   uStatus:WORD

        INVOKE  FIL4_READ,uHandle,lpBuffer,uBuffer
        or      ax,ax
        jz      EndOfFile
        cmp     ax,-1
        je      Error
        mov     uBuffer,XAX

        INVOKE  STR4_LENGTH_TO_CHAR,lpBuffer,CR
        mov     uStatus,ax
        LDS_SI  lpBuffer
        add     XSI,XAX
        mov     [XSI],word ptr 0
        inc     XAX
        inc     XAX

        sub     uBuffer,XAX
        xor     cx,cx
        mov     XDX,uBuffer

        INVOKE  FIL4_SEEK,uHandle,cx::dx,fSEEK_BAK

        mov     ax,uStatus
        jmp     J_Done
EndOfFile:
Error:
        mov     ax,-1
J_Done:

        ret

FIL4_READLINE ENDP
;-----------------------------------------------------------------------------
END
