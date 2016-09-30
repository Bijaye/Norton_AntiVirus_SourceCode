include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_MBR_GENERIC                                              SAMIZ
;
;     Repairs MBR using GENERIC. Master Boot Code is replaced with
;     generic boot code. Similar affect to using  FDISK /mbr
;
; $Header:   S:\certlib4\vcs\rep4_bpg.asv   1.5   29 Nov 1994 13:07:52   DALLEE  $
;
; Input:
;     lpBuffer:   Work buffer; SEL:OFF in WINDOWS
;     lpSEG_OFF:  SEG_OFF of lpBuffer
;     byDrive:    First Physical drive number (0x80)
;     RepairType: Used to make sure MBR is what's inteneded.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bpg.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:07:52   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:16:32   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:26:08   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.0   03 Sep 1993 19:29:24   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   19 Jul 1993 14:16:02   SZIADEH
;  Initial code.
;-----------------------------------------------------------------------------
REP4_MBR_GENERIC       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        lpBuffer:LPSTR,
                        lpSEG_OFF:LPSTR,
                        byDrive:BYTE,
                        RepairType:BYTE

;-------------------------------------
    cmp  byDrive, 080h
    jne  R_ERROR

    cmp     RepairType, REP_MBR_GENERIC
    je      @F
    cmp     RepairType, REP_MBR_PHYSICAL
    je      @F
    cmp     RepairType, REP_MBR_SAVE_PHYSICAL
    je      @F
    cmp     RepairType, REP_MBR_EOFD
    je      @F

    jmp     R_ERROR

@@:
    INVOKE  DIS4_READ_MBR, byDrive, lpSEG_OFF    ;read MBR
    cmp     ax,-1
    jz      R_ERROR

    INVOKE  DIS4_VALIDATE_MBR, lpBuffer, lpSEG_OFF
    or      ax,ax
    jz      R_ERROR
;------------------------------------;

    INVOKE  DIS4_READ_MBR, byDrive, lpSEG_OFF    ;read MBR
    cmp     ax,-1
    jz      R_ERROR

    LES_DI  lpBuffer
    push    XDI
    mov     XCX, PARTITION_OFFSET-1  ;

_padd:                               ;
    mov     byte ptr XES[XDI],0      ; padd with zeros
    inc     XDI                      ;
    loop    _padd
;------------------------------------;
    pop     XDI                      ;
    mov     XAX, OFFSET PartitionProgramStart;
    mov     XCX, OFFSET PartitionProgramEnd ;
    sub     XCX,XAX
    mov     XSI,XAX
    cld                              ; upward

DoMoreStore:
    mov     al,cs:[XSI]
    dec     al                       ; decrypt
    inc     XSI
    stosb
    loop    DoMoreStore              ; move boot code bytes into buffer


    INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,0,1,0,byDrive  ;Cyl,Sec,Head
    or      ax,ax
    jnz     R_ERROR

    mov     ax,1
    jmp     short R_EXIT

;--------------------------------------
R_ERROR:
     xor     ax,ax
;--------------------------------------
R_EXIT:
     ret
REP4_MBR_GENERIC       ENDP
;--------------------------------------

;encrypted code by adding one.

