include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_CONV_FREE                        1/5/93                          CAG
;
;       Free memory in the DOS 640k area.
;
; WORD WINAPI MEM4_CONV_FREE(UINT Segment);
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
IFNDEF MIRRORS
; &? This function should not be called from OS/2 code

MEM4_CONV_FREE PROC USES XBX,
        lSegment:UINT

GlobalDosFree PROTO FAR,:UINT

        INVOKE  GlobalDosFree,lSegment
        ret

MEM4_CONV_FREE ENDP

ENDIF
;-----------------------------------------------------------------------------
END
