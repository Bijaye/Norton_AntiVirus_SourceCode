        PAGE    ,132
;* Copyright 1992 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/prm_err.asv   1.1   02 Aug 1996 16:12:06   JBRENNA  $  *
;*                                                                      *
;* Description:                                                         *
;*      Critical Error Handler (INT 24h) for DOS Quake modules, and	*
;*	other assorted error stuff for DOS and Windows.			*
;*                                                                      *
;* Contains:                                                            *
;*	DiskErrorOff							*
;*	DiskErrorOn							*
;*      DiskErrorSetTimeout                                             *
;*      DiskErrorClearTimeout                                           *
;*	FileCheckError							*
;*	FileSetError							*
;*	FileGetReturnCode						*
;*	_CheckError							*
;*	_ClearError							*
;*	_ClearErrorFar							*
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/prm_err.asv  $  *
;  
;     Rev 1.1   02 Aug 1996 16:12:06   JBRENNA
;  Quake 8 Port: MZAREMB: Added validation for memory allocation to fix
;  crash in DX platform under low memory conditions.
;  
;     Rev 1.0   26 Jan 1996 20:21:34   JREARDON
;  Initial revision.
;  
;     Rev 1.28   11 Apr 1995 17:16:52   HENRI
;  Merged changes from BRANCH 6 for error handling on DX platforms
;
;     Rev 1.27   04 Mar 1995 13:16:54   MBARNES
;  Altered FileGetReturnCode() to correctly load DS register before storing result
;
;     Rev 1.26   02 Mar 1995 13:44:20   DALLEE
;  Fixed _GetExtendedError to use correct size RET based on memory model.
;     DOS platform PROC NEAR changed to PROC, but was still hard coded to
;     use RETN.
;
;     Rev 1.25   10 Jun 1994 08:17:24   MARKK
;  OLD_INT_24 is in DGROUP!
;
;     Rev 1.24   09 Jun 1994 20:24:02   DBUCHES
;  Removed .DATA? for DOSX
;
;     Rev 1.23   09 Jun 1994 15:31:42   MARKK
;  Fixed crit error
;
;     Rev 1.22   09 Jun 1994 12:32:42   MARKK
;  Moved error handler to C for DX
;
;     Rev 1.21   26 May 1994 07:41:40   MARKK
;  Change DX to PharLap
;
;     Rev 1.20   18 May 1994 15:51:40   MARKK
;  DX work
;
;     Rev 1.19   11 May 1994 19:24:38   BRAD
;  Changes so we can use ML, not MASM
;
;     Rev 1.18   04 May 1994 12:28:30   MARKK
;  DOSX work
;
;     Rev 1.17   29 Apr 1994 13:18:20   MARKK
;  DOSX work
;
;     Rev 1.16   18 Apr 1994 16:57:32   SKURTZ
;  Added _DISK_ERR_RTN label
;
;     Rev 1.15   07 Mar 1994 19:08:20   BRUCE
;  [FIX] Changed WIN16 to SYM_WIN
;
;     Rev 1.14   13 Oct 1993 15:50:44   MFALLEN
;  Copied file from Quake branch 1 to trunk since no changes were made in
;  the trunk.
;
;     Rev 1.13.4.2   08 Sep 1993 04:47:12   BARRY
;  Added _ClearErrorFar
;
;     Rev 1.13.4.1   06 Sep 1993 03:10:48   BARRY
;  Added DiskErrorSetTimeout and DiskErrorClearTimeout
;
;     Rev 1.13.4.0   03 Sep 1993 22:19:34   SCOTTP
;  Branch base for version QAK1
;
;     Rev 1.13   12 Feb 1993 04:30:24   ENRIQUE
;  No change.
;
;     Rev 1.12   10 Dec 1992 13:18:14   MARKK
;  Fixed nasty SI trasher
;
;     Rev 1.11   10 Nov 1992 13:43:12   MARKK
;  Reversed logic of bErrorModeOff
;
;     Rev 1.10   10 Nov 1992 13:12:22   MARKK
;  Changed DiskErrorOn to PASCAL
;
;     Rev 1.9   10 Nov 1992 12:41:56   MARKK
;  Fixed problem in diskerroron
;
;     Rev 1.8   09 Oct 1992 12:25:28   ED
;  Updated to pick up the proper C6.0/C7.0 dos version variable
;
;     Rev 1.7   04 Sep 1992 14:51:36   ED
;  Added lots of return statements and other neat things
;
;     Rev 1.6   04 Sep 1992 10:57:56   ED
;  Neat assembly trick:  Add a return statement to functions
;
;     Rev 1.5   04 Sep 1992 10:53:24   ED
;  Added __CheckErrorFar for functions not in LIB_TEXT
;
;     Rev 1.4   03 Sep 1992 16:10:28   ED
;  DOS Fix type things, and forced the segment to be LIB_TEXT in DOS
;
;     Rev 1.3   03 Sep 1992 11:40:02   ED
;  Cleanup after removed language specifier
;
;     Rev 1.2   03 Sep 1992 07:59:40   ED
;  Made the FileError functions far
;
;     Rev 1.1   03 Sep 1992 07:23:06   ED
;  Dressed up and added some things
;
;     Rev 1.0   03 Sep 1992 06:52:28   ED
;  Initial revision.
;************************************************************************

