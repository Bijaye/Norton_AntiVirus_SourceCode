include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_GETCURR                         1/26/93                         CAG
;
;       Get the current location of cursor.
;
; WORD WINAPI SCR4_GETCURR(VOID);
;
; Output:
;       1)  ROW (AH)
;           COL (AL)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_GETCURR    PROC USES XBX XCX XDX

        INVOKE  SCR4_GETPAGE
        mov     bh,al
        mov     ah,03h
        int     10h
        mov     ah,dh
        mov     al,dl
        ret

SCR4_GETCURR    ENDP
;-----------------------------------------------------------------------------
END
