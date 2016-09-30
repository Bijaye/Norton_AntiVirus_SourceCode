include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_BLANK                            8/5/91                          CAG
;
;       Blank screen, return cursor to upper left corner.
;       Using attributes that are already current.  Text mode only.
;       Page 0 only.
;
; void SCR4_BLANK(void);
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_BLANK PROC USES SDS SES XSI XDI

        mov     ah,08h                  ; Read char and attribute at Cursor
        xor     bh,bh                   ; Page 0
        int     10h

        mov     bh,ah                   ; Save Attribute in BH
        mov     ax,0600h                ; Scroll entire window UP
        xor     cx,cx
        mov     dx,0FFFFh
        int     10h

        mov     ah,02h                  ; Set cursor position (0,0)
        xor     bh,bh                   ; Page 0
        xor     dx,dx
        int     10h                     
        ret

SCR4_BLANK      ENDP
;-----------------------------------------------------------------------------
END