;-----------------------------------------------------------------------;
; ERROR.ASM								;
;									;
; Copyright 1991 by Symantec Corporation				;
;									;
; This file contains the procedures that trap disk errors.  If there	;
; is a disk error, the critical error routines calls some C code that	;
; displays a dialog box asking whether you want to abort or retry.  If	;
; you ask to abort, it sets an error flag, and issues an ignore to DOS.	;
;									;
; The code in this file handles critical errors by saving the error	;
; code and telling DOS to ignore the error.  You should call		;
; CHECK_ERROR immediately after every INT 21h to convert this error	;
; code into a set carry flag with the error code in AX.			;
;									;
;    DiskErrorOn(error_routine);					;
;    DiskErrorOff();							;
;									;
;    _CheckError		Checks for critical errors after INT 21	;
;    _HandleDiskErrors		Used by Disk_error_trapping_on().	;
;									;
; Here is a list of the global variables that are available after a	;
; critical error:							;
;									;
;    inCriticalError		!= 0 when we're inside INT 24h		;
;									;
;    extendedError		The extended error code			;
;    errorClass			The class of the error			;
;    errorAction		Suggested action to correct the error	;
;    errorSource		The source of the critical error	;
;-----------------------------------------------------------------------;

%	.MODEL	memoryModel, PASCAL

	.DATA

IFDEF	SYM_WIN                           ; Assume MSC 7.0
	EXTRN	__dosmajor:BYTE         ; Use __osmajor in Zortech!!!
	EXTRN	__dosminor:BYTE         ; Use __osminor in Zortech
ELSE
	EXTRN	__osmajor:BYTE
	EXTRN	__osminor:BYTE
ENDIF

        EXTRN   NDOSSETERROR:FAR        ; located in SYMPRIM

        PUBLIC  _error
        PUBLIC  FILECHECKERROR
        PUBLIC  FILESETERROR
        PUBLIC  FILEGETRETURNCODE

IFNDEF	SYM_WIN				; ****** DOS ONLY *****

        PUBLIC  _DiskErrorOff
        PUBLIC  DISKERRORON
        PUBLIC  DISKERRORSETTIMEOUT
        PUBLIC  DISKERRORCLEARTIMEOUT
        PUBLIC  __CheckErrorFar
        PUBLIC  __CheckError
        PUBLIC  __ClearError
        PUBLIC  __ClearErrorFar
        PUBLIC  DISK_ERR_RTN
        PUBLIC  _DISK_ERR_RTN

_CRIT_FLAG	DB	0		;1 when there was a critical error
_CRIT_ERROR	DW	0		;The error code we want to return

diskErrorInstalled db	0		; Are we installed?

if @CodeSize
_DISK_ERR_RTN    label   DWORD          ; So C folks can use it too,
DISK_ERR_RTN	label	DWORD
else
_DISK_ERR_RTN    label   WORD           ; So C folks can use it too,
DISK_ERR_RTN	label	WORD
endif

DISK_ERR_RTN_OFF DW	-1

