include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_READ                            12/28/92                        CAG
;
;       Read bytes from a file handle.
;
; UINT WINAPI FIL4_READ(UINT Handle,LPSTR Buffer, UINT Bytes)
;
; Input:
;       1)  Handle
;       2)  Buffer to use
;       3)  Number of bytes to read
; Output:
;       1)  Number of bytes read
;       2)  -1 if file error
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_READ PROC USES XBX XCX XDX SDS,
        Handle:WORD,
        Buffer:LPSTR,
        Count:WORD

        mov     ah,3Fh
        mov     bx,Handle
        LDS_DX  Buffer
        mov     XCX,Count
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_Done
        mov     XAX,-1
J_Done:

        ret

FIL4_READ ENDP
;-----------------------------------------------------------------------------
END

