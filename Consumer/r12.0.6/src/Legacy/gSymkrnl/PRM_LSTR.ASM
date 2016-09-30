        PAGE    ,132
;* Copyright 1992 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/prm_lstr.asv   1.1   20 Sep 1996 21:21:52   DHERTEL  $  *
;*                                                                      *
;* Description:                                                         *
;*      Character conversion routines and DOS equivalents for Windows   *
;*      lstr and Ansi functions.                                        *
;*                                                                      *
;* Contains:                                                            *
;*      lstrcmp                                                         *
;*      lstrcmpi                                                        *
;*      AnsiLower                                                       *
;*      AnsiUpper                                                       *
;*      CharToUpper                                                     *
;*      CharToLower                                                     *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/prm_lstr.asv  $  *
;  
;     Rev 1.1   20 Sep 1996 21:21:52   DHERTEL
;  Merged DBCS changed from Quake C.  The assembly versions of AnsiUpper() and
;  AnsiLower() are not used in DBCS.  Instead, the C versions in dbcs.c are.
;  
;     Rev 1.0   26 Jan 1996 20:21:08   JREARDON
;  Initial revision.
;  
;     Rev 1.18   15 Dec 1994 17:30:56   BRIANF
;  Removed the check for the Swiss collating table.
;  The table has been fixed in version 6.22 of MSDOS.
;  DOS versions prior to 6.22 will have to live with
;  an incorrect collating table.
;
;     Rev 1.17   23 Jun 1994 15:47:02   MARKK
;  Undid most DX stuff, rational handles it
;
;     Rev 1.16   22 Jun 1994 15:22:10   MARKK
;  Fixed far call under DOSX
;
;     Rev 1.15   21 Jun 1994 16:44:00   MARKK
;  Change to Rational
;
;     Rev 1.14   08 Jun 1994 12:18:04   MARKK
;  Fixed it under DX
;
;     Rev 1.13   06 Jun 1994 08:29:48   MARKK
;  Preserve regs around DosRealFarCall
;
;     Rev 1.12   02 Jun 1994 17:30:14   DBUCHES
;  Changed reference to DosRealFarCall() to _DosRealFarCall().
;
;
;     Rev 1.11   02 Jun 1994 16:00:24   MARKK
;  Fixed for DOSX
;
;     Rev 1.10   07 Mar 1994 19:08:18   BRUCE
;  [FIX] Changed WIN16 to SYM_WIN
;
;     Rev 1.9   12 Jul 1993 21:22:42   BRAD
;  AnsiUpper() and AnsiLower() were not returning a ptr to the
;  string.  They were returning some goofy value.
;
;     Rev 1.8   25 May 1993 15:32:26   JOHN
;  Added test to identify when we're running on country code 041 (Swiss)
;  and disable use of DOS collating table.  The Swiss table is BUSTED.
;  Microsoft more-or-less admits there is a problem, and we just have
;  to work around it.
;
;     Rev 1.7   17 May 1993 15:38:40   JOHN
;  Fixes for DOS platform, ported from 0100 branch:
;
;  Fixed the way lstrcmpi() uses the collating table.  It turns out that
;  some some versions of DOS and/or some country codes translate an ASCII
;  0x00 to a collating value of 0xFF.  The code used to assume that 0x00
;  always mapped to 0x00, so we never recognized the ends of the strings
;  we were comparing.
;
;  Also fixed lstrcmp() so it is really case sensitive.  The code was using
;  the collating table.  Now the comparisons are made on the raw bytes, but
;  if a difference is found, the collating table is used to determine which
;  byte is greater than or less than the other.  I don't think this is a
;  perfect emulation of the Windows lstrcmp(), but I think it is as close
;  as we can get with the international services available from DOS.
;
;     Rev 1.6   12 Feb 1993 04:30:14   ENRIQUE
;  No change.
;
;     Rev 1.5   20 Oct 1992 14:19:42   MARKK
;  Fixed a couple of bugs
;
;     Rev 1.4   21 Sep 1992 09:50:52   MARKK
;  Added xor ah,ah to CHARTOUPPER
;
;     Rev 1.3   20 Sep 1992 22:23:54   ENRIQUE
;  AnsiLowerBuff and AnsiUpperBuff return the count not the uppercased
;  character.  The upper or lower cased character is now placed in AX.
;
;
;     Rev 1.2   03 Sep 1992 16:10:08   ED
;  DOS Fix type things, and forced the segment to be LIB_TEXT in DOS
;
;     Rev 1.1   02 Sep 1992 11:26:12   ED
;  Updated for Windows
;
;     Rev 1.3   31 Aug 1992 12:17:12   ED
;  Added exportable versions of CharToUpper and CharToLower.  These map
;  differently in DOS and Windows
;
;     Rev 1.0   26 Aug 1992 17:03:16   ED
;  Initial revision.
;************************************************************************

