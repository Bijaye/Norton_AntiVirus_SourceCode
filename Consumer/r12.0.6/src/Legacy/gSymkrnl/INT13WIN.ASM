	PAGE ,132

; Copyright 1988-1993 by Symatec Corporation.  All rights reserved.
;**********************************************************************
;
; $Header:   S:/SYMKRNL/VCS/INT13WIN.ASv   1.0   26 Jan 1996 20:21:12   JREARDON  $
;
; File: INT13WIN.ASM
;
; Description:
;    Routines used by DiskMapLogToPhyParams() to map logical drive info
;    to physical drive info.
;
; Contains:
;    void FAR PASCAL BIOSInstallInt13Handler(Int13Record far *int13PackagePtr);
;    void FAR PASCAL BIOSUninstallInt13Handler(void);
;    _Int13Handler
;
;**********************************************************************
; $Log:   S:/SYMKRNL/VCS/INT13WIN.ASv  $
;  
;     Rev 1.0   26 Jan 1996 20:21:12   JREARDON
;  Initial revision.
;  
;     Rev 1.8   17 Oct 1994 18:14:14   BRAD
;  Added NEC support
;  
;     Rev 1.7   11 May 1994 19:24:36   BRAD
;  Changes so we can use ML, not MASM
;  
;     Rev 1.6   10 Feb 1994 18:12:16   PGRAVES
;  Added changes from QAK2 branch to trunk.
;
;     Rev 1.5   10 Nov 1993 18:33:34   JOHN
;  Ported fixes from QAK2 branch.
;
;     Rev 1.2.5.5   10 Nov 1993 12:54:04   LCOHEN
;  Added logic to fail requests to floppy, but allow them to fixed disk.
;
;     Rev 1.2.5.1   03 Nov 1993 17:48:20   JOHN
;  (1) Changed from using CMACROS to standard MASM segmenting.
;  (2) Changed to fail all INT 13H's once we've been triggered.
;
;     Rev 1.2.5.0   19 Oct 1993 09:26:14   SCOTTP
;  Branch base for version QAK2
;
;     Rev 1.2   22 Feb 1993 21:27:26   JOHN
;  Ported change from Norton Library... No longer assumes that the
;  top two bits of the head number is an extension of the cylinder.
;  DRDOS and SpeedStor do this "head hack" to support drives with
;  more than 1024 cylinders attached to non-translating controllers.
;  The problem is that new SCSI controllers are using all 8 bits
;  of the head, to support drives over 1GB.  My opinion is that the
;  SCSI controllers are doing the right thing, and DRDOS and SpeedStor
;  are wrong.  IBM always documented the head number as an 8 bit field.
;  The INT 13H hook has been changed to just store what it sees in the
;  registers; DiskMapLogToPhyParams() is now responsible for determining
;  how the head number is being used.
;
;     Rev 1.1   12 Feb 1993 04:30:32   ENRIQUE
;  No change.
;
;     Rev 1.0   27 Aug 1992 10:33:32   ED
;  Initial revision.
;*********************************************************************

;; THIS IS THE WINDOWS VERSION OF THIS MODULE.  DO NOT USE UNDER DOS!!!!

	.MODEL	MEDIUM, PASCAL

;**********************************************************************
; DPMI register translation structure
;**********************************************************************

DPMIrec STRUC
    realModeEDI		dd	0
    realModeESI		dd	0
    realModeEBP		dd	0
    realModeReserved	dd	0
    realModeEBX		dd	0
    realModeEDX		dd	0
    realModeECX		dd	0
    realModeEAX		dd	0
    realModeFlags	dw	0
    realModeES		dw	0
    realModeDS		dw	0
    realModeFS		dw	0
    realModeGS		dw	0
    realModeIP		dw	0
    realModeCS		dw	0
    realModeSP		dw	0
    realModeSS		dw	0
DPMIrec	ENDS

;**********************************************************************
; Int 13h handler information package.
; Make sure this object is kept in synch with the 'C' definition.
;**********************************************************************

