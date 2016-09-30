;  $Header:   S:/SYMKRNL/VCS/getcp.asv   1.1   19 Feb 1997 12:14:30   MZAREMB  $

;-----------------------------------------------------------------------;
;									;
; GETCP.ASM								;
;									;
; Copyright 1992 by Symantec Corporation				;
;									;
;-----------------------------------------------------------------------;
;
;  $Log:   S:/SYMKRNL/VCS/getcp.asv  $
;  
;     Rev 1.1   19 Feb 1997 12:14:30   MZAREMB
;  Set DS to point to proper data.
;  
;     Rev 1.0   25 Sep 1996 14:20:32   BMCCORK
;  Initial revision.
;  
;     Rev 1.0   26 Jan 1996 20:27:10   JCUBBAG
;  Initial revision.
;  
;     Rev 1.1   15 Dec 1992 13:58:54   CRAIG
;  Check for IOCTL failure.
;
;     Rev 1.0   09 Sep 1992 17:17:08   MARKK
;  Initial revision.
;
;     Rev 1.1   29 Mar 1992 18:05:20   JOHN
;  Changed to use a Generic IOCTL to query code page on CON: rather than using
;  DOS Function 66H.  Function 66H only works right when NLSFUNC is loaded,
;  and it looks like some international users don't bother to load NLSFUNC.
;
;     Rev 1.0   20 Aug 1990 13:01:20   DAVID
;  Initial revision.
;
;-----------------------------------------------------------------------;

%	.MODEL	memoryModel, C

	.DATA

szCON		db	'CON',0

IOCTLData	dw	0002h, 0000h
tempIOCTLData	dw	0002h, 0000h

%	.CODE	segmentName

;-----------------------------------------------------------------------;
; int DOSGetCodePage(void);						;
;									;
; Returns the current code page.					;
; If no code page is available or error, returns 0			;
;-----------------------------------------------------------------------;

DOSGetCodePage	proc	uses si di

        assume  ds:seg IOCTLData

        push    ds
        mov     ax, seg IOCTLData
        mov     ds, ax

        mov     ax,6601h                ;Get active/global code page
	int	21h
	jc	noCodePage		;No code page

; DOS fuction 6601h doesn't work right if NLSFUNC isn't loaded.  It appears
; that not all international users bother to load NLSFUNC.  A more generally
; reliable method of getting the code page is to use IOCTL to query which
; code page is active on the CON: device.

        mov     [IOCTLData+2],bx        ;Use result of 6601H as default
					;if IOCTL doesn't return anything

	mov	ax,3D02h		;DOS Open function
	mov	dx,offset szCON		;"CON" device name
	int	21h
	jc	noCodePage		;Major problem if we can't open CON!
	mov	bx,ax			;Move handle to BX for IOCTL call

	mov	ax,440Ch		;Generic IOCTL for character devices
	mov	cx,036Ah		;Query selected code page subfunction
        mov     dx,offset DGROUP:tempIOCTLData
	int	21h			;Do it!
	jc	failedIOCTL
	mov	ax,[tempIOCTLData+2]
	mov	[IOCTLData+2],ax	;Copy from temp to real CodePage-struct

failedIOCTL:
	mov	ah,3Eh			;DOS Close function
	int	21h			;Close CON device

	mov	ax,[IOCTLData+2]	;Get returned code page
	jmp	short codePageRet

noCodePage:
	xor	ax,ax			;No code page available

codePageRet:
        pop     ds
	ret

DOSGetCodePage	endp

	end
