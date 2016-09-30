include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_XMS_FREE                         1/5/93                          CAG
;
;       Free memory in the XMS area.
;
; WORD WINAPI MEM4_XMS_FREE(WORD Handle);
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
MEM4_XMS_FREE PROC USES XBX,
        lHandle:WORD

        mov     dx,lHandle
        mov     ah,MEM_FREE_XMS
        INVOKE  XMS_CALL
        ret

MEM4_XMS_FREE ENDP
;-----------------------------------------------------------------------------
END
