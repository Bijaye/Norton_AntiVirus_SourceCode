;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_SYS.ASv   1.3   10 Apr 1996 15:10:50   DALLEE  $
;;
;; Description: Defines REP4_SYS_NORM function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_SYS.ASv  $
;  
;     Rev 1.3   10 Apr 1996 15:10:50   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   23 Aug 1994 20:37:30   RSTANEV
;  Using the INC file with a prototype so that the function goes public.
;
;     Rev 1.1   17 Aug 1994 13:26:18   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.0   23 May 1994 16:59:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_SYS_NORM
;
; Does repair on Norm SYS infections (INVOL type).
;
; BOOL WINAPI REP4_SYS ( UINT HostHandle, UINT uBuffer, LPVOID lpBuffer );
;
; Input:
;       1) Handle
;       2) Buffer size
;       3) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;----------------------------------------------------------------------------
REP4_SYS_NORM  PROC  CODEDISTANCE,
        HostHandle:UINT,                ; Handle of infected file
        uBuffer:UINT,                   ; Size of work buffer
        lpBuffer:LPBYTE                 ; Ptr to my work buffer

        ECHO Warning: I see this function is doing hell of a job.

        xor     XAX,XAX

        ret

REP4_SYS_NORM  ENDP

UniqueSegmentEnd
END
