;* Copyright 1993 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/INT13DOS.ASv   1.4   18 Mar 1998 11:53:58   SKURTZ  $  *
;*                                                                      *
;* Description:                                                         *
;*     Called to map a logical drive to a physical drive.  The physical *
;*     attributes of the drive are passed back in a record called       *
;*     'Int13Record'  It will return the drive number, starting head,   *
;*     starting track, starting sector, and whether the drive ever      *
;*     generated an INT 13h.                                            *
;*                                                                      *
;* Contains:                                                            *
;*      BIOSInstallInt13Handler                                         *
;*      BIOSUnInstallInt13Handler                                       *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/INT13DOS.ASv  $  *
;  
;     Rev 1.4   18 Mar 1998 11:53:58   SKURTZ
;  Same fix. Different platform
;
;     Rev 1.3   18 Mar 1998 11:43:30   SKURTZ
;  Fix compiler errors because of new absdiskrec members
;
;     Rev 1.2   11 Mar 1998 15:05:42   SKURTZ
;  Numerous changes for Extended Int13 support
;
;     Rev 1.1   15 Oct 1996 12:30:42   SKURTZ
;  Selectively merging in changes from Quake C
;
;     Rev 1.0   26 Jan 1996 20:21:32   JREARDON
;  Initial revision.
;
;     Rev 1.15   02 Aug 1995 01:02:12   AWELCH
;  Merge changes from Quake 7.
;
;     Rev 1.14.2.2   24 May 1995 21:29:10   TORSTEN
;  Fix to allow the INT13 handler to be overlayed with .RTLink/Plus.
;
;     Rev 1.14.2.1   12 May 1995 20:30:30   TORSTEN
;  Fixed problem of not loading DS register correctly, made sure that
;  multiple BIOSInstallINT13Handler() calls can be nested correctly.
;
;     Rev 1.14.2.0   24 Apr 1995 19:32:54   SCOTTP
;  Branch base for version QAK7
;
;     Rev 1.14   27 Dec 1994 13:41:54   SKURTZ
;  DX work. No longer using BIOSINSTALLINT13HANDLER under DX platform.
;  Instead, the int 13 handler code (at the end of this file) is copied to real
;  mode memory.  See dsk_int.c for the install/uninstall code for this.
;
;     Rev 1.13   09 Jun 1994 12:31:54   MARKK
;  Removed .DATA? command
;
;     Rev 1.12   01 Jun 1994 09:17:40   MARKK
;  Moved the int13 handler to C for DOSX
;
;     Rev 1.11   11 May 1994 19:24:30   BRAD
;  Changes so we can use ML, not MASM
;
;     Rev 1.10   09 May 1994 13:18:54   MARKK
;  Reverted to orgininal code, I don't know why it seems to work now!
;
;     Rev 1.9   09 May 1994 10:42:08   MARKK
;  DOSX work
;
;     Rev 1.8   06 May 1994 18:10:48   MARKK
;  DOSX work
;
;     Rev 1.7   05 May 1994 09:20:46   MARKK
;  DOSX work
;
;     Rev 1.6   05 May 1994 05:15:50   MARKK
;  DOSX work
;
;     Rev 1.5   29 Apr 1994 13:18:22   MARKK
;  DOSX work
;
;     Rev 1.4   02 Aug 1993 12:31:40   JOHN
;  [Fix] INT 13H hook was trashing BX -- now it doesn't.
;
;     Rev 1.3   22 Feb 1993 21:21:22   JOHN
;  Ported change from Norton Library... No longer assumes that the
;  top two bits of the head number is an extension of the cylinder.
;  DRDOS and SpeedStor do this "head hack" to support drives with
;  more than 1024 cylinders attached to non-translating controllers.
;  The problem is that new SCSI controllers are using all 8 bits
;  of the head, to support drives over 1GB.  My opinion is that the
;  SCSI controllers are doing the right thing, and DRDOS and SpeedStor
;  are wrong.  IBM always document the head number as being an 8 bit
;  field.  The INT 13H hook has been changed to just store what it
;  sees in the registers; DiskMapLogToPhyParams() is now responsible
;  for determining how the head number is being used.
;
;     Rev 1.2   12 Feb 1993 04:30:28   ENRIQUE
;  No change.
;
;     Rev 1.1   15 Dec 1992 22:39:16   JOHN
;  Changed to take a far pointer to the INT13REC.  The prototype in
;  DISK.H already says the function takes a far pointer, and the Windows
;  version of the code works that way.  The mismatch between DISK.H and
;  the DOS code was causing all sorts of fun on the stack when callers
;  pushed four bytes of arguments and the function only cleared two.
;
;     Rev 1.0   27 Aug 1992 10:33:32   ED
;  Initial revision.
;************************************************************************

