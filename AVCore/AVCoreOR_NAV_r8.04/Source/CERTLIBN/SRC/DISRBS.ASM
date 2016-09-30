;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DISRBS.ASv   1.2   08 Mar 1996 14:56:38   DALLEE  $
;;
;; Description: Defines DIS4_READ_BOOTSECTOR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DISRBS.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:38   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:44   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   25 May 1994 12:46:40   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_READ_BOOTSECTOR
;
; Reads the boot sector and stores in buffer.
;
; UINT WINAPI DIS4_READ_BOOTSECTOR ( BYTE Drive, LPBYTE Buffer );
;
; Input:
;       1)  Drive to read boot sector
;       2)  Pointer to buffer 1024 bytes minimum.
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
;----------------------------------------------------------------------------
DIS4_READ_BOOTSECTOR  PROC  CODEDISTANCE  USES XDX,
        Drive:BYTE,
        Buffer:LPBYTE

                                        ; find the location of bootsector
        INVOKE  DIS4_LOC_BOOTSECTOR,Drive,Buffer
        mov     bx,ax
        inc     ax
        jz      Err
                                        ; read the bootsector
        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,dx,bl,bh,Drive
        or      ax,ax
        jz      Done
Err:
        mov     ax,-1
Done:
IF OS_32
        cwde
ENDIF
        ret

DIS4_READ_BOOTSECTOR  ENDP

UniqueSegmentEnd
END
