;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fdtime.asv   1.12   02 Feb 1996 21:21:16   RSTANEV  $
;;
;; Description: Defines FIL4_DATETIME function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fdtime.asv  $
;  
;     Rev 1.12   02 Feb 1996 21:21:16   RSTANEV
;  Added support for SYM_NTK platform.
;
;     Rev 1.11   20 Dec 1994 14:27:08   JMILLARD
;
;  add SYM_NLM
;
;     Rev 1.10   19 Aug 1994 21:46:36   RSTANEV
;  Using new data segment convention.
;
;     Rev 1.9   18 Aug 1994 23:22:50   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.8   17 Aug 1994 13:26:34   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.7   09 Aug 1994 16:56:56   CAREY
;  fixed to use new callbacks
;
;     Rev 1.6   08 Aug 1994 18:56:12   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.5   01 Aug 1994 18:08:52   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.4   01 Aug 1994 17:17:34   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.3   01 Aug 1994 14:55:20   CAREY
;
;     Rev 1.2   29 Jul 1994 13:20:28   CAREY
;
;     Rev 1.1   27 Jul 1994 13:09:00   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.0   23 May 1994 16:34:28   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------


TIME_LASTWRITE      equ     0
TIME_CREATION       equ     2
TIME_LASTACCESS     equ     4


INCLUDE CTSN.INC
INCLUDE AVAPI.INC

UniqueDataSegment

EXTERN          C lpgCallBack:DWORD

UniqueDataSegmentEnd

UniqueSegment


;----------------------------------------------------------------------------
; FIL4_DATETIME
;
; Get or set the file date and time.
;
; DWORD WINAPI FIL4_DATETIME ( UINT Handle, WORD Date,
;                              WORD Time, UINT Flag );
;
; Input:
;       1)  Handle of file
;       2)  Date to set to if Flag = SET_INFO
;       3)  Time to set to if Flag = SET_INFO
;       4)  Flag = GET_INFO/SET_INFO
; Output:
;       1)  FAILED (0) (If Flag = SET_INFO)
;       2)  HIWORD = DATE, LOWORD = TIME
;
;----------------------------------------------------------------------------

IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16 OR bSYM_NLM

  FIL4_DATETIME  PROC  CODEDISTANCE  USES XBX XCX XSI XDI SDS,
        Handle:UINT,
        Date:WORD,
        Time:WORD,
        Flags:UINT

        LOCAL   uDOSDate:UINT
        LOCAL   uDOSTime:UINT

        cmp     Flags, SET_INFO
        je      SetDateTime

        ; get the date/time

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF
        LDS_SI  DWORD PTR lpgCallBack
        INVOKE  [XSI].CALLBACKREV1.FileGetDateTime, Handle, TIME_LASTWRITE, ADDR uDOSDate, ADDR uDOSTime
        cmp     XAX, 0                  ; FALSE = ERROR!
        je      J_Error

  Load_Return_Value:

        mov     XDX, uDOSDate
        mov     XAX, uDOSTime

        jmp     SHORT EnoughOfThis

  SetDateTime:

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  DWORD PTR lpgCallBack
        INVOKE  [XSI].CALLBACKREV1.FileSetDateTime, Handle, TIME_LASTWRITE, Date, Time
        cmp     XAX, 0
        jne     Load_Return_Value

J_Error:

        xor     XAX, XAX                ; error!
        xor     XDX, XDX

EnoughOfThis:

        ret

  FIL4_DATETIME  ENDP

ELSEIF bSYM_WIN32 OR bSYM_VXD OR bSYM_NTK

  FIL4_DATETIME  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SDS,
        Handle:UINT,
        Date:WORD,
        Time:WORD,
        Flags:UINT

        LOCAL   uDOSDate:UINT
        LOCAL   uDOSTime:UINT

        cmp     Flags, SET_INFO
        je      SetDateTime

        ; get the date/time

        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileGetDateTime, Handle, TIME_LASTWRITE, ADDR uDOSDate, ADDR uDOSTime
        cmp     XAX, 0
        je      J_Error

  Load_Return_Value:

        mov     ax, word ptr uDOSDate
        shl     XAX,10h
        mov     ax, word ptr uDOSTime

        jmp     SHORT EnoughOfThis

  SetDateTime:
        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileSetDateTime, Handle, TIME_LASTWRITE, Date, Time
        cmp     XAX, 0
        jne     Load_Return_Value

  J_Error:

        xor     XAX, XAX                ; error!

  EnoughOfThis:

        ret

  FIL4_DATETIME  ENDP


ELSE
        ECHO Warning: Target platform does not support FIL4_DATETIME.
ENDIF

UniqueSegmentEnd

END
