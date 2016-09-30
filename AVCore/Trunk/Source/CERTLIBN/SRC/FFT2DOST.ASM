;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FFT2DOST.ASv   1.2   08 Mar 1996 14:56:52   DALLEE  $
;;
;; Description: Defines ex_FileTimeToDosDateTime function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FFT2DOST.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:52   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:54   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   20 May 1994 08:29:02   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; ex_FileTimeToDosDateTime
;
; Convert the time from FILETIME structure to DOS time.
;
; BOOL WINAPI ex_FileTimeToDosDateTime ( FILETIME FAR * lprFileTime,
;                                        LPWORD lpwDate, LPWORD lpwTime );
;
; Input:
;       1)  Pointer to FILETIME structure to convert
;       2)  Pointer to a WORD to place Date
;       3)  Pointer to a WORD to place Time
;
; Output:
;       1)  ERROR (0)
;       2)  PASSED (1)
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ex_FileTimeToDosDateTime  PROC  CODEDISTANCE USES XBX XCX XDX XSI SDS,
        lprFileTime:LPBYTE,
        lpwDate:LPBYTE,
        lpwTime:LPBYTE

        mov     ax,71A7h                ; Extended function
        LDS_SI  lprFileTime             ; Get the filetime
        xor     bx,bx                   ; FileTime->DosTime
        stc                             ; Assume not supported
        CALLDOS
        mov     ax,0
        jc      EnoughOfThis
        inc     ax
        LDS_SI  lpwDate
        mov     XDS[XBX],dx             ; Set DOS Date
        LDS_SI  lpwTime
        mov     XDS[XBX],cx             ; Set DOS Time
  EnoughOfThis:
        ret

  ex_FileTimeToDosDateTime  ENDP

ELSE
        ECHO Warning: Target platform does not support ex_FileTimeToDosDateTime.
ENDIF

UniqueSegmentEnd
END
