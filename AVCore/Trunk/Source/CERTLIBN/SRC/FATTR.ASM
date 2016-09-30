;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fattr.asv   1.13   02 Feb 1996 21:08:32   RSTANEV  $
;;
;; Description: Defines FIL4_ATTR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fattr.asv  $
;  
;     Rev 1.13   02 Feb 1996 21:08:32   RSTANEV
;  Added support for SYM_NTK platform.
;
;     Rev 1.12   25 May 1995 09:56:10   RSTANEV
;  bSYM_??? are always defined to TRUE or FALSE, so IFDEF bSYM_??? will always
;  be true.  Use bSYM_??? with an IF!
;
;     Rev 1.11   13 Feb 1995 14:48:10   SZIADEH
;  save es di ds si
;
;     Rev 1.10   20 Dec 1994 14:27:46   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.9   19 Aug 1994 21:40:56   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.8   18 Aug 1994 23:25:06   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.7   17 Aug 1994 13:26:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.6   08 Aug 1994 18:56:10   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.5   01 Aug 1994 18:08:54   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.4   01 Aug 1994 17:17:32   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.3   29 Jul 1994 13:20:16   CAREY
;
;     Rev 1.2   27 Jul 1994 13:08:58   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.1   01 Jun 1994 11:03:48   RSTANEV
;  Fixed a stupid bug.
;
;     Rev 1.0   31 May 1994 13:21:36   RSTANEV
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
; FIL4_ATTR
;
; Get or set the file attributes.
;
; UINT WINAPI FIL4_ATTR ( LPSTR Filename, UINT Attr, UINT Flag );
;
; Input:
;       1)  Filename
;       2)  Attribute to change to if SET_INFO in FLAG
;       1)  Flag = GET_INFO or SET_INFO
; Output:
;       1)  ERROR (-1) if failed
;           PASSED (?) SET_INFO
;           ATTR if GET_INFO
;----------------------------------------------------------------------------

IF bSYM_WIN32 OR bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_VXD OR bSYM_NLM OR bSYM_NTK

  FIL4_ATTR  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        Filename:LPSTR,
        Attr:UINT,
        Flags:UINT

        LOCAL   uAttr:UINT

        cmp     Flags,SET_INFO
        je      SetFlags

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileGetAttr, Filename, ADDR uAttr
        cmp     XAX, -1
        je      J_Done

        mov     XAX, uAttr
        jmp     J_Done

  SetFlags:

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF

        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileSetAttr, Filename, Attr

        ; return undefined

J_Done:

        ret

  FIL4_ATTR  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_ATTR.
ENDIF

UniqueSegmentEnd

END
