;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\strl.asv   1.1   17 Aug 1994 13:26:32   CAREY  $
;;
;; Description: Defines STR4_LENGTH function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\strl.asv  $
;  
;     Rev 1.1   17 Aug 1994 13:26:32   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   22 Jun 1994 09:43:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; STR4_LENGTH
;
; Return the length of a string, not including NULL.
;
; UINT WINAPI STR4_LENGTH ( LPSTR Buffer );
;
; Input:
;       1)  String
; Output:
;       1)  0 Not found in buffer
;       2)  ? String length
;
;----------------------------------------------------------------------------
STR4_LENGTH  PROC  CODEDISTANCE  USES XCX XSI SDS,
        lpFrom:LPSTR

        cld
        LDS_SI  lpFrom
        xor     XCX,XCX

MoreToDo:
        lodsb
        or      al,al
        jz      Done
        inc     XCX
        jmp     MoreToDo
Done:
        mov     XAX,XCX

        ret

STR4_LENGTH  ENDP

UniqueSegmentEnd
END
