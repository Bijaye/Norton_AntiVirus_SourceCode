include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $HEADER: $
;-----------------------------------------------------------------------------
; $LOG: $
;-----------------------------------------------------------------------------
; SYS4_GET_CPU_TYPE
;       This routine determines the CPU type and sets CPU_Type.
;
; Returns:
;	AL = 1 ==> 8086 CPU
;	AL = 2 ==> 80286 CPU
;	AL = 3 ==> 80386/80486 CPU
;
;       Lifted from NAV 2.1
;-----------------------------------------------------------------------------
SYS4_GET_CPU_TYPE PROC

	push	bx
	mov	bx,0f000h
	pushf
	sub	ax,ax
	push	ax			;Move 0 into the flags register
	popf
	pushf
	pop	ax
	and	ax,bx			;If the top four bits are set...
	cmp	ax,bx			;
	je	CPU_86			;it is 8086/88 CPU

	mov	ax,bx
	push	ax		
	popf				;Move 0F000h into the flags register
	pushf
	pop	ax
	and	ax,bx			;if the top four bits aren't set...
	jz	CPU_286			;it is an 80286 CPU
	
	inc	ax			;Fall thru to 386 CPU types, al = 3
CPU_286:				;
	inc	ax			;al = 2, it is an 80286 CPU
CPU_86:
	inc	ax			;al = 1, it is 8086/88 CPU
	popf
	pop	bx
	ret

SYS4_GET_CPU_TYPE	endp
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