%	.MODEL	memoryModel, PASCAL

IFDEF NCD
    DBCS EQU TRUE
ENDIF
IFDEF DVD
    DBCS EQU TRUE
ENDIF

IFDEF   SYM_WIN
        EXTRN   ANSIUPPERBUFF:FAR
        EXTRN   ANSILOWERBUFF:FAR
ELSE
	EXTRN	__osmajor:BYTE
	EXTRN	__osversion:WORD

	.DATA

; NOTE: Do not move these items into .DATA? or change their values
;	unless you are very sure you know what you're doing.

bFirstCall		DB	1
bNoCollatingTable	DB	0

IntlData		DB	06H
lpCollatingTable	DD	0

firstUpper	        DB	1
firstLower	        DB	1

IFDEF	SYM_DOSXX
rRegs	LABEL WORD
rax	dw	0
rbx	dw	0
rcx	dw	0
rdx	dw	0
rsi	dw	0
rdi	dw	0
rflags	dw	0

sRegs	LABEL WORD
res	dw	0
rcs	dw	0
rss	dw	0
rds	dw	0
ENDIF

lowerTable      DB 128 dup(0)

caseMapFuncPtr	        DD	?

ENDIF   ; SYM_WIN

;************************************************************************
;************************************************************************

IFDEF   SYM_WIN
%	.CODE	segmentName             ;; use supplied name for Windows
ELSE
        .CODE   LIB_TEXT                ;; force LIB_TEXT for DOS
ENDIF

IFDEF	SYM_DOSX
	.286
	EXTRN	RealModeFarCall:PROC
	EXTRN	GetProtModePtr:PROC
ENDIF

IFDEF DBCS
	EXTRN   ISJAPSTRING:PROC
ENDIF

;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      Converts a character to uppercase.                              *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 08/24/1992 ED Created.                                               *
;************************************************************************

CHARTOUPPER     proc    character:WORD

IFDEF   SYM_WIN
.286
        lea	ax,character
	push	ss                      ; address of the character
	push	ax
	push	1                       ; do one character
	call	ANSIUPPERBUFF
        mov     ax,character
.8086
ELSE
        mov     dl,byte ptr character
        call    _CharToUpper
        mov     al,dl
ENDIF
	xor	ah, ah
        ret

CHARTOUPPER     endp

;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      Converts a character to lowercase.                              *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 08/24/1992 ED Created.                                               *
;************************************************************************

CHARTOLOWER     proc    character:WORD

IFDEF   SYM_WIN
.286
        lea	ax,character
	push	ss                      ; address of the character
	push	ax
	push	1                       ; do one character
	call	ANSILOWERBUFF
        mov     ax,character
.8086
ELSE
        mov     dl,byte ptr character
        call    _CharToLower
        mov     al,dl
ENDIF
        xor     ah,ah
        ret

CHARTOLOWER     endp


IFNDEF  SYM_WIN

;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      LPSTR AnsiLower (LPSTR string);                                 *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 07/07/1992 ED Created.                                               *
;* 07/12/1993 BRAD Not returning ptr to string                          *
;************************************************************************

