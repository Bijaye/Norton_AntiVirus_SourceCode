;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/INCLUDE/VCS/ctsn.inv   1.59   14 May 1996 20:37:38   JWILBER  $
;;
;; Description:  Declares function prototypes, macros, equates and types
;;               for the CERTLIBN project.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/INCLUDE/VCS/ctsn.inv  $
;  
;     Rev 1.59   14 May 1996 20:37:38   JWILBER
;  Changed the offset of the non-resident name table in the NE header
;  structure definition to a DWORD instead of a WORD.
;
;     Rev 1.58   10 May 1996 14:36:02   YVANG
;  Added DBCS handling function prototypes.
;
;     Rev 1.57   16 Apr 1996 10:38:34   TCASHIN
;  Back out progress callback and ScanMemoryBufferEX.
;
;     Rev 1.56   15 Apr 1996 20:33:32   TCASHIN
;  Added support for the progress callback.
;
;     Rev 1.55   15 Apr 1996 18:20:26   TCASHIN
;  Added prototype for ScanMemoryBufferEx.
;
;     Rev 1.54   08 Mar 1996 14:56:18   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;
;     Rev 1.53   05 Feb 1996 09:54:56   RSTANEV
;  NTK platform uses STDCALL, not PASCAL functions.
;
;     Rev 1.52   02 Feb 1996 21:29:44   RSTANEV
;  Deleted a lot of old junk.
;
;     Rev 1.51   11 Jan 1996 16:32:44   JWORDEN
;  Port changes from Quake 8
;
;     Rev 1.50   09 Jan 1996 09:30:06   RSTANEV
;  Added SYM_NTK support.
;
;     Rev 1.49   29 Nov 1995 14:22:42   DCHI
;  Added FIL4_NEW_GET_START, SCN4_GET_NAME, and SCN4_GET_VIRUS prototypes.
;
;     Rev 1.48.1.5   01 Dec 1995 18:09:06   BARRY
;  Seems to be a MASM bug -- the previous version breaks because a line ends on a magic boundary
;
;     Rev 1.48.1.4   30 Nov 1995 16:26:14   MKEATIN
;  Added parameter byDrive to prototype DIS4_VALIDATE_MBR
;
;     Rev 1.48.1.1   15 Jul 1995 23:40:02   SZIADEH
;  added NOT_IN_TSR flag
;
;     Rev 1.48   16 May 1995 11:12:18   RSTANEV
;  SIZE_VIRUSNAME does not include EOS.
;
;     Rev 1.47   11 May 1995 17:59:58   SZIADEH
;  added dis4_int13 function.
;
;     Rev 1.46   22 Mar 1995 11:29:38   SZIADEH
;  added UNC support.
;
;     Rev 1.45   30 Jan 1995 17:45:10   BRAD
;  Added FILE_PROTECTL structure
;
;     Rev 1.44   30 Jan 1995 17:35:16   BRAD
;  Changed Inoc prototypes to take size of item record
;
;     Rev 1.43   27 Jan 1995 19:19:56   SZIADEH
;  added a flag to allow using default settings in InitCertlib()
;     INIT_CERTLIB_USE_DEFAULT
;
;     Rev 1.42   25 Jan 1995 16:11:38   BRAD
;  Added JXCXZ
;
;     Rev 1.41   19 Jan 1995 15:20:24   SZIADEH
;  added dis4_is_bootlocked and dis4_bootlocked_state
;
;     Rev 1.40   05 Jan 1995 16:22:14   RSTANEV
;  Nuked ABSDISKREC from here.
;
;     Rev 1.39   27 Dec 1994 15:58:44   BRAD
;  Changed handle parameters from WORD to UINT
;
;     Rev 1.38   22 Dec 1994 13:06:58   BRAD
;  FILE_PROTECT struct NOW in synch with .H
;
;     Rev 1.37   22 Dec 1994 03:15:48   JMILLARD
;  add uSigHeaderSize to ScanMemoryCD
;
;     Rev 1.36   21 Dec 1994 23:17:44   JMILLARD
;  change ScanMemoryBuffer and ScanMemoryCD to UINT args throughout
;
;     Rev 1.35   20 Dec 1994 17:13:10   BRAD
;  Added string functions from CERTLIB4
;
;     Rev 1.34   17 Dec 1994 01:13:56   SZIADEH
;  match with cts4.inc.
;
;     Rev 1.33   16 Dec 1994 18:22:16   SZIADEH
;  Ported from CERTLIB4 to CERTLIBN
;
;     Rev 1.32   13 Dec 1994 02:40:58   SZIADEH
;  corrected the previous comment.
;
;     Rev 1.31   12 Dec 1994 23:05:00   SZIADEH
;  added function prototypes to sync certlibn with certlib4.
;
;     Rev 1.30   12 Dec 1994 19:49:14   SZIADEH
;  Added a few new functions
;
;     Rev 1.29   12 Dec 1994 18:24:14   SZIADEH
;  Added DBC functions
;
;    Rev 1.28   27 Oct 1994 12:39:00   JMILLARD
;  recover prototypes for scn4_verify_virus and RelinquishControl
;
;     Rev 1.27   26 Oct 1994 19:28:00   keith
;  Add some prototypes and related to make the build more reproducable
;
;     Rev 1.26   29 Aug 1994 10:58:52   RSTANEV
;  Added prototypes of memory scan functions.
;
;     Rev 1.25   23 Aug 1994 20:25:18   RSTANEV
;  Added some prototypes so that VxD platform compiles.
;
;     Rev 1.24   23 Aug 1994 19:21:42   RSTANEV
;  Seems like VxD platform does not like SYSCALL language, and I forgot the
;  reason why I put it there...  Let's hope STDCALL doesn't choke big time.
;
;     Rev 1.23   22 Aug 1994 11:06:06   SZIADEH
;  added two constants for memory scanning
;
;     Rev 1.22   19 Aug 1994 21:42:32   RSTANEV
;  Added UniqueDataSegment and UniqueDataSegmentEnd.
;
;     Rev 1.21   17 Aug 1994 13:13:14   RSTANEV
;  Got rid of all places where FILETIME is being used.
;
;     Rev 1.20   16 Aug 1994 22:27:30   RSTANEV
;  Added global UniqueSegmentEnd and OPTION LANGUAGE:SYSCALL for
;  SYM_VXD platform.
;
;     Rev 1.19   12 Aug 1994 18:26:32   RSTANEV
;  added bitCTRL_BOOT_REPAIR constant.
;
;     Rev 1.18   27 Jul 1994 15:43:52   MFALLEN
;  It turned out that the OPTION: stuff should be before .model, so
;  I moved the inclusion of PLATFORM.INC after all the extra options.
;
;     Rev 1.17   27 Jul 1994 12:10:50   RSTANEV
;  Moved all platform specific stuff to PLATFORM.INC.
;
;     Rev 1.16   24 Jun 1994 14:38:24   RSTANEV
;  Added EXCLUDECHECK.
;
;     Rev 1.15   22 Jun 1994 16:20:24   RSTANEV
;  Added support for new callback for FIL4_FIND_ functions.
;
;     Rev 1.14   22 Jun 1994 09:45:08   RSTANEV
;  Added STR4_LENGTH and STR4_LENGTH_TO_LAST.
;
;     Rev 1.13   22 Jun 1994 09:14:42   RSTANEV
;  Added FIL4_IS_AT.
;
;     Rev 1.12   31 May 1994 15:22:58   RSTANEV
;  Added MEM4_SEGTOSEL.
;
;     Rev 1.11   31 May 1994 14:46:58   RSTANEV
;  Added DBC4_??? and some other functions
;
;     Rev 1.10   30 May 1994 11:01:48   RSTANEV
;  Changed DBC4_HEADER.Filehandle from WORD to DWORD.
;
;     Rev 1.9   26 May 1994 14:57:22   RSTANEV
;  Added a prototype.
;
;     Rev 1.8   26 May 1994 09:47:08   RSTANEV
;  Repair functions added.
;
;     Rev 1.7   24 May 1994 08:22:56   RSTANEV
;  Added SYS4_VECTOR and SYS4_UNVECTOR.
;
;     Rev 1.6   23 May 1994 19:44:26   RSTANEV
;  Added file repair functions.
;
;     Rev 1.5   20 May 1994 08:29:26   RSTANEV
;  Added support for the new DOS extended FileFind functions.
;
;     Rev 1.4   13 May 1994 16:59:26   RSTANEV
;  Added ERROR_NO_MORE_FILES.
;
;     Rev 1.3   12 May 1994 12:42:02   RSTANEV
;  Added prototype for _lcreat().
;
;     Rev 1.2   11 May 1994 16:11:10   RSTANEV
;  Added FIL4_WRITE.
;
;     Rev 1.1   09 May 1994 18:41:14   RSTANEV
;  Added support for FIL4_FIND_ALL.
;
;     Rev 1.0   05 May 1994 12:46:38   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

;----------------------------------------------------------------------------
;  Define macros and constants for each platform which PLATFORM.INC does
;  not define, because it is so universal and cool :-)
;----------------------------------------------------------------------------
IFDEF  SYM_TSR

  OPTION  LANGUAGE:PASCAL
  UniqueSegment  MACRO
        .code
        ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_DOS

  DOS16   EQU  TRUE
  OPTION  LANGUAGE:PASCAL
  UniqueSegment  MACRO
        .code @Filename
        ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_DOS32

  OPTION  LANGUAGE:PASCAL
  UniqueSegment  MACRO
        .code @Filename
	ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_WIN16

  WIN16   EQU  TRUE
  OPTION  LANGUAGE:PASCAL
  OPTION  PROC:EXPORT
  UniqueSegment  MACRO
        .code @Filename
	ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_WIN32

  OPTION  LANGUAGE:PASCAL
  UniqueSegment  MACRO
        .code @Filename
	ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_OS2

  OPTION  LANGUAGE:PASCAL
  OPTION  PROC:EXPORT
  UniqueSegment  MACRO
        .code @Filename
	ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_NLM

  OPTION  LANGUAGE:PASCAL
  UniqueSegment  MACRO
        .code @Filename
	ENDM
  UniqueSegmentEnd  MACRO
        ENDM
  UniqueDataSegment  MACRO
        .data
        ENDM
  UniqueDataSegmentEnd  MACRO
        ENDM

ELSEIFDEF  SYM_VXD

  OPTION  LANGUAGE:STDCALL
  UniqueSegment  MACRO
    VxD_LOCKED_CODE_SEG
        ENDM
  UniqueSegmentEnd  MACRO
    VxD_LOCKED_CODE_ENDS
        ENDM
  UniqueDataSegment  MACRO
    VxD_LOCKED_DATA_SEG
        ENDM
  UniqueDataSegmentEnd  MACRO
    VxD_LOCKED_DATA_ENDS
        ENDM

ELSEIFDEF  SYM_NTK

  OPTION  LANGUAGE:STDCALL
  UniqueSegment  MACRO
    DRIVER_PAGEABLE_CODE_SEGMENT
        ENDM
  UniqueSegmentEnd  MACRO
    DRIVER_PAGEABLE_CODE_SEGMENT_END
        ENDM
  UniqueDataSegment  MACRO
    DRIVER_PAGEABLE_DATA_SEGMENT
        ENDM
  UniqueDataSegmentEnd  MACRO
    DRIVER_PAGEABLE_DATA_SEGMENT_END
        ENDM

ENDIF

INCLUDE platform.inc

;----------------------------------------------------------------------------
; This is some sort of protection to unknown platforms.  Need to figure out
; if it is used and how.  That's why I will leave it here as comments, and
; will let PLATFORM.INC display error message if necessary.
;----------------------------------------------------------------------------
;IFNDEF  OS_16
;OS_16           EQU     TRUE
;OS_32           EQU     FALSE
;FARCALL         EQU     TRUE
;FARDATA         EQU     FALSE
;CODEDISTANCE    TEXTEQU <FAR>
;DATADISTANCE    TEXTEQU <NEAR>
;.model medium
;UniqueSegment   MACRO
;.code @Filename
;        ENDM
;ENDIF

;------------------------------------------------------------------------------
; Define the REGISTER set we want to use.
;------------------------------------------------------------------------------
IF OS_16