if @CodeSize
DISK_ERR_RTN_SEG DW	-1
endif


  	COMM near	_inCriticalError:WORD
	COMM near	_extendedError:WORD
	COMM near	_errorClass:WORD
	COMM near	_errorSource:WORD
	COMM near	_errorAction:WORD
        COMM near       _bErrorModeOff:WORD
        COMM near       _dwCriticalTicks:DWORD
        COMM near       _wCriticalDefault:WORD
        COMM near       _int13HardDiskError:BYTE

LOCAL_NAME	DB	9 DUP (?)	;Room for 8 char name, plus trailing 0
OLD_INT_24	DD	?		; Old INT 24h handler

ENDIF					; ****** END DOS ONLY *****

        .DATA

IFDEF	SYM_DOSX
_wCritHandle	DW	?
ENDIF

_error  DW      ?

IFDEF	SYM_WIN
%	.CODE	segmentName
ELSE
	.CODE	LIB_TEXT
ENDIF

IFDEF	SYM_DOSX
	.286
	EXTRN	CSSelector:WORD
	EXTRN	GetProtModePtr:PROC
	EXTRN	SelectorFree:PROC
	EXTRN	_DXHookInterrupt:PROC
	PUBLIC	_GetExtendedError
	PUBLIC	_CRIT_FLAG
	PUBLIC	_CRIT_ERROR
	PUBLIC	_fInt24
	EXTRN	D16CALLRMPROCV:PROC
	EXTRN	_d16CallRMProc:PROC
	EXTRN	d16SetRMVector:PROC
	EXTRN	GLOBALDOSALLOC:PROC
	EXTRN	GLOBALDOSFREE:PROC
	PUBLIC	_wCritHandle
ENDIF

IFNDEF	SYM_WIN				; ****** DOS ONLY *****

;-----------------------------------------------------------------------;
; The following variables are used by the critical error code.  When	;
; DOS generates an INT 24h and we ask for an abort, the code in this	;
; file sets _CRIT_FLAG to 1 and saves the error code in _CRIT_ERROR. ;
;-----------------------------------------------------------------------;

_fInt24		DB	0		;Is > 0 if we're in INT 24 call

;****************************************************************************
;
; Description: VOID PASCAL DiskErrorSetTimeout(UINT uTimeout, WORD wDefault)
;               Sets the timeout value for a critical error message.  0 = None
;
;****************************************************************************
; 09/06/1993 BARRY Function Created.
;****************************************************************************

DISKERRORSETTIMEOUT PROC        USES ax, dwTimeout:DWORD, wDefault:WORD
        mov     ax               , wDefault
        mov     _wCriticalDefault, ax

        mov     ax                         , WORD PTR dwTimeout
        mov     WORD PTR _dwCriticalTicks  , ax
        mov     ax                         , WORD PTR dwTimeout+2
        mov     WORD PTR _dwCriticalTicks+2, ax

        RET
DISKERRORSETTIMEOUT ENDP

;****************************************************************************
;
; Description: VOID PASCAL DiskErrorClearTimeout(VOID)
;               Sets the default timeout value for a critical error message.  0 = None
;
;****************************************************************************
; 09/06/1993 BARRY Function Created.
;****************************************************************************

DISKERRORCLEARTIMEOUT PROC
        mov     _wCriticalDefault          , 2    ; Default = Abort
        mov     WORD PTR _dwCriticalTicks  , 0    ; Default = 0 (no timeout)
        mov     WORD PTR _dwCriticalTicks+2, 0

        RET
DISKERRORCLEARTIMEOUT ENDP

;-----------------------------------------------------------------------;
; This procedure restores the INT 24h vector to the value it had before	;
; we called Disk_error_trapping_on();					;
;-----------------------------------------------------------------------;

_DiskErrorOff	proc

	cmp	diskErrorInstalled, 0	; Are we installed?
	jz	locRet			; No, don't uninstall

	mov	diskErrorInstalled, 0	; Yes, signal that we are uninstalled

IFNDEF	SYM_DOSX
        push    ds

	MOV	AX,2524h		;Restore DOS's vector for INT 24h
	LDS	DX,OLD_INT_24		;Get the old INT 24h vector
	INT	21h			;Set to old vector

        pop     ds
