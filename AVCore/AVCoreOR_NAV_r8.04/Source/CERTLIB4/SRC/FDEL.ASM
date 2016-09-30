include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_DELETE                           12/29/92                        CAG
;
;       Delete a file from the drive.
;
; BOOL WINAPI FIL4_DELETE(LPSTR Filename);
;
; Input:
;       1)  Filename to delete
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_DELETE PROC USES XDX SDS,
        Filename:LPSTR

        LDS_DX  Filename
        mov     ah,41h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        mov     ax,FALSE
        jc      J_Done
        mov     ax,TRUE
J_Done:
        ret

FIL4_DELETE ENDP
;-----------------------------------------------------------------------------
END

