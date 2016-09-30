

;----------------------------------------------------------------------------
; DiskAbsOperation (command_code, absDiskRecordPtr)
;   performs the absolute sector operation specified by 'command_code'
;	using the information in the record ptr., 'absDiskRecordPtr'
;
;	command_code can be any of the following:
;		RESET_COMMAND
;		READ_COMMAND
;		WRITE_COMMAND
;		VERIFY_COMMAND
;		FORMAT_COMMAND
;
; DiskAbsOperation (Word commandCode, AbsDiskRec *absDiskPtr)
;
; returns the error number if an error occurs, else 0
;
;--------------------------------------------------------------------------
;
;  PC generic
;
; (C) Copyright Peter Norton, 1988
;
; 06/09/90	Brad: Added Disk Manager and SpeedStor reading of
;		cylinders > 1024.  This is 'dead' code, since in
;		this version, we won't allow the user to go beyond
;		1024 cylinders.
; 06/26/88	Brad: save number of sectors processed.
; 05/19/88	Brad: created.
;---------------------------------------------------------------------------

include types.inc
include struct.inc

%	.MODEL	memoryModel, C


RESET_COMMAND	equ	03h		; Initialize command code
READ_COMMAND	equ	06h		; Read command code
WRITE_COMMAND	equ	05h		; Write command code
VERIFY_COMMAND	equ	01h		; Verify command code
FORMAT_COMMAND	equ	0Dh		; Format command code
RECALIB_COMMAND	equ	07h		; Recalibrate command code
SENSE_COMMAND	equ	04h		; Sence command code


%	.CODE	segmentName


_DiskAbsOperationNEC	PROC FAR   uses bx cx dx si di es, \
				absDiskRecordPtr: FAR PTR, commandCode:BYTE

public _DiskAbsOperationNEC

	push	ds

	;--------------------------------------
	; Address abs. disk info table
	;--------------------------------------

	lds	si, absDiskRecordPtr	; Get ptr. to abs disk record

	;--------------------------------------
	; Set up move data length
	;    by 'absDisk.sectorCode' for NEC's HD and Floppy
	;--------------------------------------
	sub	bx, bx
	mov	bl, [si].AbsDiskRecord.numSectors	    ; Get number of sectors
	mov	cl, [si].AbsDiskRecord.sectorLenCode	; sector length code	(N)
	mov	ax, 128
	shl	ax, cl			; Get sector length in ax
	mul	bx			; Mul. number of sector
	mov	bx, ax			; Set move data length


	;--------------------------------------
	; Set up drive number and head number
	;--------------------------------------
	mov	al, [si].AbsDiskRecord.dn		; Get drive number
	test	al, 01010000B		; Is this DA HD relative address ?
	jz	isHDDrive		; If Yes, cx is low word of
						;	The relative address.
	;--------------------------------------
	; Set up sector coordination Floppy
	;--------------------------------------

	mov	al, commandCode		; Get command
	and	al, 0Fh
	cmp	al, FORMAT_COMMAND
	jne	SetupIDR

	sub	bx, bx
	mov	bl, [si].AbsDiskRecord.numSectors	; Get ID data length for Format

SetupIDR:
        mov     dh, Byte Ptr [si].AbsDiskRecord.dwhead    ; Get head number       (H)
        mov     dl, Byte Ptr [si].AbsDiskRecord.dwsector  ; Get sector number     (R)
	mov	ch, Byte Ptr [si].AbsDiskRecord.sectorLenCode	; sector length code	(N)
        mov     cl, Byte Ptr [si].AbsDiskRecord.dwtrack   ; Get clinder number    (C)
	jmp	Short DoOperation

	;--------------------------------------
	; Set up sector coordination HD
	;--------------------------------------
isHDDrive:
        mov     cx, Word ptr[si].AbsDiskRecord.dwtrack            ; clinder or Low Word of relative address.
        mov     dl, Byte ptr[si].AbsDiskRecord.dwsector           ; sector or Hi Byte of relative address.
        mov     dh, Byte ptr[si].AbsDiskRecord.dwhead             ; Get head number       (H)

	dec dl

	mov	ah, commandCode		; Get command
	and	ah,0fh			; mask high
	cmp	ah,0dh			; format Drive
	je	DoOperation		;
	cmp	ah,04h			; SENSE
	je	DoOperation		;
	cmp	ah,RESET_COMMAND	; if RESET TO HD Ctrl
	jne	@f			; Then Change to RECAB
	mov	ah,RECALIB_COMMAND
;;	and	ah,NOT 020h		; if reset set retry-bits-off
@@:
	xor	ah,020h			; retry-bits
	mov	commandCode,ah

DoOperation:
	;--------------------------------------
	; Set up DA/UA and command code
	;--------------------------------------
	mov	al, [si].AbsDiskRecord.dn			; Get drive number
	mov	ah, commandCode	; Get command
	;--------------------------------------
	; Set up transfer buffer
	;--------------------------------------
	push	bp
	les	bp, Dword ptr [si].AbsDiskRecord.bufferAddress ; Get segment of buffer
	int	1Bh				; Call disk BIOS routine
	pop	bp
	pushf

	lds	si, absDiskRecordPtr 		; Get ptr. to abs disk record
	mov	al, [si].AbsDiskRecord.numSectors	  	; Get count of sectors
	mov	[si].AbsDiskRecord.sectorsProcessed, al 	; Save number of sectors processed
	;--------------------------------------
	; Set up Error/result code
	;--------------------------------------
	and	ah, 0F8h			; Mask N/A bits
	popf
	jc	Exit

	mov	al, commandCode	; Get command
	and	al, 0Fh
	cmp	al, SENSE_COMMAND
	je	Exit

	sub	ah, ah			; Sets no Error status
Exit:
	sub	al, al
	xchg	ah, al			; Put error code in low-byte

	pop     ds

	ret

_DiskAbsOperationNEC	endp

	end
