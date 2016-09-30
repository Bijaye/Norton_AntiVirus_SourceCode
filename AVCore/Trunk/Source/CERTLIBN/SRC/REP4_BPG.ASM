;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_BPG.ASv   1.7   10 Apr 1996 15:10:44   DALLEE  $
;;
;; Description: Defines REP4_MBR_GENERIC function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_BPG.ASv  $
;  
;     Rev 1.7   10 Apr 1996 15:10:44   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.6   11 Jan 1996 17:48:54   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.5   29 Nov 1995 14:24:14   DCHI
;  Changed REP4_MBR_GENERIC to also be able to repair MBRs on other
;  than the first physical hard drive.
;  
;     Rev 1.4   19 Aug 1994 21:51:04   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.3   18 Aug 1994 22:53:22   RSTANEV
;  Fixed compilation error for VxD platform.
;
;     Rev 1.2   17 Aug 1994 13:26:04   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.1   25 May 1994 17:21:48   RSTANEV
;  Fixed labels in data segments.
;
;     Rev 1.0   25 May 1994 16:54:42   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_MBR_GENERIC
;
; BOOL WINAPI REP4_MBR_GENERIC ( LPVOID lpBuffer, BYTE byDrive, BYTE byType );
;
; Repairs MBR using GENERIC. Master Boot Code is replaced with
; generic boot code. Similar affect to using  FDISK /mbr.
;
; Input:
;     lpBuffer:   Work buffer
;     byDrive:    Physical drive number
;     RepairType: Used to make sure MBR is what's inteneded.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;----------------------------------------------------------------------------
REP4_MBR_GENERIC  PROC  CODEDISTANCE  USES XCX SDS SES XSI XDI,
        lpBuffer:LPBYTE,
        byDrive:BYTE,
        RepairType:BYTE

        cmp     byDrive,080h
        jb      R_ERROR                 ; Return if not a hard drive

        cmp     RepairType,REP_MBR_GENERIC
        je      @F
        cmp     RepairType,REP_MBR_PHYSICAL
        je      @F
        cmp     RepairType,REP_MBR_SAVE_PHYSICAL
        je      @F
        cmp     RepairType,REP_MBR_EOFD
        jne     R_ERROR
@@:
        INVOKE  DIS4_READ_MBR,byDrive,lpBuffer ; read MBR
        inc     XAX
        jz      R_ERROR

        INVOKE  DIS4_VALIDATE_MBR,byDrive,lpBuffer
        or      XAX,XAX
        jz      R_ERROR

        INVOKE  DIS4_READ_MBR,byDrive,lpBuffer ; read MBR
        inc     XAX
        jz      R_ERROR

        LES_DI  lpBuffer
        push    XDI
        mov     XCX,PARTITION_OFFSET - 1

        cld                             ; upward

_padd:
        mov     BYTE PTR XES[XDI],0     ; padd with zeros
        inc     XDI
        loop    _padd

        pop     XDI
        mov     XSI,OFFSET PartitionProgramStart
        mov     XCX,OFFSET PartitionProgramEnd - OFFSET PartitionProgramStart

        smov    ds,cs

DoMoreStore:
        lodsb
        dec     al                      ; decrypt
        stosb
        loop    DoMoreStore             ; move boot code bytes into buffer


        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
        or      al,al
        jnz     R_ERROR

        mov     XAX,TRUE
        jmp     SHORT R_EXIT

R_ERROR:
        xor     XAX,XAX
R_EXIT:
        ret
REP4_MBR_GENERIC  ENDP

UniqueSegmentEnd

IF OS_32
UniqueDataSegment
ENDIF

; encrypted code by adding one.

