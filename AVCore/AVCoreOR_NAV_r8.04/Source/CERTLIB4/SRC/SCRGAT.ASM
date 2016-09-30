include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_GETATTR                         1/26/93                         CAG
;
;       Get the current attribute at cursor.
;
; WORD WINAPI SCR4_GETPAGE(VOID);
;
; Output:
;       1)  Attribute.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_GETATTR    PROC USES XBX

        INVOKE  SCR4_GETPAGE
        mov     bh,al
        mov     ah,08h
        int     10h
        xchg    al,ah
        xor     ah,ah
        ret

SCR4_GETATTR    ENDP
;-----------------------------------------------------------------------------
END