;; THIS IS THE DOS VERSION OF THIS MODULE.  DO NOT USE UNDER WINDOWS!!!!

	include	struct.inc
	include	types.inc

	.MODEL	LARGE, PASCAL

IFDEF   NCD
DISKBIOS_VECT	equ	1Bh
ELSE
DISKBIOS_VECT	equ	13h
ENDIF


	.DATA

IFNDEF   SYM_DOSX
_preCommand	db	0		; Disk Manager Pre-command (1024 cylinders)
_packageAddress	dd	?		; Address of INT 13h package
ENDIF


%       .CODE segmentName

IFDEF	SYM_DOSX
	.286
ENDIF


IFNDEF	SYM_DOSX

assume ds:NOTHING

_int13Installed	db	0		; Not installed yet
_oldInt13Handler	dd	?		; Keep this in CS


;----------------------------------------------------------------------

BIOSINSTALLINT13HANDLER	proc    far uses ds es di si, packagePtr:DWORD

	;------------------------------------------
	; Get pointer to Int 13h record
	;------------------------------------------

	les   	bx,packagePtr		; get address of Int 13h record
	mov	word ptr [_packageAddress+0], bx	; Save ptr. to package
	mov	word ptr [_packageAddress+2], es	; Save ptr. to package

	mov	es:[bx].Int13Record.generated,FALSE	; Mark as not generated, yet
	mov	_preCommand, FALSE	; Mark as no pre-command

	cmp	cs:_int13Installed, 0	; Are we already installed?
	jnz	locRet			; Yes, don't install again

	;----------------------------------
	; save old Int 13h handler
	;----------------------------------


	inc	cs:_int13Installed	; Mark as installed
;       mov     ax,3513h                ; get old int 13 handler
	mov	ah,35h
        mov     al,DISKBIOS_VECT
	int	21h			;

	mov	WORD PTR CS:_oldInt13Handler, bx   ; Save offset
	mov	WORD PTR CS:_oldInt13Handler[2],es ; Save segment
	;----------------------------------
	; set new handler
	;----------------------------------

        MOV     DX,offset _Int13Handler
        PUSH    CS
        POP     DS
;	mov	ax,2513h		; set new int 13 handler
	mov	ah,25h
        mov     al,DISKBIOS_VECT
	int	21h			;

locRet:
        ret

BIOSINSTALLINT13HANDLER endp

;----------------------------------------------------------------------

BIOSUNINSTALLINT13HANDLER	proc	far uses ds di si

	mov	_preCommand, FALSE	; Mark as no pre-command

	cmp	cs:_int13Installed, 0	; Are we already installed?
	jz	locRet			; No, don't uninstall

	dec     cs:_int13Installed	; Mark as uninstalled
;	mov	ax,2513h		; set interrupt 13h function
	mov	ah,25h
        mov     al,DISKBIOS_VECT
	lds	dx,CS:_oldInt13Handler	; Get the old INT 13h handler
	int	21h

locRet:
	ret

BIOSUNINSTALLINT13HANDLER endp

;---------------------------------------------------------------------
; Int 13h handler when determining the mapping from a logical device
; to a physical one
;
; This routine is ALWAYS FAR and private (not PUBLIC)
;---------------------------------------------------------------------

_Int13Handler proc	far

	cmp	cs:_int13Installed, 0	; Are we already installed?
	jz	callOldHandler

IFNDEF   SYM_DOSX
        test    ah, 80h
        jz      callOldHandler

        and     ah, 7Fh
