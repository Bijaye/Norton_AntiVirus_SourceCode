include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_BOOT_GENERIC                                         SAMIZ
; $Header:   S:\certlib4\vcs\rep4_bge.asv   1.11   29 Nov 1994 13:08:52   DALLEE  $
;
; UINT WINAPI REP4_BOOT_GENERIC(lpBuffer:LPSTR,byDrive:BYTE)
; Input:
;   lpBuffer: Work buffer minimum of 1024 bytes
;   byDrive:  Physical drive number
; Output:
;   TRUE  succ
;   FALSE fail
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bge.asv  $
;  
;     Rev 1.11   29 Nov 1994 13:08:52   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.7.1.1   23 Nov 1993 11:38:00   SZIADEH
;  Use segment override (XES) in the line "mov  word ptr XES:[XDI], 55AA".
;  
;  
;     Rev 1.7.1.0   19 Oct 1993 07:16:06   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.7   12 Oct 1993 16:25:36   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.6.2.0   03 Sep 1993 19:27:38   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.6   28 Jun 1993 16:24:52   SZIADEH
;  Added disk type to parameter list in generic boot repair
;  
;     Rev 1.5   21 Jun 1993 16:58:30   SZIADEH
;  Modified code to work under WINDOWS...
;  
;     Rev 1.4   17 Jun 1993 12:06:54   CGUC
;  Windows fix, we can read from the CS, but not set DS to CS.
;  
;     Rev 1.3   17 Jun 1993 11:19:00   CJKUO
;  Temporary version to allow WINDOWS make to compile.
;  
;     Rev 1.2   16 Jun 1993 18:58:08   SZIADEH
;  Added WIN16 conditional compilation to handle situation where DS==CS
;-----------------------------------------------------------------------------
REP4_BOOT_GENERIC       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        lpBuffer:LPSTR,
                        lpSEG_OFF:LPSTR,
                        byDrive:BYTE,
                        byType:BYTE
;---------------------------------------;
IFDEF DEBUG
int     3h
nop
ENDIF
;---------------------------------------;
       cmp     byDrive, 07fh            ;
       ja      R_ERROR                  ; return if hard drive
;---------------------------------------;
       LES_DI  lpBuffer
       push    XDI                      ; save index
       mov     cx, 01FEh                ;
_padd:                                  ;
       mov     byte ptr XES[XDI],0      ; padd with zeros
       inc     XDI                      ;
       loop    _padd                    ;
       mov     word ptr XES[XDI], 0AA55h    ; put signature
;---------------------------------------;
;load oem message
       pop     XDI                      ; restore index
       mov     XSI, OFFSET OEM_MESSAGE  ;
       mov     cx, 0bh                  ; counter 11 bytes (jmp+ oem message)

COPY_OEM:
       mov     al,cs:[XSI]
       inc     XSI
       stosb
       loop    COPY_OEM
;---------------------------------------;
; build the BPB stuff

       cmp     byType,1                 ; 360k?
       je      R_360
       cmp     byType,2                 ; 720
       je      R_720
       cmp     byType,3                 ; 1.2m?
       je      R_12
       cmp     byType,4                 ; 1.44m?
       je      R_144
       cmp     byType,5                 ; 2.88m?
       je      R_288
       jmp     R_ERROR                  ; none standard type
;---------------------------------------;
R_360:
       mov     bx, OFFSET BPB_360
       jmp     @F
R_720:
       mov     bx, OFFSET BPB_720
       jmp     @F
R_12:
       mov     bx, OFFSET BPB_122
       jmp     @F
R_144:
       mov     bx, OFFSET BPB_144
       jmp     @F
R_288:
       mov     bx, OFFSET BPB_144

@@:
       mov     XSI, bx                  ; copy from
       mov     cx, 010h                 ; 16 bytes
COPY_BPB:
       mov     al,cs:[XSI]
       inc     XSI
       stosb
       loop    COPY_BPB                 ; move boot code bytes into buffer
