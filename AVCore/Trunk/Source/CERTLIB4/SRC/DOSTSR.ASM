include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_TSR                             1/12/93                         CAG
;
;       Leave this program resident, calculate paragraphs needed.	
;
; VOID WINAPI SYS4_TSR(WORD Offset);
;
; Input:
;       1)  The offset of the last routine in the CODE segment
;           that SHOULD NOT be left in memory.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       This routine DOES NOT RETURN.  It goes resident.
;
;-----------------------------------------------------------------------------
SYS4_TSR PROC,
        uOffset:WORD

        mov     ax,3100h                ; TSR
        mov     dx,uOffset
        add     dx,16                   ; Next paragraph
        shr     dx,1
        shr     dx,1
        shr     dx,1
        shr     dx,1                    ; Divide by 16
        add     dx,10h                  ; Allow for PSP
        int     21h

SYS4_TSR ENDP
;-----------------------------------------------------------------------------
END
