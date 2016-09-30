include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_EXISTS                          2/11/93                         CAG
;
;       Determine if a files exists or not.
;
; BOOL WINAPI FIL4_EXISTS(LPSTR lpFilename);
;
; Input:
;       1)  File to look for
; Output:
;       1)  TRUE (1) File exits
;       2)  FALSE (0) File doesn't exits
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_EXISTS PROC,
        lpFilename:LPSTR

        INVOKE  FIL4_ATTR,lpFilename,0,GET_INFO
        cmp     ax,-1
        je      NotHere
        mov     ax,TRUE
        jmp     J_Done
NotHere:
        mov     ax,FALSE
J_Done:

        ret

FIL4_EXISTS ENDP
;-----------------------------------------------------------------------------
END
