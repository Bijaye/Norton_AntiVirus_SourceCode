;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\fopen.asv   1.15   19 Dec 1994 19:28:56   BRAD  $
;;
;; Description: Defines FIL4_OPEN function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\fopen.asv  $
;  
;     Rev 1.15   19 Dec 1994 19:28:56   BRAD
;  Pass 0 to create, not flags.  This was incorrectly changed
;  
;     Rev 1.14   19 Aug 1994 21:47:18   RSTANEV
;  Using new data segment convention.
;
;     Rev 1.13   18 Aug 1994 23:24:02   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.12   17 Aug 1994 13:25:50   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.11   08 Aug 1994 18:56:12   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.10   01 Aug 1994 18:08:20   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.9   01 Aug 1994 17:17:08   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.8   29 Jul 1994 13:20:32   CAREY
;
;     Rev 1.7   27 Jul 1994 17:21:14   CAREY
;  The code now passes the attributes to the FileCreate callback...
;
;     Rev 1.6   27 Jul 1994 13:09:02   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.5   26 May 1994 15:06:08   RSTANEV
;  Minor changes.
;
;     Rev 1.4   17 May 1994 14:18:46   RSTANEV
;  Fixed international stuff.
;
;     Rev 1.3   12 May 1994 16:31:22   RSTANEV
;  Added support for DOS 7 extended file open/create functions.
;
;     Rev 1.2   05 May 1994 12:41:26   RSTANEV
;  Now saving the general registers when calling _lopen.
;
;     Rev 1.1   02 May 1994 18:50:34   RSTANEV
;  A quick fix.
;
;     Rev 1.0   02 May 1994 18:01:48   RSTANEV
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
; FIL4_OPEN
;
; Open a file for a paticular mode.
;
; UINT WINAPI FIL4_OPEN ( LPSTR Name, UINT OpenMode )
;
; Input:
;       1)  Filename (in target platform's native character set)
;       2)  Open mode:
;                  fREAD
;                  fWRITE
;                  fREAD_WRITE
;
;                  fSHARE_COMPAT
;                  fSHARE_EXCLUSIVE
;                  fSHARE_DENY_WRITE
;                  fSHARE_DENY_READ
;                  fSHARE_DENY_NONE
;
;                  fCREATE         Create a file (will be destoyed if exists)
;                  fAPPEND         If not found, create; if found, go to end
; Output:
;       1)  FAILED (-1)
;       2)  Handle
;
;----------------------------------------------------------------------------

  FIL4_OPEN  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SDS,
        FileName:LPSTR,
        OpenMode:UINT

        mov     XBX,OpenMode            ; Let XBX be the keeper of OpenMode

        test    XBX,fCREATE
        jnz     Create

        push    XBX                     ; Save OpenMode
        and     XBX,01110011y           ; Keep known modes only

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF

        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileOpen, FileName, XBX

        pop     XBX                     ; Restore OpenMode

        cmp     XAX, -1
        jne     CheckForAppend

        test    XBX,fAPPEND
        jz      EnoughOfThis

  Create:

IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF

        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.FileCreate, FileName, 0

        jmp     SHORT EnoughOfThis

  CheckForAppend:
        test    XBX,fAPPEND
        jz      EnoughOfThis
        push    XAX
        INVOKE  FIL4_SEEK, XAX, 0, fSEEK_END
        pop     XAX                     ; Ignore seek errors...

  EnoughOfThis:

        ret

  FIL4_OPEN  ENDP

UniqueSegmentEnd

END
