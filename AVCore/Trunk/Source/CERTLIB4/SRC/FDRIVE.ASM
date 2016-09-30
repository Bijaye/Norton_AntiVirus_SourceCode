include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_DRIVE                           1/6/93                          CAG
;
;       Get or set the current drive.
;
; BOOL WINAPI FIL4_DRIVE(LPSTR Buffer,WORD Flag);
;
; Input:
;       1)  Buffer, where to place or change to 2 bytes
;       2)  Flag = GET_INFO or SET_INFO
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
;       Will return or expect C: in the buffer
;
;-----------------------------------------------------------------------------
FIL4_DRIVE PROC USES XDX SDS XSI,
        lpBuffer:LPSTR,
        bFlag:WORD

        LDS_SI  lpBuffer

        cmp     bFlag,GET_INFO
        je      GetDRIVE

        mov     dl,[XSI]
        sub     dl,'A'
        mov     ah,0Eh
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

        mov     ah,19h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        cmp     al,dl
        je      Passed
J_Error:
        xor     ax,ax
        jmp     J_Done

GetDRIVE:
        mov     ah,19h                  ; Get drive
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        add     al,'A'
        mov     [XSI],al
        mov     byte ptr [XSI+1],':'
Passed:
        mov     ax,1
J_Done:
        ret

FIL4_DRIVE ENDP
;-----------------------------------------------------------------------------
END

