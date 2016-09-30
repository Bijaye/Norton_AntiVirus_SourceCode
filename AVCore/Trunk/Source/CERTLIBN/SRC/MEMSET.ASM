;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/MEMSET.ASv   1.2   08 Mar 1996 14:57:12   DALLEE  $
;;
;; Description: Defines MEM4_SET function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/MEMSET.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:57:12   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:26:00   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   31 May 1994 14:06:26   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; MEM4_SET
;
; Set the memory buffer to this character.
;
; void WINAPI MEM4_SET ( LPVOID Buffer, BYTE Char, UINT Size );
;
; Input:
;       1)  Buffer to set
;       2)  Char to set to
;       3)  Number of bytes to set
; Output:
;       ---
;----------------------------------------------------------------------------
MEM4_SET  PROC  CODEDISTANCE  USES XCX SES XDI,
        lpBuffer:LPBYTE,
        byChar:BYTE,
        uSize:UINT

        cld
        LES_DI  lpBuffer
        mov     XCX,uSize
        mov     al,byChar
        rep     stosb
        ret

MEM4_SET  ENDP

UniqueSegmentEnd
END