IFDEF DBCS

; The DBCS version of AnsiLower is defined in dbcs.c

ELSE
    
ANSILOWER       proc    string:DWORD

                push    ds

	        lds	bx,string
                push    ds              ; Save return value
	        push	bx              ;       "
	        jmp 	short L17

LC:             mov	dl,al
	        call	_CharToLower
	        mov	al,dl
	        mov	[bx],al
L16:	        inc	bx
L17:	        mov	al,[bx]
	        or	al,al
	        jne	LC

                pop     ax              ; Restore return value
                pop     dx              ;        "

                pop     ds

                ret

ANSILOWER       endp

ENDIF

;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      LPSTR AnsiUpper (LPSTR string);                                 *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 07/07/1992 ED Created.                                               *
;* 07/12/1993 BRAD Not returning ptr to string                          *
;************************************************************************

IFDEF DBCS

; The DBCS version of AnsiUpper is defined in dbcs.c

ELSE

ANSIUPPER       proc    string:DWORD

                push    ds

	        lds	bx,string
                push    ds              ; Save return value
	        push	bx              ;       "
	        jmp 	short L17

LC:             mov	dl,al
	        call	_CharToUpper
	        mov	al,dl
	        mov	[bx],al
L16:	        inc	bx
L17:	        mov	al,[bx]
	        or	al,al
	        jne	LC

                pop     ax              ; Restore return value
                pop     dx              ;        "

                pop     ds

                ret

ANSIUPPER       endp

ENDIF

;-----------------------------------------------------------------------;
; This procedure converts one character to a lower case character.	;
;									;
; On entry:	DL	Character we want to convert to lower case	;
; Returns:	DL	converted character				;
;-----------------------------------------------------------------------;

_CharToLower	proc    NEAR

	PUSH	AX
	PUSH	DS
	MOV	AX,DGROUP
	MOV	DS,AX

	CMP	DL,'A'			;See if it's an upper-case character
	JB	DONE_TO_LOWER		;It's not, so we're done
	CMP	DL,'Z'			;See if it's an upper-case character
	JA	CHECK_FOREIGN_UPPER	;See if it's a foreign character
	ADD	DL,'a'-'A'		;Convert to a lower-case character
	JMP	Short DONE_TO_LOWER

CHECK_FOREIGN_UPPER:
	CMP	DL,80h			;Is this a foreign character?
	JB	DONE_TO_LOWER		;No, then return character

	CMP	[__osmajor],3	        ;Is this DOS version 3 or above?
	JB	DONE_TO_LOWER		;No, so we don't support foreign chars

	TEST	[firstLower],0FFH	;Is this first call to _CharToLower?
	JZ	NOT_FIRST_CALL		;No
	MOV	[firstLower],00H	;Clear first call flag

	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	SI

;	Start by creating a table with one-to-one mapping
	MOV	BX,OFFSET DGROUP:lowerTable
	MOV	CX,0080H
	MOV	AL,CL
SETUP_LOOP1:
	MOV	[BX],AL
	INC	AL
	INC	BX
	LOOP	SETUP_LOOP1

;	Determine which characters are lower case
	MOV	SI,OFFSET DGROUP:lowerTable
	MOV	CX,0080H
	MOV	DL,CL
	XOR	BH,BH
SETUP_LOOP2:
	PUSH	DX
	CALL	_CharToUpper
	MOV	AL,DL
	POP	DX
	CMP	AL,DL			;Is char already in upper case?
	JE	SETUP_NEXT		;Yes, so ignore it
	SUB	AL,80H
	JC	SETUP_NEXT		;Char maps to ASCII -- ignore it
	MOV	BL,AL
	MOV	[SI+BX],DL		;Set loweTable entry
SETUP_NEXT:
	INC	DL
	LOOP	SETUP_LOOP2

	POP	SI
	POP	DX
	POP	CX
	POP	BX

