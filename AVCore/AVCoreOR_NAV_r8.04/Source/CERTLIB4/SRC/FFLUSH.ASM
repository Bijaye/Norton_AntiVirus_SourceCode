include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_FLUSH                           12/29/92                        CAG
;
;       Flush a file handle to the drive.
;
; void WINAPI FIL4_FLUSH(UINT Handle);
;
; Input:
;       1)  Handle of file to flush
; Output:
;       1)  PASSED (0)
;       2)  ERROR (?)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_FLUSH PROC,
        hHandle:UINT

        mov     ah,68h
        mov     XBX,hHandle
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Error

        mov     ah,45h
        mov     XBX,hHandle
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Error2
        mov     bx,ax
        mov     ah,3Eh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
J_Error2:
        xor     ax,ax
J_Error:
        ret

FIL4_FLUSH ENDP
;-----------------------------------------------------------------------------
END
