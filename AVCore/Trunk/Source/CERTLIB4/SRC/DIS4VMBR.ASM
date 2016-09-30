include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
;
; DIS4_VALIDATE_MBR
; Active DOS partitions ONLY. If the active partition is non-DOS and does
; not end with the 0x55AA,this test fails.
;
; $Header:   S:/CERTLIB4/VCS/dis4vmbr.asv   1.4   09 Dec 1996 11:45:20   JALLEE  $
; Input:
;   lpBuffer: work buffer (SEL:OFF in WINDOWS)
;   lpSEG_OFF: Segment:Offset of lpBuffer
; Output:
;   fail 0
;   succ 1
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIB4/VCS/dis4vmbr.asv  $
;  
;     Rev 1.4   09 Dec 1996 11:45:20   JALLEE
;  Ported from Qak2
;  ;     Rev 1.1.1.1   12 Jun 1995 14:15:52   SZIADEH
;  ;  xchg bl,bh in order to comeup with the right cylinder value.
;  
;     Rev 1.3   29 Nov 1994 13:07:46   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:23:16   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:33:38   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:30:14   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   19 Jul 1993 14:17:16   SZIADEH
;  Initial revision.
;  
;-----------------------------------------------------------------------------
DIS4_VALIDATE_MBR    PROC USES XBX XCX XDX SDS SES XSI XDI,
                     lpBuffer:LPSTR,
                     lpSEG_OFF:LPSTR

                     LOCAL uCyl: WORD
                     LOCAL bySec:BYTE
                     LOCAL byHead:BYTE

;-------------------------------------
    LDS_SI   lpBuffer

    add  XSI, PARTITION_OFFSET      ; point to first partition
    mov  XCX,  4                    ; counter

next_part:

    cmp  BYTE PTR [XSI], 080h       ; Look for active partition
    je   @F                         ; found it! jmp
    cmp  BYTE PTR[XSI], 0           ; not active
    jne  R_ERROR                    ; if neither 0 nor 80, jmp error
    add  XSI, 010h                  ; point to next partition
                                    ;
    loop next_part                  ;
    jmp  short R_ERROR              ; no active partition was found
;------------------------------------
                                    ;
@@:                                 ;
    mov  dh, BYTE PTR[XSI+01h]      ;
    mov  byHead, dh                 ;
    mov  bx, WORD PTR[XSI+02h]      ;
    mov  bySec, bl                  ;
    and  bySec, 03Fh                ; 11 1111b
    mov  XCX, 06                    ;
    shr  bx, cl                     ;
    xchg bh, bl                     ; get the right cylinder value
    mov  uCyl, bx                   ;
                                    ;
    INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,uCyl,bySec,byHead,080h ;cyl,sec, hd,drive
    or      ax,ax                   ;
    jnz     R_ERROR                 ;
;------------------------------------
    LDS_SI  lpBuffer

    mov ax, 0AA55h
    cmp ax, WORD PTR [XSI+510]
    jne R_ERROR

    jmp short R_EXIT
;--------------------------------------
R_ERROR:
    xor     ax,ax
;--------------------------------------
R_EXIT:
    ret
DIS4_VALIDATE_MBR  ENDP
;--------------------------------------
END