NOT_FIRST_CALL:
	PUSH	BX
	MOV	BX,OFFSET DGROUP:lowerTable
	MOV	AL,DL
	SUB	AL,80H
	XLAT
	MOV	DL,AL
	POP	BX

DONE_TO_LOWER:
	POP	DS
	POP	AX
	RET

_CharToLower	endp


;-----------------------------------------------------------------------;
; This procedure converts one character to an upper-case character.	;
; On entry:	DL	Character we want to convert to lower case	;
; Returns:	DL	Converted character				;
;-----------------------------------------------------------------------;

_CharToUpper	PROC    NEAR

	PUSH	AX
	PUSH	DS
	MOV	AX,DGROUP
	MOV	DS,AX

	CMP	DL,'a'			;See if it's lower case
	JB	CHECK_FOREIGN		;Check for foreign symbols
	CMP	DL,'z'
	JA	CHECK_FOREIGN		;Check for foreign symbols
	ADD	DL,'A'-'a'		;Convert to upper case
	JMP	DONE_TO_UPPER

CHECK_FOREIGN:
	CMP	DL,80h			;Is this a foreign character?
	JB	DONE_TO_UPPER		;No, then return character

	CMP	[__osmajor],3	        ;Is this DOS version 3 or above?
	JB	DONE_TO_UPPER		;No, so we don't support foreign chars

	TEST	[firstUpper],0FFH	;Is this first call to _CharToUpper?
	JZ	NOT_FIRST_CALL		;No
	MOV	[firstUpper],00H	;Clear first call flag

	PUSH	BX
	PUSH	DX
	PUSH	BP
	MOV	BP,SP
	SUB	SP,48			;Make space for intl data buffer

	MOV	AX,3800H
	MOV	DX,SP
	INT	21H
	MOV	BX,SP
	MOV	AX,[BX+12H]
	MOV	WORD PTR [caseMapFuncPtr],AX
	MOV	AX,[BX+14H]
	MOV	WORD PTR [caseMapFuncPtr+2],AX

	MOV	SP,BP
	POP	BP
	POP	DX
	POP	BX

NOT_FIRST_CALL:

	MOV	AL,DL
IFDEF	SYM_DOSXX
	mov	res, es
	mov	rds, ds
	mov	rss, ss
	mov	rcs, cs
	mov	rdi, di
	mov	rsi, si
	mov	rbx, bx
	mov	rdx, dx
	mov	rcx, cx
	mov	rax, ax
	mov	rflags, 0

	pusha
	push	WORD PTR [caseMapFuncPtr+2]
	push	WORD PTR [caseMapFuncPtr]
	push	ds
	push	OFFSET rRegs
	push	ds
	push	OFFSET sRegs
	call	RealModeFarCall
	popa

	mov	ax, rax

ELSE
	CALL	[caseMapFuncPtr]	;Call DOS to convert to upper case
ENDIF
	MOV	DL,AL

DONE_TO_UPPER:
	POP	DS
	POP	AX
	RET

_CharToUpper	ENDP


;-----------------------------------------------------------------------;
; Set lpCollatingTable pointer						;
;									;
; This code assumes DS==DGROUP on entry!				;
;-----------------------------------------------------------------------;

__DOSGetCollatingTable	PROC	NEAR

	CMP	[bFirstCall],1		;Is this the first call?
	JNE	DONE			;Yes - Just return pointer to table

	MOV	[bFirstCall],0		;Indicate that we've been here before
	MOV	[bNoCollatingTable],1	;Assume no table until one is found

	MOV	AX,[__osversion]
	XCHG	AH,AL
	CMP	AX,031EH		;Is this DOS version 3.30 or above?
	JB	DONE			;No - We can't get a collating table

	PUSH	DS
	SUB	SP,0080H		;Reserve space on stack for buffer
	PUSH	SS
	POP	DS			;DS = Buffer segment
	MOV	DX,SP			;DX = Buffer offset
	MOV	AX,3800H
	XOR	BX,BX
	INT	21H			;Get country information
	ADD	SP,0080H
	POP	DS