Int13Record STRUC
    generated		db	?	; Was Int 13h generated?
    driveNum		db	?	; Drive number
    startHead		db	?	; Starting head
    startTrack		dw	?	; Starting track
    startSector		db	?	; Starting sector
    trigger		db	?	; Time to take special int 13h action?
    sectorLenCode	db	?	; Sector length code (NEC only)
Int13Record ENDS

;**********************************************************************
; Name the code segment according the Norton conventions
;**********************************************************************

	.CODE	_INT13WIN

extern          HWISNEC:FAR

;**********************************************************************
; These variables go in the CODE segment so they can be accessed by
; _Int13Handler
;**********************************************************************

preCommand	db	0	; Flag to indicate detection of Disk Manager
				; Pre-command for cylinders >= 1024

origInt13Seg	dw	0	; SEGMENT addr of old real mode int 13 handler
origInt13Off	dw	0	; OFFSET  addr of old real mode int 13 handler

newInt13Seg	dw	0	; SEGMENT addr of new real mode int 13 callback
newInt13Off	dw	0	; OFFSET  addr of new real mode int 13 callback

csAlias		dw	0	; SELECTOR to code seg with write access

wHWIsNEC        dw      0       ; Is this an NEC computer?

packageAddr	label	dword
packageOff	dw	0	; OFFSET   addr of Int 13h package
packageSel	dw	0	; SELECTOR addr of Int 13h package

realModeCallStruc DPMIrec <>	; DPMI register translation record

PAGE
;**********************************************************************
; BIOSInstallInt13Handler()
;
; Description:
;	Called to prepare for logical to physical drive mapping.
;	The physical attributes of the drive are passed back in a record
;	called 'Int13Record'  It will return the drive number, starting
;	head, starting track, starting sector, and whether the drive ever
;	generated an INT 13h.
;
;	THIS CODE ASSUMES IT IS RUNNING IN PROTECTED MODE.
;
;**********************************************************************

BIOSINSTALLINT13HANDLER	proc    far uses ds es di si, packagePtr:DWORD

	;-----------------------------------------------
	; Create a code segment alias so new handler can
	; modify the DPMI real mode call structure.
	; We also need it to modify our CS variables.
	; Save the alias so UnInstall routine can free it.
	;-----------------------------------------------

	mov	ax, 000Ah		; Create Code Segment Alias Descriptor
	mov	bx, cs			; BX = Code segment selector
	int	31h			; DPMI Memory Services
	mov	ds, ax			; AX = New data selector
	ASSUME	DS:_INT13WIN
	mov	csAlias,ax

	;-----------------------------------------------
        ; Is this an NEC computer?
	;-----------------------------------------------
        call    HWISNEC
        mov     wHWIsNEC, ax

	;-----------------------------------------------
	; Get pointer to Int 13h packet record.
	; Save in CS so new Int 13h handler can access it.
	; Initialize packet and other global variables.
	;-----------------------------------------------

	les	di,packagePtr		; ES:DI = Sel:Offset of package
	mov	packageSel, es		; Save addr in CS vars
	mov	packageOff, di		;

	mov	es:[di].Int13Record.generated, 0	; Mark as not generated, yet

	mov	es:[di].Int13Record.trigger, 0	; Don't take action yet, because
					; Windows will generate Int 13s
					; before we issue our dummy read
					; request.

	mov	BYTE PTR preCommand, 0	; Mark as no pre-command found

	;-----------------------------------------------
	; Get current Int 13h real mode SEG:OFFSET.
	; Store in CS variables so new handler can use
	; them to chain to old handler.
	;-----------------------------------------------

	mov	ax, 0200h		; Get Real Mode Interrupt Vector
	mov	bl, 13h			; BL = Interrupt number
        cmp     wHWIsNEC, 0             ; Is this an NEC?
        je      @F                      ; No, skip
	mov	bl, 1bh			; BL = Interrupt number
