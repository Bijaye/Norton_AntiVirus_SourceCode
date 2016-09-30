include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_EMS_FREE                        1/5/93                          CAG
;
;       Free memory in the EMS area.
;
; WORD WINAPI MEM4_EMS_FREE(WORD Handle);
;
; Input:
;       1)  Handle to free, must be unlocked
; Output:
;       1)  ERROR (1)
;       2)  PASSED(0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_EMS_FREE PROC USES XBX,
        lHandle:WORD

        mov     ah,45h
        mov     dx,lHandle
        int     67h
        or      ah,ah
        jnz     J_Error
        xor     ax,ax
J_Error:
J_Done:
        ret

MEM4_EMS_FREE ENDP
;-----------------------------------------------------------------------------
END