;LPBYTE          TYPEDEF FAR PTR DWORD
FAR16_NEAR32    EQU     FAR

sizeXSI         EQU     2
sizeXDI         EQU     2
sizeSES         EQU     2

FARNEAR         TYPEDEF FAR
WORD32          TYPEDEF WORD
HGLOBAL         TYPEDEF WORD
HLOCAL          TYPEDEF WORD
HINSTANCE       TYPEDEF WORD
HWND            TYPEDEF WORD

ENDIF

IF OS_32

;LPBYTE          TYPEDEF NEAR PTR DWORD
FAR16_NEAR32    EQU     NEAR

sizeXSI         EQU     4
sizeXDI         EQU     4
sizeSES         EQU     0

FARNEAR         TYPEDEF NEAR
WORD32          TYPEDEF DWORD
HGLOBAL         TYPEDEF WORD
HLOCAL          TYPEDEF WORD
HINSTANCE       TYPEDEF WORD
HWND            TYPEDEF WORD

ENDIF

;****************************************************************************
; MACRO Section
;****************************************************************************

;----------------------------------------------------------------------------
; CALLDOS - if the platform is WIN16, then does call DOS3CALL.
;           otherwise does int 21h for all DOS based patforms.
;----------------------------------------------------------------------------
CALLDOS  MACRO

  IF bSYM_WIN16
    call    DOS3CALL
  ELSEIF bSYM_TSR OR bSYM_DOS
    int     21h
  ELSE
    ; put in your own platform here
  ENDIF

ENDM

;----------------------------------------------------------------------------
; LDS_SI
;----------------------------------------------------------------------------
LDS_SI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	lds     si,arg1.arg2.arg3
  ELSE
	lds     si,arg1.arg2
  ENDIF
 ELSE
	lds     si,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     esi,arg1.arg2.arg3
  ELSE
	mov     esi,arg1.arg2
  ENDIF
 ELSE
	mov     esi,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LDS_DI
;----------------------------------------------------------------------------
LDS_DI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	lds     di,arg1.arg2.arg3
  ELSE
	lds     di,arg1.arg2
  ENDIF
 ELSE
	lds     di,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     edi,arg1.arg2.arg3
  ELSE
	mov     edi,arg1.arg2
  ENDIF
 ELSE
	mov     edi,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LDS_BX
;----------------------------------------------------------------------------
LDS_BX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	lds     bx,arg1.arg2.arg3
  ELSE
	lds     bx,arg1.arg2
  ENDIF
 ELSE
	lds     bx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     ebx,arg1.arg2.arg3
  ELSE
	mov     ebx,arg1.arg2
  ENDIF
 ELSE
	mov     ebx,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LDS_DX
;----------------------------------------------------------------------------
LDS_DX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	lds     dx,arg1.arg2.arg3
  ELSE
	lds     dx,arg1.arg2
  ENDIF
 ELSE
	lds     dx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     edx,arg1.arg2.arg3
  ELSE
	mov     edx,arg1.arg2
  ENDIF
 ELSE
	mov     edx,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LES_SI
;----------------------------------------------------------------------------
LES_SI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	les     si,arg1.arg2.arg3
  ELSE
	les     si,arg1.arg2
  ENDIF
 ELSE
	les     si,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     esi,arg1.arg2.arg3
  ELSE
	mov     esi,arg1.arg2
  ENDIF
 ELSE
	mov     esi,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LES_DI
;----------------------------------------------------------------------------
LES_DI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	les     di,arg1.arg2.arg3
  ELSE
	les     di,arg1.arg2
  ENDIF
 ELSE
	les     di,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     edi,arg1.arg2.arg3
  ELSE
	mov     edi,arg1.arg2
  ENDIF
 ELSE
	mov     edi,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LES_BX
;----------------------------------------------------------------------------
LES_BX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	les     bx,arg1.arg2.arg3
  ELSE
	les     bx,arg1.arg2
  ENDIF
 ELSE
	les     bx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     ebx,arg1.arg2.arg3
  ELSE
	mov     ebx,arg1.arg2
  ENDIF
 ELSE
	mov     ebx,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; LES_DX
;----------------------------------------------------------------------------
LES_DX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	les     dx,arg1.arg2.arg3
  ELSE
	les     dx,arg1.arg2
  ENDIF
 ELSE
	les     dx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
	mov     edx,arg1.arg2.arg3
  ELSE
	mov     edx,arg1.arg2
  ENDIF
 ELSE
	mov     edx,arg1
 ENDIF
ENDIF
	ENDM

;----------------------------------------------------------------------------
; JXCXZ
;----------------------------------------------------------------------------
JXCXZ   MACRO addr1

IF OS_16
    jcxz    addr1
ELSE
    jecxz   addr1
ENDIF

	ENDM

;----------------------------------------------------------------------------
; SMOV move segment to a segment, destroy! AX register
;----------------------------------------------------------------------------
SMOV  MACRO  seg1,seg2

IF OS_16
	push    ax
	mov     ax,seg2
	mov     seg1,ax
	pop     ax
ENDIF

	ENDM

;----------------------------------------------------------------------------
; move to ds from a register
;----------------------------------------------------------------------------
MOV_DS  MACRO   arg1

IF OS_16
        mov     ds,arg1
ENDIF

        ENDM

;----------------------------------------------------------------------------
; move to es from a register
;----------------------------------------------------------------------------
MOV_ES  MACRO   arg1

IF OS_16
        mov     es,arg1
ENDIF

        ENDM

;----------------------------------------------------------------------------
; move to ss from a register
;----------------------------------------------------------------------------
MOV_SS  MACRO   arg1

IF OS_16
        mov     ss,arg1
ENDIF

        ENDM

;----------------------------------------------------------------------------
; 32bit multiplication
;----------------------------------------------------------------------------
MUL32  MACRO  source

IF OS_16
        push    bx
        mov     bx,dx
        mul     source
        push    ax
        push    dx
        mov     ax,bx
        mul     source
        pop     dx
        add     dx,ax
        pop     ax
        pop bx
ENDIF

        ENDM

;----------------------------------------------------------------------------
; Clear a string
;----------------------------------------------------------------------------
macClearString  MACRO  String,lenString
        LES_DI  String
        xor     XCX,XCX         ; 32 bit protection
        mov     cx,lenString
        xor     al,al
        rep     stosb
        ENDM

;----------------------------------------------------------------------------
; Copy a string
;----------------------------------------------------------------------------
macCopyString  MACRO  FromString,ToString
        LDS_SI  FromString
        LES_DI  ToString
mCS0:
        lodsb
        stosb
        or      al,al
        jnz     mCS0
        ENDM

;----------------------------------------------------------------------------
; Copy memory
;----------------------------------------------------------------------------
macMemCopy  MACRO  FromMem,ToMem,Count
	LDS_SI  FromMem
	LES_DI  ToMem
	xor     XCX,XCX         ; 32 bit protection
	mov     cx,Count
	rep     movsb
	ENDM

;----------------------------------------------------------------------------
;  ANSI2OEM
;
;  Usage:
;       ANSI2OEM  lpStr1, lpStr2
;
;  Calls GetAnsiFlag and then calls Windows' function
;  ANSITOOEM lpStr1, lpStr2 if the ANSI flag was set.
;  If the target platform is not Windows, the macro does not expand.
;
;----------------------------------------------------------------------------
ANSI2OEM  MACRO  lpStr1, lpStr2

IF bSYM_WIN16 OR bSYM_WIN32
;
;        LOCAL   NoConversion
;
;IFNDEF  ANSITOOEM
;        ANSITOOEM  PROTO  CODEDISTANCE
;ENDIF
;
;IFNDEF  GetAnsiFlag
;        GetAnsiFlag  PROTO  CODEDISTANCE
;ENDIF
;
;        INVOKE  GetAnsiFlag
;        or      XAX,XAX
;        jz      NoConversion
;        INVOKE  ANSITOOEM, lpStr1, lpStr2
;NoConversion:
;
ELSEIF bSYM_TSR OR bSYM_DOS OR bSYM_NLM
                                        ; No need of ANSITOOEM
ELSE
  ECHO Warning: ANSI2OEM macro has not been implemented for this platform!
ENDIF

ENDM

;----------------------------------------------------------------------------
;  OEM2ANSI
;
;  Usage:
;       OEM2ANSI  lpStr1, lpStr2, strPush, strPop
;
;  Calls GetAnsiFlag and then calls Windows' function
;  OEMTOANSI lpStr1, lpStr2 if the ANSI flag was set.
;  If the target platform is not Windows, the macro does not expand.
;
;----------------------------------------------------------------------------
OEM2ANSI  MACRO  lpStr1, lpStr2, strPush, strPop

IF bSYM_WIN16 OR bSYM_WIN32
;        LOCAL   NoConversion
;
;IFNDEF  OEMTOANSI
;        OEMTOANSI  PROTO  CODEDISTANCE
;ENDIF
;IFNDEF  GetAnsiFlag
;        GetAnsiFlag  PROTO  CODEDISTANCE
;ENDIF
;
;        rpush   <strPush>
;        INVOKE  GetAnsiFlag
;        or      XAX,XAX
;        jz      SHORT NoConversion
;        INVOKE  OEMTOANSI, lpStr1, lpStr2
;NoConversion:
;        rpop    <strPop>
;
ELSEIF bSYM_TSR OR bSYM_DOS OR bSYM_NLM
                                        ; No need of OEMTOANSI
ELSE
  ECHO Warning: OEM2ANSI macro has not been implemented for this platform!
ENDIF

ENDM

;****************************************************************************
; EQUATE Section
;****************************************************************************

IFNDEF SYM_VXD

FILETIME  STRUCT
    dwLowDateTime   DWORD  0
    dwHighDateTime  DWORD  0
FILETIME  ENDS

WIN32_FIND_DATA  STRUCT
  dwFileAttributes    DWORD     0
  ftCreationTime      FILETIME  <0,0>
  ftLastAccessTime    FILETIME  <0,0>
  ftLastWriteTime     FILETIME  <0,0>
  nFileSizeHigh       DWORD     0
  nFileSizeLow        DWORD     0
  dwReserved0         DWORD     0
  dwReserved1         DWORD     0
  cFileName           BYTE      260d DUP (0)
  cAlternateFileName  BYTE      14d  DUP (0)
WIN32_FIND_DATA  ENDS

LPFFD  TYPEDEF  NEAR PTR WIN32_FIND_DATA

FINDITEM  STRUCT
  szLongName    BYTE   260d DUP (0)
  szShortName   BYTE   14d  DUP (0)
  dwCookie      DWORD  0
  bIsDir        BOOL   0
  dwFileSize    DWORD  0
FINDITEM  ENDS

ENDIF

fREAD                   EQU     0       ; Open types
fWRITE                  EQU     1
fREAD_WRITE             EQU     2
fCREATE                 EQU     4
fAPPEND                 EQU     8

fSHARE_COMPAT           EQU     000h    ; Share bit flags
fSHARE_EXCLUSIVE        EQU     010h
fSHARE_DENY_WRITE       EQU     020h
fSHARE_DENY_READ        EQU     030h
fSHARE_DENY_NONE        EQU     040h

fSEEK_BEG               EQU     1       ; Used for Seeking handle in file
fSEEK_FOR               EQU     2
fSEEK_BAK               EQU     4
fSEEK_END               EQU     8

COM_PREPEND_REPAIR      equ     1
COM_JUMP_REPAIR         equ     2

DIR_ENTRY_LEN           EQU     256     ; MAX Length of directory entry

GET_INFO                EQU     0       ; Get or set info flag
SET_INFO                EQU     1

MAXPAD                  EQU     64      ; Used for Get Start padding count

;----------------------------------------------------------------------------
; MAX_PATH commented out since it does not appear to be used in any
; .ASM's and it has been removed from CTSN.H (it is #define'd to 144
; in UNIVERSL.H).
; MAX_PATH        EQU 78                  ; Max with of a DOS path
;----------------------------------------------------------------------------

MAX_PATH_SIZE           EQU     144
MAX_NET_PATH_SIZE       EQU     256

