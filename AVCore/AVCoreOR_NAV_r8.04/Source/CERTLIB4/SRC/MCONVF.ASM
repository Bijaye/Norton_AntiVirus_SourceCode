include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_CONV_FREE                        1/5/93                          CAG
;
;       Free memory in the DOS 640k area.
;
; WORD WINAPI MEM4_CONV_FREE(WORD Segment);
;
; Input:
;       1)  Segment of memory to free
; Output:
;       1)  ERROR (?)
;       2)  PASSED (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_CONV_FREE PROC USES SES XDI,
        lSegment:WORD

        MOV_ES  lSegment
        mov     ah,49h
        int     21h
        jc      J_Done
        xor     ax,ax
J_Done:
        ret

MEM4_CONV_FREE ENDP
;-----------------------------------------------------------------------------
END
