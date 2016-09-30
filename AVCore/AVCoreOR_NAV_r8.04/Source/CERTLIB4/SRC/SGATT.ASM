include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_GET_ATTR                 8/5/91                          CAG
;
;       Get attribute from screen.
;
; BYTE WINAPI SCR4_GET_ATTR(void)
;
; Output:
;       1)  Byte of attribute on screen
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_GET_ATTR PROC USES XBX

        mov     ah,08h
        int     10h
        mov     al,ah
        ret

SCR4_GET_ATTR ENDP
;-----------------------------------------------------------------------------
END
