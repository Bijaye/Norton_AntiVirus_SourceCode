include CTSN.INC

IF bSYM_TSR OR bSYM_DOS
UniqueSegment
;-----------------------------------------------------------------------------
; $HEADER: $
;-----------------------------------------------------------------------------
; $LOG: $
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; MUS4_BEEP                     5/28/93                                 CAG
;
;       Play the NAV music.
;       Lifted from NAV 2.1
;
; void WINAPI MUS4_BEEP(BYTE BeepLevel);
;
; Input:
;       1) BeepLevel, see CTS4.INC for BEEP_xxx defines
; 
;-----------------------------------------------------------------------------
EXTRN   PASCAL HWIsNEC:FAR

MUS4_BEEP PROC USES XBX XCX XDX SDS XSI XDI,
        bType:BYTE

    pushf                               ; Windows tweek!
    call    HWIsNEC
    or      AX,AX                       ;Q: Is the machine a NEC?
    je      @f                          ; N: Jump
    mov     bIsNEC,1                    ; Y: Set flag
@@:
    xor     XBX, XBX
    mov     bl,bType
    mov     XSI,XBX
    cli                                 ; Windows tweek!
    call    SpeakerOn                   ;
    call    SetPort                     ;
    mov     XCX, 3
A21:
    push    XCX
    mov     XBX,BEEP_DURATION
    mov     XCX,400
    mov     XDI,200                     ; frequency for PC/AT
A22:
    call    SetTone
    call    Delay
    add     XDI, XSI
    loop    A22

    pop     XCX
    loop    A21

    call    SpeakerOff
    sti
    popf

	ret   
        
MUS4_BEEP ENDP
;-----------------------------------------------------------------------------
SetPort:				
    test    bIsNEC,1
    jz      @f                          ;not NEC
    call    SetPortNEC
    ret
@@:
    mov     AL, 0B6H                    ;Initialize channel 2 of
    out     43H,AL                      ;timer chip
    ret                                 
;-----------------------------------------------------------------------------
SetTone:				;
    test    bIsNEC,1
    jz      @f          ;not NEC
    call    SetToneNEC
    ret
@@:
    mov XDX,14H         ;Divide 1,331,000 Hz
    mov XAX,4F38H       ;by
    div DI              ;desired frequency in DI
    out 42H,AL          ;low byte of count to timer
    mov AL,AH           ;
    out 42H,AL          ;high byte of count to timer
    ret                 ;
;-----------------------------------------------------------------------------
SpeakerOn:				
    test    bIsNEC,1
    jz      @f                          ;not NEC
    call    SpeakerOnNEC
    ret
@@:
    push    es
    push    dx
    IN      AL,61H                      ;read byte at Port 61h
    mov     dx, SEG Port61
    mov     es, dx
    MOV     es:Port61,AL                ;save value at Port 61
    OR      AL,3                        ;set first two bits
    OUT     61H,AL                      ;turn speaker on
    pop     dx
    pop     es
    ret                 
;-----------------------------------------------------------------------------
Delay:
    push    XBX
    push    XCX

D10:
    xor     XCX, XCX
    xor     XAX, XAX
    mov     cl,03h
    mov     XAX,700                     ;Duration for 86/286/386 machines
    dec     XCX
    shl     XAX, cl
    mov     XCX, XAX
    LOOP    $
    DEC     XBX
    JNZ     D10
    pop     XCX
    pop     XBX

    ret
;-----------------------------------------------------------------------------
SpeakerOff:
    test    bIsNEC,1
    jz      @f                          ;not NEC
    call    SpeakerOffNEC
    ret
@@:
    push    es
    push    dx
    mov     dx, SEG Port61
    mov     es, dx
    MOV     AL,es:Port61
    OUT     61H,AL
    pop     dx
    pop     es
    ret
;-----------------------------------------------------------------------------
SetPortNEC:				;
    mov     al, 076h                    ; Prepare timer 1 (speaker) 
    mov	    dx,3fdfh                    ; for frequency code
    out     dx,al
    jmp     $+2
    ret                 
;-----------------------------------------------------------------------------
SetToneNEC:				;
    in	    al, 42h                     ; Read signal
    test    al, 00100000B
    jnz     is8m
    mov     dx, 25h                     ; MSW
    mov     ax, 8000h                   ; LSW (2,457,600)
    jmp	    short countset
is8m:
    mov     dx, 1Eh                     ; MSW
    mov     ax, 7800h                   ; LSW (1,996,800)
countset:
    div	    di			
    mov     dx, 3fdbh	
    out     dx, al
    jmp     $+2
    mov     al, ah                      ; Get high byte
    out     dx, al
    jmp     $+2

;-----------------------------------------------------------------------------

NEC_SPKR_MASK         equ 08h           ; 00001000b
NEC_SPKR_PORT         equ 035h

SpeakerOnNEC:				
    in      al, NEC_SPKR_PORT
    and     al, not NEC_SPKR_MASK       
    out     NEC_SPKR_PORT, al
    ret
;-----------------------------------------------------------------------------
SpeakerOffNEC:
    in      al, NEC_SPKR_PORT
    or      al, NEC_SPKR_MASK
    out     NEC_SPKR_PORT, al
    ret
;-----------------------------------------------------------------------------

UniqueSegmentEnd
;-----------------------------------------------------------------------------
UniqueDataSegment
Port61  byte    0
bIsNEC  byte    0
UniqueDataSegmentEnd
;-----------------------------------------------------------------------------
ENDIF
END



