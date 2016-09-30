include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_DATETIME                        12/28/92                        CAG
;
;       Get or set the file date and time.
;
; DWORD WINAPI FIL4_DATETIME(UINT Handle,WORD Date, WORD Time,WORD Flag);
;
; Input:
;       1)  Handle of file
;       2)  Date to set to if Flag = SET_INFO
;       3)  Time to set to if Flag = SET_INFO
;       4)  Flag = GET_INFO/SET_INFO
; Output:
;       1)  FAILED (0) (If Flag = SET_INFO)
;       2)  HIWORD = DATE, LOWORD = TIME
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_DATETIME PROC USES XBX XCX,
        Handle:UINT,
        Date:WORD,
        Time:WORD,
        Flags:WORD

        mov     ax,Flags
        mov     ah,57h
        mov     cx,Time
        mov     dx,Date
        mov     XBX,Handle
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      J_Error
        mov     ax,cx                   ; CX was time
        jmp     J_Done

J_Error:
        xor     ax,ax
J_Done:

        ret

FIL4_DATETIME ENDP
;-----------------------------------------------------------------------------
END
