;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DOSUNV.ASv   1.2   08 Mar 1996 14:56:40   DALLEE  $
;;
;; Description: Defines SYS4_UNVECTOR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DOSUNV.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:40   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:46   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   24 May 1994 08:23:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SYS4_UNVECTOR
;
; Unvector a previously trapped vector.
;
; BOOL WINAPI SYS4_UNVECTOR ( BYTE Vector, FARPROC Hooked, FAR *OrigVect );
;
; Input:
;       1)  Vector to unvector
;       2)  The replacement vector to undo.
;       3)  Address where the original vector was stored.
; Output:
;       1)  TRUE Passed (1)
;       2)  FALSE Failed (0)
;
; Note:
;       This routine will take an image of what the replacement vector
;       was, get the address of the current vector, compare the two
;       then use the offset of OrigVect with the Segment of the replacement
;       vector to change the vector back.
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  SYS4_UNVECTOR  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        byVector:BYTE,
        lpHooked:LPBYTE,
        lpOrig:LPBYTE

                                        ; Get the vector
        mov     ah,35h
        mov     al,byVector
        CALLDOS

        mov     di,bx
                                        ; Look at the first 20 bytes
        LDS_SI  lpHooked
        mov     cx,10
        cld
        repe    cmpsw
        jne     NotSame

                                        ; Same then get address (offset) to
                                        ; get unvector number
        mov     di,WORD PTR lpOrig      ; Offset of where value was stored
        LDS_DX  XES[XDI]                ; DS:DX = Original vector

        mov     ah,25h
        CALLDOS                         ; Revector
  IsSame:
        mov     XAX,TRUE
        jmp     Done

  NotSame:
        xor     XAX,XAX
  Done:
        ret

  SYS4_UNVECTOR  ENDP

ELSE
        ECHO Warning: Target platform does not support SYS4_UNVECTOR.
ENDIF

UniqueSegmentEnd
END