@@:
	int	31h			; DPMI Memory Services
	mov	origInt13Seg, cx	; CX = Segment of real mode int handler
	mov	origInt13Off, dx	; DX = Offset  of real mode int handler

	;-----------------------------------------------
	; Allocate a real mode call-back.
        ; This routine will switch to protected mode and
	; and call our _Int13Handler hook routine.
	; Save call-back address so it can be freed later.
	;-----------------------------------------------

	mov	ax, 0303h		; Allocate Real Mode Call-Back Address
	push	ds
	push	ds
	mov	bx, cs
	mov	ds, bx
	mov	si, OFFSET _Int13Handler	; DS:SI=Sel:Off of proc to call
	pop	es				; ES = writable Sel of code seg
	mov	di, OFFSET realModeCallStruc	; ES:DI=Sel:Off of call struct
	int	31h				; DPMI Memory Services
	pop	ds
	mov	newInt13Seg, cx		; CX = Segment of allocated call-back
	mov	newInt13Off, dx		; DX = Offset  of allocated call-back

	;-----------------------------------------------
	; Hook the real mode int 13h with the allocated
	; call-back address
	;-----------------------------------------------

	mov	ax, 0201h		; Set Real Mode Interrupt Vector
	mov	bl, 13h			; BL = Interrupt number, CX:DX = callback
        cmp     wHWIsNEC, 0             ; Is this an NEC?
        je      @F                      ; No, skip

	mov	es:[di].DPMIrec.realModeDS, cx
	mov	word ptr es:[di].DPMIrec.realModeEDX, dx
	mov	word ptr es:[di].DPMIrec.realModeEDX+2,	dx

	mov	ax,	0300h		; Emurate Real Mode Interrupt
	mov	bx, 21h			; BL = Interrupt number BH = A20 Line
	mov	cx, 0h			; Num copysize
	mov	word ptr es:[di].DPMIrec.realModeEAX, 251bh	; Set AH = 25h, AL = 1Bh
	mov	word ptr es:[di].DPMIrec.realModeEAX+2,	251bh

@@:
	int	31h			; DPMI Memory Services

locRet:
	ret

BIOSINSTALLINT13HANDLER	endp


PAGE
;*********************************************************************
; BIOSUninstallInt13Handler()
;
; Description:
;	Counterpart to (you guessed it) BIOSInstallInt13Handler()
;
;*********************************************************************

BIOSUNINSTALLINT13HANDLER	proc	far uses ds di si

	;-----------------------------------------------
	; Retrieve the code segment alias.
	;-----------------------------------------------

	mov	ax,cs:csAlias
	mov	ds,ax
	ASSUME	DS:_INT13WIN

	;-----------------------------------------------
	; Put back the original Int 13h real mode handler.
	;-----------------------------------------------

	mov	ax, 0201h		; Set Real Mode Interrupt Vector
	mov	bl, 13h			; BL = Interrupt number
	mov	cx, origInt13Seg	; CX = Segment of real mode int handler
	mov	dx, origInt13Off	; DX = Offset  of real mode int handler
        cmp     wHWIsNEC, 0             ; Is this an NEC?
        je      @F                      ; No, skip

	mov	bl, 1bh			; BL = Interrupt number

	push	ds
	pop	es
	mov	di, OFFSET realModeCallStruc ; ES:DI=Sel:Off of call struct	 
	mov	cx, origInt13Seg	; CX = Segment of real mode int handler
	mov	dx, origInt13Off	; DX = Offset  of real mode int handler
	mov	es:[di].DPMIrec.realModeDS, cx
	mov	word ptr es:[di].DPMIrec.realModeEDX, dx
	mov	word ptr es:[di].DPMIrec.realModeEDX+2,	dx

	mov	ax,	0300h		; Emurate Real Mode Interrupt
	mov	bx, 21h			; BL = Interrupt number BH = A20 Line
	mov	cx, 0h			; Num copysize
	mov	word ptr es:[di].DPMIrec.realModeEAX, 251bh	; Set AH = 25h, AL = 1Bh
	mov	word ptr es:[di].DPMIrec.realModeEAX+2,	251bh