;    CMP BX,041D         ;Country == Switzerland?
;    JE  DONE            ;Yes -- The collating table is BUSTED!
					;We have to pretend it doesn't exist
                    ; -BCF 12/15/94
                    ;I removed the check for the Swiss Country code.  The Swiss
                    ;collating table is fixed in DOS 6.22.
	PUSH	AX
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	DI
	PUSH	ES

	PUSH	DS
	POP	ES
	MOV	DI,OFFSET IntlData	;ES:DI = Country data packet
	MOV	AX,6506H		;Get collating table
	MOV	BX,0FFFFH		;Current code page
	MOV	DX,0FFFFH		;Current country
	MOV	CX,0005			;Size of data packet
	INT	21H
	JC	NO_TABLE

IFDEF	SYM_DOSXX

	PUSHA
	push	WORD PTR [lpCollatingTable+2]
	push	WORD PTR [lpCollatingTable]
	call	GetProtModePtr
	mov	WORD PTR [lpCollatingTable], ax
	mov	WORD PTR [lpCollatingTable+2], dx
	POPA

ENDIF
	LES	DI,[lpCollatingTable]	;Test for non-NULL table pointer
	MOV	AX,ES
	OR	AX,DI			;Is pointer valid?
	JZ	NO_TABLE		;No -- Must not be a collating table

	ADD	WORD PTR [lpCollatingTable],0002H  ;Skip over count word

	MOV	[bNoCollatingTable],0	;Indicate we found the table

NO_TABLE:
	POP	ES
	POP	DI
	POP	DX
	POP	CX
	POP	BX
	POP	AX

DONE:	RET

__DOSGetCollatingTable	ENDP

;-----------------------------------------------------------------------;
; Converts character in DL into sort value				;
;									;
; This code makes no assumption about DS on entry.			;
;-----------------------------------------------------------------------;

_CharToSortValue PROC NEAR

	PUSH	DS
	PUSH	ES
	PUSH	SI
	PUSH	AX
	MOV	AX,DGROUP
	MOV	DS,AX

	CMP	[bNoCollatingTable],1	;Do we know that there is no table?
	JE	DONE			;Yes - Table doesn't exist

	LES	SI,[lpCollatingTable]	;ES:SI = Collating table
	MOV	AX,ES
	OR	AX,SI			;Was table already found?
	JNZ	USE_TABLE		;Yes!

	CALL	__DOSGetCollatingTable	;Find the collating table

	LES	SI,[lpCollatingTable]	;ES:SI = Collating table
	MOV	AX,ES
	OR	AX,SI			;Do we have the table now?
	JZ	DONE			;No!

USE_TABLE:
	PUSH	BX
	XOR	BH,BH
	MOV	BL,DL			;BX = Character
	MOV	DL,ES:[SI+BX]		;AX = Sort value
	POP	BX

DONE:	POP	AX
	POP	SI
	POP	ES
	POP	DS
	RET

_CharToSortValue ENDP

;-----------------------------------------------------------------------;
; Byte far * PASCAL DOSGetCollatingTable(void);				;
;									;
; Returns pointer to start of the DOS collating table			;
;-----------------------------------------------------------------------;

DOSGETCOLLATINGTABLE	PROC

	CALL	__DOSGetCollatingTable
	MOV	AX,WORD PTR [lpCollatingTable]
	MOV	DX,WORD PTR [lpCollatingTable+2]
	RET

DOSGETCOLLATINGTABLE	ENDP

;-----------------------------------------------------------------------;
; Byte PASCAL CharToSortValue(Word ch);					;
;									;
; Returns the sorting value for a character				;
;-----------------------------------------------------------------------;

CHARTOSORTVALUE	PROC	chr:WORD

	MOV	DX,[chr]
	CALL	_CharToSortValue
	XOR	AH,AH
	MOV	AL,DL
	RET

CHARTOSORTVALUE	ENDP


;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      int PASCAL lstrcmpi (LPSTR string1, LPSTR string2);             *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 07/02/1992 ED Created.                                               *
;************************************************************************

