include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_GENERIC               4/1/93                             SAMIZ
; $Header:   S:\certlib4\vcs\rep4_bfg.asv   1.4   29 Nov 1994 13:09:18   DALLEE  $
;
; UINT WINAPI REP4_FLOP_GENERIC(WORD uDrive, LPSTR lpBootfile, LPSTR lpBuffer, LPSTR lpGlobal WORD uType);
;
; Input:
;       1) Drive Number with logical in high byte, physical in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bfg.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:09:18   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:18:48   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:30:04   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:28:46   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   14 May 1993 12:29:40   SZIADEH
;  
;  added a parameter to hold the path for BOOT.BIN
;  
;     Rev 1.0   10 May 1993 16:23:58   SZIADEH
;  Initial revision.
;
;-----------------------------------------------------------------------------

REP4_FLOP_GENERIC       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBootfile:LPSTR,
                        lpBuffer:LPSTR,
                        uType:WORD

                        LOCAL  uHandle:WORD
                        LOCAL  bySector:BYTE
                        LOCAL  byHead:BYTE
                        LOCAL  uCylinder:WORD
                        LOCAL  byDrive:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
       mov     ax, uDrive
       mov     byDrive, al     ;physical

       INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,0,1,0,byDrive
       or      ax,ax                   ;function returns Zero if no errors
       jnz     R_ERROR


       LDS_SI  lpBuffer
       cmp     byte ptr[XSI],0E9h
       je      _longjoffset             ; long branch

       cmp     byte ptr[XSI],0EBh
       je      _shortjoffset            ;

       mov     byte ptr [XSI],0EBh      ;
       mov     byte ptr [XSI+1],03Ch    ;


_shortjoffset:
       mov ax,2
       add al,byte ptr [XSI+1]         ; get jump offset
       jmp short _verify_room

_longjoffset:
       mov ax,word ptr [XSI+1] ; get jump offset
       add ax,3

;	jump address in ax, verify enough room for noboot code

_verify_room:
       mov bx,ax
       add bx,_NoBootProgramSize
       cmp bx,0200h                ; are some sectors bigger than this?
       jg  R_ERROR                 ; no room, fail repair

       mov cx,_NoBootProgramSize   ; copy NoBoot into boot sector
       LES_DI  lpBuffer

       push    cs
       pop     ds
       mov     XSI, OFFSET _NoBootProgram

       add     di,ax               ; point to destination of jmp
       rep movsb                   ; move boot code bytes into buffer

       INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
       or      ax,ax                   ;function returns Zero if no errors
       jnz     R_ERROR

       mov     ax,1
       jmp     R_EXIT
;--------------------------------------
R_ERROR:
       xor     ax,ax
;--------------------------------------
R_EXIT:
       ret

REP4_FLOP_GENERIC      ENDP

;--------------------------------------
; GENERIC BOOT CODE
;--------------------------------------
_NoBootProgram:
       cli
       XOR AX,AX
       MOV SS,AX           ; Set SS to 0
       MOV SP,7C00h        ; Set SP to 0x7C00h

       MOV DS,AX           ; normal segment registers
       MOV ES,AX
       STI

       MOV SI,OFFSET NON_SYSTEM_DISK-SIBASE ; Print "Non-System or disk error"

       CALL    SIBASE                  ; offset si to the real address
SIBASE: 
       POP     AX                      ; put address of SIBASE in ax
       ADD SI,AX           ; actual address of message

       CALL    PrintString     ; Display the message

       XOR AH,AH           ; Get a key
       INT 16h

       INT 19h                 ; Reboot with a good disk, hopefully

; Display the message pointed to by SI
PrintString:
       LODSB
       OR      AL,AL
       JZ  donePrinting
       MOV AH,0Eh
       MOV BX,0007h
       INT 10h
       JMP PrintString
donePrinting:	
       RET
	
NON_SYSTEM_DISK  db	'Non-System disk or disk error',13, 10
       db 'Replace and strike any key when ready',13, 10, 0

ProgramEnd	label	byte

_NoBootProgramSize dw	ProgramEnd - _NoBootProgram
;

END

