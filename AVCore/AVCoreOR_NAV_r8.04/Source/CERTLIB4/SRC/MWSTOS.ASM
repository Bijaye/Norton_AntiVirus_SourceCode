include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SEGTOSEL                        1/7/93                          CAG
;
;       Convert a DOS segment to a Windows selector.
;
; UINT WINAPI MEM4_SEGTOSEL(WORD Segment);
;
; Input:
;       1)  DOS segment to convert to a selector
; Output:
;       1)  Error (0)
;       2)  Selector to use
;
; LIBRARY: WIN16.LIB
;          WIN32.LIB
;
;-----------------------------------------------------------------------------
MEM4_SEGTOSEL PROC USES XBX,
        uSegment:WORD

IFDEF MIRRORS
        ; this function is invoked with the segment in AX, BX, or DX
        ; and always returns the segment in AX
        mov     ax,uSegment
ELSE
        mov     bx,uSegment
        mov     ax,0002h
        int     31h
        jnc     J_Done
        xor     ax,ax
ENDIF

J_Done:
        ret

MEM4_SEGTOSEL ENDP
;-----------------------------------------------------------------------------
END
