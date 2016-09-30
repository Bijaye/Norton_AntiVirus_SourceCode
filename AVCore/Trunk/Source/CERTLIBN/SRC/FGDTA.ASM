;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FGDTA.ASv   1.3   08 Mar 1996 14:56:52   DALLEE  $
;;
;; Description: Defines FIL4_GET_DTA function.
;;
;; See Also: FCDTA.ASM
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FGDTA.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:52   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Aug 1994 13:25:56   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   26 May 1994 15:37:38   RSTANEV
;  Changed prototype to return LPSTR instead of DWORD.
;
;     Rev 1.0   20 May 1994 07:51:00   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_GET_DTA
;
; Get the current DTA and return;
;
; LPBYTE WINAPI FIL4_GET_DTA ( void );
;
; Output:
;       1)  Pointer to DTA.
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_GET_DTA  PROC  CODEDISTANCE  USES XBX SES

        mov     ah,2Fh                  ; Get Current DTA
        CALLDOS
        mov     dx,es
        mov     ax,bx
        ret

  FIL4_GET_DTA  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_GET_DTA.
ENDIF

UniqueSegmentEnd
END
