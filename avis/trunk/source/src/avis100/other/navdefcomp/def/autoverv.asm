;===================================================================
;
; File name:     autoverv.asm
;
; Description:   binary virus verification & repair - scaffolding.
;
; Statement:     Licensed Materials - Property of IBM
;                (c) Copyright IBM Corp. 1998-1999
;
; Author:        Andy Raybould
;
;                U.S. Government Users Restricted Rights - use,
;                duplication or disclosure restricted by GSA ADP
;                Schedule Contract with IBM Corp.
;
;
;===================================================================

.MODEL TINY

CSEG    SEGMENT 'CODE'
        ASSUME  CS:CSEG
        org     0h

include autoverv.dcl                    ; shared declarations

Start:
        org     8000h

   ; set ds = cs
	mov     bx,cs
	mov     ds,bx

   ; set up the parameters
   mov   bx, offset Instructions
   mov   cx, offset InstrEnd

   ; call the autoverv routine in the PAM/ERS foundation.
	int	0EBh
	jc	Fail

OK:
	mov	ax, 0FD00h
        int	21h

Fail:
	mov	ax, 0FDFFh
        int	21h

;------------------------------------------------------------------------------

Instructions:
	INCLUDE AUTOVERV.ERS
InstrEnd:

;------------------------------------------------------------------------------

CSEG    ENDS

end Start
end
