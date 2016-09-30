include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_EMS_LOCK                        1/5/93                          CAG
;
;       Lock the EMS memory for use.
;
; WORD WINAPI MEM4_EMS_LOCK(WORD Handle);
;
; Input:
;       1)  Handle to lock
; Output:
;       1)  ERROR (0)
;       2)  Segment of memory to use.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_EMS_LOCK PROC USES XBX,
        lHandle:WORD

        mov     dx,lHandle              ; Save the current state of EMS
        mov     ax,4700h
        int     67h
        cmp     ah,8Dh                  ; Already done
        je      GetPage
        or      ah,ah
        jnz     J_Error

        mov     ax,4400h                ; Swap EMS into the page frame
        xor     bx,bx
        int     67h
        or      ah,ah
        jnz     J_Error

GetPage:

        INVOKE  MEM4_EMS_GETPAGE
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

MEM4_EMS_LOCK ENDP
;-----------------------------------------------------------------------------
END

