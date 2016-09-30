include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_UNVECTOR                        1/18/93                         CAG
;
;       Unvector a previously trapped vector.
;
; BOOL WINAPI SYS4_UNVECTOR(BYTE Vector,FARPROC Hooked,FAR *OrigVect)
;
; Input:
;       1)  Vector to unvector
;       2)  The replacement vector to undo.
;       3)  Address where the original vector was stored.
; Output:
;       1)  TRUE Passed (1)
;       2)  FALSE Failed (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       This routine will take an image of what the replacement vector
;       was, get the address of the current vector, compare the two
;       then use the offset of OrigVect with the Segment of the replacement
;       vector to change the vector back.
;
;-----------------------------------------------------------------------------
SYS4_UNVECTOR PROC USES XBX XCX XDX SDS SES XSI XDI,
        byVector:BYTE,
        lpHooked:LPSTR,
        lpOrig:LPSTR

; Get the vector
        mov     ah,35h
        mov     al,byVector
        int     21h                     ; Get the vector

        mov     di,bx
; Look at the first 20 bytes
        LDS_SI  lpHooked
        mov     cx,10
        repe    cmpsw
        jne     NotSame

; Same then get address (offset) to get unvector number
        mov     di,word ptr lpOrig      ; Offset of where value was stored
        LDS_DX  XES[XDI]                ; DS:DX = Original vector

        mov     ah,25h
        int     21h                     ; Revector
IsSame:
        mov     ax,TRUE
        jmp     Done

NotSame:
        mov     ax,FALSE
Done:
        ret

SYS4_UNVECTOR ENDP
;-----------------------------------------------------------------------------
END
