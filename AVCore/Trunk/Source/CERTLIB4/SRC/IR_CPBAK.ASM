; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\ir_cpbak.asv   1.4   29 Nov 1994 13:11:30   DALLEE  $
;
; Filename:     IR_CPBAK.ASM
; Programmer:   Jimmy Kuo
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_COPY_BACK
;
; Note:         Copies stuff back to the beginning of the file
;
; $Log:   S:\certlib4\vcs\ir_cpbak.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:11:30   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:20:32   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:31:06   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:29:18   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   20 May 1993 08:14:44   DDREW
;  No change.
;  
;     Rev 1.0   21 Apr 1993 21:14:36   CJKUO
;  Initial revision.
include CTS4.INC

;-----------------------------------------------------------------------------
; INOC_REP_COPY_BACK                    17APR93
;
;       Given a location, copies from location to beginning of file,
;       length = end-of-file - initial location.
;
; WORD WINAPI INOC_REP_CMP35(WORD Handle,LPSTR WorkBuf,DWORD OffsetPtr);
;
; Input:
;       1)  Handle : File Handle
;       2)  WorkBuf : Buffer to work in, must be at least 1024 bytes.
;       3)  OffsetPtr : Location to start copying to beginning of file.
; Output:
;       No error code because the routines I call don't give me an error
;       code.
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------
UniqueSegment
INOC_REP_COPY_BACK PROC USES XBX XCX XDX SDS SES XSI XDI,
        Handle:WORD,
        WorkBuffer:LPSTR,
        OffsetPtr:DWORD

        LOCAL CountRead:DWORD
        LOCAL PlaceSeek:DWORD

        mov     word ptr CountRead,0
        mov     word ptr CountRead+2,0

J_Read:
        mov     ax,word ptr OffsetPtr
        mov     dx,word ptr OffsetPtr+2
        add     ax,word ptr CountRead
        adc     dx,word ptr CountRead+2
        mov     word ptr PlaceSeek,ax
        mov     word ptr PlaceSeek+2,dx
        INVOKE  FIL4_SEEK,Handle,PlaceSeek,fSEEK_BEG
        INVOKE  FIL4_READ,Handle,WorkBuffer,1024
        or      ax,ax                   ; Bytes read
        jz      J_Done

        push    ax                      ; save the previous count
        INVOKE  FIL4_SEEK,Handle,CountRead,fSEEK_BEG
        pop     ax
        push    ax
        INVOKE  FIL4_WRITE,Handle,WorkBuffer,ax
        pop     ax

        ; Can move the following block between the two invokes and get
        ; rid of one set of push/pops.

        add     word ptr CountRead,ax
        adc     word ptr CountRead+2,0

        jmp     J_Read

;--------------------------------------
J_Done:
        ret

INOC_REP_COPY_BACK ENDP
;-----------------------------------------------------------------------------
END
