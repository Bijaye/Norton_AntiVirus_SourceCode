;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fcdir.asv   1.5   03 May 1996 09:38:14   MZAREMB  $
;;
;; Description: Defines FIL4_CREATE_DIR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fcdir.asv  $
;  
;     Rev 1.5   03 May 1996 09:38:14   MZAREMB
;  Change to allow creation of a dir in Win95 DOS box.
;  
;     Rev 1.4.1.1   02 May 1996 12:40:58   MZAREMB
;  Added DX define to allow creation of subdir in Win95 DOS box. Fixes 
;  STS NAV #54007
;  
;     Rev 1.4.1.0   12 Jul 1995 18:58:30   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.4   08 Jun 1995 20:13:38   SZIADEH
;  took out the call DirMake, moved it to mkinocd.cpp.
;  
;     Rev 1.3   20 Dec 1994 14:28:46   JMILLARD
;  
;  add SYM_NLM
;  
;     Rev 1.2   13 Dec 1994 15:39:38   BRAD
;  Now works for VxD
;  
;     Rev 1.1   08 Dec 1994 20:24:04   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;     Rev 1.3   19 Aug 1994 22:04:56   RSTANEV
;  Wrong... SYM_VXD does not support this function.
;
;     Rev 1.2   17 Aug 1994 13:26:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.1   31 May 1994 15:20:48   RSTANEV
;  Fixed a typo.
;
;     Rev 1.0   31 May 1994 14:36:04   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC


UniqueSegment

;----------------------------------------------------------------------------
; FIL4_CREATE_DIR
;
; Will create a directory on a drive.
;
; BOOL WINAPI FIL4_CREATE_DIR ( LPSTR lpPath );
;
; Input:
;       1)  Path to create, NULL terminated.
; Output:
;       1)  TRUE (1)  Created.
;       2)  FALSE (0) Not created
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_CREATE_DIR  PROC  CODEDISTANCE  USES  XDX SDS,
        Filename:LPSTR

        LDS_DX  Filename                ; Load the path name
        mov     ax,7139h                ; Extended MakeDir
        stc
        CALLDOS
        jnc     doneDirMake
ifdef SYM_DOSX
        cmp     ax,3h                   ; Problem in a DOS box?
        jle     tryOldWay               ; Then try the pre-LFN way
endif
        cmp     ax,7100h                ; Supported?
        jne     doneDirMake             ; Yes, but really an error

  tryOldWay:
        mov     ah,39h
        CALLDOS

  doneDirMake:

        ret

  FIL4_CREATE_DIR  ENDP

ELSEIF bSYM_WIN32

  FIL4_CREATE_DIR  PROC  CODEDISTANCE  USES  XBX XCX XDX,
        Filename:LPSTR

        INVOKE  CreateDirectory, Filename, 0

        ret

  FIL4_CREATE_DIR  ENDP

ELSEIF bSYM_NLM

  FIL4_CREATE_DIR  PROC  CODEDISTANCE  USES  XBX XCX XDX,
        Filename:LPSTR

        ret

  FIL4_CREATE_DIR  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_CREATE_DIR.
ENDIF

UniqueSegmentEnd
END
