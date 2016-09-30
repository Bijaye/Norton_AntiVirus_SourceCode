include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_EMS_GETPAGE                     1/5/93                          CAG
;
;       Return the current page that EMS is using.
;
; WORD WINAPI MEM4_EMS_GETPAGE(void);
;
; Output:
;       1)  ERROR (0)
;       2)  Segment of memory page
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_EMS_GETPAGE PROC USES XBX,

        mov     ah,41h
        int     67h
        or      ah,ah
        jnz     J_Error
        mov     ax,bx
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

MEM4_EMS_GETPAGE ENDP
;-----------------------------------------------------------------------------
END