;---------------------------------------;
; calculate start of boot code

       LDS_SI  lpBuffer
       mov     ax,2                     ;
       add     al,byte ptr [XSI+1]      ; get jump offset

;---------------------------------------;
       LES_DI  lpBuffer
       mov     cx,_NoBootProgramSize    ; copy NoBoot into boot sector
       mov     XSI, OFFSET _NoBootProgram
       cld                              ; upward
       add     di,ax                    ; point to destination of jmp

DoMoreStore:
       mov     al,cs:[XSI]
       inc     XSI
       stosb
       loop    DoMoreStore              ; move boot code bytes into buffer
;---------------------------------------;
; In DOS lpBuffer == lpSEG_OFF
; In WIN lpSEG_OFF is what we need to use; so, the following statement works
; under BOTH.

       INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,0,1,0,byDrive
       or      ax,ax                    ;function returns Zero if no errors
       jnz     R_ERROR

       mov     ax,1
       jmp     R_EXIT
;---------------------------------------;
R_ERROR:
       xor     ax,ax
;---------------------------------------;
R_EXIT:
       ret

REP4_BOOT_GENERIC      ENDP


;--------------------------------------------------;
;>>>>>>>>>>>>>>>> GENERIC BOOT CODE <<<<<<<<<<<<<<<;
;--------------------------------------------------;
_NoBootProgram:                         ;
       CLI                              ;
       XOR  AX,AX                       ;
       MOV  SS,AX                       ; Set SS to 0
       MOV  SP,7C00h                    ; Set SP to 0x7C00h
                                        ;
       MOV  DS,AX                       ; normal segment registers
       MOV  ES,AX                       ;
       STI                              ;
                                        ;
       MOV  SI,OFFSET NON_SYSTEM_DISK-SI_BASE ; Print "Non-System or disk error"
                                        ;
       CALL SI_BASE                     ; offset si to the real address
SI_BASE:                                ;
       POP  AX                          ; put address of SIBASE in ax
       ADD  SI,AX                       ; actual address of message
                                        ;
       CALL PrintString                 ; Display the message
                                        ;
       XOR  AH,AH                       ; Get a key
       INT  16h                         ;
                                        ;
       INT  19h                         ; Reboot with a good disk, hopefully
                                        ;
;---------------------------------------;
; Display the message pointed to by SI
PrintString:
       LODSB
       OR   AL,AL
       JZ   donePrinting
       MOV  AH,0Eh
       MOV  BX,0007h
       INT  10h
       JMP  PrintString
donePrinting:	
       RET
;---------------------------------------;
	
NON_SYSTEM_DISK  db	'Non-System disk or disk error',13, 10
                 db 'Replace and strike any key when ready',13, 10, 0

ProgramEnd	label	byte

_NoBootProgramSize dw	ProgramEnd - _NoBootProgram
;--------------------------------------------------;
;>>>>>>>>>>>>>END GENERIC BOOT CODE <<<<<<<<<<<<<<<;
;--------------------------------------------------;
; BPB information:
;OFFSET:0b  0c  0d  0e  0f  10  11   12  13   14  15   16  17  18  19  1a
BPB_360:
     DB 00h,02h,02h,01h,00h,02h,070h,00h,0D0h,02h,0FDh,02h,00h,09h,00h,02h
BPB_720:
     DB 00h,02h,02h,01h,00h,02h,070h,00h,0A0h,05h,0F9h,03h,00h,09h,00h,02h
BPB_122:
     DB 00h,02h,01h,01h,00h,02h,0E0h,00h,060h,09h,0F9h,07h,00h,0Fh,00h,02h
BPB_144:
     DB 00h,02h,01h,01h,00h,02h,0E0h,00h,040h,0Bh,0F0h,09h,00h,12h,00h,02h

OEM_MESSAGE:
     DB 0EBh,028h,090h,049h,042h,04Dh,020h,050h,04Eh,043h,049h ; .(.IBM PNCI
END
