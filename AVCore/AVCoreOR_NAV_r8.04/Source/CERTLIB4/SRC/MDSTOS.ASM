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
; LIBRARY: DOS16.LIB
;          DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_SEGTOSEL PROC USES XBX,
        uSegment:WORD

        mov     ax,uSegment
        ret

MEM4_SEGTOSEL ENDP
;-----------------------------------------------------------------------------
END