@@:
	int	31h			; DPMI Memory Services

	;-----------------------------------------------
	; Free the real mode call-back routine.
	;-----------------------------------------------

	mov	ax, 0304h		; Free Real Mode Call-Back Address
	mov	cx, newInt13Seg		; CX = Segment of allocated call-back
	mov	dx, newInt13Off		; DX = Offset  of allocated call-back
	int	31h			; DPMI Memory Services

	;-----------------------------------------------
	; Free the code segment alias.
	;-----------------------------------------------

	mov	ax,0001h		; Free LDT Descriptor
	mov	bx,ds			; BX = Selector to free
	int	31h			; DPMI Memory Services

locRet2:
	ret

BIOSUNINSTALLINT13HANDLER endp

PAGE
;*********************************************************************
; _Int13Handler
;
; Description:
;	This is the protected mode Int 31h handler that is called by
;	the real mode call-back routine allocated using DPMI services.
;
;	Used when determining the mapping from a logical device
;	to a physical one.
;
; Entry:
;	DS:SI -> Real mode SS:SP
;	ES:DI -> Real mode call structure
;	SS:SP -> locked protected mode API stack
;	All other registers undefined
;	Interrupts disabled
;
; Exit:
;	ES:DI -> Real mode call structure
;
;*********************************************************************

	ASSUME CS:_INT13WIN
	ASSUME DS:_INT13WIN

_Int13Handler:

	;-----------------------------------------------
	; See if we have already got the values we need.
	; If we do, then don't save the values--just exit.
	;-----------------------------------------------

	push	bx			; Save BX
	push	ds			; Save DS
	mov	bx,cs			;
	mov	ds,bx			; DS = code segment
	lds	bx,packageAddr		; DS:BX = far ptr to package
	cmp	[bx].Int13Record.trigger, 0		; If 0, must be Windows I/O request
	jne	keepChecking		; Else check if we need to act on it
	pop	ds			; Restore DS
	pop	bx			; Restore BX
	jmp	short callOldHandler	; Just pass request through

keepChecking:
	cmp	[bx].Int13Record.generated, 1	; Have we already generated ourselves?
	pop	ds			; Restore DS
	pop	bx			; Restore BX
        jne     short checkCmd          ; No, continue checking

        cmp     cs:wHWIsNEC, 0          ; Is this an NEC?
        jne     @F                      ; Yes, skip

        cmp     dl, 80h                 ;Q: Is it a fixed disk?
        jae     short callOldHandler    ; Y: allow the I/O to proceed
        jmp     returnError             ; N: fail all floppy access

@@:
        push    ax
        mov     ax, word ptr es:[di].DPMIrec.realModeEAX
        test    al, 50h
        pop     ax
        jz      callOldHandler
        jmp     returnError             ; N: fail all floppy access


checkCmd:
        cmp     cs:wHWIsNEC, 0          ; Is this an NEC?
        jne     @F                      ; Yes, skip

	cmp	ah, 2			; Read command?
	je	processIt		; Yes, capture the input values

	;-----------------------------------------------
	; If command is a Disk Manager pre-command for
	; a cylinder > 1023, then just flag for later
	; processing in the next read request and pass
	; it through.
	;-----------------------------------------------

	cmp	ah, 0EEh		; Is it Disk Manager Pre-command?
	jne	callOldHandler		; No, call old handler

        push    ds
        mov     ax,cs:csAlias
        mov     ds,ax
	mov	byte ptr preCommand, 1	; Yes, mark Pre-Command found
        pop     ds

        jmp     short callOldHandler

@@:
	push	ax
	mov	ax, word ptr es:[di].DPMIrec.realModeEAX
	and	ah, 0fh
	cmp	ah, 6							; A read command ?
	pop	ax
        je      processIt

	;-----------------------------------------------
	; Chain to original real mode Int 13h handler
	; by replacing the real mode CS:IP with the
	; original handler's Seg:Offset
	;-----------------------------------------------

callOldHandler:
	mov	ax, cs:origInt13Seg	; move segment to real mode CS
	mov	es:[di].DPMIrec.realModeCS, ax
	mov	ax, cs:origInt13Off	; move offset to real mode IP
	mov	es:[di].DPMIrec.realModeIP, ax
	iret

	;-----------------------------------------------
	; Process a read command by saving the Int 13h
	; entry values we need into the Int 13h packet
	; record and falling through to the error return
	; code.
	;-----------------------------------------------

