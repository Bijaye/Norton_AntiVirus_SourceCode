include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_OPEN                            12/23/92                        CAG
;
;       Open a file for a paticular mode.
;
; UINT WINAPI FIL4_OPEN(LPSTR Name,UINT OpenMode)
;
; Input:
;       1)  Filename
;       2)  Open mode
;                  fREAD (0)       Open for read
;                  fWRITE (1)      Open for write
;                  fREAD_WRITE (2) Open for R/W
;                  fCREATE (4)     Create a file (if exists will be destoyed)
;                  fAPPEND (8)     Open file, if not found, create, if found
;                                  go to its end
; Output:
;       1)  FAILED (-1)
;       2)  Handle (?)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_OPEN PROC USES XBX XCX XDX SDS,
        FileName:LPSTR,
        OpenMode:UINT

        test    OpenMode,fCREATE
        jnz     DoCreate
;---------------------------------------
        mov     XAX,OpenMode            ; Try to open file here
        and     ax,0000000001110111b    ; Strip rest of flags and open file
        LDS_DX  Filename
        mov     ah,3Dh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_0

;---------------------------------------
        test    OpenMode,fAPPEND
        jz      J_ERROR
DoCreate:
        LDS_DX  Filename
        mov     ah,3Ch                  ; Create file
        xor     cx,cx
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_1

;---------------------------------------
J_ERROR:
        mov     ax, -1                   ; Unable to open/create file
        jmp     J_1

;---------------------------------------
J_0:
        test    OpenMode,fAPPEND
        jz      J_1
        push    ax
        INVOKE  FIL4_SEEK,ax,0,fSEEK_END
        pop     ax
J_1:

        ret

FIL4_OPEN ENDP
;-----------------------------------------------------------------------------
END