ELSE

	push	24H
	push	WORD PTR OLD_INT_24+2
	push	WORD PTR OLD_INT_24
	push	0
	push	0
	call	d16SetRMVector
	push	_wCritHandle
	call	GLOBALDOSFREE
	mov	_wCritHandle, 0
ENDIF

locRet:
	RET

_DiskErrorOff	endp


;-----------------------------------------------------------------------;
; Call this procedure once at the start of the program to turn on error	;
; trapping for disk errors.  Before you call this routine, any disk	;
; errors will cause DOS to display the "Abort, Ig..." error message	;
; and take control.							;
;									;
; This procedure sets the INT 24H vector so it points to the procedure	;
; _HandleDiskErrors, which passes control onto a C function.  If this	;
; C function returns 2, _HandleDiskErrors returns to the calling		;
; procedure with the carry flag set and the error code in AX.		;
;									;
;	DiskErrorOn(trapProcedure);					;
;-----------------------------------------------------------------------;

DISKERRORON	proc	uses ds es, routine:PTR

	cmp	diskErrorInstalled, 0	; Are we installed?
	jz	installIt		; No, install

	call	_DiskErrorOff		; Yes, uninstall first

installIt:
	inc	diskErrorInstalled	; No, signal that we are installed

        call    DISKERRORCLEARTIMEOUT

;	Install custom INT 24h handler

        mov	ax,WORD PTR routine	;get offset of trap routine
	MOV	DISK_ERR_RTN_OFF,AX	;And save it in this segment
if @CodeSize
	mov	ax,WORD PTR routine+2	;Get segment of C routine
	mov	DISK_ERR_RTN_SEG,ax
endif

IFNDEF	SYM_DOSX
        push    es
	MOV	AX,3524h		;Read current INT 24h vector
	INT	21h
	MOV	Word Ptr OLD_INT_24,BX	;Save this old vector
	MOV	Word Ptr OLD_INT_24[2],ES
        pop     es

; 	Install new INT 24h handler

        push    ds
	LEA	DX,_HandleDiskErrors
	MOV	AX,CS
	MOV	DS,AX			;Set so DS:DX points to our procedure
	MOV	AX,2524h		;Set vector for INT 24h
	INT	21h
        pop     ds
ELSE
	mov	ax, OFFSET critend
	sub	ax, OFFSET critstart

	push	ax
	push	ax
	push	0
	call	GLOBALDOSALLOC
        jnz     AllocOK                 ; Did we get a valid memory handle?
        add     sp,2                    ; readjust the stack
        jmp     short EndTrap           ; and exit the routine     

AllocOK:
	mov	_wCritHandle, ax

	pop	cx
	push	ds
	mov	es, ax
	push	cs
	pop	ds
	lea	si, critstart
	xor	di, di

	rep	movsb

	pop	ds

	push	24H
	push	dx
	push	6
	push	ds
	push	OFFSET OLD_INT_24
	call	d16SetRMVector

ENDIF

EndTrap:
	RET

DISKERRORON	endp

IFDEF	SYM_DOSX

critstart	EQU	$

bErrorOccured	dw ?
wAX		dw ?
wDI		dw ?
_HandleDiskErrors	proc	far
	push	ds
	push	cs
	pop	ds
	mov	WORD PTR ds:[0], 1
	mov	WORD PTR ds:[2], ax
	mov	WORD PTR ds:[4], di
	pop	ds
	mov	al, 3
	iret
_HandleDiskErrors	endp

critend	EQU	$
ENDIF


;-----------------------------------------------------------------------;
; This procedure checks to see if there was a critical error, and if so	;
; returns with the carry flag set and the error code in AX.		;
;									;
; Returns:	CF	1	If there was a critical error, or CF	;
;				was set on entry.			;
;		AX	Error	Error code if there was an error.	;
;									;
; Sets:		_CRIT_FLAG	0	Clears waiting critical error	;
;		_fInt24		0	Clears for another error	;
;-----------------------------------------------------------------------;

__CheckErrorFar	proc    FAR

        call    __CheckError
        retf

__CheckErrorFar	endp


__CheckError	proc    NEAR

IFDEF	SYM_DOSX
	PUSH	es
	mov	es, cs:CSSelector
	MOV	ES:[_fInt24],0		;Allow critical error trapping again
	pop	es