LSTRCMPI        PROC	USES DS SI DI, lpszString1:DWORD, lpszString2:DWORD

;		Get collating table pointer

		call	DOSGETCOLLATINGTABLE
		mov	bx,ax			;Save table offset in BX
		or	ax,dx
		jnz	UseTable

;		Perform the comparison without a collating table

		les	si,[lpszString1]
		lds	di,[lpszString2]

NextByte1:	mov	dl,es:[si]		;Get byte from String1
		call	_CharToUpper
		mov	al,dl

		mov	dl,ds:[di]		;Get byte from String2
		call	_CharToUpper
		mov	ah,dl

		cmp	ah,al			;Are characters equal?
		jne	Done			;No - Stop the comparison!

		inc	si
		inc	di
		or	al,al			;Are we at end of strings?
		jnz	NextByte1		;No - Process next byte

		xor	ax,ax			;Return strings EQUAL
		jmp	short Done		;Common exit point

;		Perform the comparison using the collating table

UseTable:	push	dx
		pop	ds			;DS:BX = Collating table

NextByte2:	les	si,[lpszString1]
		inc	word ptr [lpszString1]
		mov	ah,es:[si]		;Get byte from String1

		les	si,[lpszString2]
		inc	word ptr [lpszString2]
		mov	al,es:[si]		;Get byte from String2

		or	ax,ax			;End of both strings?
		jz	Done

		xlat				;Convert String2 to sort value
		xchg	al,ah
		xlat				;Convert String1 to sort value

		cmp	al,ah			;Compare sort values
		je	NextByte2

Done:		sub	al,ah
		cbw
		ret

LSTRCMPI        endp



;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      int PASCAL lstrcmp (LPSTR string1, LPSTR string2);              *
;*                                                                      *
;* Parameters:                                                          *
;*                                                                      *
;* Return Value:                                                        *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* 07/02/1992 ED Created.                                               *
;************************************************************************

LSTRCMP         PROC	USES DS SI DI, lpszString1:DWORD, lpszString2:DWORD

;		Get collating table pointer

		call	DOSGETCOLLATINGTABLE
		mov	bx,ax			;Save table offset in BX
		or	ax,dx
		jnz	UseTable

;		Perform the comparison without a collating table

		les	si,[lpszString1]
		lds	di,[lpszString2]

NextByte1:	mov	al,es:[si]		;Get byte from String1
                                                ;Compare with byte from String2
		cmp	al,ds:[di]		;Are characters equal?
		jne	Done			;No - Stop the comparison!

		inc	si
		inc	di
		or	al,al			;Are we at end of strings?
		jnz	NextByte1		;No - Process next byte

		xor	ax,ax			;Return strings EQUAL
		jmp	short Done		;Common exit point

;		Perform the comparison using the collating table

UseTable:	push	dx
		pop	ds			;DS:BX = Collating table

NextByte2:	les	si,[lpszString1]
		inc	word ptr [lpszString1]
		mov	ah,es:[si]		;Get byte from String1

		les	si,[lpszString2]
		inc	word ptr [lpszString2]
		mov	al,es:[si]		;Get byte from String2

		or	ax,ax			;End of both strings?
		jz	Done

		cmp	al,ah			;Compare raw bytes
		je	NextByte2		;They're equal -- Go to next

		; Try to return difference between sort values rather than
		; between raw bytes.  If sort values are equal, fall back
		; to returning difference between raw bytes.

		push	ax
		pop	cx
		xchg	cl,ch			;CL = RawByte1, CH = RawByte2

		xlat				;Convert String2 to sort value
		xchg	al,ah
		xlat				;Convert String1 to sort value

		cmp	al,ah			;Are sort values equal?
		jne	Done			;No -- Return their difference
		mov	ax,cx			;Yes -- Return diff of raw bytes

Done:		sub	al,ah
		cbw
		ret

LSTRCMP         endp

ENDIF

	END
