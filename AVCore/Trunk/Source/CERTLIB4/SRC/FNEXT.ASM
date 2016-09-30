include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_FIND_NEXT                       12/28/92                        CAG
;
;       Returns information in the DTA of a file find next.
;
; WORD WINAPI FIL4_FIND_NEXT(void);
;
; Input:
;       1)  DTA from previous FIND FIRST.
; Output:
;       1)  Found (0)
;       2)  FAILED (?) DOS File Find Next errors)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_FIND_NEXT PROC

        mov     ah,4Fh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Done
        xor     ax,ax
J_Done:
        ret

FIL4_FIND_NEXT ENDP
;-----------------------------------------------------------------------------
END