ELSE
	MOV	CS:[_fInt24],0		;Allow critical error trapping again
ENDIF
	JC	WAS_ERROR		;DOS returned an error message
	CMP	_CRIT_FLAG,0		;Is the critical flag set?
	JA	RETURN_ERROR		;Yes, then return a critical error
	CLC				;No, then return with carry flag clear
	jmp	short RET_ERROR		;Return with carry clear

	;---------------------------------------------------------------;
	; There was a critical error, return the error code saved by	;
	; the INT 24h handler, then clear the error flag.		;
	;---------------------------------------------------------------;
WAS_ERROR:				;DOS reported an error
	CMP	_CRIT_FLAG,0		;Was the critical flag set?
	JZ	DONE_ERROR		;No, then return DOS's error code
RETURN_ERROR:
	MOV	AX,_CRIT_ERROR	;Yes, then return critical error code
	MOV	_CRIT_FLAG,0		;Clear the critical error flag
DONE_ERROR:
	STC				;Set carry to signal error

RET_ERROR:
	RETN				;Return with carry set

__CheckError	endp

ENDIF					; ****** END DOS ONLY *****

;-----------------------------------------------------------------------;
; This chunk of code is used by some file operation functions to	;
; return an error code if the carry bit is set.  It sets AX to -1 in	;
; case of an error, and saves the error code in ERROR.  Then returns	;
; to the calling procedure.						;
;-----------------------------------------------------------------------;

FILECHECKERROR  proc    far

IFNDEF	SYM_WIN				; ****** DOS ONLY *****
        call    __CheckError             ;clear the error, then fall through
ENDIF					; ****** END DOS ONLY *****

FILESETERROR    label   far             ;Ed did this, unless it doesn't work!

	push	si
	jnc	No_Error                ;No error, return value in AX
	mov	_error,ax		;Error, save the error code
        push    ax                      ;push error code for NDosSetError call
	xor	ax,ax			;Then return ERR
	not	ax
	jmp	short All_Done
No_Error:
	mov	_error,0                 ;Reset error code for no error
        push    _error
All_Done:
	mov	si,ax			;preserve AX across the call
        call    NDOSSETERROR            ;call the Quake error save routine
	mov	ax,si			;restore AX

	pop	si

	retf

FILECHECKERROR  endp


;-----------------------------------------------------------------------;
; This chunk of code is used by some file operation functions to	;
; return either ERR or NOERR in AX, depending on the state of the       ;
; carry flag.  This differs from the FileCheckError function in that    ;
; AX is set to zero, not preserved, if the carry is not set.            ;
;-----------------------------------------------------------------------;

FILEGETRETURNCODE    proc       far

IFNDEF	SYM_WIN				; ****** DOS ONLY *****
        call    __CheckError             ; check the critical error flag
ENDIF					; ****** END DOS ONLY *****
        push    ds
        push    si
        push    dx			; save DX
        mov     dx, SEG _error
        mov     ds, dx
        mov     si, OFFSET _error

	jc      BAD_CALL		; if carry is set, there was a bad call

        xor     dx,dx			; set return code to NOERR
        push    dx                      ; setup NDosSetError
        mov     [si], dx
;       mov	_error,dx		; set the global error code
        jmp     short DONE_CALL		; finish up

BAD_CALL:
        push    ax                      ; send the error code to NDosSetError
        mov     [si], ax
;       mov	_error,ax		; set the global error code
        mov	dx, 0FFFFh		; set return code to ERR

DONE_CALL:
        call    NDOSSETERROR		; set the error code
        mov     ax,dx       		; restore the return code

        pop     dx			; restore DX
        pop     si
        pop     ds
        retf

FILEGETRETURNCODE    endp


IFNDEF	SYM_WIN				; ****** DOS ONLY *****

;-----------------------------------------------------------------------;
; Clear the critical error flag so more errors can be processed		;
;-----------------------------------------------------------------------;

__ClearError	proc	NEAR

IFDEF	SYM_DOSX
	PUSH	es
	mov	es, cs:CSSelector
	MOV	ES:[_fInt24],0		;Allow critical error trapping again
	pop	es
