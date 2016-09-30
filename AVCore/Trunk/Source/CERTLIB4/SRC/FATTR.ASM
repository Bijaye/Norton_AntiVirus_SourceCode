include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_ATTR                            12/28/92                        CAG
;
;       Get or set the file attributes.
;
; UINT WINAPI FIL4_ATTR(LPSTR Filename,UINT Attr,UINT Flag);
;
; Input:
;       1)  Filename
;       2)  Attribute to change to if SET_INFO in FLAG
;       1)  Flag = GET_INFO or SET_INFO
; Output:
;       1)  ERROR (-1) if failed
;           PASSED (?) SET_INFO
;           ATTR if GET_INFO
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_ATTR PROC USES XCX XDX SDS,
        Filename:LPSTR,
        Attr:UINT,
        Flags:UINT

        mov     XAX,Flags
        mov     ah,43h
        mov     XCX,Attr
        LDS_DX  Filename
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        mov     ax,cx
        jnc     J_Done
        mov     ax,-1

J_Done:
        ret

FIL4_ATTR ENDP
;-----------------------------------------------------------------------------
END