PROTECT_ADD             EQU     0       ; Used for File protection
PROTECT_TEST            EQU     1
PROTECT_DELETE          EQU     2
PROTECT_REPAIR          EQU     4
PROTECT_CREATE          EQU     8
PROTECT_QUICK_INOC      EQU     16

HIDDEN                  EQU     2       ; Used to search for files
SYSTEM                  EQU     4
VOLLABEL                EQU     8
DIR                     EQU     16

IS_COM                  EQU     0       ; Used for Get Start
IS_EXE                  EQU     1
IS_SYS                  EQU     2
IS_BCOM                 EQU     3       ; Bulgarian COM
IS_VCOM                 EQU     4       ; Verified COM
IS_ZIP                  EQU     5
IS_LZEXE                EQU     6
IS_PKEXE                EQU     7
IS_WHALE                EQU     8
IS_INVALID              EQU     9
STARTSIZE               EQU     256

IS_FILE                 EQU     0       ; Used by Find All
IS_DIRECTORY            EQU     1
DIR_DEPTH               EQU     33      ; How many levels of directories

MEM_NORMAL              EQU     0h      ; Memory flags
MEM_LOCKED              EQU     1h
MEM_FIXED               EQU     2h
MEM_READONLY            EQU     4h
MEM_CODE                EQU     8h
MEM_DATA                EQU     MEM_NORMAL

MEM_TYPE_UMB            EQU     1h
MEM_TYPE_CONV           EQU     2h
MEM_TYPE_EMS            EQU     3h
MEM_TYPE_XMS            EQU     4h
MEM_TYPE_SWAP           EQU     5h

MEM_REQUEST_UMB         EQU     10h
MEM_RELEASE_UMB         EQU     11h
MEM_ALLOCATE_XMS        EQU     09h
MEM_FREE_XMS            EQU     0Ah
MEM_LOCK_XMS            EQU     0Ch
MEM_UNLOCK_XMS          EQU     0Dh

MEM_QUERY_A20           EQU     07h
MEM_LOCALENABLE_A20     EQU     05h
MEM_LOCALDISABLE_A20    EQU     06h
MEM_GLOBALENABLE_A20    EQU     03h
MEM_GLOBALDISABLE_A20   EQU     04h
MEM_REQUEST_HMA         EQU     01h

HMA_STATUS              EQU     1
HMA_ON                  EQU     2
HMA_OFF                 EQU     3

CR                      EQU     0Dh
LF                      EQU     0Ah

;----------------------------------------------------------------------------
; Command line flags
;----------------------------------------------------------------------------

CL_FLAG                 EQU     1       ; /FLAG
CL_STRING               EQU     2       ; "This string" or String
CL_FILENAME             EQU     2       ; FILENAME
CL_NUMERIC              EQU     3       ; /NUM 1234
CL_OTHER                EQU     4       ; /STR "This string" or String
CL_HEX                  EQU     5       ; /HEX AB

;----------------------------------------------------------------------------
; ERROR EQUates
;----------------------------------------------------------------------------
FIL4_NOT_FOUND          EQU     1
REPAIR_FAIL             EQU     4
FILE_CHANGED            EQU     5
INVALID_INPUT           EQU     6

SELF_TEST_BUFFER_SIZE   EQU     512
					; When computing, if the self test
					; CRC == 0, fudge it to this value,
SELF_TEST_ZERO_CRC      EQU     1       ; since 0 means SELF_TEST_NO_CRC.

SELF_TEST_SUCCESS       EQU     0
SELF_TEST_NO_CRC        EQU     1
SELF_TEST_FAIL          EQU     2
SELF_TEST_FILE_ERROR    EQU     3

;----------------------------------------------------------------------------
; Virus detection equates
;----------------------------------------------------------------------------
CRCtype                 EQU     0
ALGtype                 EQU     1
BOOtype                 EQU     2
MACtype                 EQU     3       ; Don't blame me, its Dennis' fault

MEMORY_413              EQU     0       ; must match what's defined
MEMORY_ALL              EQU     1       ; in DEFDUMP project.

SIZE_VIRUSNAME          EQU     (21 + 1); This does NOT include EOS!
SIZE_VINFO              EQU     32
SIZE_MEM                EQU     15
SIZE_SIG                EQU     17
SIZE_FIXED              EQU     64
SIZE_BOOTSIG            EQU     14
SIZE_COMMENT            EQU     301                                                                 ; 5 lines of 60 chars per line + EOS
SIZE_ALIAS              EQU     22
SIZE_CRC                EQU     2
SIZE_CHECK_DATA         EQU     2

SIZE_TSR                EQU     35056   ; Default size for DOS_TSR

uSizeVirscanHeader      EQU     256     ; "VIRSCAN.DAT" header size

bitINFO_COMFILE         EQU     0001h
bitINFO_EXEFILE         EQU     0002h
bitINFO_SYSFILE         EQU     0004h
bitINFO_COMCOM          EQU     0008h
bitINFO_FLOPBOOT        EQU     0010h
bitINFO_HDISKBOOT       EQU     0020h
bitINFO_MBOOTREC        EQU     0040h
bitINFO_DIR2            EQU     0080h
bitINFO_RESIDENT        EQU     0100h
bitINFO_SIZESTLTH       EQU     0200h
bitINFO_FULLSTLTH       EQU     0400h
bitINFO_ENCRYPT         EQU     0800h
bitINFO_POLYMORPH       EQU     1000h
bitINFO_TRIGGERED       EQU     2000h
bitINFO_INTHEWILD       EQU     4000h
bitINFO_MULTIPARTY      EQU     8000h

bitCTRL_BOOT_REPAIR     EQU     2000h
bitCTRL_NOTCLEAN        EQU     8000h

; bit Settings for uLFLAGS
bitDSSI                 EQU     0001h   ; Filename located in DS:SI not DS:DX
bitINT13                EQU     0002h   ; If SET WIN/DOS selector/segment usage
bitTSR                  EQU     8000h   ; If SET TSR doing work.
bitWINDOWS              EQU     4000h   ; If SET Windows version doing work.

; bit Setting for Buttons
bitREPAIR               EQU     0001h
bitDELETE               EQU     0002h
bitINOCULATE            EQU     0004h
bitREINOCULATE          EQU     0008h
bitCONTINUE             EQU     0010h
bitSTOP                 EQU     0020h
bitEXCLUDE              EQU     0040h

bitAUTOREPAIR           EQU     0100h
bitAUTODELETE           EQU     0200h
bitAUTOINOCUL           EQU     0400h

; Exception database flags.
excVIOLATING            EQU     0002h
excVIRUS_FOUND          EQU     0004h
excINOC_CHANGE          EQU     0008h
excFILE_WATCH           EQU     0010h
excLOW_FORMAT           EQU     0020h OR excVIOLATING
excWR_HBOOT             EQU     0040h OR excVIOLATING
excWR_FBOOT             EQU     0080h OR excVIOLATING
excWR_ATLIST            EQU     0100h OR excVIOLATING
excRO_CHANGE            EQU     0200h OR excVIOLATING
excREG_CHANGE           EQU     0400h

; Inclusion data flags (for BANKERS file registration).
incREG_CHANGE           EQU     0001h

cmbHalt                 EQU     3
cmbREPAIR               EQU     14
cmbDELETE               EQU     15
cmbINOCULATE            EQU     16
cmbREINOCULATE          EQU     17
cmbCONTINUE             EQU     18
cmbSTOP                 EQU     19
cmbEXCLUDE              EQU     20

ObjectAtList            EQU     1
ObjectLogName           EQU     2
ObjectNetUsers          EQU     3
ObjectExcept            EQU     4
ObjectLogCMB            EQU     5
ObjectPath              EQU     6
ObjectMessCMB           EQU     7
ObjectN30               EQU     8
ObjectNetMess           EQU     9
ObjectUserName          EQU     10
ObjectNetINOC           EQU     11
ObjectScanAll           EQU     12
ObjectChkBothFl         EQU     13
ObjectRemoveAft         EQU     14
ObjectSeconds           EQU     15
ObjectBeepTsr           EQU     16
ObjectAlertUsr          EQU     17
ObjectAlertCon          EQU     18
ObjectAlrtKnown         EQU     19
ObjectAlrtUnkno         EQU     20
ObjectAlrtInocu         EQU     21
ObjectAlrtVLike         EQU     22
ObjectActKnown          EQU     23
ObjectActUnknow         EQU     24
ObjectActInocu          EQU     25
ObjectActVLike          EQU     26
ObjectBackupRep         EQU     27
ObjectBackupExt         EQU     28
ObjectAlertMsg          EQU     29
ObjectInocName          EQU     30
ObjectUnLevel           EQU     31      ; Level of flags for file watch


CRITICAL_MESS           EQU     0001h
CRITICAL_QUES           EQU     0002h

;----------------------------------------------------------------------------
; File length equates
;----------------------------------------------------------------------------
sDOSFILE                EQU     78
sNETFILE                EQU     128
sLONGFILE               EQU     256
sVERYLONGFILE           EQU     260

;----------------------------------------------------------------------------
; MARC equates
;----------------------------------------------------------------------------
MARC_ORIG               EQU     0       ; Place file in original directory
MARC_NEW                EQU     1       ; Place file in new directory

;----------------------------------------------------------------------------
; FIXED ID equates
;----------------------------------------------------------------------------
FID_MTE         EQU     2      ;  1   Value is offset in word array
FID_POGUE       EQU     4      ;  2   converted to zerobase.(-1)*2
FID_WHALE       EQU     6      ;  3   Thus ID 1 = 0, 2 = 2, 3 = 4, 4 = 6, 5 = 8
FID_TPE         EQU     8      ;  4   Fixed ID
FID_V2PX        EQU     10     ;  5
FID_CASCADE     EQU     12     ;  6   For Repair Decrypt
FID_TEQUILA     EQU     14     ;  7   For Repair Decrypt
FID_MALTESE     EQU     16     ;  8
FID_PX2000      EQU     18     ;  9
FID_NOINT       EQU     20     ; 10   For Mem detect
FID_FLIP2343    EQU     22     ; 11   For Repair Decrypt
FID_FLIP2153    EQU     24     ; 12   For Repair Decrypt
FID_DIR2        EQU     26     ; 13   For custom repair
FID_TREMOR      EQU     28     ; 14   Detection
FID_TODOR       EQU     30     ; 15   Detection
FID_GROOVE      EQU     32     ; 16   Detection
FID_MONKEY      EQU     34     ; 17   Repair Monky class of viruses
FID_ONEHALF     EQU     35     ; 18   ONE_HALF repair

;----------------------------------------------------------------------------
; BOOTLOCK equates
;----------------------------------------------------------------------------
BPB_OFFSET              EQU     0bh     ; offset into the boot sector
PARTITION_OFFSET        EQU     1beh
global_offset           EQU     7C00h   ; Global offset for boot code
int13address            EQU     4Ch
ISR13seg                EQU     09FE0h
ISR13offset             EQU     0000h

;----------------------------------------------------------------------------
; Equates for Control Word
;----------------------------------------------------------------------------
GENERIC_COM             EQU     1b
GENERIC_EXE             EQU     10b
PREPENDING              EQU     100b
REBUILD_EXE             EQU     1000b
IMPROVED                EQU     10000b
EXE_CLEAN               EQU     1010b
COM_CLEAN               EQU     10101b
SECONDBOOK              EQU     1000000000b
SLIDER                  EQU     10000000000b
DECRYPT                 EQU     100000000000b
NOCLEAN                 EQU     8000h

;----------------------------------------------------------------------------
; Equates for Second Control Word
;----------------------------------------------------------------------------
CUT_BEGIN               EQU     1b
TWEEK_CS_10             EQU     10b
TWEEK_SS_10             EQU     100b
BOOK_FROM_BEGIN         EQU     1000b
COM_DISTINCT            EQU     10000b
EXEC_CODE               EQU     100000b
DECRYPT_XOR             EQU     1000000b
DECRYPT_ADD             EQU     10000000b
; high byte
DECRYPT_WORD            EQU     100000000b
NO_GENERIC              EQU     1000000000b
DISPLAY_DIALOG          EQU     10000000000b
NOT_IN_TSR              EQU     100000000000b ; 0x800

