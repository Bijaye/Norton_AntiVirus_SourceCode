include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_READ_MBR                  3/2/93                          bAd
;
;       Read the master boot record and places it in Buffer.       
;
; WORD WINAPI DIS4_READ_MBR(BYTE Drive,LPSTR Buffer);
;
; Input:
;       1)  Drive to read.
;       2)  Buffer to hold MBR.
;
; Output:
;       1)  (-1) failed
;       2)  0 Success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_READ_MBR PROC,
        Drive:BYTE,
        Buffer:LPSTR

        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,0,1,0,Drive
        or      ax,ax
        jnz     Err
        jmp     Done
Err:
        mov     ax,-1
Done:
        ret

DIS4_READ_MBR   ENDP
;-----------------------------------------------------------------------------
END
