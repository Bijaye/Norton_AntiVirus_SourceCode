include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_EMS_UNLOCK                      1/5/93                          CAG
;
;       Unlock the EMS handle.
;
; WORD WINAPI MEM4_EMS_UNLOCK(WORD Handle);
;
; Input:
;       1)  Handle to unlock
; Output:
;       1)  ERROR (0)
;       2)  PASSED(1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_EMS_UNLOCK PROC USES XBX,
        lHandle:WORD

        mov     dx,lHandle              ; Swap out the page frame
        mov     ax,4400h
        mov     bx,-1
        int     67h  
        or      ah,ah
        jnz     J_ERROR

        mov     ax,4800h                ; Restore state of EMS
        int     67h
        or      ah,ah
        jnz     J_ERROR
        mov     ax,1
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

MEM4_EMS_UNLOCK ENDP
;-----------------------------------------------------------------------------
END
