;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fdel.asv   1.11   02 Feb 1996 21:17:12   RSTANEV  $
;;
;; Description: Defines FIL4_DELETE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fdel.asv  $
;  
;     Rev 1.11   02 Feb 1996 21:17:12   RSTANEV
;  Added support for SYM_NTK platform.
;
;     Rev 1.10   20 Dec 1994 14:27:20   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.9   09 Sep 1994 16:59:18   CAREY
;  changed FIL4_DELETE to pass on the callback return value directly (Which is
;  1 on success or 0 on failure).
;
;     Rev 1.8   19 Aug 1994 21:44:58   RSTANEV
;  Fixed compilation errors.
;
;     Rev 1.7   19 Aug 1994 21:44:18   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.6   17 Aug 1994 13:26:40   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.5   08 Aug 1994 18:56:12   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.4   01 Aug 1994 18:08:56   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.3   01 Aug 1994 17:17:26   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.2   29 Jul 1994 13:20:24   CAREY
;
;     Rev 1.1   27 Jul 1994 13:09:00   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.0   31 May 1994 13:40:24   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE AVAPI.INC

UniqueDataSegment

EXTERN          C lpgCallBack:DWORD

UniqueDataSegmentEnd

UniqueSegment


;----------------------------------------------------------------------------
; FIL4_DELETE
;
; Delete a file from the drive.
;
; BOOL WINAPI FIL4_DELETE ( LPSTR Filename );
;
; Input:
;       1)  Filename to delete
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_WIN32 OR bSYM_VXD OR bSYM_NLM OR bSYM_NTK

  FIL4_DELETE  PROC  CODEDISTANCE  USES XDX XBX XCX XSI XDI SDS,
        Filename:LPSTR

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileDelete, Filename

        ret

  FIL4_DELETE  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_DELETE.
ENDIF

UniqueSegmentEnd
END
