;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\ffindc.asv   1.1   17 Aug 1994 13:25:52   CAREY  $
;;
;; Description: Defines ex_FindClose function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\ffindc.asv  $
;  
;     Rev 1.1   17 Aug 1994 13:25:52   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   20 May 1994 07:42:16   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; ex_FindClose
;
; Uses LFN DOS support to close a file find handle
;
; UINT WINAPI ex_FindClose ( HANDLE32 hFFHandle );
;
; Input:
;       1)  HANDLE32 handle returned by ex_FindFirstFile
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?) (DOS Error's)
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ex_FindClose  PROC  CODEDISTANCE  USES XBX,
        hHandle:HANDLE32

        mov     ax,71A1h
        mov     bx,hHandle
        CALLDOS
        jc      FunctionEnd
        xor     XAX,XAX
  FunctionEnd:
        ret

  ex_FindClose  ENDP

ELSE
        ECHO Warning: Target platform does not support ex_FindClose.
ENDIF

UniqueSegmentEnd
END
