include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_CLOSE                           12/23/92                        CAG
;
;       Close a file handle.
;
; void FIL4_CLOSE(UINT Handle)
;
; Input:
;       1)  Handle to close
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_CLOSE PROC USES XBX,
        Handle:WORD

        mov     XBX,Handle
        mov     ah,3Eh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

        mov     ax, 0                   ; prepare for success
        jnc     close_exit
        mov     ax, -1                  ; Report error same as FileClose()

close_exit:
        ret

FIL4_CLOSE ENDP
;-----------------------------------------------------------------------------
END
