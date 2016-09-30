;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fred.asv   1.14   16 Sep 1996 18:35:38   DCHI  $
;;
;; Description: Defines FIL4_READ function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fred.asv  $
;  
;     Rev 1.14   16 Sep 1996 18:35:38   DCHI
;  Removed instruction sequence: jnc J_Done
;                                mov XAX,-1
;                          J_Done:
;  from after invocation of callback function, for which the carry value
;  after return is undefined, but for which the return value is appropriate
;  for returning.
;  
;     Rev 1.13   08 Mar 1996 14:56:54   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.12   02 Feb 1996 21:25:02   RSTANEV
;  Added support for SYM_NTK.
;
;     Rev 1.11   20 Dec 1994 14:26:50   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.10   19 Aug 1994 21:48:30   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.9   18 Aug 1994 23:18:40   RSTANEV
;  Fixed compilation errors.
;
;     Rev 1.8   17 Aug 1994 13:26:40   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.7   08 Aug 1994 18:56:14   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.6   01 Aug 1994 18:08:48   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.5   01 Aug 1994 17:17:36   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.4   29 Jul 1994 13:20:36   CAREY
;
;     Rev 1.3   27 Jul 1994 13:09:04   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.2   26 May 1994 15:13:16   RSTANEV
;  Minor changes.
;
;     Rev 1.1   05 May 1994 12:42:32   RSTANEV
;  Now saving the general registers when calling _fread.
;
;     Rev 1.0   02 May 1994 18:12:18   RSTANEV
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
; FIL4_READ
;
; Read bytes from a file handle.
;
; UINT WINAPI FIL4_READ ( UINT Handle, LPBYTE Buffer, UINT Bytes );
;
; Input:
;       1)  Handle
;       2)  Buffer to use
;       3)  Number of bytes to read
; Output:
;       1)  Number of bytes read
;       2)  -1 if file error
;
;----------------------------------------------------------------------------

IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_WIN32 OR bSYM_VXD OR bSYM_NLM OR bSYM_NTK

  FIL4_READ  PROC  CODEDISTANCE  USES XBX XCX XDX SDS XSI XDI,
        Handle:UINT,
        Buffer:LPBYTE,
        Count:UINT

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileRead, Handle, Buffer, Count
        ret

  FIL4_READ  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_READ.
ENDIF

UniqueSegmentEnd

END
