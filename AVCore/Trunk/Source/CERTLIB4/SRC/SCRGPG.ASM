include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_GETPAGE                         1/26/93                         CAG
;
;       Get the current video page.
;
; WORD WINAPI SCR4_GETPAGE(VOID);
;
; Output:
;       1)  Active video page
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_GETPAGE    PROC USES XBX

        mov     ah,0Fh
        int     10h
        xor     ax,ax
        mov     al,bh
        ret

SCR4_GETPAGE    ENDP
;-----------------------------------------------------------------------------
END
