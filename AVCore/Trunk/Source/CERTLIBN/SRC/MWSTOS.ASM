;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/mwstos.asv   1.2   31 Jan 1996 21:56:46   MKEATIN  $
;;
;; Description: Defines MEM4_SEGTOSEL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/mwstos.asv  $
;  
;     Rev 1.2   31 Jan 1996 21:56:46   MKEATIN
;  Added DX support
;  
;     Rev 1.1   17 Aug 1994 13:26:02   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   31 May 1994 15:28:46   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

IFDEF SYM_DOSX
   bSYM_WIN16 = TRUE
ENDIF

UniqueSegment

;----------------------------------------------------------------------------
; MEM4_SEGTOSEL
;
; Convert a DOS segment to a Windows selector.
;
; WORD WINAPI MEM4_SEGTOSEL ( WORD Segment );
;
; Input:
;       1)  DOS segment to convert to a selector
; Output:
;       1)  Error (0)
;       2)  Selector to use
;----------------------------------------------------------------------------
IF bSYM_WIN16

  MEM4_SEGTOSEL  PROC  CODEDISTANCE  USES XBX,
          uSegment:WORD

        mov     bx,uSegment
        mov     ax,0002h
        int     31h
        jnc     J_Done
        xor     ax,ax
  J_Done:
        ret

  MEM4_SEGTOSEL ENDP

ELSE
        ECHO Warning: Target platform does not support MEM4_SEGTOSEL.
        ; IFDEF MIRRORS, this function does nothing, but returns uSegment.
ENDIF

UniqueSegmentEnd
END
