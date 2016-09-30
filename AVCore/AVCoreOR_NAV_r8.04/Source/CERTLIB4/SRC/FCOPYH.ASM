include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_COPY_HANDLE                     12/23/92                        CAG
;
;       Copy a file using handles.
;
; UINT WINAPI FIL4_COPY_HANDLE(UINT From,UINT To,LPSTR Buffer,UINT Size,DWORD);
;
; Input:
;       1)  Handle from
;       2)  Handle to
;       3)  Buffer to use
;       4)  Size of buffer
;       5)  Size of bytes to copy (-1) entire file.
; Output:
;       1)  TRUE (1)
;       2)  FALSE (0) Failed
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_COPY_HANDLE PROC,
        hFrom:UINT,
        hTo:UINT,
        Buffer:LPSTR,
        sBuffer:UINT,
        uSize:DWORD

J_More:
        INVOKE  FIL4_READ,hFrom,Buffer,sBuffer
        or      ax,ax
        jz      J_Done
        cmp     ax,-1
        je      IsError

        cmp     word ptr uSize+2,0
        jnz     JustDoIt
        cmp     ax,word ptr uSize
        jbe     JustDoIt
        mov     ax,word ptr uSize
        mov     word ptr uSize,0
        jmp     CopyMore
JustDoIt:
        sub     word ptr uSize,ax
        sbb     word ptr uSize+2,0
CopyMore:
        INVOKE  FIL4_WRITE,hTo,Buffer,ax
        or      ax,ax
        jz      J_Done
        cmp     ax,-1
        je      IsError

        cmp     word ptr uSize+2,0
        jnz     J_More
        cmp     word ptr uSize,0
        jnz     J_More

        jmp     J_Done
IsError:
        mov     ax,FALSE
        jmp     ReallyDone
J_Done:
        mov     ax,TRUE
ReallyDone:

        ret

FIL4_COPY_HANDLE ENDP
;-----------------------------------------------------------------------------
END
