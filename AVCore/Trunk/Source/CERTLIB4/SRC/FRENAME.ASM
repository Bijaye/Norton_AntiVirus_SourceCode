include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_RENAME                          1/8/93                          CAG
;
;       Renames a file.
;
; BOOL WINAPI FIL4_RENAME(LPSTR From, LPSTR To);
;
; Input:
;       1)  From filename
;       2)  To filename
; Output:
;       1)  TRUE (Passed)
;       2)  FALSE (Failed)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_RENAME PROC USES XDX SDS SES XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR


        LDS_DX  lpFrom
        LES_DI  lpTo

        mov     ah,56h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        mov     ax,FALSE
        jc      J_Error
        mov     ax,TRUE
J_Error:
        ret

FIL4_RENAME ENDP
;-----------------------------------------------------------------------------
END

