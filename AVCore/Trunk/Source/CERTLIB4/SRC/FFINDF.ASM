include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_FIND_FIRST                      12/28/92                        CAG
;
;       Returns information in the DTA of a file find first.
;
; UINT WINAPI FIL4_FIND_FIRST(LPSTR Filename,WORD Attributes);
;
; Input:
;       1)  Filename to search for.
;       2)  Attributes to look for
;                    HIDDEN (02h)
;                    SYSTEM (04h)
;                    LABEL  (08h)
;                    DIR    (10h)
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?) (DOS Error's)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_FIND_FIRST	PROC USES XCX XDX SDS,
        Filename:LPSTR,
        Attribute:WORD

        mov     ah,4Eh
        mov     cx,Attribute
        LDS_DX  Filename
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Done
        xor     ax,ax
J_Done:
        ret

FIL4_FIND_FIRST	ENDP
;-----------------------------------------------------------------------------
END