;----------------------------------------------------------------------------
; flags to initialize certlibn
;----------------------------------------------------------------------------
INIT_CERTLIB_USE_DEFAULT EQU     00000001b

;----------------------------------------------------------------------------
; Equates for TSR
;----------------------------------------------------------------------------
TSR_ID                  EQU     0E5E5h       ; My TSR ID
TSR_SIZE                EQU     45 * 1024    ; Size of TSR code/data
TSR_AREYOUHERE          EQU     1            ; Test if we're in memory
TSR_GETDATA             EQU     2            ; Get pointer to internal flags
TSR_NETWORK             EQU     3            ; NR Toggle
TSR_GETXMS              EQU     4            ; Get address of XMS
TSR_PUTXMS              EQU     5            ; Put XMS mem back
TSR_GETEMS              EQU     6            ; Get address of EMS
TSR_PUTEMS              EQU     7            ; Put EMS mem back

;----------------------------------------------------------------------------
; Equates for Network messageing
;----------------------------------------------------------------------------
NETMAXSERVERNAME        EQU     64
NETMAXUSERNAME          EQU     64
NOV_MAXMESSLEN          EQU     55
NAV_NLM                 EQU     0531h

;----------------------------------------------------------------------------
; Beep level defines
;----------------------------------------------------------------------------
BEEP_UNDEFINED          EQU     20
BEEP_KNOWNBOX           EQU     5
BEEP_FBOOTBOX           EQU     5
BEEP_INOCBOX            EQU     15
BEEP_REINOCBOX          EQU     7
BEEP_AUDITERROR         EQU     12
BEEP_INOCERROR          EQU     10
BEEP_BOOTPROTECT        EQU     10
BEEP_PTABPROTECT        EQU     10
BEEP_SYSFILEPROTECT     EQU     10
BEEP_WBOOTSCAN          EQU     10
BEEP_DURATION           EQU     1
BEEP_MONBOX             EQU     1

;----------------------------------------------------------------------------
; Equates for Character Conversions
;----------------------------------------------------------------------------
CHR_LOWER               EQU     0
CHR_UPPER               EQU     1

;****************************************************************************
; STRUCT Section
;****************************************************************************

;----------------------------------------------------------------------------
; File protection used with Inoculation/FileWatch/SysCheck
;----------------------------------------------------------------------------
File_Protect  STRUCT

IFDEF  SYM_NLM
  Filename    DB  256 + 30 DUP (0)
ELSE
  Filename    DB  144 + 1 DUP (0)
ENDIF
  Fileattr    DB  0
  Filetime    DW  0
  Filedate    DW  0
  Filesize    DD  0
  Header      DB  35 DUP (0)
  Version     DB  0
  wReserved1  DW  0
  wReserved2  DW  0
  StartCRC    DW  0
  CRCAllBuf   DW  0
  CRCLastK    DW  0
File_Protect  ENDS

sFile_Protect  EQU  SIZE File_Protect

;----------------------------------------------------------------------------
; File protection used with Inoculation/FileWatch/SysCheck
; Make sure this is kept in synch with FILE_PROTECT structure above.  Only
; difference between these 2 is the 'Filename' field.
;----------------------------------------------------------------------------
File_ProtectL  STRUCT
  Filename    DB  260 + 1 DUP (0)
  Fileattr    DB  0
  Filetime    DW  0
  Filedate    DW  0
  Filesize    DD  0
  Header      DB  35 DUP (0)
  Version     DB  0
  wReserved1  DW  0
  wReserved2  DW  0
  StartCRC    DW  0
  CRCAllBuf   DW  0
  CRCLastK    DW  0
File_ProtectL  ENDS

sFile_ProtectL  EQU  SIZE File_ProtectL

;----------------------------------------------------------------------------
; Format of a standard Disk Transfer Area
;----------------------------------------------------------------------------
DISK_TA  STRUCT
  DTA_Search    DB  21 DUP (0)
  DTA_Attr      DB  0
  DTA_Time      DW  0
  DTA_Date      DW  0
  DTA_SizeL     DW  0
  DTA_SizeH     DW  0
  DTA_FILENAME  DB  13 DUP (0)
DISK_TA ENDS

sDISK_TA  EQU  SIZE DISK_TA

;----------------------------------------------------------------------------
; Header of a Hashed Database structure
;----------------------------------------------------------------------------
DBC4_HEADER  STRUCT
  Free_Pointer     DD  0
  Elem_Size        DW  0
  Total_Elem_Size  DW  0
  IndexSize        DD  0
  DataStart        DD  0
  IndexStart       DD  0
  QuickStart       DD  0
  Comp_Func        DD  0
  Hash_Func        DD  0
  Quick_Func       DD  0
IF OS_16
  FileHandle       DW  0
                   DW  0
ELSE
  FileHandle       DD  0
ENDIF

DBC4_HEADER  ENDS

sDBC4_HEADER  EQU  SIZE DBC4_HEADER

;----------------------------------------------------------------------------
; Type of chaining used in hash table from DBC4_HEADER
;----------------------------------------------------------------------------
CHAIN_TYPE  STRUCT
  Next    DD  0
  Active  DB  0
CHAIN_TYPE  ENDS

sCHAIN_TYPE  EQU  SIZE CHAIN_TYPE

;----------------------------------------------------------------------------
; Structure for Get Start code routine
;----------------------------------------------------------------------------
STARTCODE  STRUCT
  Filetype      DB  0
  FirstLanding  DD  0
  BufferOffset  DD  0
  Padding       DB  0
STARTCODE  ENDS
sSTARTCODE  EQU  SIZE STARTCODE

;----------------------------------------------------------------------------
; Initialization for GET_ME_MEMORY
;----------------------------------------------------------------------------
INIT_MEMORY  STRUCT
  memTotal       DW  0              ; Total bytes wanted in paragraphs
  memUMB         DW  0              ; Total bytes wanted in UMB paragraphs
  memEMS         DW  0              ; Total bytes wanted in EMS paragraphs
  memHMA         DW  0              ; Total bytes wanted in HMA paragraphs
  memCONV        DW  0              ; Total bytes wanted in CONV paragraphs
  memlpCONV      DW  0              ; Pointer to CONV to use
  memDATA        DW  0              ; Total bytes wanted in DATA paragraphs
  memlpDATA      DW  0              ; Pointer to DATA to use
  memXMS         DW  0              ; Total bytes wanted in XMS paragraphs
  memlpFilename  DD  0              ; Pointer to temp file to create
INIT_MEMORY  ENDS

sINIT_MEMORY  EQU  SIZE INIT_MEMORY

;----------------------------------------------------------------------------
; GET_ME_MEMORY structure
;----------------------------------------------------------------------------
GMEMORY  STRUCT
  lpUMB        DW  0                    ; Segment of UMB memory
  lpCONV       DW  0                    ; Segment of CONV memory
  lpDATA       DW  0                    ; Segment of DATA memory
  lpHMA        DD  0                    ; Pointer to HMA memory
  hEMS         DW  0                    ; Handle to EMS memory
  hXMS         DW  0                    ; Handle to XMS memory
  hSwapFile    DW  0                    ; Handle to swap file
  uLastHandle  DW  0                    ; Last handle allocated
  uFreeHandle  DW  0                    ; Known freed handle
GMEMORY  ENDS

sGMEMORY  EQU  SIZE GMEMORY

;----------------------------------------------------------------------------
; Local memory struct for allocation of global memory
;----------------------------------------------------------------------------
LMEMORY  STRUCT
  hHandle  DW  0                        ; Handle to memory
  uSize    DW  0                        ; Size of memory in paragraphs
  lpNEXT   DW  0                        ; Segment of next memory
  lpPREV   DW  0                        ; Segment of previous memory
  uFlags   DW  0                        ; Flags of this memory seg
LMEMORY  ENDS

sLMEMORY  EQU  SIZE LMEMORY

;----------------------------------------------------------------------------
; Local memory struct for allocation of LOCAL memory
;----------------------------------------------------------------------------
MEMLOCAL  STRUCT
  lLocked  DB  0
  lUsed    DB  0
  lSize    DW  0
  lNext    DW  0
  lPrev    DW  0
  lHandle  DW  0
MEMLOCAL  ENDS

sMEMLOCAL  EQU  SIZE MEMLOCAL

;----------------------------------------------------------------------------
; Real Mode structure for calling bios interrupts
;----------------------------------------------------------------------------
REALMODE  STRUCT
  RM_DI  DW  0
         DW  0
  RM_SI  DW  0
         DW  0
  RM_BP  DW  0
         DW  0
         DD  0
  RM_BX  DW  0
         DW  0
  RM_DX  DW  0
         DW  0
  RM_CX  DW  0
         DW  0
  RM_AX  DW  0
         DW  0
  RM_FL  DW  0
  RM_ES  DW  0
  RM_DS  DW  0
  RM_FS  DW  0
  RM_GS  DW  0
  RM_IP  DW  0
  RM_CS  DW  0
  RM_SP  DW  0
  RM_SS  DW  0
REALMODE  ENDS

sREALMODE  EQU  SIZE REALMODE

;----------------------------------------------------------------------------
; Structure for EXE header
;----------------------------------------------------------------------------
EXEHEADER  STRUCT
  exSignature     DW  5A4Dh         ; Sig of .EXE
  exExtraBytes    DW  ?             ; Number of extra bytes in last page
  exPages         DW  ?             ; Number of pages
  exRelocItems    DW  ?             ; Number of pointers in relocation table
  exHeaderSize    DW  ?             ; Size of header in paragraphs
  exMinAlloc      DW  ?             ; Minimun allocation
  exMaxAlloc      DW  ?             ; Max allocation
  exInitSS        DW  ?             ; Initial SS value
  exInitSP        DW  ?             ; Initial SP value
  exCheckSum      DW  ?             ; Complemented checksum
  exInitIP        DW  ?             ; Initial IP value
  exInitCS        DW  ?             ; Initial CS value
  exRelocTable    DW  ?             ; bytes offset to relocation table
  exOverlay       DW  ?             ; Overlay number
  exReserved      DW  16 DUP (0)
  exExtendOffset  DW  0             ; Location of EH
EXEHEADER  ENDS

sEXEHEADER  EQU  SIZE EXEHEADER - 66
sEXELARGE   EQU  SIZE EXEHEADER

;----------------------------------------------------------------------------
; NE Header structure
;----------------------------------------------------------------------------
NEHEADER  STRUCT
  neSignature     DW  454Eh     ; Sig of the WINDOWS.EXE
  neLinkVersion   DB  ?         ; Version of LINK used
  neLinkRevision  DB  ?         ; Revision of LINK used
  neEntryOffset   DW  ?         ; Offset to entry table from EH Header
  neEntryLength   DW  ?         ; Length of entry table
  neNextDLL       DW  0         ; Reserved for windows
  neReserved1     DW  ?         ; Reserved
  neExeFlags      DW  ?         ; Flags describing EXE information
  neAutoDataNumb  DW  ?         ; Number of auto data segment
  neHeapSize      DW  ?         ; Initial size of heap
  neStackSize     DW  ?         ; Initial size of stack
  neCSIP          DD  ?         ; Segment:Offset of CS:IP
  neSSSP          DD  ?         ; Segment:Offset of SS:SP
  neSegTabNumb    DW  ?         ; Number of entries in the seg table
  neModRefNumb    DW  ?         ; Number of entries in the Mod ref table
  neNonresNumb    DW  ?         ; Number of entries in the NonResName table
  neSegTabOff     DW  ?         ; Relative offset to the segment table
  neResourceOff   DW  ?         ; Relative offset to the resource table
  neResidentOff   DW  ?         ; Relative offset to the ResidentName table
  neModRefOff     DW  ?         ; Relative offset to the ModuleRef table
  neImportOff     DW  ?         ; Relative offset to the ImportedName table
  neNonresOff     DD  ?         ; Relative (beg of file) to Nonres name table
  neMovable       DW  ?         ; Number of movable entry points
  neShift         DW  ?         ; Shift count to align the logical sectors
  neResourceNumb  DW  ?         ; Number of resource segments
  neOSFlags       DB  ?         ; Flags describing the OS System
  neExeFlags2     DB  ?         ; Flags describing EXE information
  neFastSector    DD  ?         ; Offset of fast load area in sectors
  neFastLength    DW  ?         ; Length of fast load area in sectors
  neVersion       DW  ?         ; Windows version number