PartitionProgramStart  LABEL  BYTE
        DB 0FBh, 034h, 0C1h, 08Fh, 0D1h, 0BDh, 001h, 07Dh, 08Ch, 0F5h, 08Fh, 0C1h, 08Fh, 0D9h, 0FCh, 0FDh
        DB 0C0h, 001h, 007h, 0BAh, 001h, 002h, 0F3h, 0A6h, 0EBh, 01Eh, 007h, 001h, 001h, 0BFh, 0BFh, 008h
        DB 0B4h, 005h, 081h, 03Dh, 081h, 075h, 00Fh, 081h, 03Dh, 001h, 076h, 01Dh, 084h, 0C7h, 011h, 0FFh
        DB 0CCh, 076h, 0F0h, 0CEh, 019h, 08Ch, 0EFh, 08Ch, 015h, 08Ch, 04Dh, 003h, 084h, 0C7h, 011h, 0FFh
        DB 0CCh, 075h, 00Bh, 081h, 03Dh, 001h, 075h, 0F5h, 0BFh, 0C8h, 007h, 0ECh, 031h, 0C0h, 005h, 001h
        DB 0B9h, 002h, 003h, 0BCh, 001h, 07Dh, 058h, 0CEh, 014h, 060h, 074h, 00Dh, 034h, 0C1h, 0CEh, 014h
        DB 050h, 076h, 0EEh, 0BFh, 0A9h, 007h, 0ECh, 016h, 0C0h, 0FFh, 07Eh, 082h, 03Eh, 056h, 0ABh, 075h
        DB 006h, 0BFh, 090h, 007h, 0ECh, 008h, 08Ch, 0F6h, 0EBh, 001h, 07Dh, 001h, 001h, 0ADh, 00Bh, 0C1h
        DB 075h, 00Ch, 057h, 0B5h, 00Fh, 0BCh, 008h, 001h, 0CEh, 011h, 05Fh, 0ECh, 0F1h, 0ECh, 0FFh, 04Eh
        DB 06Ah, 074h, 074h, 06Ah, 06Fh, 068h, 021h, 070h, 071h, 066h, 073h, 062h, 075h, 06Ah, 06Fh, 068h
        DB 021h, 074h, 07Ah, 074h, 075h, 066h, 06Eh, 001h, 046h, 073h, 073h, 070h, 073h, 021h, 06Dh, 070h
        DB 062h, 065h, 06Ah, 06Fh, 068h, 021h, 070h, 071h, 066h, 073h, 062h, 075h, 06Ah, 06Fh, 068h, 021h
        DB 074h, 07Ah, 074h, 075h, 066h, 06Eh, 001h, 04Ah, 06Fh, 077h, 062h, 06Dh, 06Ah, 065h, 021h, 071h
        DB 062h, 073h, 075h, 06Ah, 075h, 06Ah, 070h, 06Fh, 021h, 075h, 062h, 063h, 06Dh, 066h, 001h, 057h
        DB 042h, 04Fh
PartitionProgramEnd  LABEL  BYTE

IF OS_32
UniqueDataSegmentEnd
ENDIF

END

Unencrypted partition
----------------------
DB 0FAh, 033h, 0C0h, 08Eh, 0D0h, 0BCh, 000h, 07Ch, 08Bh, 0F4h, 08Eh, 0C0h, 08Eh, 0D8h, 0FBh, 0FCh
DB 0BFh, 000h, 006h, 0B9h, 000h, 001h, 0F2h, 0A5h, 0EAh, 01Dh, 006h, 000h, 000h, 0BEh, 0BEh, 007h
DB 0B3h, 004h, 080h, 03Ch, 080h, 074h, 00Eh, 080h, 03Ch, 000h, 075h, 01Ch, 083h, 0C6h, 010h, 0FEh
DB 0CBh, 075h, 0EFh, 0CDh, 018h, 08Bh, 0EEh, 08Bh, 014h, 08Bh, 04Ch, 002h, 083h, 0C6h, 010h, 0FEh
DB 0CBh, 074h, 00Ah, 080h, 03Ch, 000h, 074h, 0F4h, 0BEh, 0C7h, 006h, 0EBh, 030h, 0BFh, 004h, 000h
DB 0B8h, 001h, 002h, 0BBh, 000h, 07Ch, 057h, 0CDh, 013h, 05Fh, 073h, 00Ch, 033h, 0C0h, 0CDh, 013h
DB 04Fh, 075h, 0EDh, 0BEh, 0A8h, 006h, 0EBh, 015h, 0BFh, 0FEh, 07Dh, 081h, 03Dh, 055h, 0AAh, 074h
DB 005h, 0BEh, 08Fh, 006h, 0EBh, 007h, 08Bh, 0F5h, 0EAh, 000h, 07Ch, 000h, 000h, 0ACh, 00Ah, 0C0h
DB 074h, 00Bh, 056h, 0B4h, 00Eh, 0BBh, 007h, 000h, 0CDh, 010h, 05Eh, 0EBh, 0F0h, 0EBh, 0FEh, 04Dh
DB 069h, 073h, 073h, 069h, 06Eh, 067h, 020h, 06Fh, 070h, 065h, 072h, 061h, 074h, 069h, 06Eh, 067h
DB 020h, 073h, 079h, 073h, 074h, 065h, 06Dh, 000h, 045h, 072h, 072h, 06Fh, 072h, 020h, 06Ch, 06Fh
DB 061h, 064h, 069h, 06Eh, 067h, 020h, 06Fh, 070h, 065h, 072h, 061h, 074h, 069h, 06Eh, 067h, 020h
DB 073h, 079h, 073h, 074h, 065h, 06Dh, 000h, 049h, 06Eh, 076h, 061h, 06Ch, 069h, 064h, 020h, 070h
DB 061h, 072h, 074h, 069h, 074h, 069h, 06Fh, 06Eh, 020h, 074h, 061h, 062h, 06Ch, 065h, 000h, 042h
DB 056h, 041h, 04Eh