PartitionProgramStart:
db 0FBh, 034h, 0C1h, 08Fh, 0D1h, 0BDh, 001h, 07Dh, 08Ch, 0F5h, 08Fh, 0C1h, 08Fh, 0D9h, 0FCh, 0FDh
db 0C0h, 001h, 007h, 0BAh, 001h, 002h, 0F3h, 0A6h, 0EBh, 01Eh, 007h, 001h, 001h, 0BFh, 0BFh, 008h
db 0B4h, 005h, 081h, 03Dh, 081h, 075h, 00Fh, 081h, 03Dh, 001h, 076h, 01Dh, 084h, 0C7h, 011h, 0FFh
db 0CCh, 076h, 0F0h, 0CEh, 019h, 08Ch, 0EFh, 08Ch, 015h, 08Ch, 04Dh, 003h, 084h, 0C7h, 011h, 0FFh
db 0CCh, 075h, 00Bh, 081h, 03Dh, 001h, 075h, 0F5h, 0BFh, 0C8h, 007h, 0ECh, 031h, 0C0h, 005h, 001h
db 0B9h, 002h, 003h, 0BCh, 001h, 07Dh, 058h, 0CEh, 014h, 060h, 074h, 00Dh, 034h, 0C1h, 0CEh, 014h
db 050h, 076h, 0EEh, 0BFh, 0A9h, 007h, 0ECh, 016h, 0C0h, 0FFh, 07Eh, 082h, 03Eh, 056h, 0ABh, 075h
db 006h, 0BFh, 090h, 007h, 0ECh, 008h, 08Ch, 0F6h, 0EBh, 001h, 07Dh, 001h, 001h, 0ADh, 00Bh, 0C1h
db 075h, 00Ch, 057h, 0B5h, 00Fh, 0BCh, 008h, 001h, 0CEh, 011h, 05Fh, 0ECh, 0F1h, 0ECh, 0FFh, 04Eh
db 06Ah, 074h, 074h, 06Ah, 06Fh, 068h, 021h, 070h, 071h, 066h, 073h, 062h, 075h, 06Ah, 06Fh, 068h
db 021h, 074h, 07Ah, 074h, 075h, 066h, 06Eh, 001h, 046h, 073h, 073h, 070h, 073h, 021h, 06Dh, 070h
db 062h, 065h, 06Ah, 06Fh, 068h, 021h, 070h, 071h, 066h, 073h, 062h, 075h, 06Ah, 06Fh, 068h, 021h
db 074h, 07Ah, 074h, 075h, 066h, 06Eh, 001h, 04Ah, 06Fh, 077h, 062h, 06Dh, 06Ah, 065h, 021h, 071h
db 062h, 073h, 075h, 06Ah, 075h, 06Ah, 070h, 06Fh, 021h, 075h, 062h, 063h, 06Dh, 066h, 001h, 057h
db 042h, 04Fh
PartitionProgramEnd:
END
;--------------------------------------

unencrypted
PartitionProgramStart:
db 0FAh, 033h, 0C0h, 08Eh, 0D0h, 0BCh, 000h, 07Ch, 08Bh, 0F4h, 08Eh, 0C0h, 08Eh, 0D8h, 0FBh, 0FCh
db 0BFh, 000h, 006h, 0B9h, 000h, 001h, 0F2h, 0A5h, 0EAh, 01Dh, 006h, 000h, 000h, 0BEh, 0BEh, 007h
db 0B3h, 004h, 080h, 03Ch, 080h, 074h, 00Eh, 080h, 03Ch, 000h, 075h, 01Ch, 083h, 0C6h, 010h, 0FEh
db 0CBh, 075h, 0EFh, 0CDh, 018h, 08Bh, 0EEh, 08Bh, 014h, 08Bh, 04Ch, 002h, 083h, 0C6h, 010h, 0FEh
db 0CBh, 074h, 00Ah, 080h, 03Ch, 000h, 074h, 0F4h, 0BEh, 0C7h, 006h, 0EBh, 030h, 0BFh, 004h, 000h
db 0B8h, 001h, 002h, 0BBh, 000h, 07Ch, 057h, 0CDh, 013h, 05Fh, 073h, 00Ch, 033h, 0C0h, 0CDh, 013h
db 04Fh, 075h, 0EDh, 0BEh, 0A8h, 006h, 0EBh, 015h, 0BFh, 0FEh, 07Dh, 081h, 03Dh, 055h, 0AAh, 074h
db 005h, 0BEh, 08Fh, 006h, 0EBh, 007h, 08Bh, 0F5h, 0EAh, 000h, 07Ch, 000h, 000h, 0ACh, 00Ah, 0C0h
db 074h, 00Bh, 056h, 0B4h, 00Eh, 0BBh, 007h, 000h, 0CDh, 010h, 05Eh, 0EBh, 0F0h, 0EBh, 0FEh, 04Dh
db 069h, 073h, 073h, 069h, 06Eh, 067h, 020h, 06Fh, 070h, 065h, 072h, 061h, 074h, 069h, 06Eh, 067h
db 020h, 073h, 079h, 073h, 074h, 065h, 06Dh, 000h, 045h, 072h, 072h, 06Fh, 072h, 020h, 06Ch, 06Fh
db 061h, 064h, 069h, 06Eh, 067h, 020h, 06Fh, 070h, 065h, 072h, 061h, 074h, 069h, 06Eh, 067h, 020h
db 073h, 079h, 073h, 074h, 065h, 06Dh, 000h, 049h, 06Eh, 076h, 061h, 06Ch, 069h, 064h, 020h, 070h
db 061h, 072h, 074h, 069h, 074h, 069h, 06Fh, 06Eh, 020h, 074h, 061h, 062h, 06Ch, 065h, 000h, 042h
db 056h, 041h, 04Eh
PartitionProgramEnd:
