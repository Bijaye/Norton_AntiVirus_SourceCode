;* Copyright 1993 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/CVTFT.ASv   1.0   26 Jan 1996 20:22:20   JREARDON  $ *
;*                                                                      *
;* Description:                                                         *
;* time_t seconds = ConvertFileTimeToSeconds(LPFILETIME lpFileTime);    *
;*                                                                      *
;*      Divides a 64-bit filetime by 10^7, so it will now be in         *
;*      seconds.                                                        *
;*      Note that this function relies upon a MACRO, not a function     *
;*      call to DIV32.                                                  *
;*                                                                      *
;*      NOTE: This function is only used in DOS and Win16, since all    *
;*      other platforms have a conversion built in. NLM doesn't, but    *
;*      this is dummy for now.                                          *
;*                                                                      *
;* Description:                                                         *
;* ConvertSecondsToFileTime(LPFILETIME lpFileTime, time_t seconds);     *
;*                                                                      *
;*      Multiples a second count by 10^7, so it will now be in          *
;*      FILETIME units (100-ns).                                        *
;*      Note that this function relies upon a MACRO, not a function     *
;*      call to MUL32.                                                  *
;*                                                                      *
;*      NOTE: This function is only used in DOS and Win16, since all    *
;*      other platforms have a conversion built in. NLM doesn't, but    *
;*      this is dummy for now.                                          *
;*                                                                      *
;* Contains:                                                            *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/CVTFT.ASv  $ *
;  
;     Rev 1.0   26 Jan 1996 20:22:20   JREARDON
;  Initial revision.
;  
;     Rev 1.2   01 Nov 1994 13:33:18   JMILLARD
;  
;  make dummy versions for NLM - real ones to come
;  
;     Rev 1.1   11 May 1994 21:14:30   BRAD
;  Added ConvertSecondsToFileTime
;  
;     Rev 1.0   11 May 1994 20:24:00   BRAD
;  Initial revision.
;************************************************************************

ifdef SYM_NLM

INCLUDE platform.inc

        OPTION  LANGUAGE:PASCAL
        .code @Filename

else

%		.MODEL	memoryModel, PASCAL
%		.CODE	segmentName

endif

include        mprec.inc

                

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

                PUBLIC  CONVERTFILETIMETOSECONDS
CONVERTFILETIMETOSECONDS  proc    FAR     lpFileTime:FAR PTR

ifdef SYM_NLM

    RET

else

	PUSH	DS
	PUSH	SI
	PUSH	DI
        PUSH    BP

	LDS	SI,[lpFileTime]
        lodsw                           ; Move the 64-bit number into registers
        mov     cx, ax                  ; It will be stored in BX:BP:DX:AX
        lodsw                           
        mov     dx, ax
        lodsw
        mov     bp, ax                  
        lodsw
        mov     bx, ax
        mov     ax, cx                  ; Use AX, not CX for loword of lo dword

	DIV32	bx,bp,00989680h         ; Divide the 64-bit number by 10^7
                                        ; Result is found in DX:AX
        POP     BP
	POP	DI
	POP	SI
	POP	DS
        RET

endif

CONVERTFILETIMETOSECONDS        endp

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

                PUBLIC  CONVERTSECONDSTOFILETIME
CONVERTSECONDSTOFILETIME  proc    FAR     lpFileTime:FAR PTR, seconds:DWORD

ifdef SYM_NLM

    RET

else

	PUSH	DS
	PUSH	SI
	PUSH	DI
        PUSH    BP

	LDS	SI,[lpFileTime]         ; Point to the product (result)
        mov     bx, WORD PTR seconds    ; Get the first multiplicand in CX:BX
        mov     cx, WORD PTR (seconds+2)
        mov     bp, 0098h               ; 2nd multiplicand in BP:DI (10^7)
        mov     di, 9680h

	MUL32	[si],cx,bx,bp,di        ; Multiply the seconds to get 64-bit #

        POP     BP
	POP	DI
	POP	SI
	POP	DS
        RET

endif

CONVERTSECONDSTOFILETIME        endp

		END
