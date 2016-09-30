
;  $Header:   S:/SYMKRNL/VCS/drvparam.asv   1.3   18 Mar 1998 11:43:32   SKURTZ  $

;  $Log:   S:/SYMKRNL/VCS/drvparam.asv  $
;  
;     Rev 1.3   18 Mar 1998 11:43:32   SKURTZ
;  Fix compiler errors because of new absdiskrec members
;
;     Rev 1.2   16 Dec 1996 15:03:18   SKURTZ
;  DeskGetPhyInfoNec not preserving DS register
;
;     Rev 1.1   19 Sep 1996 21:14:38   SPASNIK
;  Fix this without breaking cvtf...whatever...
;
;     Rev 1.0   26 Jan 1996 20:21:12   JREARDON
;  Initial revision.
;
;     Rev 1.0   11 Sep 1995 09:38:28   SKURTZ
;  Initial revision.
;
;
;    Rev 1.3   10 Jun 1993 15:56:30   Ryo
; Build 3 Modification
;
;    Rev 1.2   12 May 1993 19:40:50   Ryo
; Build 2 Modification
;
;    Rev 1.1   27 Apr 1993 15:22:30   Ryo
; Build 1 Exactry same as Original
;
;    Rev 1.0   22 Apr 1993 17:24:36   Ryo
; Initial revision.
;
;     Rev 1.0   20 Aug 1990 13:08:40   DAVID
;  Initial revision.

ifdef NCD
OPTION M510
endif


include struct.inc

;-----------------------------------------------------------------------------
;  _DiskGetPhysInfoNEC ()
;	Gets the drive parameters returned by Int 13h, function 08h.
;		This function will clobber the absDisk field, 'bufferAddress'
;		by having it point to the DDPT for a floppy disk.  Also
;		the CMOS_Type field is only valid for floppy disks.
;
; Byte PASCAL _DiskGetPhysicalInfo (AbsDiskRec *absDiskPtr);
;
; (C) Copyright Peter Norton, 1988-89
;
; 08/24/88 - Returns the number of hard disks (drive Num >= 0x80), or
;	     number of floppies (drive Num < 0x80) for newer PS/2 machines
; 05/22/88 - Created: Brad
;
;  Returns 0 if no error, else error code
;---------------------------------------------------------------------------

%	.MODEL	memoryModel, PASCAL


	.DATA
ifdef NCD

OPTION M510

NEC_STD_HD	label	byte
;	    Cylinders	Upre Byte is Numbre of heads
;               CX              DX              BIOS sence returns
	dw	152,		0421h	;  5 MB - xxxx0000B (0)
	dw	309,		0421h	; 10 MB - xxxx0001B (1)
        dw      0,              0       ; Dummy             (2)
	dw	307,		0821h	; 20 MB - xxxx0011B (3)
        dw      0,              0       ; Dummy             (4)
	dw	614,		0821h	; 40 MB - xxxx0101B (5)

public	NEC_STD_HD
endif

%	.CODE	segmentName


IFDEF SYM_DOSX
        .286
        EXTRN   GetProtModePtr:PROC
        EXTRN   SelectorFree:PROC
ENDIF




_DiskGetPhyInfoNEC	proc	uses bx cx dx si di es, absDiskPtr:PTR

public _DiskGetPhyInfoNEC

ifdef NCD
        push    ds                      ; LDS throttles DS (duh)
	lds	si, absDiskPtr		; Get ptr. to record
	mov	al, [si].dn		; Get physical drive number
	xor	cx, cx			; Clear a few registers
	mov	cx, 0FFFFh		; Set for later evaluation
	mov	ah, 84h			; Extended sense call
	int	1Bh			; Disk service handler
	jc	errorLabel		; If carry, then error

	test	Byte Ptr [si].dn, 01010000B	; drive number is HD ?
	jne	NECFloppie


