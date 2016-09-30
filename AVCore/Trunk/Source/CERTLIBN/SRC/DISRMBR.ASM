;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DISRMBR.ASv   1.3   08 Mar 1996 14:56:40   DALLEE  $
;;
;; Description: Defines DIS4_READ_MBR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DISRMBR.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:40   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Aug 1994 13:25:44   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   26 May 1994 14:01:30   RSTANEV
;  Minor change.
;
;     Rev 1.0   25 May 1994 17:26:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_READ_MBR
;
; Read the master boot record and place it in lpBuffer.
;
; UINT WINAPI DIS4_READ_MBR ( BYTE Drive, LPBYTE lpBuffer );
;
; Input:
;       1)  Drive to read.
;       2)  Buffer to hold MBR.
; Output:
;       1)  (-1) failed
;       2)  0 Success
;----------------------------------------------------------------------------
DIS4_READ_MBR  PROC  CODEDISTANCE,
        Drive:BYTE,
        Buffer:LPBYTE

        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,0,1,0,Drive
        or      XAX,XAX
        jz      Done
Err:
        mov     XAX,-1
Done:
        ret

DIS4_READ_MBR  ENDP

UniqueSegmentEnd
END
