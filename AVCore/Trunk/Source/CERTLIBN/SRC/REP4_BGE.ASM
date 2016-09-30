;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/rep4_bge.asv   1.9   27 Jun 1996 17:25:14   DALLEE  $
;;
;; Description: Defines REP4_BOOT_GENERIC function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/rep4_bge.asv  $
;  
;     Rev 1.9   27 Jun 1996 17:25:14   DALLEE
;  LuigiPlus port:
;  Added generic repair support for 1.68MB DMF formatted floppies.
;  
;     Rev 1.8   10 Apr 1996 15:10:44   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.7   15 Feb 1995 01:23:58   SZIADEH
;  removed the temporary ret  from start of file.
;  
;     Rev 1.6   14 Feb 1995 20:09:30   SZIADEH
;  converted the generic boot code to hex numbers so that it works
;  correctly in 32-bit land.
;  
;     Rev 1.5   14 Feb 1995 16:38:30   SZIADEH
;  made sure ds is set properly in DOS platform when the generic 
;  boot code is copied to lpBuffer.
;  
;     Rev 1.4   30 Aug 1994 18:38:46   SZIADEH
;  took out the ifdef stuff. it compiles under all platform.
;  
;     Rev 1.1   26 May 1994 08:09:38   RADSLACK
;  Added one more parameter to the comments.
;
;     Rev 1.0   25 May 1994 17:59:30   RADSLACK
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_BOOT_GENERIC
;
; BOOL WINAPI REP4_BOOT_GENERIC ( LPVOID lpBuffer, BYTE byDrive,
;                                 BYTE byType );
;
; Input:
;       lpBuffer        Work buffer minimum of 1024 bytes
;       byDrive         Physical drive number
; Output:
;       TRUE            if successful
;       FALSE           if fail
;----------------------------------------------------------------------------

REP4_BOOT_GENERIC  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPBYTE,
        byDrive:BYTE,
        byType:BYTE

        cmp     byDrive,07Fh
        ja      R_ERROR                 ; return if hard drive

        LES_DI  lpBuffer
        push    XDI                     ; save index
        mov     XCX,01FEh

        cld

_padd:
        mov     BYTE PTR XES[XDI],0     ; padd with zeros
        inc     XDI
        loop    _padd

        mov     WORD PTR XES[XDI],0AA55h ; put signature

                                        ; load oem message
        pop     XDI
        mov     XSI,OFFSET OEM_MESSAGE
        mov     XCX,0bh                 ; counter 11 bytes (jmp+ oem message)

        smov    ds,cs

        rep     movsb

                                        ; Build the BPB stuff

        cmp     byType,1                ; 360k?
        je      R_360
        cmp     byType,2                ; 720
        je      R_720
        cmp     byType,3                ; 1.2m?
        je      R_12
        cmp     byType,4                ; 1.44m?
        je      R_144
        cmp     byType,5                ; 2.88m?
        je      R_288
        cmp     byType,6                ; 1.68M DMF 1024 byte cluster
        je      R_168_1024
        cmp     byType,7                ; 1.68M DMF 2048 byte cluster
        je      R_168_2048

        jmp     R_ERROR                 ; none standard type

R_360:
        mov     XSI, OFFSET BPB_360
        jmp     @F
R_720:
        mov     XSI, OFFSET BPB_720
        jmp     @F
R_12:
        mov     XSI, OFFSET BPB_122
        jmp     @F
R_144:
        mov     XSI, OFFSET BPB_144
        jmp     @F
R_288:
        mov     XSI, OFFSET BPB_144
        jmp     @F
R_168_1024:
        mov     XSI, OFFSET BPB_168_1024
        jmp     @F
R_168_2048:
        mov     XSI, OFFSET BPB_168_2048
        ; jmp     @F

@@:
        mov     XCX,010h                ; 16 bytes

        rep     movsb                   ; move boot code bytes into buffer

                                        ; calculate start of boot code

        LDS_SI  lpBuffer
        mov     XAX,2
        add     al,BYTE PTR [XSI+1]     ; get jump offset

        smov    ds,cs                   ; ds=cs
        mov     XSI,OFFSET _NoBootProgram

        LES_DI  lpBuffer
        movuw   XCX,_NoBootProgramSize  ; copy NoBoot into boot sector
        add     XDI,XAX                 ; point to destination of jmp

        rep     movsb                   ; move boot code bytes into buffer

        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
        or      XAX,XAX                 ; function returns Zero if no errors
        jnz     R_ERROR

        mov     XAX,TRUE
        jmp     SHORT R_EXIT

R_ERROR:
        xor     XAX,XAX

R_EXIT:
        ret

REP4_BOOT_GENERIC  ENDP

;--------------------------------------------------;
;>>>>>>>>>>>>>>>> GENERIC BOOT CODE <<<<<<<<<<<<<<<;
;--------------------------------------------------;
_NoBootProgram:                         ;
DB 0FAh, 033h, 0C0h, 08Eh, 0D0h, 0BCh, 000h, 07Ch, 08Eh, 0D8h, 08Eh
DB 0C0h, 0FBh, 0BEh, 01Bh, 000h, 0E8h, 000h, 000h, 058h, 003h, 0F0h
DB 0E8h, 006h, 000h, 032h, 0E4h, 0CDh, 016h, 0CDh, 019h, 0ACh, 00Ah
DB 0C0h, 074h, 009h, 0B4h, 00Eh, 0BBh, 007h, 000h, 0CDh, 010h, 0EBh
DB 0F2h, 0C3h

;---------------------------------------;
NON_SYSTEM_DISK  db 'Non-System disk or disk error',13, 10
                 db 'Replace and strike any key when ready',13, 10, 0

ProgramEnd  label   byte

_NoBootProgramSize dw   ProgramEnd - _NoBootProgram
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
BPB_168_1024:
     DB 00h,02h,02h,01h,00h,02h,010h,00h,020h,0Dh,0F0h,05h,00h,15h,00h,02h
BPB_168_2048:
     DB 00h,02h,04h,01h,00h,02h,010h,00h,020h,0Dh,0F0h,03h,00h,15h,00h,02h

OEM_MESSAGE:
     DB 0EBh,028h,090h,049h,042h,04Dh,020h,050h,04Eh,043h,049h ; .(.IBM PNCI

UniqueSegmentEnd
END

;0100  FA            CLI
;0101  33C0          XOR AX,AX
;0103  8ED0          MOV SS,AX
;0105  BC007C        MOV SP,7C00
;0108  8ED8          MOV DS,AX
;010A  8EC0          MOV ES,AX
;010C  FB            STI
;010D  BE1B00        MOV SI,001B
;0110  E80000        CALL    0113
;0113  58            POP AX
;0114  03F0          ADD SI,AX
;0116  E80600        CALL    011F
;0119  32E4          XOR AH,AH
;011B  CD16          INT 16
;011D  CD19          INT 19
;011F  AC            LODSB
;0120  0AC0          OR  AL,AL
;0122  7409          JZ  012D
;0124  B40E          MOV AH,0E
;0126  BB0700        MOV BX,0007
;0129  CD10          INT 10
;012B  EBF2          JMP 011F
;012D  C3            RET