ENDIF

	;-------------------------------------------
	; See if we have already got the values we
	; need. If we do, then don't save the values
	; and return ERROR.
	;-------------------------------------------
	push	bx
	push	ds
	mov	bx,DGROUP
	mov	ds,bx
	lds	bx,[_packageAddress]	; Get ptr. to package
	cmp	[bx].Int13Record.generated, TRUE	; Have we already generated ourselves?
	pop	ds
	pop	bx
	je	returnError		; Yes, just return ERROR

ifndef NCD

        cmp     ah, 42h                 ; Q:is it an extended Int13 Read?
        je      processIt               ; Y:process it
        cmp     ah, 2                   ; Q:Read command?
else
        Push    ax
        and     ah,0Fh                  ; an NCD read command?
        cmp     ah,6
        Pop     ax
endif

        je      processIt               ; Y:Yes, use it



ifndef NCD
	;-------------------------------------------
	; Disk Manager > 1023 cylinder pre-command?
	; 	If yes, process it.
	;-------------------------------------------
        cmp     ah, 0EEh                ; Q:Is it Disk Manager Pre-command
					;	for cylinders > 1023?
        jne     callOldHandler          ; N:call old handler

	push	ax			; Save AX
	push	ds			; Save DS
	mov	ax,DGROUP		; Point to our data segment
	mov	ds,ax			;
	mov	[_preCommand],TRUE	; Pre-Command found
	pop	ds			; Restore DS
	pop	ax			; Restore AX
endif

	;------------------------------------------------------------
	; Call old handler
	;------------------------------------------------------------
callOldHandler:
	jmp	DWORD PTR CS:_oldInt13Handler

processIt:
	push	bx
	push	cx
	push	dx
	push	ds

	mov	bx,DGROUP
	mov	ds,bx
	lds	bx,[_packageAddress]	; Get ptr. to package

	mov	[bx].Int13Record.generated, TRUE	; Signal TRUE

ifndef NCD
        mov     [bx].Int13Record.driveNum, dl   ; Save physical drive number

	;------------------------------------------------------------
        ; Check once more for extended Int13 read after this registry setup
	;------------------------------------------------------------

        cmp     ah,42h                  ; Q:Is it an extended int13?
        je      ExtendedInt13RM         ; Y:jmp to that part


        mov     BYTE PTR [bx].Int13Record.startHead, dh  ; Save head number

	mov	ax, cx			; Get track/sector combo
	rol	al, 1			;
	rol	al, 1			;
	and	al, 3			; Remove 2 bits at hi-end
	xchg	al, ah			; Swap them

        cmp     [_preCommand], TRUE     ; Q:Pre-command specified?
        jne     noPreCommand            ; N:skip it
	add	ax, 1024		; Add 1024 cylinders to cylinder count
noPreCommand:

        mov     WORD PTR [bx].Int13Record.startTrack, ax ; Save track number

	and	cl, 3Fh			; Remove 2 hi-bits
        mov     BYTE PTR [bx].Int13Record.startSector, cl        ; Save starting sector number
        Mov     [bx].Int13Record.ExtendedInt13,0   ;Old int13 generated.
        jmp     short ExitNow



ExtendedInt13RM:

        Mov     [bx].Int13Record.ExtendedInt13,1  ;Setting the information flag.

        Pop     es                       ;Move pushed DS into ES.
        Push    es                       ;and set it back on the stack

	;------------------------------------------------------------
        ; Int13x 42h, uses DS:SI pointing to a "Disk Address Packet"
        ; Reach into that packet now that ES=DS and get the Quadword
        ; Absolute block number.
	;------------------------------------------------------------


        mov     ax,WORD PTR es:[si].DiskAddressPacket.blockNumLo
        mov     WORD PTR [bx].Int13Record.blockNumLo,ax

        mov     ax,WORD PTR es:[si+2].DiskAddressPacket.blockNumLo
        mov     WORD PTR [bx+2].Int13Record.blockNumLo,ax

        mov     ax,WORD PTR es:[si].DiskAddressPacket.blockNumHi
        mov     WORD PTR [bx].Int13Record.blockNumHi,ax

        mov     ax,WORD PTR es:[si+2].DiskAddressPacket.blockNumHi
        mov     WORD PTR [bx+2].Int13Record.blockNumHi,ax


