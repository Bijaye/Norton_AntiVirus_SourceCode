;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\getdosen.asv   1.1   17 Dec 1994 01:03:40   SZIADEH  $
;;
;; Description:
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\getdosen.asv  $
;  
;     Rev 1.1   17 Dec 1994 01:03:40   SZIADEH
;  added UniqueSegmentEnd
;  
;     Rev 1.0   16 Dec 1994 18:23:32   SZIADEH
;  Initial revision.
;  
;
;;---------------------------------------------------------------------------
include ctsn.inc

UniqueSegment

if bSYM_DOS
DOS4_GetEnvironment PROC USES SES SDS XBX,

        mov     ah,62h                   ; Get PSP of program
        int     21h

        mov     es, bx
        mov     dx, es:[2Ch]             ; Segment of ENVIROMENT
        xor     ax, ax

        ret       ; dx:ax
DOS4_GetEnvironment ENDP

endif
UniqueSegmentEnd

END