NEHEADER  ENDS

sNEHEADER  EQU  NEHEADER

;----------------------------------------------------------------------------
; NE Support structures
;----------------------------------------------------------------------------
NESEGMENT_TABLE  STRUCT
  neSegDataSec  DW  ?      ; Offset, in sectors, to the segment data (relative to begin of file)
  neSegLength   DW  ?      ; Segment length in file
  neSTFlags     DW  ?      ; Segment table flags
  neSegMin      DW  ?      ; Minimum size of segment
NESEGMENT_TABLE  ENDS

sNESEGMENT_TABLE  EQU  SIZE NESEGMENT_TABLE

NEMODULE_REFERENCE  STRUCT
  neModuleOffset  DW  ?         ; Offset to names in imported-name table
NEMODULE_REFERENCE  ENDS

sNEMODULE_REFERENCE  EQU  SIZE NEMODULE_REFERENCE

NEENTRY_TABLE  STRUCT
  neBundleCount  DB  ?          ; Number of entries in bundle, 0 = END
  neBundleFlag   DB  ?          ; Type of bundle (FF=Fixed, FE=Movable)
NEENTRY_TABLE  ENDS

sNEENTRY_TABLE  EQU  SIZE NEENTRY_TABLE

NEENTRY_MOVABLE  STRUCT
  neMovableFlag  DB  ?          ; Flags for movable code
  neMovable3Fh   DW  ?          ; The INTERRUPT 3F
  neMovableSeg   DB  ?          ; Specifies the segment number
  neMovableOff   DW  ?          ; Specifies the segment offset
NEENTRY_MOVABLE  ENDS

sNEENTRY_MOVABLE  EQU  SIZE NEENTRY_MOVABLE

NEENTRY_FIXED  STRUCT
  neFixedFlag    DB  ?          ; Flags for fixed code
  neFixedOffset  DW  ?          ; Specifies an offset
NEENTRY_FIXED  ENDS

sNEENTRY_FIXED  EQU  SIZE NEENTRY_FIXED

NERELOCATION_HEADER  STRUCT
  neCount  DW  ?                ; Number of relocations
NERELOCATION_HEADER ENDS

sNERELOCATION_HEADER  EQU  SIZE NERELOCATION_HEADER

NEREL_IMP_NAME  STRUCT
  neAddressType  DB  ?          ; Type of address to fix
  neRelType      DB  ?          ; Type of relocation to fix
  neRelFixMe     DW  ?          ; Offset needing to be fixed
  neIndexModule  DW  ?          ; Index to module reference table
  neOrdalNumber  DW  ?          ; Ordial number
NEREL_IMP_NAME  ENDS

sNEREL_IMP_NAME  EQU  SIZE NEREL_IMP_NAME

NEREL_IMP_ORDIAL  STRUCT
  neAddressType  DB  ?          ; Type of address to fix
  neRelType      DB  ?          ; Type of relocation to fix
  neRelFixMe     DW  ?          ; Offset needing to be fixed
  neIndexModule  DW  ?          ; Index to module reference table
  neNameOffset   DW  ?          ; Offset in name table
NEREL_IMP_ORDIAL  ENDS

sNEREL_IMP_ORDIAL  EQU  SIZE NEREL_IMP_ORDIAL

NEREL_INT_FIXED  STRUCT
  neAddressType    DB  ?        ; Type of address to fix
  neRelType        DB  ?        ; Type of relocation to fix
  neRelFixMe       DW  ?        ; Offset needing to be fixed
  neSegmentNumber  DB  ?        ; Segment number
                   DB  0        ; Always a 0
  neOffset         DW  ?        ; Offset in segment
NEREL_INT_FIXED  ENDS

sNEREL_INT_FIXED  EQU  SIZE NEREL_INT_FIXED

NEREL_INT_MOVE  STRUCT
  neAddressType  DB  ?          ; Type of address to fix
  neRelType      DB  ?          ; Type of relocation to fix
  neRelFixMe     DW  ?          ; Offset needing to be fixed
                 DB  0FFh       ; Always a 0FFh
                 DB  0          ; Always a 0
  neOrdial       DW  ?          ; Ordial to segment entry table
NEREL_INT_MOVE  ENDS

sNEREL_INT_MOVE  EQU  SIZE NEREL_INT_MOVE

;----------------------------------------------------------------------------
; Dynamic loading of a DLL for DOS structure
;----------------------------------------------------------------------------
DYNA_HEADER  STRUCT
  lpSegment    DD  0            ; Pointer to segment table
  lpResource   DD  0            ; Pointer to resource table
  lpResident   DD  0            ; Pointer to resident name table
  lpModuleRef  DD  0            ; Pointer to module reference table
  lpImport     DD  0            ; Pointer to import name table
  lpEntry      DD  0            ; Pointer to entry table
  lpNonres     DD  0            ; Pointer to non resident table
  lpNextDLL    DD  0            ; Pointer to next loaded DLL
  hFilehandle  DW  0            ; Handle to DLL to load from
DYNA_HEADER  ENDS

sDYNA_HEADER  EQU  SIZE DYNA_HEADER

;----------------------------------------------------------------------------
; THE TSR Structure
;----------------------------------------------------------------------------
TSR_Struct  STRUCT
; Global   - Data used by all EV, thru pointers
  lpRegs       DD  0            ; Register structure, not used in here
  lpBuffer     DD  0            ; Work buffer
  lpExtra      DD  0            ; Extra text buffer pointer
  uSizeBuffer  DW  0            ; Size of work buffer
  uSizeExtra   DW  0            ; Size of extra buffer

; Internal "Message" handler
  uMessage1    DW  0            ; Message to place for user
  uQuestion1   DW  0            ; Question 1 to ask
  uMessage2    DW  0            ; Message to place for user if first failed.
  uQuestion2   DW  0            ; Question 2 to ask
  uLogBase     DW  0            ; Base to add to log

; Local    - Data only used by a single EV
  lpData       DD  0            ; Pointer to data for function
  uLFlags      DW  0            ; Flags for messages (FLAGS1)
  uCurrPSP     DW  0            ; Current Program Segment Prefix

; Extension to TSR functionallity
  lpGlobal     DD  0            ; Extension of GLOBAL Data
  lpCallBack   DD  0            ; Extension of CALLBACK Data
TSR_Struct  ENDS

sTSR_Struct  EQU  SIZE TSR_Struct

;----------------------------------------------------------------------------
HOOK  STRUCT
  Interrupt  WORD  0            ; Interrupt to intercept
  Registers  WORD  0            ; Kernel function to hook
  Multi      DB    0            ; Set to true if uses previous TSR_STRUCT
  EVDLL      DB    20 DUP (0)   ; EV Function DLL
  EVFunc     DB    40 DUP (0)   ; EV Function
  DataDLL    DB    20 DUP (0)   ; Data dll
  DataFunc   DB    40 DUP (0)   ; Data build routine
HOOK  ENDS

sHOOK  EQU  SIZE HOOK

;----------------------------------------------------------------------------
BINHEADER  STRUCT
  Header         DB    128 DUP (0)
  EndOvl         DB    0
  Signature      DB    4 DUP (0)
  uNumStructs    UINT  0
  uNumHooks      UINT  0
  uNumCallbacks  UINT  0
  bUSEUMB        DB    0
  bUSEEMS        DB    0
  bUSEXMS        DB    0
  uTsrID         UINT  0        ; Id number for interrupt 2F
  uTsrSize       UINT  0
  uWorkBuffer    UINT  0
  uExtraBuffer   UINT  0
  GlobalDLL      DB    20 DUP (0)
  GlobalFunc     DB    40 DUP (0)
  EndOvl2        DB    0
BINHEADER  ENDS

sBINHEADER  EQU  SIZE BINHEADER

;----------------------------------------------------------------------------
; Event Verify Structure
;----------------------------------------------------------------------------
EV  STRUCT
  evUseMe      DB  0            ; Use this or not
  evINTERRUPT  DW  0            ; Interrupt
  evFUNCTION   DW  0            ; Function, wild cards = ff
  evRoutine    DW  0            ; Pointer to offset of far routine
EV  ENDS

sEV  EQU  SIZE EV

;----------------------------------------------------------------------------
; Structure for callbacks in OVL file
;----------------------------------------------------------------------------
EVCALLBACK  STRUCT
  CallbackDLL   DB  20 DUP (0)  ; DLL Name
  CallbackFunc  DB  40 DUP (0)  ; Function name
EVCALLBACK  ENDS
sEVCALLBACK  EQU  SIZE EVCALLBACK

;----------------------------------------------------------------------------
; Structure for CMB file
;----------------------------------------------------------------------------
MESSAGE_HEADER  STRUCT
  POINTER       DD    0         ; Pointer to where in file message is
  MESSAGE_SIZE  WORD  0         ; Size of this message
MESSAGE_HEADER  ENDS

sMESSAGE_HEADER  EQU  SIZE MESSAGE_HEADER

;----------------------------------------------------------------------------
; Register structure for TSR work
;----------------------------------------------------------------------------
TSR_REGS  STRUCT
  r_ES  WORD  0
  r_DS  WORD  0
  r_DI  UINT  0
  r_SI  UINT  0
  r_BP  UINT  0
  r_SP  UINT  0
  r_BX  UINT  0
  r_DX  UINT  0
  r_CX  UINT  0
  r_AX  UINT  0
TSR_REGS  ENDS

sTSR_REGS  EQU  SIZE TSR_REGS

;----------------------------------------------------------------------------
; Window Structure
;----------------------------------------------------------------------------
WINDOW  STRUCT
  wRow     DW  0
  wColumn  DW  0
  wWidth   DW  0
  wHeigth  DW  0
  wAttrib  DB  0
  wBuffer  DD  0
  wVideo   DD  0
WINDOW  ENDS

sWINDOW  EQU  SIZE WINDOW

;----------------------------------------------------------------------------
; NOVELL Request/Reply structures
;----------------------------------------------------------------------------
Novell_SendConsole  STRUCT
  Request  DW  0
  id       DB  0
  MessLen  DB  0
  Message  DB  55 DUP (0)
  Reply    DW  0
Novell_SendConsole  ENDS

sSendConsole_Reply    EQU  2
sSendConsole_Request  EQU  SIZE Novell_SendConsole - sSendConsole_Reply

Novell_SendMessage  STRUCT
  Request  DW  0
  id       DB  0
  Count    DB  0
  UserID   DB  0
  MessLen  DB  0
  Message  DB  55 DUP (0)
  Reply    DW  0
  Cnt      DB  0
  Result   DB  2 DUP (0)
Novell_SendMessage  ENDS

sSendMessage_Reply    EQU  5
sSendMessage_Request  EQU  SIZE Novell_SendMessage - sSendMessage_Reply

Novell_GetUserNumb  STRUCT
  Request  DW  0
  id       DB  15h
           DB  0
  ObjType  DB  0
  ObjLen   DB  0
  ObjName  DB  47 DUP (0)
  Reply    DW  0
  NumCon   DB  0
  ConList  DB  100 DUP (0)
Novell_GetUserNumb  ENDS

sGetUserNumb_Reply    EQU  103
sGetUserNumb_Request  EQU  SIZE Novell_GetUserNumb - sGetUserNumb_Reply

Novell_GetUserName  STRUCT
  Request DW  0
  id      DB  16h
  ConNumb DB  0
  Reply   DW  0
  ObjID   DD  0
          DB  0
  ObjType DB  0
  ObjName DB  48 DUP (0)
  LogTime DB  7 DUP (0)
Novell_GetUserName  ENDS

sGetUserName_Reply    EQU  63
sGetUserName_Request  EQU  SIZE Novell_GetUserName - sGetUserName_Reply

