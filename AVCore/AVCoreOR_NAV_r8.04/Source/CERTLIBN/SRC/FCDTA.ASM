;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FCDTA.ASv   1.2   08 Mar 1996 14:56:50   DALLEE  $
;;
;; Description: Defines FIL4_CHANGE_DTA function.
;;
;; See Also: FGDTA.ASM
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FCDTA.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:50   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:50   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   20 May 1994 07:50:48   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_CHANGE_DTA
;
; Change the DTA and return pointer to old DTA.
;
; LPSTR WINAPI FIL4_CHANGE_DTA ( LPBYTE lpNewDTA );
;
; Input:
;       1)  Address to change DTA to
; Output:
;       2)  Original DTA before change.
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_CHANGE_DTA  PROC  CODEDISTANCE  USES XBX SDS SES,
        lpNewDTA:LPBYTE

        mov     ah,2Fh                  ; Get Current DTA
        CALLDOS
        LDS_DX  lpNewDTA                ; Set to this DTA
        mov     ah,1Ah
        int     21h
        mov     dx,es
        mov     ax,bx
        ret

  FIL4_CHANGE_DTA  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_CHANGE_DTA.
ENDIF

UniqueSegmentEnd
END
