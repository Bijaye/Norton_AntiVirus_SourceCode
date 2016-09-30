include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_RESET                           12/29/92                        CAG
;
;       Reset the hard drive, will flush buffers.
;
; void WINAPI DIS4_RESET(void);
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
DIS4_RESET PROC

        mov     ah,0Dh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        ret

DIS4_RESET ENDP
;-----------------------------------------------------------------------------
END
