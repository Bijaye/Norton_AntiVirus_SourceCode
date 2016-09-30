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
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------

IFNDEF MIRRORS
; &? This function should not be called from OS/2 code

MEM4_CONV_ALLOCATE PROC USES XBX,
        uParagraph:WORD

GlobalDosAlloc PROTO FAR,:DWORD

        mov     bx,uParagraph
        shl     bx,1
        shl     bx,1
        shl     bx,1
        shl     bx,1
        INVOKE  GlobalDosAlloc,bx
        xchg    ax,dx

        ret

MEM4_CONV_ALLOCATE ENDP
ENDIF
;-----------------------------------------------------------------------------
END
