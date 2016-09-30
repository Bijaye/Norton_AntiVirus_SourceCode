;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\strcomp.asv   1.2   09 Dec 1994 20:30:24   SZIADEH  $
;;
;; Description: Defines STR4_COMPARE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\strcomp.asv  $
;  
;     Rev 1.2   09 Dec 1994 20:30:24   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.1   08 Dec 1994 20:28:46   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;;---------------------------------------------------------------------------


INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; STR4_COMPARE
;
; Compare two string for an exact match.
;
; BOOL WINAPI STR4_COMPARE ( LPSTR String1, LPSTR String2 );
;
; Input:
;       1)  String to compare
;       2)  String to compare to
; Output:
;       1)  TRUE (1)  Match
;       2)  FALSE (0) Not match
;
;-----------------------------------------------------------------------------
STR4_COMPARE  PROC  CODEDISTANCE  USES SDS SES XSI XDI,
        lpString1:LPSTR,
        lpString2:LPSTR

        cld
        xor     XAX,XAX
        LDS_SI  lpString1
        LES_DI  lpString2

more_test_compair1055:
        cmpsb
        jne     didnotmatch
        cmp     [XSI - 1],BYTE PTR 0
        jnz     more_test_compair1055
        mov     XAX,TRUE

didnotmatch:

        ret

STR4_COMPARE ENDP
;----------------------------------------------------------------------------
UniqueSegmentEnd
END