; get the number of cylinders
	inc	cx			; get number of cylinder
	jnz	NECExtendHD		; NEC extended format HD.

	and     ah, 0Fh                 ; bit 3 - 0 has the size of unit
	shl     ah, 1
	shl     ah, 1
	mov	bl, ah
	sub	bh, bh                  ; parameter table index

;	mov	cx, NEC_STD_HD[bx]	; get Clinders
;	mov	dx, NEC_STD_HD[bx].2	; get Heads ans sector/track
	mov	cx, Word Ptr NEC_STD_HD[bx]	; get Clinders
	mov	dx, Word Ptr NEC_STD_HD[bx].2	; get Heads ans sector/track
	mov	bx, 256			; sector size
        or      cx, cx
	jz	errorLabel		; error in getting parameters

NECExtendHD:		; Save given HD. parameters
        mov     BYTE PTR [si].dwtotalHeads, dh     ; Save maximum heads
        mov     WORD PTR [si].dwtotalTracks, cx    ; Save maximum tracks
        mov     BYTE PTR [si].dwsectorsPerTrack, dl ; Save maximum sectors

	sub	ah, ah			; Sets default
	or	bh, bh			; sector size under 256 ?
	jz	setUnitMask		; skip test.
sectorLenLoop:
	inc	ah
	shr	bh, 1
	jnc	sectorLenLoop

setUnitMask:
	mov	[si].sectorLenCode, ah	; Sets Sector length code

;	mov	ax, 0300h		; HD unit mask
;	mov	ax, 0F00h		; HD unit mask // 1992.12.19 T.Sakakibara
	mov	ax, 0F7Fh		; HD unit mask // 1992.12.19 T.Sakakibara
	jmp	Short GetNumDrive

NECFloppie:
	mov	[si].CMOS_Type, 2	; Save CMOS drive type 1.2M
	mov	ax, 0F0FFh		; FD unit mask

; get the number of drives
GetNumDrive:
IFDEF SYM_DOSX
        push    es
        push    ax                      ;save mask

        push    0                       ;point down to 0:482 and 0:55Ch
        push    482h
        Call    GetProtModePtr
        mov     es,dx                   ;get ES:BX pointing to it

        mov     dx, Word Ptr es:[55Ch]          ;DISK_EQUIP Floppy & SASI
        mov     dl, Byte Ptr es:[482h]          ;DISK_EQUIPS SCSI

        Push    dx                      ;save the DISK_EQUIP value
        push    es
        Call    SelectorFree            ;free the selector
        pop     dx

        pop     ax                      ;restore mask.
        pop     es

ELSE
	push	ds
	sub	dx, dx
	mov	ds, dx
	mov	dx, Word Ptr ds:[55Ch]		;	DISK_EQUIP Floppy & SASI
	mov	dl, Byte Ptr ds:[482h]		;	DISK_EQUIPS SCSI
	pop	ds
ENDIF

	and	dx, ax
	sub	ax, ax
	mov	cx, 16				; Test all bits

getnumdrvLoop:
	shr	dx, 1
	adc	al, ah				; add avilable FD status
	loop	getnumdrvLoop


	mov	byte ptr [si].numDrives, al ; Save number of drives

	xor	ax, ax			; Signal NO ERROR
	jmp	short returnLabel	; Return to caller


errorLabel:
	and	ah, 0F0h		; Mask N/A bits
	cmp	ah, 60h			; NOT Ready ?
	jne	errorLabel01

	test	Byte Ptr [si].dn, 01010000B	; drive number is FD ?
	jnz	NECFloppie

errorLabel01:
;***    and	ah, 0F8h		; Mask N/A bits
	xchg	ah, al			; Reverse error code
	sub	ah, ah			; Clear high-byte
	or	ax, ax			; Is it 0?
	jnz	returnLabel		; No, return

	mov	ax, 1			; Return 'Invalid function'


returnLabel:
        pop     ds                      ; Restore caller's DS

endif

        ret


_DiskGetPhyInfoNEC	endp

	end
