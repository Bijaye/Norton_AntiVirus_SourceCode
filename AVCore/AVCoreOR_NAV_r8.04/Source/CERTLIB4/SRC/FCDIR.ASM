include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_CREATE_DIR                      2/7/93                          CAG
;
;       Will create a directory on a drive, multiple levels if necessary.
;
; BOOL WINAPI FIL4_CREATE_DIR(LPSTR lpPath);
;
; Input:
;       1)  Path to create, NULL terminated.
; Output:
;       1)  TRUE (1)  Created.
;       2)  FALSE (0) Not created
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
; LIBRARY:WIN16.LIB
; LIBRARY:WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_CREATE_DIR PROC USES SDS XSI,
        lpPath:LPSTR

        LOCAL   lpNewPath:LPSTR

        mov     ax,word ptr lpPath
        mov     word ptr lpNewPath,ax
        mov     ax,word ptr lpPath+2
        mov     word ptr lpNewPath+2,ax

MorePath:
        INVOKE  STR4_SRCH_CHAR,lpNewPath,5Ch
        or      ax,ax
        jz      NoSlash

        mov     word ptr lpNewPath,ax
        LDS_SI  lpNewPath
        mov     [XSI],byte ptr 0        ; Remove slash for now

        mov     ah,39h
        LDS_DX  lpPath
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     DoMorePath

        cmp     ax,5                    ; Access denied or already exists
        je      DoMorePath
IsError:
        mov     ax,FALSE
        jmp     J_Error

NoSlash:
        mov     ah,39h
        LDS_DX  lpPath
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      IsError
        mov     ax,TRUE
        jmp     J_Done

DoMorePath:
        LDS_SI  lpNewPath
        mov     [XSI],byte ptr 5Ch      ; Place slash back in
        add     word ptr lpNewPath,1
        jmp     MorePath

J_Error:
J_Done:
        ret

FIL4_CREATE_DIR ENDP
;-----------------------------------------------------------------------------
END
