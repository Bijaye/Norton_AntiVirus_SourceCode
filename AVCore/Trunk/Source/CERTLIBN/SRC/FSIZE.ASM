;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FSIZE.ASv   1.4   02 Feb 1996 21:29:00   RSTANEV  $
;;
;; Description: Defines FIL4_SIZE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FSIZE.ASv  $
;  
;     Rev 1.4   02 Feb 1996 21:29:00   RSTANEV
;  Added support for SYM_NTK.
;
;     Rev 1.3   17 Aug 1994 13:26:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.2   26 May 1994 15:23:22   RSTANEV
;  Minor changes.
;
;     Rev 1.1   19 May 1994 16:55:56   RSTANEV
;  Fixed compilation errors.
;
;     Rev 1.0   02 May 1994 18:20:06   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_SIZE
;
; Return the size of a file.
;
; DWORD WINAPI FIL4_SIZE ( UINT Handle );
;
; Input:
;       1)  Handle
; Output:
;       1)  File size
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_WIN32 OR bSYM_NLM or bSYM_VXD OR bSYM_NTK

  FIL4_SIZE  PROC  CODEDISTANCE,
        Handle:UINT

        LOCAL   Current:DWORD

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_FOR

IF OS_16
        mov     WORD PTR Current + 2,dx
        mov     WORD PTR Current,ax
ELSE
        mov     Current, XAX
ENDIF

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_END

IF OS_16
        rpush   XDX,XAX
ELSE
        push    XAX
ENDIF

        INVOKE  FIL4_SEEK,Handle,Current,fSEEK_BEG

IF OS_16
        rpop    XAX,XDX
ELSE
        pop     XAX
ENDIF

        ret

  FIL4_SIZE  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_SIZE.
ENDIF

UniqueSegmentEnd
END
