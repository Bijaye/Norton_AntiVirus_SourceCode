;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DOSVEC.ASv   1.2   08 Mar 1996 14:56:40   DALLEE  $
;;
;; Description: Defines SYS4_VECTOR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DOSVEC.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:40   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:46   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   24 May 1994 08:24:02   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SYS4_VECTOR
;
; Change the vector requested to a new location.
;
; VOID WINAPI SYS4_VECTOR ( BYTE Vector, LPBYTE SetTo, LPBYTE StoreHere );
;
; Input:
;       1)  Vector number to change
;       2)  Address of new handler.
;       3)  Address to store old value at.
;
;----------------------------------------------------------------------------

IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  SYS4_VECTOR  PROC  CODEDISTANCE  USES XDX XBX SDS SES XSI,
        byVector:BYTE,
        lpHandler:LPBYTE,
        lpStore:LPBYTE

        mov     ah,35h
        mov     al,byVector
        CALLDOS
        LDS_SI  lpStore
        mov     [XSI],bx
        mov     [XSI + 2],es

        mov     ah,25h
        mov     al,byVector
        LDS_DX  lpHandler
        CALLDOS

        ret

  SYS4_VECTOR  ENDP

ELSE
        ECHO Warning: Target platform does not support SYS4_VECTOR.
ENDIF

UniqueSegmentEnd
END
