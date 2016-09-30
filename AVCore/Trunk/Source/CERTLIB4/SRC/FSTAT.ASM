include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_STAT                            12/28/92                        CAG
;
;       Get the statistics of a file, and store.
;	
; BOOL WINAPI FIL4_STAT(LPSTR Filename,LPSTR Storage,LPSTR Buffer);
;
; Input:
;       1)  Filename
;       2)  Where to store
;       3)  Buffer to use (43 bytes)
; Output:
;       1)  PASSED (0) and Storage filled in
;               Attr(DB), Time(DW), Date(DW), Size(DD)
;       2)  FAILED (1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_STAT PROC,
        Filename:LPSTR,
        Storage:LPSTR,
        Buffer:LPSTR

        LOCAL OldDTA:LPSTR
        LOCAL nStatus:WORD

        mov     nStatus,1

        INVOKE  FIL4_CHANGE_DTA,Buffer
        mov     word ptr OldDTA+2,dx
        mov     word ptr OldDTA,ax

        INVOKE  FIL4_FIND_FIRST,Filename,HIDDEN OR SYSTEM
        or      ax,ax
        jnz     J_Done

        add     word ptr Buffer,15h
        macMemCopy Buffer,Storage,9

        mov     nStatus,0
J_Done:
        INVOKE  FIL4_CHANGE_DTA,OldDTA
        mov     ax,nStatus

        ret

FIL4_STAT ENDP
;-----------------------------------------------------------------------------
END