else
        mov     [bx].Int13Record.driveNum, al       ; DA/UA code
        mov     BYTE PTR [bx].Int13Record.startHead, dh      ; head number   (H)
        mov     BYTE PTR [bx].Int13Record.startSector, dl    ; sector number (R),( or Hi Byte of
					;		The relative address)
        mov     [bx].Int13Record.NECsectorLenCode, ch       ; sector code   (N)
	test	al, 01010000B		; This DA is HD ?
	jz	isHDRelative		; If Yes, cx is low word of
					;	The relative address.
	sub	ch, ch
isHDRelative:
        mov     word ptr [bx].Int13Record.startTrack, cx     ; clinder number(C)

endif


ExitNow:

	pop	ds
	pop	dx
	pop	cx
	pop	bx

returnError:
ifndef NCD
	mov	ax, 0100h		; Mark as INVALID FUNCTION REQUEST
else
	mov	ax, 6000h		; Not Ready error status code
endif

	stc				; Set the CARRY as error occurred

	sti				; Enable interupts
	retf	2			; Don't pop flags

_Int13Handler endp


ENDIF








IFDEF SYM_DOSX
;---------------------------------------------------------------------
; Int 13h handler when determining the mapping from a logical device
; to a physical one
;
;
; Special version for protected mode code calling this REAL mode
; interrupt handler.  This actual code is not really executed. This is
; actually just a binary image that is COPIED to a real mode memory block
;
; This routine is ALWAYS FAR
;---------------------------------------------------------------------

_RealModeInt13Handler proc      far
           jmp short     @f
;
; WARNING:  _RMoldInt13Handler is referenced by absolute address into
; this segment.  Do not change its location within this segment unless
; you know what you are doing!
;
_RMoldInt13Handler        dd      0
_RMInt13Rec               db      (size Int13Record) dup (0)
_RMpreCommand             db      0
;counter                   WORD    0


	;-------------------------------------------
	; See if we have already got the values we
	; need. If we do, then don't save the values
	; and return ERROR.
	;-------------------------------------------
@@:
	push	bx
	push	ds
        mov     bx,cs
	mov	ds,bx
            ; Get ptr. to package
        Mov     bx,offset _RMInt13Rec - offset _RealModeInt13Handler     ; Get ptr. to package

;
; simply for debugging
;
;
;
;       Inc     WORD PTR [BX +(size Int13Record) + 1]
;
;
;


	cmp	[bx].Int13Record.generated, TRUE	; Have we already generated ourselves?
	pop	ds
	pop	bx
	je	returnError		; Yes, just return ERROR

ifndef NCD
	cmp	ah, 2			; Read command?
else
	push	ax
	and	ah, 0Fh
	cmp	ah, 6			; A read command ?
	pop	ax
endif
	je	processIt		; Yes, use it

ifndef NCD
	;-------------------------------------------
        ; New extended Int13 disk handler?
        ;
	;-------------------------------------------

        cmp     ah, 042h
        je      processIt

	;-------------------------------------------
	; Disk Manager > 1023 cylinder pre-command?
	; 	If yes, process it.
	;-------------------------------------------
        cmp     ah, 0EEh                ; Is it Disk Manager Pre-command
					;	for cylinders > 1023?
	jne	callOldHandler		; No, call old handler

	push	ax			; Save AX
	push	ds			; Save DS
        mov     ax,cs                   ; Point to our data segment
	mov	ds,ax			;
        Mov     bx,Offset _RMpreCommand - offset _RealModeInt13Handler

        mov     Byte Ptr [BX],TRUE       ; Pre-Command found
	pop	ds			; Restore DS
	pop	ax			; Restore AX
endif
	;------------------------------------------------------------
	; Call old handler
	;------------------------------------------------------------
callOldHandler:
	;------------------------------------------------------------
        ; WARNING: Watch out for this absolute reference.  The address
        ; must remain two bytes into this segment
	;------------------------------------------------------------

        jmp     DWORD PTR CS:[2]

