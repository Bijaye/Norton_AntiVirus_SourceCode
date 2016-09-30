;* Copyright 1992 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/CFGINIT.ASv   1.0   26 Jan 1996 20:21:20   JREARDON  $  *
;*                                                                      *
;* Description:                                                         *
;*	Configuration Manager startup code hook.  This module installs	*
;*	a hook into the Microsoft and Zortech startup code (which is	*
;*	a feature of these products, not a hack!) so the Configuration	*
;*	manager can grab a copy of the command line.  Basically, this	*
;*	module creates a far pointer to the command line, then 		*
;*	immediately passes off control to ConfigProcessCmdLine.		*
;*                                                                      *
;* Contains:                                                            *
;*                                                                      *
;* See Also:                                                            *
;*                                                                      *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/CFGINIT.ASv  $  *
;  
;     Rev 1.0   26 Jan 1996 20:21:20   JREARDON
;  Initial revision.
;  
;     Rev 1.3   27 Apr 1994 15:28:48   BRAD
;  Removed DOS_VERSION define.  Use SYM_DOS
;  
;     Rev 1.2   12 Feb 1993 15:26:44   ENRIQUE
;  No change.
;  
;     Rev 1.1   09 Feb 1993 16:21:10   CRAIG
;  Call NORTONINIT if platform is DOS.
;
;     Rev 1.0   23 Sep 1992 14:06:40   ED
;  Initial revision.
;************************************************************************

%	.MODEL	memoryModel, PASCAL

	EXTRN	__psp:WORD
	EXTRN	CONFIGPROCESSCMDLINE:FAR

IFDEF SYM_DOS
        EXTRN   NORTONINIT:FAR
ENDIF

;; This is a trick to force this code to be linked in to an app
;; along with the switch processing code.  The switch code makes
;; a (benign) reference to this value, so the linker has to resolve
;; it by dragging in this module.

public	__ncfgswitchinit
	__ncfgswitchinit = 0EDEDh

;; --------------------------------------------------------------------------
;; Microsoft and Zortech Hook Segments
;; --------------------------------------------------------------------------
;;
;;  The initialization and termination routines are grouped in threes as
;;  follows:
;;     XIB     "B" for beginning segment
;;      XI             middle segment
;;     XIE     "E" for ending segment
;;  These routines contain pointers to routines that the C run-time
;;  library calls at either startup or termination. Contributions are made
;;  to these segments at link time so that you only get routines that you
;;  need. For example, if you use standard input/output (stdio) routines,
;;  you will get a pointer to the "flushall" routine stored in an ending
;;  segment. A label is put in a beginning segment (for example, "start"
;;  in XIB) and one in the ending segment (for example, "end" in XIE). At
;;  startup/termination, the code walks the table from "start" to "end"
;;  and calls any routine whose pointer is stored in the middle segment
;;  (that is, XI).
;;
;;  And now, a note from Ed:
;;
;;  PUT POINTERS ONLY IN THE MIDDLE SEGMENT.  IF YOU PUT STUFF IN THE
;;  BEGIN OR END SEGMENTS, YOUR STUFF WILL NEVER GET CALLED!
;; -----------------------------------------------------------------------

;Function pointers to far initializers or constructors
XIFB		SEGMENT	WORD PUBLIC 'DATA'
XIFB		ENDS
XIF		SEGMENT	WORD PUBLIC 'DATA'
                DD      _INITCMDLINE
XIF		ENDS
XIFE		SEGMENT	WORD PUBLIC 'DATA'
XIFE		ENDS

;Function pointers to far initializers or destructors
XCFB		SEGMENT	WORD PUBLIC 'DATA'
XCFB		ENDS
XCF		SEGMENT	WORD PUBLIC 'DATA'
XCF		ENDS
XCFE		SEGMENT	WORD PUBLIC 'DATA'
XCFE		ENDS

_TEXT	        SEGMENT WORD PUBLIC 'CODE'

;**************************************************************************
; _InitCmdLine
;
; This function is our hook into the startup code, called by the _cinit
; routine.  All we do is create a far pointer to the command line and
; call the command line initialization function of the Configuration Manager.
; This allows us to copy the command line into our own buffer and parse it
; our way.  Otherwise, the startup code screws it up!
;
;**************************************************************************

_INITCMDLINE	PROC FAR

	assume	ds:_DATA

IFDEF SYM_DOS
        call    NORTONINIT              ; In case not called first (stupid linker)
ENDIF

	mov	dx,__psp
	mov	ax,80h			; DS:AX = far pointer to command line
	push	dx
	push	ax
	call    CONFIGPROCESSCMDLINE
        ret

_INITCMDLINE	ENDP

_TEXT	        ENDS

	        END
