;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\fclose.asv   1.11   19 Aug 1994 21:43:14   RSTANEV  $
;;
;; Description: Defines FIL4_CLOSE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\fclose.asv  $
;  
;     Rev 1.11   19 Aug 1994 21:43:14   RSTANEV
;  Using new data segment convention.
;
;     Rev 1.10   18 Aug 1994 23:26:26   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.9   17 Aug 1994 13:25:50   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.8   08 Aug 1994 18:56:10   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.7   01 Aug 1994 18:08:44   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.6   01 Aug 1994 17:17:38   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.5   29 Jul 1994 13:20:22   CAREY
;
;     Rev 1.4   27 Jul 1994 13:08:58   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.3   26 May 1994 15:09:36   RSTANEV
;  Wrong PVCS log.
;
;     Rev 1.2   26 May 1994 15:08:42   RSTANEV
;  Minor changes.
;
;     Rev 1.1   05 May 1994 12:40:42   RSTANEV
;  Now saving the general registers when calling _lclose.
;
;     Rev 1.0   02 May 1994 18:04:48   RSTANEV
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
; FIL4_CLOSE
;
; Close a file handle.
;
; UINT WINAPI FIL4_CLOSE ( UINT Handle );
;
; Input:
;       1)  Handle to close
; Output:
;        0 if successful
;       -1 if error
;----------------------------------------------------------------------------

  FIL4_CLOSE  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SDS,
        Handle:UINT

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI    <DWORD PTR lpgCallBack>
        INVOKE    [XSI].CALLBACKREV1.FileClose, Handle

        ret

  FIL4_CLOSE  ENDP


UniqueSegmentEnd
END
