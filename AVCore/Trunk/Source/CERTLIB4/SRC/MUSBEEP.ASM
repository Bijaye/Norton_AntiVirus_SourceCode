include CTS4.INC

UniqueSegment
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
MUS4_BEEP PROC USES XBX XCX XDX SDS XSI XDI,
        bType:BYTE

	pushf				;Windows tweek!

	xor	bx,bx
	mov     bl,bType
	mov	si,bx
	cli				;Windows tweek!
	call	SpeakerOn		;
	call	SetPort			;
	mov	cx,3			;
A21:	push	cx			;
	mov	bx,BEEP_DURATION	;
	mov	cx,400			;
	mov	di,200			;
A22:	call	SetTone			;
	call	Delay			;
	add	di,si			;
	loop	A22			;
	pop	cx			;
	loop	A21			;
	call	SpeakerOff		;
					;
	sti
	popf				;

	ret   
        
MUS4_BEEP ENDP
;-----------------------------------------------------------------------------
SetPort:				;
	MOV	AL,0B6H			;Initialize channel 2 of
	OUT	43H,AL			;timer chip
	ret				;
;-----------------------------------------------------------------------------
SetTone:				;
	MOV	DX,14H			;Divide 1,331,000 Hz
	MOV	AX,4F38H		;by
	DIV	DI			;desired frequency in DI
	OUT	42H,AL			;low byte of count to timer
	MOV	AL,AH			;
	OUT	42H,AL			;high byte of count to timer
	ret				;
;-----------------------------------------------------------------------------
SpeakerOn:				;
	IN	AL,61H			;read byte at Port 61h
	MOV	Port61,AL		;save value at Port 61
	OR	AL,3			;set first two bits
	OUT	61H,AL			;turn speaker on
	ret				;
;-----------------------------------------------------------------------------
Delay:	push	bx			;
	push	cx			;

D10:
        xor     cx,cx
        INVOKE  SYS4_GET_CPU_TYPE
        mov     cl,al
	mov	ax,700			;Duration for 86/286/386 machines
	dec	cx
	shl	ax,cl
	mov	cx,ax
	LOOP	$			;
	DEC	BX			;
	JNZ	D10			;
	pop	cx			;
	pop	bx			;
	ret				;
;-----------------------------------------------------------------------------
SpeakerOff:				;
	MOV	AL,Port61		;
	OUT	61H,AL			;
	ret				;
;-----------------------------------------------------------------------------
ifdef WIN16
.data
endif
Port61  byte    0
;-----------------------------------------------------------------------------
END
