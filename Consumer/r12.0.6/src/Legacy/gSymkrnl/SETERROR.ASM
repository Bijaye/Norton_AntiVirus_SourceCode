        PAGE    ,132
;* Copyright 1992 Symantec Corporation                                  *
;************************************************************************
;*                                                                      *
;* $Header:   S:/SYMKRNL/VCS/SETERROR.ASv   1.0   26 Jan 1996 20:21:26   JREARDON  $  *
;*                                                                      *
;* Description:                                                         *
;*      The SetErrorMode function.  Use of this function will install   *
;*      hooks into the startup code for DiskErrorOn and DiskErrorOff.   *      
;*                                                                      *
;* Contains:                                                            *
;*                                                                      *
;* See Also:                                                            *
;*      Ed Carlin                                                       *
;************************************************************************
;* $Log:   S:/SYMKRNL/VCS/SETERROR.ASv  $  *
;  
;     Rev 1.0   26 Jan 1996 20:21:26   JREARDON
;  Initial revision.
;  
;     Rev 1.2   23 May 1993 18:04:14   BRAD
;  DiskErrorOn() had been changed to PASCAL, rather than CDECL.
;  
;     Rev 1.1   12 Feb 1993 04:30:20   ENRIQUE
;  No change.
;  
;     Rev 1.0   02 Sep 1992 11:26:12   ED
;  Initial revision.
;************************************************************************

%       .MODEL   memoryModel, PASCAL

        include mixed.inc

        extrn   _bErrorModeOff:WORD

        extrn   _DiskErrorOff:PROC              ; this is CDECL
        extrn   DISKERRORON:PROC

;;Function pointers to far initializers or constructors
XIFB		SEGMENT	WORD PUBLIC 'DATA'
XIFB		ENDS
XIF		SEGMENT	WORD PUBLIC 'DATA'
                DD      INITDISKERROR
XIF		ENDS
XIFE		SEGMENT	WORD PUBLIC 'DATA'
XIFE		ENDS

;;Function pointers to far initializers or destructors
XCFB		SEGMENT	WORD PUBLIC 'DATA'
XCFB		ENDS
XCF		SEGMENT	WORD PUBLIC 'DATA'
		DD	_DiskErrorOff
XCF		ENDS
XCFE		SEGMENT	WORD PUBLIC 'DATA'
XCFE		ENDS

        .CODE

;************************************************************************
;*                                                                      *
;* Description:                                                         *
;*      DOS implementation of the Windows SetErrorMode function.        *
;*      Basically, this enables or disables critical error reporting,   *
;*      which in DOS code is equivalent to DiskErrorOn/Off calls.       *
;*                                                                      *
;* Parameters:                                                          *
;*      fuErrorMode     TRUE if critical error dialog should not        *
;*                      appear, even if an error dialog exists.         *
;*                      FALSE to allow critical errors through to       *
;*                      hit the dialog if it exists.                    *
;*                                                                      *
;* Return Value:                                                        *
;*      The previous state, which should be saved and restored after    *
;*      making a call that might cause a critical error.                *
;*                                                                      *
;* Registers:                                                           *
;*                                                                      *
;* See Also:                                                            *
;*      Windows Programmer's Reference                                  *
;************************************************************************
;* 08/28/1992 ED Created.                                               *
;************************************************************************

SETERRORMODE    proc    far     fuErrorMode:WORD

                mov     dx,fuErrorMode          ; get state to set
                mov     ax,_bErrorModeOff       ; get current state
                mov     _bErrorModeOff,dx       ; set current state

                ret

SETERRORMODE    endp


;; -------------------------------------------------------------------------
;; This just does a DiskErrorOn(-1) call.
;; -------------------------------------------------------------------------

INITDISKERROR   proc    far

                xor     ax,ax                   ; AX = 0
                mov     _bErrorModeOff,ax       ; clear the SetErrorMode() state
                dec     ax                      ; AX = -1
                push    ax
                push    ax
                call    DISKERRORON
                add     sp,4
                ret

INITDISKERROR   endp

                end
