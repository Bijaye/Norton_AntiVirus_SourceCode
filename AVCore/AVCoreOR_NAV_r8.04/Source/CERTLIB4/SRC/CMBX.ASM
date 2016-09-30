include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_CREATE                          12/9/92                         CAG
;
;      Delete a message from the CMB.
;
; UINT WINAPI CMB4_CREATE(LPSTR Filename,WORD IndexSize);
;
; Input:
;       1)  Name of file to open and create
;       2)  Size to create index
; Output:
;       1)  FAILED (0)
;       2)  Handle (?)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_CREATE PROC USES XBX XCX SDS XSI,
        Filename:LPSTR,
        IndexSize:WORD

        LOCAL   SmallBuffer:MESSAGE_HEADER

        mov     word ptr SmallBuffer.Pointer,0
        mov     word ptr SmallBuffer.Pointer+2,0
        mov     word ptr SmallBuffer.Message_Size,0

        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE OR fCREATE
        cmp     ax, -1
        jz      J_File_Error
        mov     bx,ax

        INVOKE  FIL4_WRITE,bx,ADDR SmallBuffer,2     ; Write number of messages
        or      ax,ax
        jz      J_Error

MoreIndex:
        INVOKE  FIL4_WRITE,bx,ADDR SmallBuffer,sMESSAGE_HEADER
        or      ax,ax
        jz      J_Error
        dec     IndexSize
        cmp     IndexSize,0
        jne     MoreIndex
        mov     ax,bx
        jmp     J_Done

J_File_Error:
        xor     ax, ax
        jmp     J_Done              

J_Error:
J_Done:
        ret

CMB4_CREATE      ENDP
;-----------------------------------------------------------------------------
END


