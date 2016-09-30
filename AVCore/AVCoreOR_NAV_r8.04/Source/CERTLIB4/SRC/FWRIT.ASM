include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_WRITE                           12/28/92                        CAG
;
;       Write bytes to a file handle.
;
; UINT WINAPI FIL4_WRITE(UINT Handle,LPSTR Buffer, UINT Bytes)
;
; Input:
;       1)  Handle to write to
;       2)  Buffer to write
;       3)  Number of bytes to write
; Output:
;       1)  Number of bytes written.
;       2)  -1 if error
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_WRITE PROC USES XBX XCX XDX SDS,
        Handle:WORD,
        Buffer:LPSTR,
        Count:WORD

        mov     ah,40h
        mov     XBX,Handle
        LDS_DX  Buffer
        mov     XCX,Count
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_Done
        mov     ax,-1
J_Done:
        ret

FIL4_WRITE ENDP
;-----------------------------------------------------------------------------
END
