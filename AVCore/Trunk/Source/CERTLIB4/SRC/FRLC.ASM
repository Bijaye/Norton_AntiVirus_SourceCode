include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_READLINE_CACHE                  2/16/93                         CAG
;
;       Read a line from a cached buffer.
;
; UINT WINAPI FIL4_READLINE_CACHE(LPSTR lpCache,UINT Handle,LPSTR lpBuffer,
;                                               UINT uBuffer);
;
; Input:
;       1)  Cache to read from
;       2)  Handle of file to read from
;       3)  Buffer to store in
;       4)  Size of buffer
; Output:
;       1)  Bytes read
;       2)  0 End of file
;       3)  -1 Error
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_READLINE_CACHE PROC USES XCX XDX SDS XSI,
        lpCache:LPSTR,
        uHandle:UINT,
        lpBuffer:LPSTR,
        uBuffer:UINT

        LOCAL   uStatus:WORD

        INVOKE  FIL4_READ,uHandle,lpBuffer,uBuffer
        or      ax,ax
        jz      EndOfFile
        cmp     ax,-1
        je      Error
        mov     uStatus,ax
        mov     uBuffer,XAX

        INVOKE  STR4_LENGTH_TO_CHAR,lpBuffer,CR
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
EndOfFile:
Error:
        ret

FIL4_READLINE_CACHE ENDP
;-----------------------------------------------------------------------------
END