ELSE
	MOV	CS:[_fInt24],0		;Allow critical error trapping again
ENDIF
	MOV	_CRIT_FLAG,0		;Clear the critical error flag
	RETN

__ClearError	endp


;-----------------------------------------------------------------------;
; Special version for non LIB_TEXT segment routines
;-----------------------------------------------------------------------;
__ClearErrorFar PROC    FAR

        call    __ClearError
	RETF

__ClearErrorFar	ENDP

;-----------------------------------------------------------------------;
; DOS calls this procedure whenever there is a disk or device error	;
; through an INT 24h call.  On entry, the registers of interest are	;
; set as follows:							;
;									;
;	AH	Bit 7	0	disk errors				;
;			1	other errors, such as device errors	;
;	DI		The lower byte contains the error code		;
;	BP:SI		Points to the Device Header Control Block	;
;									;
; This procedure calls a C routine to display an error message and	;
; a Retry or Abort message:
;									;
;	error_trap(error_info, error_code, device_name);		;
;									;
; that should return a code:						;
;									;
;	2	Abort DOS function call and return an error message to	;
;		the calling procedure.					;
;		Returns (error_code + 100H)				;
;	0,1	Ignore or retry operation.				;
;									;
; Error codes:								;
;	100H	Attempt to write on write-protected diskette		;
;	101H	Unknown drive						;
;	102H	Drive not ready						;
;	103H	Unknown command						;
;	104H	Data error						;
;	105H	Bad request structure length				;
;	106H	Seek error						;
;	107H	Non-DOS disk						;
;	108H	Sector not found					;
;	109H	Printer out of paper					;
;	10AH	Write fault						;
;	10BH	Read fault						;
;	10CH	General, non-specific error (often Disk failure)	;
;-----------------------------------------------------------------------;

DEVICE_HEADER	        STRUC
NEXT_DEVICE		DD	?
ATTRIBUTES		DW	?
DEVICE_STRATEGY		DW	?
DEVICE_INTERRUPT	DW	?
DEVICE_NAME		DB	8 DUP (?)
DEVICE_HEADER	        ENDS


IFNDEF	SYM_DOSX

_HandleDiskErrors	proc	far

	TEST	CS:[_fInt24],-1		;Are we still in critical error?
	JZ	Handle_INT24		;No, this is the first time
	XOR	AL,AL			;Yes, return an Ignore message
	jmp	JUST_RETURN

Handle_INT24:
	STI				;Enable interrupts again
	PUSH	BX			;Save for return to DOS
	PUSH	CX
	PUSH	DX
	PUSH	BP
	PUSH	DS
	PUSH	ES

	MOV	DX,DGROUP		;Set up seg registers for C
	MOV	DS,DX			;ES and DS point to data segment
	MOV	ES,DX

	CALL	_GetExtendedError	;Get the extended error info

        cmp     _bErrorModeOff,0        ;check SetErrorMode() state
	JNZ	GENERATE_ERROR_CODE	; No, return error code

	CMP	[DISK_ERR_RTN_OFF],0FFFFH  ;Is there a C error procedure?
	JZ	GENERATE_ERROR_CODE	; No, return error code

	;-----------5------------------------------------;
	; This code calls the C error procedure		;
	;-----------------------------------------------;
	PUSH	AX
	PUSH	SI
	PUSH	DI
	PUSH	DS
IFDEF	SYM_DOSX
	PUSH	BP
	xor	bp, bp
	push	bp
	call	GetProtModePtr
	mov	ds, dx
ELSE
	MOV	DS,BP			;Set up to point to device header
ENDIF
	LEA	DI,LOCAL_NAME		;Copy device name to this variable
	CLD				;Set for increment
	TEST	[SI].DEVICE_HEADER.ATTRIBUTES,8000H	;Is this a character device?
	JZ	DONE_COPY_NAME		;Yes, return NULL
	LEA	SI,[SI].DEVICE_HEADER.DEVICE_NAME	;Set SI to point to device name
	MOV	CX,8			;Copy 8 chars of device name
	REP	MOVSB
DONE_COPY_NAME:				;Put a 0 at the end of the name
	XOR	AL,AL
	STOSB
IFDEF	SYM_DOSX
	push	ds
	call	SelectorFree