processIt:
	push	ax
	push	bx
	push	cx
	push	dx
	push	ds

	mov	bx,cs			;
	mov	ds,bx			; DS = code segment
	lds	bx,packageAddr		; DS:BX = far ptr to package

	mov	[bx].Int13Record.generated, 1	; Signal TRUE

        cmp     cs:wHWIsNEC, 0          ; Is this an NEC?
        jne     @F                      ; Yes, skip

	mov	[bx].Int13Record.driveNum, dl	; Save physical drive number
	mov	[bx].Int13Record.startHead, dh	; Save head number

	mov	ax, cx			; Get track/sector combo
	rol	al, 1			;
	rol	al, 1			;
	and	al, 3			; Remove 2 bits at hi-end
	xchg	al, ah			; Swap them

	cmp	cs:[preCommand], 1	; Pre-command specified?
	jne	noPreCommand		; No, skip it
	add	ax, 1024		; Add 1024 cylinders to cylinder count
noPreCommand:

	mov	[bx].Int13Record.startTrack, ax	; Save track number

	and	cl, 3Fh			; Remove 2 hi-bits
	mov	[bx].Int13Record.startSector, cl	; Save starting sector number
        jmp     short popReg

@@:
	mov	dx, word ptr es:[di].DPMIrec.realModeEDX
	mov	cx, word ptr es:[di].DPMIrec.realModeECX
	mov	ax, word ptr es:[di].DPMIrec.realModeEAX
        mov     [bx].Int13Record.driveNum, al    ; Save physical drive number
        mov     [bx].Int13Record.startHead, dh   ; Save head number
        mov     [bx].Int13Record.startSector, dl ; Save starting sector number
        mov     [bx].Int13Record.sectorLenCode, ch ; Save sector length
	test	al, 50h				; HD ?
	jz	hd00

	xor	ch, ch
hd00:
        mov     [bx].Int13Record.startTrack, cx  ; Save track number

popReg:
	pop	ds
	pop	dx
	pop	cx
	pop	bx
	pop	ax

        ;-----------------------------------------------------------------------
        ; If we fail this call, the block device driver will retry and we will
        ; let the second attempt succeed???  Why not allow the first one to
        ; succeed and save a second attempt by the block device driver!
        ;
        ; On some machines, a reset to the secondary drive on the IDE controller
        ; causes a long reset operation.  Why?????  Therefore, we will allow the
        ; request to go on through if it is a fixed disk.
        ;-----------------------------------------------------------------------
        cmp     dl, 80h                 ;Q: Is this a fixed disk?
        jae     short callOldHandler    ; Y: do the I/O

	;-----------------------------------------------
	; Return from the Int 13h with the real mode
	; carry flag set to indicate an error, and with
	; the real mode AX set to 100h to indicate an
	; invalid function request.
	; The error prevents us from spinning up the disk
	; motors when all we want is mapping information.
	; We must simulate an iret on the real mode stack
	; and then do a real iret to get back to the
	; allocated DPMI call-back.
	;-----------------------------------------------

returnError:
	cld				; Prepare for lodsw's
	lodsw				; Get real mode return address IP
	mov	es:[di].DPMIrec.realModeIP,ax	; Move it from stack to real mode reg
	lodsw				; Get real mode return address CS
	mov	es:[di].DPMIrec.realModeCS,ax	; Move it from stack to real mode reg
	lodsw				; Get real mode flags
	or	ax, 1			; Set the CARRY to indicate an error
	mov	es:[di].DPMIrec.realModeFlags,ax; Move it from stack to real mode reg
	add	es:[di].DPMIrec.realModeSP,6	; Popped six bytes from stack
	mov	WORD PTR es:[di].DPMIrec.realModeEAX,0100h     ; Set AX = 100h

        cmp     cs:wHWIsNEC, 0          ; Is this an NEC?
        je      @F                      ; No, skip

	mov	WORD PTR es:[di].DPMIrec.realModeEAX, 6000h     ; Set AX = 6000h
@@:
	iret

;*********************************************************************

	end

