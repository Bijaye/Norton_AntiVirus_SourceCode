; Copyright 1993 Symantec Corporation                                  
;****************************************************************************
; $Log:   S:\certlib4\vcs\ddint25.asv  $  
;  
;     Rev 1.4   29 Nov 1994 13:05:30   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:27:08   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:22:58   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.1   05 Sep 1993 18:59:12   BARRY
;  Fix int 25 and int 26 problems:  not preserving registers (esp. stack)
;****************************************************************************

include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_INT25                           1/7/93                          CAG
;
;       Low level routine that processes an interrupt 25h (READ).
;       This function preserves all unused registers (real int 25 won't)
;       but ax and cx (passed in) are modified
;
; UINT WINAPI DIS4_INT25(LPSTR Buffer, DWORD Sector);
;
; Input:        NOTE:  Some values are passed by register
;       0)  AL = drive number (0=A, 1=B, etc.)
;           CX = # of sectors to read
;       1)  Buffer -- storage for data read
;       2)  Starting Sector, DWORD for large media
; Output:
;       What interrupt 25h would give you
;
; LIBRARY: DOS16.LIB
;          DOS32.LIB
;
; NOTES:
; 09/5/93 BarryG: Changed the way registers are preserved.  Based it on
;                 the way SYMKRNL:dsk_block.c:_DiskBlockDevice() does it.
;                 Undocumented DOS says all registers are destroyed except
;                 the segment registers.  Also note that the flags are not
;                 popped off the stack so you must do that yourself.
;                 MOREWORK:  My goal was to get it working without changing
;                 the call parameters or hierarchy (didn't want to break
;                 anything 2 weeks before shipping!).  Ideally, this function
;                 should be rewritten to simplify the call sequence and
;                 register usage (see _DiskBlockDevice()).  See also:
;                 DIS4_READ_LOGICAL(DRL), DIS4_SET_IO(DDSETIO).
;-----------------------------------------------------------------------------
DIS4_INT25 PROC USES XBX XDX XSI XDI SDS SES,
        lpBuffer:LPSTR,
        uSector:DWORD

        LOCAL   SaveDriveNum:WORD
        LOCAL   lpExtRead[10]:BYTE

        xor     ah, ah                  ; clear error code
        mov     SaveDriveNum, ax        ; save drive number

        INVOKE  DIS4_SET_IO,ADDR lpExtRead,uSector,cx
        or      ax,ax
        jz      NoAdjust
        mov     cx,-1                   ; Do extended read
        mov     ax, word ptr lpBuffer+2
        mov     word ptr lpExtRead[8], ax
        mov     ax, word ptr lpBuffer
        mov     word ptr lpExtRead[6], ax
        mov     ax, ss
        mov     DS, ax
        lea     bx, lpExtRead           ; DS:BX <- lpExtRead
        jmp     short DoRead
        
NoAdjust:
        LDS_BX  lpBuffer
        mov     dx, word ptr uSector    ; Get low word of start sector

DoRead:
        mov     di, 0FFFCh              ; Magic PC-MOS number (ignored by DOS)
        mov     ax,SaveDriveNum         ; Restore drive number


        push    bp                      ; Gets lost in call
        int     25h
        pop     bp                      ; Pop flags left from call
        pop     bp                      ; restore bp (destroyed in call)

        ret
DIS4_INT25 ENDP
;-----------------------------------------------------------------------------
END
