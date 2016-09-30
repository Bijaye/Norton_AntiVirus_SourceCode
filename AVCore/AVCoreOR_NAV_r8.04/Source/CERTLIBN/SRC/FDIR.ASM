;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fdir.asv   1.4   19 Apr 1996 10:17:54   MZAREMB  $
;;
;; Description: Defines FIL4_DIRECTORY function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fdir.asv  $
;  
;     Rev 1.4   19 Apr 1996 10:17:54   MZAREMB
;  Fixed subdirectory recursion problem in DX platform through IFDEFs.
;  
;     Rev 1.3   17 Aug 1994 13:25:52   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.2   26 May 1994 15:28:50   RSTANEV
;  Minor changes.
;
;     Rev 1.1   19 May 1994 11:23:44   RSTANEV
;  Now supports extended DOS functions.
;
;     Rev 1.0   19 May 1994 09:29:56   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_DIRECTORY
;
; Get or set the current directory of a drive.
;
; BOOL WINAPI FIL4_DIRECTORY ( BYTE Drive, LPSTR Buffer, UINT Flag );
;
; Input:
;       1)  Drive to get directory of (0=Default, 1=A:..)
;       2)  Buffer, where to place result
;       3)  Flag = GET_INFO or SET_INFO
; Output:
;       1)  ERROR (FALSE)
;       2)  PASSED (TRUE)
;
; Note:
;       Will return the directory with the slash before it.
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_DIRECTORY  PROC  CODEDISTANCE  USES XDX SDS XSI,
        bDrive:BYTE,
        lpBuffer:LPSTR,
        bFlag:UINT
                                        ; we will need the drive here
        mov     dl,bDrive
                                        ; see what action to take
        cmp     bFlag,GET_INFO
        je      GetDirectory
                                        ; set specific drive if necessary
        or      dl,dl
        jz      NoChangeDrive
        dec     dl
        mov     ah,0Eh
        CALLDOS

  NoChangeDrive:
        LDS_DX  lpBuffer
IFNDEF SYM_DOSX
        mov     ax,713Bh
        stc
        CALLDOS
        jnc     ReturnToCaller
        cmp     ax,7100h
        stc
        jne     ReturnToCaller
ENDIF
        mov     ah,3Bh
        CALLDOS
        jmp     SHORT ReturnToCaller
                                        ; get current directory (dl loaded)
  GetDirectory:
        LDS_SI  lpBuffer
        mov     [XSI],BYTE PTR '\'
        inc     XSI
IFNDEF SYM_DOSX
        mov     ax,7147h
        CALLDOS
        jnc     ReturnToCaller
        cmp     ax,7100h
        stc
        jne     ReturnToCaller
ENDIF
        mov     ah,47h
        CALLDOS
                                        ; move the result in XAX
  ReturnToCaller:
        mov     XAX,FALSE
        jc      EnoughOfThis
        inc     XAX

  EnoughOfThis:

        ret

  FIL4_DIRECTORY  ENDP

ELSE
        ECHO Warning: The target platform does not support FIL4_DIRECTORY.
ENDIF

UniqueSegmentEnd
END
