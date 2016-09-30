; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIBN/VCS/IR_FND35.ASv   1.5   08 Mar 1996 14:57:10   DALLEE  $
;
; Filename:     IR_FND35.ASM
; Programmer:   Jimmy Kuo
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_FIND35
;
; Note:         Finds where the 35 bytes header has been moved to.
;
; $Log:   S:/CERTLIBN/VCS/IR_FND35.ASv  $
;  
;     Rev 1.5   08 Mar 1996 14:57:10   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   12 Apr 1995 11:35:44   DALLEE
;  Cleanup for 32-bit platforms.  Mostly to handle 32-bit return values.
;  
;     Rev 1.3   27 Dec 1994 15:59:02   BRAD
;  Changed handle parameters from WORD to UINT
;  
;     Rev 1.2   09 Dec 1994 20:30:04   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.1   08 Dec 1994 20:27:42   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;     Rev 1.5   29 Nov 1994 13:08:12   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:20:24   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:31:02   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.0   03 Sep 1993 19:29:16   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   20 May 1993 08:14:46   DDREW
;  No change.
;  
;     Rev 1.1   24 Apr 1993 14:05:38   CJKUO
;  Would infinite loop (64K) if last read was for 35 bytes.
;  
;     Rev 1.0   21 Apr 1993 21:19:48   CJKUO
;  Initial revision.
;-----------------------------------------------------------------------------
; INOC_REP_FIND35                       17APR93
;
;       Finds location in file of 35 byte fragment stored by inoculation.
;
; DWORD WINAPI INOC_REP_FIND35(UINT Handle, DWORD StartSeek,
;                              LPBYTE Look35Buffer, LPVOID WorkBuffer)
;
; Input:
;       1)  Handle: File handle
;       2)  StartSeek: Where in the file to start looking
;       3)  Look35Buffer: 35 byte buffer to look for
;       4)  WorkBuffer: An area of scratch memory at least 1024 bytes big
; Output: DX,AX
;       0       35 byte string not found
;       DX,AX   Location in file where 35 byte string was found
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------

include CTSN.INC

UniqueSegment
INOC_REP_FIND35 PROC USES XCX SDS SES XSI XDI,
        Handle:UINT,
        StartSeek:DWORD,
        Look35Buffer:LPBYTE,
        WorkBuffer:LPBYTE

        LOCAL CountRead:DWORD
        LOCAL FirstWord:WORD

        xor     XAX,XAX                  ; Initialize CountRead
IF OS_16
        mov     word ptr CountRead,ax
        mov     word ptr CountRead+2,ax
ELSE
        mov     CountRead,eax
ENDIF

        INVOKE  FIL4_SEEK,Handle,StartSeek,fSEEK_BEG

        LDS_SI  Look35Buffer
        lodsw                           ; get first word of 35 byte string
        mov     word ptr FirstWord,ax
        jmp     J_Read

J_LookMore:
; Back up 35 bytes because the last 35 bytes of the previous read are there
; only to have room for the header to do a complete compare.  See "sub ax,35"
; below.
        INVOKE  FIL4_SEEK,Handle,35,fSEEK_BAK

J_Read:
        INVOKE  FIL4_READ,Handle,WorkBuffer,1024
; Should check to see if anything is read in.  But since 0 is less than 35,
; the next check accounts for this.
;       or      ax,ax
;       jz      J_Done

; We read in 1024 bytes, but the last 35 can't be used since INOC_REP_CMP35
; wouldn't work right if you started the checking in that area.
        sub     XAX,35
        jbe     J_Done                  ; jmp if CF=1 or ZF=1

IF OS_16
        add     word ptr CountRead,ax
        adc     word ptr CountRead+2,0
ELSE
        add     CountRead,eax
ENDIF

        mov     XCX,XAX                   ; Bytes to check = bytes read - 35.

        LDS_SI  WorkBuffer
        dec     XSI
        mov     dx,word ptr FirstWord
J_LookFor:
        inc     XSI
        cmp     word ptr XDS[XSI],dx
        loopnz  J_LookFor
        jcxz    J_LookMore

        INVOKE  INOC_REP_CMP35,Look35Buffer,VSDS XSI
        or      XAX,XAX
        jnz     J_LookFor

J_FOUND:
        inc     XCX                      ; to offset the loopnz's dec cx
IF OS_16
        sub     word ptr CountRead,cx   ; We were cx from the end.
        sbb     word ptr CountRead+2,0
        mov     ax,word ptr StartSeek   ; Get starting point.
        mov     dx,word ptr StartSeek+2
        add     ax,word ptr CountRead   ; Add the amount processed.
        adc     dx,word ptr CountRead+2
ELSE
        sub     CountRead,ecx
        mov     eax,StartSeek
        add     eax,CountRead
ENDIF
        ret


;--------------------------------------
J_Done:
IF OS_16
        xor     ax,ax                   ; Didn't find it.
        xor     dx,dx
ELSE
        XOR     eax,eax
ENDIF
        ret

INOC_REP_FIND35 ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
