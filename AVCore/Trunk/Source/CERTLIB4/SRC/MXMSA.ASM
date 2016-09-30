include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_XMS_ALLOCATE                     1/5/93                          CAG
;
;       Allocate memory in the Extended Memory Area (XMS)
;
; WORD WINAPI MEM4_XMS_ALLOCATE(WORD Paragraphs)
;
; Input:
;       1)  Number of paragraphs wanted
; Output:
;       1)  ERROR (0)
;       2)  Handle to memory, must be locked to use
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_XMS_ALLOCATE PROC USES XBX,
        uParagraph:WORD

        mov     ah,MEM_ALLOCATE_XMS
        mov     dx,uParagraph
        shl     dx,1
        shl     dx,1
        shl     dx,1
        shl     dx,1                    ; Convert to bytes
        INVOKE  XMS_CALL
        or      ax,ax
        jz      J_Error
        mov     ax,dx
J_Error:

        ret

MEM4_XMS_ALLOCATE ENDP
;-----------------------------------------------------------------------------
END

