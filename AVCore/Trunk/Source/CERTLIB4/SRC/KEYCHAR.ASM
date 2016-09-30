include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; KEY4_GET_CHAR                        2/28/93                         CAG
;
;       Get a key.
;
; WORD WINAPI KEY4_GET_CHAR(void);
;
; Output:
;       1)  Keystroke
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
KEY4_GET_CHAR PROC

        xor     ax,ax
        int     16h
        ret

KEY4_GET_CHAR ENDP
;-----------------------------------------------------------------------------
END

