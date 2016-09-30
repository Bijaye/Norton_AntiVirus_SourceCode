include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_DIRECTORY                       1/6/93                          CAG
;
;       Get or set the current directory of the current drive.
;
; BOOL WINAPI FIL4_DIRECTORY(BYTE Drive, LPSTR Buffer,WORD Flag);
;
; Input:
;       1)  Drive to get directory of (0=Default, 1=A:..)
;       2)  Buffer, where to place or change to 64 bytes
;       3)  Flag = GET_INFO or SET_INFO
; Output:
;       1)  ERROR (0)
;       2)  PASSED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Note:
;       Will return the directory with the slash before it.
;
;-----------------------------------------------------------------------------
FIL4_DIRECTORY PROC USES XDX SDS XSI,
        bDrive:BYTE,
        lpBuffer:LPSTR,
        bFlag:WORD

        cmp     bFlag,GET_INFO
        je      GetDirectory

        cmp     bDrive,byte ptr 0
        je      NoChangeDrive

        mov     dl,bDrive
        dec     dl
        mov     ah,0Eh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

NoChangeDrive:
        mov     ah,3Bh
        LDS_DX  lpBuffer
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        mov     ax,1
        jnc     J_Done
J_Error:
        xor     ax,ax
        jmp     J_Done

GetDirectory:
        mov     ah,47h
        mov     dl,bDrive
        LDS_SI  lpBuffer
        mov     [XSI],byte ptr 5Ch
        inc     XSI
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Error
        mov     ax,1
J_Done:
        ret

FIL4_DIRECTORY ENDP
;-----------------------------------------------------------------------------
END
