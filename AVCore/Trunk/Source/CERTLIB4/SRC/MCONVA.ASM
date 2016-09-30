include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_CONV_ALLOCATE                   1/5/93                          CAG
;
;       Allocate memory in the DOS 640k area.
;
; DWORD WINAPI MEM4_CONV_ALLOCATE(WORD Paragraphs)
;
; Input:
;       1)  Number of paragraphs wanted
; Output:
;       1)  LOWORD ERROR (0) HIWORD Paragraphs available
;       2)  LOWORD Segment of memory
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_CONV_ALLOCATE PROC USES XBX,
        uParagraph:WORD

        mov     bx,uParagraph
        mov     ah,48h
        int     21h
        jnc     J_Done
        xor     ax,ax
        mov     dx,bx
J_Done:
        ret

MEM4_CONV_ALLOCATE ENDP
;-----------------------------------------------------------------------------
END
