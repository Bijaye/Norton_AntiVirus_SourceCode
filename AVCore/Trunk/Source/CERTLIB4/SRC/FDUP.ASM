include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_DUP_HANDLE                       12/29/92                        CAG
;
;       Duplicates a file handle, allowing it to be closed without
;       effecting other handle.
;
; UINT WINAPI FIL4_DUP_HANDLE(UINT Handle);
;
; Input:
;       1)  Handle to DUP
; Output:
;       1)  Handle
;       2)  FALSE (0) Failed
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_DUP_HANDLE PROC,
        Handle:UINT

        mov     ah,45h
        mov     XBX,Handle
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_Done
        mov     ax,FALSE
J_Done:
        ret

FIL4_DUP_HANDLE ENDP
;-----------------------------------------------------------------------------
END

