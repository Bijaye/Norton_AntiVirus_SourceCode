;  $Header:   S:/SYMKRNL/VCS/TSR_ASM.ASv   1.0   26 Jan 1996 20:23:12   JREARDON  $

;-----------------------------------------------------------------------;
;									;
;	TSRASM.ASM -- TSR call for Symantec TSR programs.               ;
;									;
;	Copyright 1991-93 by Symantec Corporation                       ;
;									;
;-----------------------------------------------------------------------;
;  $Log:   S:/SYMKRNL/VCS/TSR_ASM.ASv  $
;  
;     Rev 1.0   26 Jan 1996 20:23:12   JREARDON
;  Initial revision.
;  
;     Rev 1.5   29 Apr 1994 10:59:12   MARKK
;  Fixed for DOSX
;  
;     Rev 1.4   07 Mar 1994 19:08:20   BRUCE
;  [FIX] Changed WIN16 to SYM_WIN
;  
;     Rev 1.3   17 Jun 1993 14:29:32   DALLEE
;  Removed code which was switching high and low bytes of the app ID
;  in Do_INT2F_Call.
;  
;     Rev 1.2   09 Jun 1993 16:34:16   MFALLEN
;  Modified TSR detection so it works under Windows too
;  
;     Rev 1.1   09 Jun 1993 13:02:56   DALLEE
;  Changed proc near TSRIs2FSet to NearTSRIs2FSet.  This function was
;  already defined in DOSTOOL.
;  
;     Rev 1.0   31 May 1993 20:30:14   BRAD
;  Initial revision.

		include	mixed.inc

%		.MODEL	memoryModel, C

%		.CODE	segmentName

AH_Sig		=	0FEh
ALT_AH_Sig      =       0FDh
DI_Sig		=	'NU'


;-----------------------------------------------------------------------;
;									;
;       Boolean CDECL NearTSRIs2FSet(void);                             ;
;									;
;	Returns TRUE if INT 2FH is valid, FALSE if not			;
;									;
;-----------------------------------------------------------------------;
; Changed name from TSRIs2FSet to NearTSRIs2FSet - This function is     ;
; already defined in DOSTOOL with a default return, not a near return   ;
;-----------------------------------------------------------------------;

NearTSRIs2FSet  proc    near

IFDEF	SYM_DOSX
		mov	ax, 352Fh
		int	21H
		mov	ax, es
		xor	ax, bx
ELSE
		push	ds
		xor	ax,ax

IFDEF   SYM_WIN
                mov     bx, bx                  ; Convert from Segment
                mov     ax,0002h                ; to selector
                int     31h                     ; There could be better way
                jnc     @F                      ; of doing but I'm not a
                xor     ax,ax                   ; ASM person. MArTiN
@@:
ENDIF

		mov	ds,ax
		mov	ax,word ptr ds:[2FH*4]
		or	ax,word ptr ds:[2FH*4 + 2]
		pop	ds
ENDIF
		jz	@F
		mov	ax,0001H
@@:		ret

NearTSRIs2FSet  endp

;-----------------------------------------------------------------------;
;									;
;	Do_INT2F_Call							;
;									;
;	Entry:	AL = Command						;
;               SI = Application ID                                     ;
;									;
;   Exit:   AX = 0001H and CY clear if OK ,                             ;
;           DX:AX = 0000:0000H, and CY=1 if error                       ;
;									;
;-----------------------------------------------------------------------;
; 6/17/93 DALLEE, Removed code which was switching high and low bytes   ;
;           of the Application ID passed in SI.                         ;
;-----------------------------------------------------------------------;

Do_INT2F_Call	proc

		push	ax
                call    NearTSRIs2FSet  ;Check if INT 2FH vector set
		pop	ax
		jz	INT2F_Error	;Vector isn't set

		mov	ah,AH_Sig	;Load Norton signature...
tryAlternative:
                push    ax              ;Save Sig
		push	si		;Remember App ID
		mov	di,DI_Sig	;...into AH and DI
		stc
		int	2FH		;Call the TSR
		pop	di		;Recover App ID
		jc	TryAlt          ;Assume error if carry set
		or	di,2020H	;Convert ID to lower case
		cmp	si,di		;Did TSR process the command?
		jne	TryAlt          ;Nope!

INT2F_OK:
                pop     si              ;Remove signature from stack
                mov	si,ax		;Save AX for TSRIsEnabled()
		mov	ax,0001H
		clc
		ret

TryAlt:
                pop     ax              ; Get signature
                cmp     ah, ALT_AH_Sig  ; Is it the alternative one?
                je      INT2F_Error     ; If yes, then we really got an error

                mov     si,di           ;Get App Id
		mov	ah,Alt_AH_Sig	;Load Norton signature...
                jmp     short tryAlternative

INT2F_Error:
                xor ax,ax
                mov dx,ax               ;TSRGetDataPtr needs a far ptr in <dx>:<ax>
                stc                     ;and if error, must be NULL ptr
		ret

Do_INT2F_Call	endp

		end

