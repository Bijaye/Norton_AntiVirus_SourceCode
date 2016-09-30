include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_XMS_UNLOCK                       1/5/93                          CAG
;
;       Unlock XMS memory.
;
; WORD WINAPI MEM4_XMS_UNLOCK(WORD Handle);
;
; Input:
;       1)  Handle of XMS (Must be unlocked)
; Output:
;       1)  ERROR (0)
;       2)  PASSED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_XMS_UNLOCK PROC USES XBX,
        lHandle:WORD

        mov     dx,lHandle
        mov     ah,MEM_UNLOCK_XMS
        INVOKE  XMS_CALL
        ret

MEM4_XMS_UNLOCK ENDP
;-----------------------------------------------------------------------------
END

