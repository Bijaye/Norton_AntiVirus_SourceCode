;* Copyright 1993 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/ROMCOPY.ASv   1.0   26 Jan 1996 20:22:58   JREARDON  $  *
;*                                                                      *
;* Description:                                                         *
;*      Copy data from a ROM segment, even under QEMM stealth mode.     *
;*                                                                      *
;* Contains:                                                            *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/ROMCOPY.ASv  $  *
;  
;     Rev 1.0   26 Jan 1996 20:22:58   JREARDON
;  Initial revision.
;  
;     Rev 1.4   24 Jan 1995 14:20:26   TORSTEN
;  STEALTHCOPY was trying to call the QEMM driver by retrieving the
;  address of the API entry point from DS:lpQPI, but it had to load
;  DS:SI with the source of the copy operation -- this was not good.
;  
;     Rev 1.3   28 Sep 1993 14:22:00   ED
;  Tried to implement a Windows solution for this stuff, but it is a
;  real nightmare to do.
;  
;     Rev 1.2   23 Sep 1993 13:00:32   ED
;  Renamed to StealthCopy and moved out some code to MemoryCopyROM
;  
;     Rev 1.1   14 Sep 1993 15:32:46   ED
;  Duh.  Reversed the source and destination parameters.
;  
;     Rev 1.0   14 Sep 1993 14:29:32   ED
;  Initial revision.
;************************************************************************

%		.MODEL	memoryModel, PASCAL

                PUBLIC  STEALTHCOPY

TRUE		EQU	0001H
FALSE		EQU	0000H

                .DATA 
bTested		DB	0
bInstalled	DB	0
lpQPI		DD	0
Int2F_ID	DB	0

%		.CODE	segmentName

ASSUME DS:NOTHING

;-----------------------------------------------------------------------------
; Boolean QEMMCopyStealthROM(void far *lpSource,
;			     void far *lpDest,
;			     Ulong dwByteCount)
;
; Copy all or part of a "stealthed" ROM into conventional memory
; Returns TRUE if copy completed, FALSE if error
;-----------------------------------------------------------------------------

STEALTHCOPY   PROC FAR  lpDest:FAR PTR, lpSource:FAR PTR, wByteCount:WORD

	PUSH	DS
	PUSH	SI
	PUSH	DI

        CALL	FindQEMM                ; check for QEMM
        JZ	Not_QEMM

        MOV	AX,30*100H + 0          ; check for QEMM stealth mode
	XOR	CL,CL
        call    [lpQPI]                 ; Far pointer to QPI entry point
        CMP     CL,'F'
        JZ      Is_Stealth
        CMP     CL,'M'
        JNZ	Not_QEMM

Is_Stealth:

.386
        MOVZX   ECX, wByteCount
.8086

	LES	DI,[lpDest]
	LDS	SI,[lpSource]
	MOV	AX,33*100H + 0

        call    [lpQPI]                 ; Far pointer to QPI entry point

                                        ; return TRUE if stealth was attempted.
        MOV     AX,1                    ; (we have no way of knowing success)
        JMP     SHORT All_Done

Not_QEMM:
        MOV     AX,0                    ; return FALSE if not QEMM or Stealth

All_Done:
	POP	DI
	POP	SI
	POP	DS
        RET

STEALTHCOPY     ENDP

;-----------------------------------------------------------------------------
; FindQEMM
;
; Parameters:
;
;	None
;
; Returns:
;
;	AX		Same as [bInstalled]
;	Zero flag	Clear if QEMM installed, Set if not
;
;	[bInstalled]	TRUE if QEMM installed, FLASE if not
;	[lpQPI]		Far pointer to QPI entry point
;	[bTested]	Always set to TRUE
;-----------------------------------------------------------------------------

FindQEMM	LABEL	NEAR

;		Only test for QEMM once

	PUSH	SI
	PUSH	DI
	PUSH	BP

	CMP	[bTested],TRUE
	JE	X_FindQEMM
	MOV	[bTested],TRUE
	MOV	[bInstalled],FALSE
		
	CALL	FindInt2FID
	JZ	X_FindQEMM

	MOV	AL,01H			;Get driver type
	MOV	BX,'QE'
	MOV	CX,'MM'
	MOV	DX,'42'
	INT	2FH
	CMP	BX,'OK'
	JNE	X_FindQEMM

	MOV	WORD PTR [lpQPI],DI
	MOV	WORD PTR [lpQPI+2],ES
	MOV	[bInstalled],TRUE
X_FindQEMM:
        POP	BP
	POP	DI
	POP	SI
	XOR	AH,AH
	MOV	AL,[bInstalled]
	OR	AX,AX
	RET


;-----------------------------------------------------------------------------
; FindInt2FID
;
; Parameters:
;
;	None
;
; Returns:
;
;	AH		Same as [Int2F_ID]
;	Zero flag	Clear if ID found, Set if not found
;
;	[Int2F_ID]	QEMM's INT 2FH ID or 00H if not found
;-----------------------------------------------------------------------------

FindInt2FID	LABEL	NEAR

	PUSH	DS

        MOV	AH,0D2H			;First possible QEMM ID
	XOR	AL,AL
FindLoop:
        MOV	BX,'QD'
	MOV	CX,'ME'
	MOV	DX,'M0'
	PUSH	AX
	INT	2FH
	MOV	DI,AX
	POP	AX
	AND	DI,00FFH
	JZ	NotFound
	CMP	DI,00FFH
	JNE	TryNext
	CMP	BX,'ME'
	JNE	TryNext
	CMP	CX,'MD'
	JNE	TryNext
	CMP	DX,'VR'
	JE	FoundIt
TryNext:
        INC	AH
	JNZ	CheckForWrap
	MOV	AH,0C0H
CheckForWrap:	
        CMP	AH,0D2H
	JNE	FindLoop
NotFound:
        XOR	AH,AH
FoundIt:
        POP	DS
	MOV	[Int2F_ID],AH
	OR	AH,AH
	RET

;-----------------------------------------------------------------------------

		END