ENDIF
	POP	DS
	POP	DI
	POP	SI
	POP	AX

	LEA	DX,LOCAL_NAME		;Put address of device name on stack
	PUSH	DS
	PUSH	DX
	PUSH	DI			;Put error code onto stack
	PUSH	AX			;Error information
	CLD				;MS C needs direction flag clear

	INC	_inCriticalError	;Signal inside INT 24h handler
	CALL	DISK_ERR_RTN		;Call the C procedure
	DEC	_inCriticalError	;Signal no longer inside INT 24h handler

	MOV	BX,AX			;Save return code in BX
	POP	AX			;Restore the AX register
	ADD	SP,6			;Remove other parameters from stack

	;-----------------------------------------------;
	; At this point, BL == 1 for retry, 2 for abort	;
	;-----------------------------------------------;
	CMP	BL,2			;Should we return error code?
	JE	GENERATE_ERROR_CODE	;Yes.

RETRY:					;Tell DOS to retry this operation
	MOV	AL,1			;Ask for retry operation
	jmp	short DONE_HANDLE_ERRORS

GENERATE_ERROR_CODE:			;Return to calling proc with error
IFDEF	SYM_DOSX
	PUSH	es
	mov	es, cs:CSSelector
	INC	ES:[_fInt24]
	pop	es
ELSE
	INC	CS:[_fInt24]		;Don't trap the next error
ENDIF
	MOV	BX,DI			;Put the error code into BL
	MOV	BH,1			;Return 1xx for the error code
	MOV	_CRIT_FLAG,1		;Raise the critical flag
	MOV	_CRIT_ERROR,BX	;Save the error code
	MOV	AL,0			;Ask for Ignore

	;-----------------------------------------------;
	; If we have DOS 3.2 or above, we can ask for	;
	; a Fail rather than Ignore operation to abort	;
	; a system call.				;
	;-----------------------------------------------;
	PUSH	AX
IFDEF	SYM_WIN                           ; Assume MSC 7.0
	MOV	AH,__dosmajor	        ;Put DOS version number into AX
	MOV	AL,__dosminor
ELSE
	MOV	AH,__osmajor	        ;Put DOS version number into AX
	MOV	AL,__osminor
ENDIF
	CMP	AX,0314h		;Is this DOS 3.2 or above?
	POP	AX
	JB	DONE_HANDLE_ERRORS	;No, ask for ignore operation
	MOV	AL,3			;Ask for fail operation

DONE_HANDLE_ERRORS:
	POP	ES			;Restore registers for retry or ignore
	POP	DS
	POP	BP
	POP	DX
	POP	CX
	POP	BX

JUST_RETURN:
	IRET

_HandleDiskErrors	endp

ENDIF

;-----------------------------------------------------------------------;
; This procedure gets the extended error information after there was an	;
; error.  It saves the error codes in the following global variables:	;
;									;
;	extendedError		The extended error code			;
;	errorClass		The class of the error			;
;	errorAction		Suggested action to correct the error	;
;	errorSource		The source of the error			;
;-----------------------------------------------------------------------;

_GetExtendedError	proc

IFDEF	SYM_WIN                           ; Assume MSC 7.0
	CMP	__dosmajor, 3	;Is this DOS 3.0 or above?
ELSE
	CMP	__osmajor, 3	;Is this DOS 3.0 or above?
ENDIF
	JB	DONE_GET_EXTENDED	;No, then no extended error info

	PUSH	AX
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	SI
	PUSH	DI
	PUSH	DS
	PUSH	ES
	MOV	AH,59h			;Ask for the extended error
	XOR	BX,BX			;Must be 0 (according to the manual)
	INT	21h			;Ask for the extended error info
	POP	ES
	POP	DS
	MOV	_extendedError,AX	;Save the extended error code
	MOV	Byte Ptr _errorClass, BH
	MOV	Byte Ptr _errorAction, BL
	MOV	Byte Ptr _errorSource, CH
	POP	DI
	POP	SI
	POP	DX
	POP	CX
	POP	BX
	POP	AX

DONE_GET_EXTENDED:
        RET

_GetExtendedError	endp

ENDIF					; ****** END DOS ONLY *****

	end