Novell_ScanBindery  STRUCT
  Request  DW  0
  id       DB  37h
  LastObj  DD  0
           DB  0
  ObjType  DB  0
  ObjLen   DB  1
  ObjName  DB  '*'
  Reply    DW  0
  ObjID    DD  0
           DB  0
  rType    DB  0
  rText    DB  48 DUP (0)
  OFlag    DB  0
  OSec     DB  0
  OProp    DB  0
Novell_ScanBindery  ENDS

sScanBindery_Reply    EQU  59
sScanBindery_Request  EQU  SIZE Novell_ScanBindery - sScanBindery_Reply

tagNETUSERITEM  STRUC
  szServer  BYTE  NETMAXSERVERNAME + 1 DUP (0)
  szObject  BYTE  NETMAXUSERNAME + 1 DUP (0)
  uFlag     WORD  0
tagNETUSERITEM  ENDS

stagNETUSERITEM  EQU  SIZE tagNETUSERITEM

NOVELLREQ_E33D  STRUC
  wBuffLen             WORD  0          ; Buffer length-2
  bCmd                 BYTE  3Dh        ; Command I.D.
  bObjectType          BYTE  0          ; Object Type WORD (hi-lo)
                       BYTE  0
  bObjectLength        BYTE  0          ; Object Length
  bObjectName          BYTE  47 DUP (0) ; Object Name
  bSegmentNumber       BYTE  0          ; Number of the data segment
  bPropertyNameLength  BYTE  0          ; Property Name Length (1-15)
  bPropertyName        BYTE  15 DUP (0) ; Property Name (max 15 char.)
NOVELLREQ_E33D  ENDS

sNOVELLREQ_E33D  EQU  SIZE NOVELLREQ_E33D

NOVELLREP_E33D  STRUC
  wBuffLen        WORD  0               ; Buffer length-2
  bPropertyValue  BYTE  128 DUP (0)     ; Property Value;
  bMoreSegments   BYTE  0               ; More Segment Flags (0=no, 255=more)
  bPropertyFlags  BYTE  0               ; Property Flags (dynamic, static, ...)
NOVELLREP_E33D  ENDS

sNOVELLREP_E33D  EQU  SIZE NOVELLREP_E33D

;----------------------------------------------------------------------------
ScanBinderyRequest  STRUC
  wScanBinderyRequestLength  WORD   0           ; lo-hi
  bReserved                  BYTE   37h
  dwLastObjectID             DWORD  0           ; hi-lo
  wObjectType                WORD   0           ; hi-lo
  bObjectNameLength          BYTE   0
  sObjectName                BYTE   47 DUP (0)
ScanBinderyRequest  ENDS

sScanBinderyRequest  EQU  SIZE ScanBinderyRequest

;----------------------------------------------------------------------------
ScanBinderyReply  STRUC
  wScanBinderyReplyLength  WORD   0             ; lo-hi
  dwObjectID               DWORD  0             ; hi-lo
  wObjectType              WORD   0             ; hi-lo
  sObjectName              BYTE   48 DUP (0)
  bObjectFlag              BYTE   0
  bOjectSecurity           BYTE   0
  bOjectHasProperties      BYTE   0
ScanBinderyReply  ENDS

sScanBinderyReply  EQU  SIZE ScanBinderyReply

;----------------------------------------------------------------------------
tagECB  STRUC
  sLinkAddress       BYTE  4 DUP (0)
  sESRAddress        BYTE  4 DUP (0)
  fInUseFlag         BYTE  0
  bCompletionCode    BYTE  0
  wSocketNumber      WORD  0                    ; hi - lo byte order
  sIPXWordspace      BYTE  4 DUP (0)
  sDriverWorkspace   BYTE  12 DUP (0)
  sImmediateAddress  BYTE  6 DUP (0)
  wFragmentCount     WORD  0                    ; hi - lo byte order
  sFragmentAddress1  BYTE  4 DUP (0)
  wFragmentSize1     WORD  0
  sFragmentAddress2  BYTE  4 DUP (0)
  wFragmentSize2     WORD  0
tagECB  ENDS

stagECB  EQU  SIZE tagECB

;----------------------------------------------------------------------------
ReadPropertyValueRequest  STRUC
  wReadPropertyValueRequestLength  WORD  0      ; lo-hi
  bReserved                        BYTE  3Dh
  wObjectType                      WORD  0      ; hi-lo
  bObjectNameLength                BYTE  0
  sObjectName                      BYTE  47 DUP (0)
                                   BYTE  17 DUP (0)
ReadPropertyValueRequest  ENDS

sReadPropertyValueRequest  EQU  SIZE ReadPropertyValueRequest

;----------------------------------------------------------------------------
ReadPropertyValueReply  STRUC
  wReadPropertyValueReplyLength  WORD  0        ; lo-hi
  sPropertyValue                 BYTE  128 DUP (0)
  bMoreSegments                  BYTE  0
  bPropertyFlags                 BYTE  0
ReadPropertyValueReply  ENDS

sReadPropertyValueReply  EQU  SIZE ReadPropertyValueReply

;----------------------------------------------------------------------------
tagIPXHeader  STRUC
  wChecksum            WORD  0                  ; hi-lo
  wLength              WORD  0                  ; hi-lo
  bTransportLayer      BYTE  0
  bPacketType          BYTE  0
  sDestinationNetwork  BYTE  4 DUP (0)          ; hi-lo
  sDestinationNode     BYTE  6 DUP (0)          ; hi-lo
  sDestinationSocket   WORD  0                  ; hi-lo
  sSourceNetwork       BYTE  4 DUP (0)          ; hi-lo
  sSourceNode          BYTE  6 DUP (0)          ; hi-lo
  sSourceSocket        WORD  0                  ; hi-lo
tagIPXHeader  ENDS

stagIPXHeader  EQU  SIZE tagIPXHeader

;----------------------------------------------------------------------------
; Command line stuff
;----------------------------------------------------------------------------
COMMANDLINE  STRUCT
  CL_Type    DB  0                      ; Type of command line option
  CL_MASK    DD  0                      ; Look for this
  CL_STORE   DD  0                      ; Where to store value
COMMANDLINE  ENDS

sCOMMANDLINE  EQU  SIZE COMMANDLINE

;----------------------------------------------------------------------------
; Virus database structes
;----------------------------------------------------------------------------
N30  STRUCT
  wVirID         WORD   0                       ; Virus ID Number
  wType          WORD   0                       ; Type
  wCrc           WORD   0                       ; CRC
  wControl       WORD   0                       ; Control word for Cleaning
  wInfo          WORD   0                       ; Information Word
  wFixedID       WORD   0                       ; Fixed algo virus ID number

  lpVName        BYTE   SIZE_VIRUSNAME DUP (0)  ; Virus Name
  lpMem          BYTE   SIZE_MEM DUP (0)        ; Memory Signature
  lpSig          BYTE   SIZE_SIG DUP (0)        ; Regular Signature

  wInfSize       WORD   0
  wTagSize       WORD   0
  wHostJmp       WORD   0
  wJmpLen        WORD   0
  wSS_reg        WORD   0
  wSP_reg        WORD   0
  wCS_reg        WORD   0
  wIP_reg        WORD   0
  wHeader        WORD   0
  wHeadLen       WORD   0
  wBookLocation  WORD   0
  wBookMark      WORD   0
  dwComOff       DWORD  0                       ; Offset into N30.NFO
  wControl2      WORD   0
N30  ENDS

sN30  EQU  SIZE N30

HEADER  STRUCT
  uNumALGs   WORD  0                  ; # of Algorythmic virus signatures
  uNumBOOs   WORD  0                  ; # of Boot virus signatures
  uNumCRCs   WORD  0                  ; # of CRC virus signatures
  uNumREMs   WORD  0                  ; # of signatures which are turned off
  wFixedIDs  WORD  SIZE_FIXED DUP (0) ; N30 Record Numbers
HEADER  ENDS

sHEADER  EQU  SIZE HEADER

N30NOTES  STRUCT
  lpVirName  BYTE  SIZE_VIRUSNAME DUP (0)
  wVirID     WORD  0
  wType      WORD  0
  wControl2  WORD  0
  wInfo      WORD  0
  wSize      WORD  0
  hNext      WORD  0
N30NOTES  ENDS

sN30NOTES  EQU  SIZE N30NOTES

VIRSCANDATHEADER  STRUCT
  szSignature      BYTE   16 DUP (0)    ; Signature "VIRSCAN.DAT"
  dwVersion        DWORD  0             ; Numeric version number
  byDay            BYTE   0             ; Day of this version
  byMonth          BYTE   0             ; Month of this version
  wYear            WORD   0             ; Year of this version
  wNumSigs         WORD   0             ; Number of sigs total
  wNumALGs         WORD   0             ; Number of ALG sigs
  wNumBOOs         WORD   0             ; Number of BOO sigs
  wNumCRCs         WORD   0             ; Number of CRC sigs
  wNumREMs         WORD   0             ; Number of REMed out sigs
  wHeaderCheckSum  WORD   0             ; check sum of header(see note below)
  wTotalVir        WORD   0             ; total # of viruses we "detect"
  byFiller         BYTE   218 DUP (0)   ; Filler to add extra 'stuff'
VIRSCANDATHEADER  ENDS

sVIRSCANDATHEADER  EQU  SIZE VIRSCANDATHEADER

;----------------------------------------------------------------------------
; SDA Infomation block
;----------------------------------------------------------------------------
SDA  STRUCT
  lpFrom   DD  0
  lpTo     DD  0
  lpINDOS  DD  0
  uSmall   DW  0
  uLarge   DW  0
SDA  ENDS

sSDA  EQU  SIZE SDA

;----------------------------------------------------------------------------
; External message load information
;----------------------------------------------------------------------------
EXTERN_MESSAGE  STRUCT
  MessageID    DB  0                    ; Message number
  MessageText  DD  0                    ; Where to store it
  MessageSize  DW  0                    ; Max size of message
EXTERN_MESSAGE  ENDS

sEXTERN_MESSAGE  EQU  SIZE EXTERN_MESSAGE

;----------------------------------------------------------------------------
; Simple archive method
;----------------------------------------------------------------------------
MARC  STRUCT
  ArcDir   DB  64 DUP (0)               ; Original directory
  ArcName  DB  15 DUP (0)               ; Name of file in arc
  ArcAttr  DB  0                        ; Attr of file in arc
  ArcTime  DW  0                        ; Time of file in arc
  ArcDate  DW  0                        ; Date of file in arc
  ArcSize  DD  0                        ; Size of file in arc
  ArcRes   DB  7 DUP (0)                ; Reserved for future
MARC  ENDS

sMARC  EQU  SIZE MARC

;----------------------------------------------------------------------------
; For DLL load of array of DLL's
;----------------------------------------------------------------------------
LIB_ARRAY  STRUCT
  LibraryName   DB         78 DUP (0)
  LibraryArray  HINSTANCE  101 DUP (0)
LIB_ARRAY  ENDS

sLIB_ARRAY  EQU  SIZE LIB_ARRAY

;----------------------------------------------------------------------------
; STARTUP_INFO, instance data for Windows
;----------------------------------------------------------------------------
STARTUP_INFO  STRUCT
  VersionHigh  DB  0                    ; Major version
  VersionLow   DB  0                    ; Minor version
  NextStartup  DD  0                    ; Pointer to next startup
  ASCIZ_Name   DD  0                    ; Pointer to name
  Ref_Data     DD  0                    ; Reference data for ASCIZ_Name
  InstRecords  DD  0                    ; Pointer to instance records (Struct)
STARTUP_INFO  ENDS

sSTARTUP_INFO  EQU  SIZE STARTUP_INFO

;----------------------------------------------------------------------------
; INSTANCE_RECORD
;----------------------------------------------------------------------------
INSTANCE_RECORD  STRUCT
  InstanceAddr  DD  0                   ; Address of instance record
  InstanceSize  DW  0                   ; Size of instance record
INSTANCE_RECORD  ENDS

sINSTANCE_RECORD  EQU  SIZE INSTANCE_RECORD

