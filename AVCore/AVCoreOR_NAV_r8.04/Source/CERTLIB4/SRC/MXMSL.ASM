include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_XMS_LOCK                         1/5/93                          CAG
;
;       Lock XMS for our use.
;
; DWORD WINAPI MEM4_XMS_LOCK(WORD Handle);
;
; Input:
;       1)  Handle of XMS to lock
; Output:
;       1)  ERROR (0)
;       2)  SEG:OFF of XMS to use
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_XMS_LOCK PROC USES XBX,
        lHandle:WORD

        mov     dx,lHandle
        mov     ah,MEM_LOCK_XMS
        INVOKE  XMS_CALL
        or      ax,ax
        jz      J_Error
        mov     ax,bx
J_Error:
        ret

MEM4_XMS_LOCK ENDP
;-----------------------------------------------------------------------------
END

