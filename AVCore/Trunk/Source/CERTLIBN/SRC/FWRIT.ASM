;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fwrit.asv   1.14   16 Sep 1996 18:36:08   DCHI  $
;;
;; Description: Defines FIL4_WRITE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fwrit.asv  $
;  
;     Rev 1.14   16 Sep 1996 18:36:08   DCHI
;  Removed instruction sequence: jnc J_Done
;                                mov XAX,-1
;                          J_Done:
;  from after invocation of callback function, for which the carry value
;  after return is undefined, but for which the return value is appropriate
;  for returning.
;  
;     Rev 1.13   08 Mar 1996 14:56:56   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.12   02 Feb 1996 21:29:12   RSTANEV
;  Added support for SYM_NTK.
;
;     Rev 1.11   20 Dec 1994 14:28:06   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.10   19 Aug 1994 21:49:02   RSTANEV
;  Using new data segment convention.
;
;     Rev 1.9   18 Aug 1994 23:27:28   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.8   17 Aug 1994 13:26:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.7   08 Aug 1994 18:56:14   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.6   01 Aug 1994 18:08:50   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.5   01 Aug 1994 17:17:34   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.4   29 Jul 1994 13:20:44   CAREY
;
;     Rev 1.3   27 Jul 1994 13:09:06   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.2   26 May 1994 15:25:28   RSTANEV
;  Minor changes.
;
;     Rev 1.1   17 May 1994 15:14:34   RSTANEV
;  Wrong function name.
;
;     Rev 1.0   11 May 1994 16:11:04   RSTANEV
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
; FIL4_WRITE
;
; Write bytes to a file handle.
;
; UINT WINAPI FIL4_WRITE ( UINT Handle, LPBYTE Buffer, UINT Bytes );
;
; Input:
;       1)  Handle to write to
;       2)  Buffer to write
;       3)  Number of bytes to write
; Output:
;       1)  Number of bytes written.
;       2)  -1 if error
;
;----------------------------------------------------------------------------

IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_WIN32 or bSYM_VXD OR bSYM_NLM OR bSYM_NTK

  FIL4_WRITE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS XSI XDI,
        Handle:UINT,
        Buffer:LPBYTE,
        Count:UINT

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileWrite, Handle, Buffer, Count
        ret

  FIL4_WRITE  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_WRITE.
ENDIF

UniqueSegmentEnd

END