processIt:
	push	bx
	push	cx
	push	dx
	push	ds

        mov     bx,CS
	mov	ds,bx
            ; Get ptr. to package
        Mov     bx,offset _RMInt13Rec  - offset _RealModeInt13Handler

	mov	[bx].Int13Record.generated, TRUE	; Signal TRUE

ifndef NCD
        mov     [bx].Int13Record.driveNum, dl   ; Save physical drive number

	;------------------------------------------------------------
        ; Now that the basic information has been set up, test once more
        ; for extended Int13 call.  If present, fill out Int13Record for it
	;------------------------------------------------------------

        cmp     ah, 042h
        je      ExtendedInt13PM



        mov     BYTE PTR [bx].Int13Record.startHead, dh  ; Save head number

	mov	ax, cx			; Get track/sector combo
	rol	al, 1			;
	rol	al, 1			;
	and	al, 3			; Remove 2 bits at hi-end
	xchg	al, ah			; Swap them

        push    bx
        Mov     bx,Offset _RMpreCommand - offset _RealModeInt13Handler
        cmp     Byte Ptr [BX],TRUE       ; Pre-Command found
        pop     bx

	jne	noPreCommand		; No, skip it
	add	ax, 1024		; Add 1024 cylinders to cylinder count
noPreCommand:

        mov     WORD PTR [bx].Int13Record.startTrack, ax ; Save track number

	and	cl, 3Fh			; Remove 2 hi-bits
        mov     BYTE PTR [bx].Int13Record.startSector, cl        ; Save starting sector number
        Mov     [bx].Int13Record.ExtendedInt13,IsInt13X_No
        jmp short ExitNow


ExtendedInt13PM:

        Mov     [bx].Int13Record.ExtendedInt13,IsInt13X_Yes

        Pop     es                       ;Move pushed DS into ES.
        Push    es                       ;and set it back on the stack

	;------------------------------------------------------------
        ; Int13x 42h, uses DS:SI pointing to a "Disk Address Packet".
        ; Reach into that packet now that ES=DS and get the Quadword
        ; Absolute block number.
	;------------------------------------------------------------

        mov     ax,WORD PTR es:[si].DiskAddressPacket.blockNumLo
        mov     WORD PTR[bx].Int13Record.blockNumLo,ax

        mov     ax,WORD PTR es:[si+2].DiskAddressPacket.blockNumLo
        mov     WORD PTR[bx+2].Int13Record.blockNumLo,ax

        mov     ax,WORD PTR es:[si].DiskAddressPacket.blockNumHi
        mov     WORD PTR[bx].Int13Record.blockNumHi,ax

        mov     ax,WORD PTR es:[si+2].DiskAddressPacket.blockNumHi
        mov     WORD PTR[bx+2].Int13Record.blockNumHi,ax



;;;;;;;;;;;;;NEC
else
        mov     BYTE PTR [bx].Int13Record.driveNum, al       ; DA/UA code
        mov     BYTE PTR [bx].Int13Record.startHead, dh      ; head number   (H)
        mov     BYTE PTR [bx].Int13Record.startSector, dl    ; sector number (R),( or Hi Byte of
                                                    ; The relative address)
        mov     [bx].Int13Record.NECsectorLenCode, ch       ; sector code   (N)
	test	al, 01010000B		; This DA is HD ?
	jz	isHDRelative		; If Yes, cx is low word of
					;	The relative address.
	sub	ch, ch
isHDRelative:
        mov     WORD PTR [bx].Int13Record.startTrack, cx     ; clinder number(C)
endif
;;;;;;;;;;;;;



ExitNow:
	pop	ds
	pop	dx
	pop	cx
	pop	bx

returnError:
ifndef NCD
	mov	ax, 0100h		; Mark as INVALID FUNCTION REQUEST
else
	mov	ax, 6000h		; Not Ready error status code
endif

	stc				; Set the CARRY as error occurred

	sti				; Enable interupts
	retf	2			; Don't pop flags


_wRMInt13HandlerSize     DW      ($ - _RealModeInt13Handler)

PUBLIC _wRMInt13HandlerSize
PUBLIC _RealModeInt13Handler

_RealModeInt13Handler endp




ENDIF

	end