;----------------------------------------------------------------------------
; Structure for Bios Parameter Block
;----------------------------------------------------------------------------
BPB_struc  STRUC
  BytesPerSec  DW  ?                   ; bytes per sector
  SecPerClust  DB  ?                   ; sectors per cluster
  ResSectors   DW  ?                   ; number of reserved sectors
  FATs         DB  ?                   ; number of file allocation tables
  RootDirEnts  DW  ?                   ; number of root-directory entries */
  Sectors      DW  ?                   ; total number of sectors (if 0 refer to HugeSectors
  Media        DB  ?                   ; media descriptor
  FATsecs      DW  ?                   ; number of sectors per FAT
  SecPerTrack  DW  ?                   ; sectors per track
  Heads        DW  ?                   ; number of heads
  HiddenSecs   DD  ?                   ; number of hidden sectors
  HugeSectors  DD  ?                   ; number of sectors if Sectors == 0
BPB_struc  ENDS

sBPB_struc  EQU  SIZE BPB_struc

;----------------------------------------------------------------------------
; Structure for Partition Table Entries
;----------------------------------------------------------------------------
PARTENTRY  STRUC
  peBootable       DB  ?               ; 80h - bootable, 00h = nonbootable
  peBeginHead      DB  ?               ; beginning head
  peBeginSector    DB  ?               ; beginning sector
  peBeginCylinder  DB  ?               ; beginning cylinder
  peFileSystem     DB  ?               ; name of file system
  peEndHead        DB  ?               ; ending head
  peEndSector      DB  ?               ; ending sector
  peEndCylinder    DB  ?               ; ending cylinder
  peStartSector    DD  ?               ; starting sector (relative to beg. of disk)
  peSectors        DD  ?               ; number of sectors in partition
PARTENTRY  ENDS

sPARTENTRY  EQU  SIZE PARTENTRY

;----------------------------------------------------------------------------
; Drive Parameter Block (DPB)
;----------------------------------------------------------------------------
DPB  STRUC
  dpbDrive              DB      ?       ;drive number (0=A,1=B,etc.)
  dpbUnit               DB      ?       ;unit number for drive
  dpbSectorSize         DW      ?       ;sector size, in bytes
  dpbClusterMask        DB      ?       ;sectors per cluster - 1
  dpbClusterShift       DB      ?       ;sectors per cluster, as power of 2
  dpbFirstFAT           DW      ?       ;first sector containing FAT
  dpbFATCount           DB      ?       ;number of FATs
  dpbRootEntries        DW      ?       ;number of root-directory entries
  dpbFirstSector        DW      ?       ;first sector of first cluster
  dpbMaxCluster         DW      ?       ;number of clusters on drive + 1
  dpbFATSize            DW      ?       ;number of sectors occupied by FAT
  dpbDirSector          DW      ?       ;first sector containing director
  dpbDriverAddr         DD      ?       ;address of device driver
  dpbMedia              DB      ?       ;media descriptor
  dpbFirstAccess        DB      ?       ;indicates access to the drive
  dpbNextDPB            DD      ?       ;address of next drive paramter block
  dpbNextFree           DW      ?       ;last allocated cluster
  dpbFreeCnt            DW      ?       ;number of free cluster
DPB  ENDS

sDPB  EQU  SIZE DPB

;----------------------------------------------------------------------------
; Exclude structure used for the exclude binary file
;----------------------------------------------------------------------------
EXCLUDE  STRUCT
  szText         BYTE  MAX_PATH_SIZE + 1 DUP (0)
  bSubDirectory  BYTE  0
  wBits          WORD  0
EXCLUDE  ENDS

sEXCLUDE  EQU  SIZE EXCLUDE

;****************************************************************************
; TYPEDEF section
;****************************************************************************

IF FARCALL
;----------------------------------------------------------------------------
;  FARCALL, anyDATA
;----------------------------------------------------------------------------
  DB_ROUTINES       PROTO   FAR,:WORD,:LPSTR
  DB_PROTO          TYPEDEF PROTO FAR,:WORD,:LPSTR
  DB_PTR            TYPEDEF PTR DB_PROTO

  DBC4_PROTO_HASH   TYPEDEF PROTO FAR,:LPSTR
  DBC4_HASH         TYPEDEF PTR DBC4_PROTO_HASH

  DBC4_PROTO_COMP   TYPEDEF PROTO FAR,:LPSTR,:LPSTR
  DBC4_COMP         TYPEDEF PTR DBC4_PROTO_COMP

  DBC4_PROTO_QUICK  TYPEDEF PROTO FAR,:LPSTR
  DBC4_QUICK        TYPEDEF PTR DBC4_PROTO_QUICK

  FF_CALLBACK       PROTO   FAR,:LPSTR
  FF_PROTO          TYPEDEF PROTO FAR,:LPSTR
  FF_PTR            TYPEDEF PTR FF_PROTO

  EV_PROTO          TYPEDEF PROTO FAR,:LPSTR
  EV_PTR            TYPEDEF PTR EV_PROTO

  SCNPTR_PROTO      TYPEDEF PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
  SCNPTR            TYPEDEF PTR SCNPTR_PROTO

  SCNPTRTSR_PROTO   TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
  SCNPTRTSR         TYPEDEF PTR SCNPTRTSR_PROTO

  REPAIR_PROTO      TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
  REPAIR_PTR        TYPEDEF PTR REPAIR_PROTO

  REP_BVIR_PROTO    TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR
  REP_BVIR_PTR      TYPEDEF PTR REP_BVIR_PROTO

  KILL_PROTO        TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD
  KILL_PTR          TYPEDEF PTR KILL_PROTO

  NET_PROTO         TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:LPSTR
  NET_PTR           TYPEDEF PTR NET_PROTO

ELSEIF FARDATA
;----------------------------------------------------------------------------
;  NEARCALL, FARDATA
;----------------------------------------------------------------------------
  DB_ROUTINES       PROTO   FAR,:WORD,:LPSTR
  DB_PROTO          TYPEDEF PROTO FAR,:WORD,:LPSTR
  DB_PTR            TYPEDEF PTR DB_PROTO

  DBC4_PROTO_HASH   TYPEDEF PROTO FAR,:LPSTR
  DBC4_HASH         TYPEDEF PTR DBC4_PROTO_HASH

  DBC4_PROTO_COMP   TYPEDEF PROTO FAR,:LPSTR,:LPSTR
  DBC4_COMP         TYPEDEF PTR DBC4_PROTO_COMP

  DBC4_PROTO_QUICK  TYPEDEF PROTO FAR,:LPSTR
  DBC4_QUICK        TYPEDEF PTR DBC4_PROTO_QUICK

  FF_CALLBACK       PROTO   FAR,:LPSTR
  FF_PROTO          TYPEDEF PROTO FAR,:LPSTR
  FF_PTR            TYPEDEF PTR FF_PROTO

  EV_PROTO          TYPEDEF PROTO FAR,:LPSTR
  EV_PTR            TYPEDEF PTR EV_PROTO

  SCNPTR_PROTO      TYPEDEF PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
  SCNPTR            TYPEDEF PTR SCNPTR_PROTO

  SCNPTRTSR_PROTO   TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
  SCNPTRTSR         TYPEDEF PTR SCNPTRTSR_PROTO

  REPAIR_PROTO      TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
  REPAIR_PTR        TYPEDEF PTR REPAIR_PROTO

  REP_BVIR_PROTO    TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR
  REP_BVIR_PTR      TYPEDEF PTR REP_BVIR_PROTO

  KILL_PROTO        TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD
  KILL_PTR          TYPEDEF PTR KILL_PROTO

  NET_PROTO         TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:LPSTR
  NET_PTR           TYPEDEF PTR NET_PROTO

ELSE
;----------------------------------------------------------------------------
;  NEARCALL, NEARDATA
;----------------------------------------------------------------------------
  DB_ROUTINES       PROTO   NEAR,:WORD,:LPSTR
  DB_PROTO          TYPEDEF PROTO NEAR,:WORD,:LPSTR
  DB_PTR            TYPEDEF PTR DB_PROTO

  DBC4_PROTO_HASH   TYPEDEF PROTO NEAR,:LPSTR
  DBC4_HASH         TYPEDEF PTR DBC4_PROTO_HASH

  DBC4_PROTO_COMP   TYPEDEF PROTO NEAR,:LPSTR,:LPSTR
  DBC4_COMP         TYPEDEF PTR DBC4_PROTO_COMP

  DBC4_PROTO_QUICK  TYPEDEF PROTO NEAR,:LPSTR
  DBC4_QUICK        TYPEDEF PTR DBC4_PROTO_QUICK

  FF_CALLBACK       PROTO   NEAR,:LPSTR
  FF_PROTO          TYPEDEF PROTO NEAR,:LPSTR
  FF_PTR            TYPEDEF PTR FF_PROTO

  EV_PROTO          TYPEDEF PROTO NEAR,:LPSTR
  EV_PTR            TYPEDEF PTR EV_PROTO

  SCNPTR_PROTO      TYPEDEF PROTO NEAR,:LPSTR,:UINT,:LPSTR,:LPSTR
  SCNPTR            TYPEDEF PTR SCNPTR_PROTO

  SCNPTRTSR_PROTO   TYPEDEF PROTO NEAR,:LPSTR,:UINT,:LPSTR,:LPSTR
  SCNPTRTSR         TYPEDEF PTR SCNPTRTSR_PROTO

  REPAIR_PROTO      TYPEDEF PROTO NEAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
  REPAIR_PTR        TYPEDEF PTR REPAIR_PROTO

  REP_BVIR_PROTO    TYPEDEF PROTO NEAR,:WORD,:WORD,:LPSTR
  REP_BVIR_PTR      TYPEDEF PTR REP_BVIR_PROTO

  KILL_PROTO        TYPEDEF PROTO NEAR,:LPSTR,:LPSTR,:WORD,:WORD
  KILL_PTR          TYPEDEF PTR KILL_PROTO

  NET_PROTO         TYPEDEF PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
  NET_PTR           TYPEDEF PTR NET_PROTO

ENDIF

;****************************************************************************
; PROTO Type section
;****************************************************************************

;----------------------------------------------------------------------------
; Declare DOS 7 extended stuff
;----------------------------------------------------------------------------
HANDLE32  TYPEDEF  NEAR PTR BYTE

ERROR_NO_MORE_FILES  =  18d

FS_LFN_APIS          =  4000h

FILE_ATTRIBUTE_NORMAL     = 0000h
FILE_ATTRIBUTE_READONLY   = 0001h
FILE_ATTRIBUTE_HIDDEN     = 0002h
FILE_ATTRIBUTE_SYSTEM     = 0004h
FILE_ATTRIBUTE_VOLUME     = 0008h
FILE_ATTRIBUTE_DIRECTORY  = 0010h


;----------------------------------------------------------------------------
; Declare Quake external functions.
;----------------------------------------------------------------------------
DiskAbsOperation       PROTO  CODEDISTANCE,:BYTE,:LPSTR
DiskReadMBR            PROTO  CODEDISTANCE,:LPSTR
DiskWriteMBR           PROTO  CODEDISTANCE,:LPSTR

;----------------------------------------------------------------------------
; Declare Windows exported functions
;----------------------------------------------------------------------------
IF bSYM_WIN16
EXTRN   DOS3CALL:FAR
ENDIF

IF bSYM_WIN32

CreateDirectory        TEXTEQU  <CreateDirectoryA>
DeleteFile             TEXTEQU  <DeleteFileA>
GetCurrentDirectory    TEXTEQU  <GetCurrentDirectoryA>
GetFileAttributes      TEXTEQU  <GetFileAttributesA>
FindFirstFile          TEXTEQU  <FindFirstFileA>
FindNextFile           TEXTEQU  <FindNextFileA>
SetCurrentDirectory    TEXTEQU  <SetCurrentDirectoryA>
SetFileAttributes      TEXTEQU  <SetFileAttributesA>

_lclose                PROTO  CODEDISTANCE,:UINT
_lcreat                PROTO  CODEDISTANCE,:LPSTR,:UINT
_llseek                PROTO  CODEDISTANCE,:UINT,:DWORD,:UINT
_lopen                 PROTO  CODEDISTANCE,:LPSTR,:UINT
_lread                 PROTO  CODEDISTANCE,:UINT,:LPSTR,:UINT
_lwrite                PROTO  CODEDISTANCE,:UINT,:LPSTR,:UINT

CreateDirectoryA       PROTO  CODEDISTANCE,:LPSTR,:LPSTR
DeleteFileA            PROTO  CODEDISTANCE,:LPSTR
DosDateTimeToFileTime  PROTO  CODEDISTANCE,:LPSTR,:LPSTR,:LPSTR
GetCurrentDirectoryA   PROTO  CODEDISTANCE,:DWORD,:LPSTR
GetFileAttributesA     PROTO  CODEDISTANCE,:LPSTR
GetFileTime            PROTO  CODEDISTANCE,:UINT,:LPSTR,:LPSTR,:LPSTR
GetLastError           PROTO  CODEDISTANCE
FileTimeToDosDateTime  PROTO  CODEDISTANCE,:LPSTR,:LPSTR,:LPSTR
FindClose              PROTO  CODEDISTANCE,:HANDLE32
FindFirstFileA         PROTO  CODEDISTANCE,:LPSTR,:LPFFD
FindNextFileA          PROTO  CODEDISTANCE,:HANDLE32,:LPFFD
SetCurrentDirectoryA   PROTO  CODEDISTANCE,:LPSTR
SetFileAttributesA     PROTO  CODEDISTANCE,:LPSTR,:UINT
SetFileTime            PROTO  CODEDISTANCE,:UINT,:LPSTR,:LPSTR,:LPSTR
SetLastError           PROTO  CODEDISTANCE,:DWORD

ENDIF

;----------------------------------------------------------------------------
; Declare CERTLIBN functions.
;----------------------------------------------------------------------------

ex_FindClose              PROTO CODEDISTANCE,:HANDLE32
ex_FindFirstFile          PROTO CODEDISTANCE,:LPSTR,:LPBYTE,:WORD
ex_FindNextFile           PROTO CODEDISTANCE,:HANDLE32,:LPBYTE
ex_FileTimeToDosDateTime  PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:LPBYTE

DBC4_CREATE               PROTO CODEDISTANCE,:LPSTR,:DWORD,:WORD,:LPBYTE,:LPBYTE,:LPBYTE,:LPBYTE
DBC4_OPEN                 PROTO CODEDISTANCE,:LPSTR,:DWORD,:WORD,:LPBYTE,:LPBYTE,:LPBYTE,:LPBYTE,:BOOL
DBC4_CLOSE                PROTO CODEDISTANCE,:LPBYTE
DBC4_DELETE               PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:LPBYTE
DBC4_INSERT               PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:LPBYTE
DBC4_SEARCH               PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:UINT,:LPBYTE
DBC4_OPEN_CREATE          PROTO CODEDISTANCE,:LPSTR,:DWORD,:WORD,:LPBYTE,:LPBYTE,:LPBYTE,:LPBYTE
DBC4_GET_NUM_ENTRIES      PROTO CODEDISTANCE,:LPBYTE
DBC4_GET_ENTRY            PROTO CODEDISTANCE,:LPBYTE,:DWORD,:LPBYTE

DBCS_AnsiNext             PROTO CODEDISTANCE,:LPSTR
DBCS_DBCSIsLeadByte       PROTO CODEDISTANCE,:BYTE

DIS4_GET_BPB              PROTO CODEDISTANCE,:BYTE,:LPBYTE,:LPBYTE
DIS4_LOC_BOOTSECTOR       PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_LOC_HIDDEN_MBR       PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_READ_BOOTSECTOR      PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_READ_MBR             PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_READ_PHYSICAL        PROTO CODEDISTANCE,:LPBYTE,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_VALIDATE_MBR         PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_WRITE_PHYSICAL       PROTO CODEDISTANCE,:LPBYTE,:BYTE,:WORD,:BYTE,:BYTE,:BYTE

EXCLUDECHECK              PROTO CODEDISTANCE,:LPSTR,:LPSTR,:WORD

FIL4_ATTR                 PROTO CODEDISTANCE,:LPSTR,:UINT,:UINT
FIL4_CHANGE_DTA           PROTO CODEDISTANCE,:LPBYTE
FIL4_CLOSE                PROTO CODEDISTANCE,:UINT
FIL4_CREATE_DIR           PROTO CODEDISTANCE,:LPSTR
FIL4_DATETIME             PROTO CODEDISTANCE,:UINT,:WORD,:WORD,:UINT
FIL4_DELETE               PROTO CODEDISTANCE,:LPSTR
FIL4_DIRECTORY            PROTO CODEDISTANCE,:BYTE,:LPSTR,:UINT
FIL4_GET_DTA              PROTO CODEDISTANCE
FIL4_GET_START            PROTO CODEDISTANCE,:UINT,:LPBYTE,:LPBYTE
FIL4_NEW_GET_START        PROTO CODEDISTANCE,:UINT,:LPBYTE,:LPBYTE
FIL4_FIND_ALL             PROTO CODEDISTANCE,:LPBYTE,:DWORD,:BOOL,:LPBYTE
FIL4_FIND_FIRST           PROTO CODEDISTANCE,:LPSTR,:WORD
FIL4_FIND_NEXT            PROTO CODEDISTANCE
FIL4_IS_AT                PROTO CODEDISTANCE,:LPSTR,:LPSTR
FIL4_OPEN                 PROTO CODEDISTANCE,:LPSTR,:UINT
FIL4_READ                 PROTO CODEDISTANCE,:UINT,:LPBYTE,:UINT
FIL4_SEEK                 PROTO CODEDISTANCE,:UINT,:DWORD,:WORD
FIL4_SIZE                 PROTO CODEDISTANCE,:UINT
FIL4_WRITE                PROTO CODEDISTANCE,:UINT,:LPBYTE,:UINT
FIL4_PROTECT              PROTO CODEDISTANCE,:LPSTR,:LPSTR,:LPSTR,:WORD,:LPBYTE,:UINT,:LPBYTE,:LPBYTE
FIL4_KILL                 PROTO CODEDISTANCE,:LPSTR,:LPSTR,:UINT,:WORD

MEM4_CRC                  PROTO CODEDISTANCE,:WORD,:LPBYTE,:UINT
MEM4_SEGTOSEL             PROTO CODEDISTANCE,:WORD
MEM4_SET                  PROTO CODEDISTANCE,:LPBYTE,:BYTE,:UINT

NUM4_MODULUS              PROTO CODEDISTANCE,:DWORD,:DWORD

SCN4_ALG                  PROTO CODEDISTANCE,:UINT,:LPSTR,:UINT,:LPSTR, :LPSTR
GetInfectionData          PROTO CODEDISTANCE,:LPBYTE, :LPSTR, :DWORD
SCN4_BOOT                 PROTO CODEDISTANCE,:LPSTR,:LPSTR,:BOOL
SCN4_CRC                  PROTO CODEDISTANCE,:LPSTR,:UINT,:LPSTR,:WORD
SCN4_GET_ENTRY            PROTO CODEDISTANCE,:UINT,:WORD,:LPSTR
SCN4_ME                   PROTO CODEDISTANCE,:UINT,:LPSTR,:LPSTR,:UINT,:LPSTR,:LPSTR,:LPSTR,:WORD
SCN4_VERIFY_NAME          PROTO CODEDISTANCE,:UINT,:WORD,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_GET_NAME             PROTO CODEDISTANCE,:UINT,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_MALTA                PROTO CODEDISTANCE,:LPSTR,:UINT,:LPSTR,:LPSTR
SCN4_MTE                  PROTO CODEDISTANCE,:LPSTR,:UINT,:LPSTR,:LPSTR
SCN4_V2P                  PROTO CODEDISTANCE,:LPSTR,:UINT,:LPSTR,:LPSTR
SCN4_DELETE               PROTO CODEDISTANCE,:UINT,:LPSTR,:LPSTR

ScanMemoryBuffer          PROTO CODEDISTANCE, :LPBYTE,:UINT, :LPBYTE,:UINT, :UINT, :UINT
ScanMemoryCD              PROTO CODEDISTANCE, :LPBYTE,:UINT, :LPBYTE,:UINT, :UINT, :UINT




STR4_COPY                 PROTO CODEDISTANCE,:LPSTR,:LPSTR
STR4_LENGTH               PROTO CODEDISTANCE,:LPSTR
STR4_LENGTH_TO_LAST       PROTO CODEDISTANCE,:LPSTR,:BYTE

SYS4_VECTOR               PROTO CODEDISTANCE,:BYTE,:LPBYTE,:LPBYTE
SYS4_UNVECTOR             PROTO CODEDISTANCE,:BYTE,:LPBYTE,:LPBYTE

IFDEF SYM_DOS
SCN4_EXEC                 PROTO CODEDISTANCE,:UINT,:LPSTR,:LPSTR,:LPSTR
ENDIF

Seek_to_index_entry       PROTO CODEDISTANCE,:LPBYTE,:DWORD
Seek_to_data_record       PROTO CODEDISTANCE,:LPBYTE,:DWORD
Seek_to_data_chain        PROTO CODEDISTANCE,:LPBYTE,:DWORD
Seek_to_quick_entry       PROTO CODEDISTANCE,:LPBYTE,:DWORD
Find_free_data_entry      PROTO CODEDISTANCE,:LPBYTE

SCN4_VERIFY_VIRUS PROTO CODEDISTANCE,:WORD,:WORD,:LPSTR,:LPSTR
SCN4_GET_VIRUS    PROTO CODEDISTANCE,:WORD,:LPSTR,:LPSTR

SCN4_MEMORY               PROTO CODEDISTANCE,:LPSTR,:BOOL

RelinquishControl       PROTO CODEDISTANCE SYSCALL      ; NetWare Os Call

;----------------------------------------------------------------------------
; sync CTSN.INC with CTS4.INC
;----------------------------------------------------------------------------

FIL4_STAT               PROTO CODEDISTANCE,:LPSTR,:LPBYTE,:LPBYTE
FIL4_CRC                PROTO CODEDISTANCE,:UINT,:WORD,:DWORD,:LPBYTE,:WORD
INOC_REP_MAIN           PROTO CODEDISTANCE,:LPSTR,:LPBYTE,:UINT,:LPBYTE,:LPBYTE
INOC_REP_CMP35          PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
INOC_REP_COPY_BACK      PROTO CODEDISTANCE,:UINT,:LPBYTE,:DWORD
INOC_REP_TYPE           PROTO CODEDISTANCE,:UINT,:LPBYTE,:LPBYTE,:DWORD
INOC_REP_COM_APPEND     PROTO CODEDISTANCE,:UINT,:LPBYTE,:LPBYTE,:DWORD
INOC_REP_COM_PREPEND    PROTO CODEDISTANCE,:UINT,:LPBYTE,:DWORD,:DWORD
INOC_REP_FIND35         PROTO CODEDISTANCE,:UINT,:DWORD,:LPBYTE,:LPBYTE
SYS4_GET_CPU_TYPE       PROTO CODEDISTANCE
MEM4_SRCH_STRING        PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:UINT
STR4_SRCH_CHAR          PROTO CODEDISTANCE,:LPSTR,:BYTE
STR4_COMPARE            PROTO CODEDISTANCE,:LPSTR,:LPSTR
STR4_ICOMPARE           PROTO CODEDISTANCE,:LPSTR,:LPSTR
STR4_UPPER              PROTO CODEDISTANCE,:LPSTR
DOS4_GetEnvironment     PROTO CODEDISTANCE
DIS4_IS_BOOTLOCKED      PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_BOOTLOCKED_STATE   PROTO CODEDISTANCE,:BYTE,:LPBYTE
DIS4_GET_BOOTABLE_PARTITION  PROTO CODEDISTANCE,:BYTE,:LPBYTE,:LPBYTE
IsUNCName                    PROTO CODEDISTANCE,:LPSTR
UNC_SkipServerVolumeName     PROTO CODEDISTANCE,:LPSTR
UNC_GetServerVolumeName      PROTO CODEDISTANCE,:LPSTR, :LPSTR
DIS4_INT13              PROTO CODEDISTANCE,:LPSTR
