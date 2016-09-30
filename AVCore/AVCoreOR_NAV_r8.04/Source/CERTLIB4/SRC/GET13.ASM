include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_ORIG13                      1/19/93                     MRH/CAG
;
;       Returns the original interrupt 13h address.
;
; DWORD WINAPI SYS4_GET_ORIG13h(WORD ROM_START, WORD ROM_END);
;
; Input:
;	1)  ROM_START, lowest address of ROM to look in (include)
;	2)  ROM_END, highest address of ROM to look in (include)
; Output:
;       1)  Address of original INT 13h.
;       2)  ERROR (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       If thru sneaky methods we can't get the orig 13, then we'll
;       return 0.  The caller is then responsible to do a regular
;       get of INT 13h
;
;-----------------------------------------------------------------------------
ISR01	LABEL	WORD
	cli				; Disable all hardware interrupts
	cmp	cs:InMe,TRUE
	je	NoGo
	mov	cs:InMe,TRUE	

        mov  	cs:debug_ax,ax		; Save AX and DX in memory.
        mov     cs:debug_dx,dx

        pop     ax			; Get IP from stack.
        pop     dx			; Get CS from stack.
        push    dx			; Put back CS
        push    ax			; Put back IP	

        cmp     dx,cs:CSRomStart
	jb	NoGo
	cmp	dx,cs:CSRomEnd
	ja	NoGo

        pop     ax			; Get IP from stack
	mov	cs:Off13,ax		; Store this value
	pop	dx			; Get CS from stack
	mov	cs:Seg13,dx		; Store this value
	pop	ax			; Get flags from stack
	and 	ax,0FEFFh		; Clear DEBUG
	push	ax			; Store flags on stack
	push	dx			; Store CS on stack
	mov	ax,cs:Off13
	push	ax			; Store IP on stack
	mov	cs:InMe,FALSE
NoGo:
        mov     ax,cs:debug_ax
        mov     dx,cs:debug_dx
	sti				; Turn on hardware interrupts
        iret
;-----------------------------------------------------------------------------
InMe		db	FALSE
Seg13		dw	0
Off13		dw	0

debug_ax	dw	?
debug_dx	dw	?

CSRomStart	dw	?
CSRomEnd	dw	?
;-----------------------------------------------------------------------------
SYS4_GET_ORIG13 PROC USES XBX XCX,
	ROM_START:WORD,
	ROM_END:WORD
	
	LOCAL	lpAddr13:DWORD
	LOCAL	PrevISR01:DWORD

        mov	ax,ROM_START
        mov	cs:CSRomStart,ax
        mov	ax,ROM_END
        mov	cs:CSRomEnd,dx

	mov	cs:Seg13,word ptr 0
	mov	cs:Off13,word ptr 0

	INVOKE	SYS4_VECTOR,01h,ADDR ISR01,ADDR PrevISR01
	INVOKE	SYS4_GET_VECTOR,013h
        mov	word ptr lpAddr13,ax
	mov	word ptr lpAddr13+2,dx

	pushf				; Set the DEBUG flag
	pop	ax
	or	ax,0100h
	push	ax
	popf	

	mov	ah,0			; Reset drive 80h
	mov	dl,80h
	pushf
	call	dword ptr lpAddr13

	pushf				; Clear the debug flag
	pop	ax
	and	ax,0FEFFh
	push	ax
	popf

	INVOKE	SYS4_UNVECTOR,01h,ADDR ISR01,ADDR PrevISR01

        mov	dx,cs:Seg13
        mov	ax,cs:Off13
        ret

SYS4_GET_ORIG13 ENDP
;-----------------------------------------------------------------------------
END
