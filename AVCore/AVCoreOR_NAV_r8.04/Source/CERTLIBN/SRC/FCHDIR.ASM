include CTSN.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; $HEADER: $
;-----------------------------------------------------------------------------
; $LOG: $
;-----------------------------------------------------------------------------
;
; FIL4_CHANGE_DIR                      1/6/93                          CAG
;
;       Change to this directory.
;
; BOOL WINAPI FIL4_CHANGE_DIR(LPSTR Buffer);
;
; Input:
;       1)  Directory to change to
; Output:
;       1)  ERROR (0)
;       2)  PASSED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_CHANGE_DIR PROC USES XDX SDS,
        lpBuffer:LPSTR

        mov     ah,3Bh
        LDS_DX  lpBuffer
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Error
        mov     ax,1
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

FIL4_CHANGE_DIR ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END

