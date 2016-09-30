include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_IS_RO                           12/23/92                        CAG
;
;       Determine if a file is marked as R/O.
;
; WORD WINAPI FIL4_IS_RO(LPSTR Filename)
;
; Input:
;       1)  Filename
; Output:
;       1)  FALSE (0) Not read only
;       2)  TRUE (1) Is read only
;       3)  -1 File not found
;       
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_IS_RO PROC,
        Filename:LPSTR

        INVOKE  FIL4_ATTR,FileName,0,GET_INFO
        or      ax,ax
        jz      J_ERROR

        and     ax,1h                   ; Adjust the R/O bit
        jmp     J_0
J_ERROR:
        mov     ax,-1
J_0:
        ret

FIL4_IS_RO ENDP
;-----------------------------------------------------------------------------
END
