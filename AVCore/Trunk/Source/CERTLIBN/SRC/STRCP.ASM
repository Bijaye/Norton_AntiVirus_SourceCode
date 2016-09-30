;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\strcp.asv   1.1   17 Aug 1994 13:26:30   CAREY  $
;;
;; Description: Defines STR4_COPY function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\strcp.asv  $
;  
;     Rev 1.1   17 Aug 1994 13:26:30   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   22 Jun 1994 16:47:12   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; STR4_COPY
;
; Copy a string, including NULL.
;
; UINT WINAPI STR4_COPY ( LPSTR FromBuffer, LPSTR ToString );
;
; Input:
;       1)  Copy from here
;       2)  Copy to here
; Output:
;       1)  0 Not found in buffer
;       2)  ? Number of bytes copied
;
;----------------------------------------------------------------------------
STR4_COPY  PROC  CODEDISTANCE  USES XCX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        xor     XCX,XCX

  MoreToDo:
        lodsb
        stosb
        or      al,al
        jz      Done
        inc     XCX
        jmp     SHORT MoreToDo
  Done:
        mov     XAX,XCX

        ret

STR4_COPY  ENDP

UniqueSegmentEnd
END
