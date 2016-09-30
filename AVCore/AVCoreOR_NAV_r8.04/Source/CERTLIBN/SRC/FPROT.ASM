; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIBN/VCS/FPROT.ASv   1.24   10 May 1996 20:44:58   YVANG  $
;
; Filename:     FPROT.ASM
; Programmer:   see below
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:
;
; Note:
;
; $Log:   S:/CERTLIBN/VCS/FPROT.ASv  $
;  
;     Rev 1.24   10 May 1996 20:44:58   YVANG
;  DBCS enable.
;  
;     Rev 1.23   08 Mar 1996 14:56:54   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.22   11 Jan 1996 18:17:48   JWORDEN
;  Port changes from Quake 6 and Quake 8
;  
;     Rev 1.21   10 Nov 1995 16:07:00   GDZIECI
;  Ifdef'd out unused code for NLM.
;  
;     Rev 1.20.1.1   13 Jul 1995 16:49:58   DALLEE
;  Saving address of Short name after drive spec was broken for 16-bit
;  platforms.  Need to save DS at lpszShortName+2.
;  
;     Rev 1.20   09 Jul 1995 21:01:56   DALLEE
;  Error checking from FIL4_READ was testing for 0 instead of -1.
;  When the 2nd read failed, -1 was passed for the # of bytes read to
;  MEM4_CRC... BAD BAD BAD.
;  
;  
;     Rev 1.19   01 Jul 1995 22:18:24   RSTANEV
;  Fixed such that when calculating CRCs we never include the drive character
;  nor server/volume names.
;
;     Rev 1.18   19 Jun 1995 09:53:02   KEITH
;  Commented the code and changed a parameter name. Buffer256 can no longer
;  simply be 256 bytes long as it has to contain the File_ProtectL buffer.
;  INOC_REP_MAIN is passed this buffer which it uses in a recursive call back
;  to FIL4_PROTECT. A bug was caused because on this second call, Buffer256
;  was used as the "Entry" parameter which changed to require the larger
;  File_ProtectL buffer.
;
;     Rev 1.17   28 Apr 1995 15:46:18   SZIADEH
;  if fstart fails, get a crc of the first 16 bytes in the file anyway.
;  this was done to handle the case of currepted CSIP's in exe files.
;
;     Rev 1.16   20 Apr 1995 21:00:04   DALLEE
;  @*#*!*#!! 32-bit conversion bug.
;  VxD and W32 were seeking to a random location to take CRC of last 1K.
;  They were using DX::AX for return from FIL4_SEEK instead of EAX.
;
;     Rev 1.15   05 Apr 1995 21:53:56   RSTANEV
;  Now FIL4_PROTECT will not modify the strings passed by the caller.
;  This is a fix only for VxD platform.  If you want to add another
;  platform to include this fix, please keep in mind that this will
;  eat another MAX_PATH_SIZE + 30 bytes off the stack.
;
;     Rev 1.14   04 Apr 1995 20:54:54   RSTANEV
;  Apparently "macClearString" was not used correctly and so Entry was
;  initialized only up to it's second byte.
;
;     Rev 1.13   23 Mar 1995 11:17:16   SZIADEH
;  check the two word value for zero in IsUNCName(). This change was made
;  to please DavidA, the God of Programmers :-)
;
;     Rev 1.12   23 Mar 1995 11:09:20   SZIADEH
;  changed cmp to or in IsUNCName()
;
;     Rev 1.11   23 Mar 1995 11:05:42   SZIADEH
;  fixed a build  error.
;
;     Rev 1.10   22 Mar 1995 11:29:34   SZIADEH
;  added UNC support.
;
;     Rev 1.9   30 Jan 1995 17:35:38   BRAD
;  Changed some functions to take a size record paramater
;
;     Rev 1.8   28 Dec 1994 13:50:10   BRAD
;  Fixed typo
;
;     Rev 1.7   28 Dec 1994 13:40:08   BRAD
;  Count parameter for MEM4_CRC must be UINT, not WORD
;
;     Rev 1.6   20 Dec 1994 17:11:50   BRAD
;  CRC the long filename, if different from short
;
;     Rev 1.5   16 Dec 1994 19:14:52   BRAD
;  Converted to 32-bit
;
;     Rev 1.4   16 Dec 1994 14:07:02   BRAD
;  Handle needs to be UINT, not WORD
;
;     Rev 1.3   09 Dec 1994 20:29:44   SZIADEH
;  added UniqueSegmentEnd.
;
;     Rev 1.2   09 Dec 1994 17:48:26   SZIADEH
;  deleted all references to ANSITOOEM and OEMTOANSI stuff.
;
;     Rev 1.1   08 Dec 1994 20:24:38   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;
;     Rev 1.15   29 Nov 1994 13:05:24   DALLEE
;  Reget from Qak2 post NAV.305
;
;     Rev 1.11.1.3   22 Nov 1993 10:02:38   JMILLARD
;  ifndef NLM out unused slow inoculation code for NLM certification
;
;     Rev 1.11.1.2   11 Nov 1993 15:31:36   DDREW
;  IFNDEF'd out a call to repair functions that were including a lot of code we
;   never useing causing us to fail Novell certification.
;
;
;     Rev 1.11.1.1   01 Nov 1993 15:39:52   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;
;     Rev 1.11.1.0   19 Oct 1993 07:23:50   SCOTTP
;  Branch base for version QAK2
;
;     Rev 1.11   12 Oct 1993 16:33:58   PATRICKM
;  Baseline from QAK1 branch
;
;     Rev 1.10.1.1   11 Oct 1993 18:55:10   MFALLEN
;  Function expects ANSI filename under Windows.
;
;     Rev 1.10.1.0   03 Sep 1993 19:27:12   SCOTTP
;  Branch base for version QAK1
;
;     Rev 1.10   30 Aug 1993 20:53:36   CJKUO
;  Assert version number and check
;
;     Rev 1.9   24 Aug 1993 11:58:40   DALLEE
;  Open for taking file protect info in share deny none mode.
;
;     Rev 1.8   30 Jul 1993 09:12:04   BRADR
;  Added an external buffer to save on stack space.  256 bytes.
;
;     Rev 1.7   26 Jul 1993 12:10:36   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;
;     Rev 1.6   26 May 1993 16:02:58   DDREW
;  No change.
;
;     Rev 1.5   05 May 1993 21:25:32   CJKUO
;  Introduce quick inoculation route that skips taking CRC of last K.
;  Also, writes name to inoculation file without drive letter.
;
;     Rev 1.4   28 Apr 1993 18:48:10   DALLEE
;  Fixed bug in number of bytes to use for StartCRC.
;
;     Rev 1.3   24 Apr 1993 23:37:38   CJKUO
;  Introduce new inoculation fields and code.
;
;     Rev 1.2   21 Apr 1993 21:09:42   CJKUO
;  Replaced inoculation repair by a call to inoc_rep_main, found in
;  ir_main.asm.

include CTSN.INC

VERSION_NUMBER EQU 3
UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_PROTECT                         12/23/92                        CAG
;
;       Create or repair a PROTECTED file.
;
; WORD WINAPI FIL4_PROTECT(LPSTR    lpszFile,
;                          LPSTR    lpszOEMFile,
;                          LPSTR    lpszOEMShortFile,
;                          WORD     Flags,
;                          LPVOID   Entry,
;                          UINT     uEntrySize,
;                          LPVOID   Buffer,
;                          LPVOID   Buffer256);
;
; Input:
;       1)  lpszFile =  ptr to what the name should be, will use
;                       as is, with or without directory. this version
;                       of the file name is used for IO operations
;       2)  lpszOEMFile = OEM version of the (long) file name (CRC of name stored).
;                         (if short file name is different than this filename,
;                         then we store a CRC of the long file name).
;       3)  lpszOEMShortFile  = OEM version of the short file name (this is
;                               stored in DB).
;       4)  Create entry/or repair (PROTECT_CREATE/PROTECT_REPAIR)
;       5)  Where the entry will be created.
;       6)  Size of entry record.
;       7)  Work buffer, minimum size = 1024
;       8)  2nd Work buffer, minimum size = at least sizeof(File_ProtectL).
;           **CAUTION** Inoc repair (INOC_REP_MAIN) will recursively call
;           FIL4_PROTECT using this buffer as the "Entry"parameter.
; Output:
;       1)  PASSED (FALSE)(0)
;       2)  FAILED
;               FIL4_NOT_FOUND  (1)
;               REPAIR_FAIL     (4)
;               INVALID_INPUT   (6)
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------
FIL4_PROTECT PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpszFile:LPSTR,
        lpszOEMFile:LPSTR,
        lpszOEMShortFile:LPSTR,
        Flags:WORD,
        Entry:LPBYTE,
        uEntrySize:UINT,
        Buffer:LPBYTE,
        Buffer2:LPBYTE

        LOCAL nStatus:WORD
        LOCAL OldDTA:DWORD
        LOCAL Handle:UINT
        LOCAL OldPTR:DWORD
        LOCAL StartStruct:STARTCODE
        LOCAL uEntrySizeDifference:UINT
        LOCAL lpszShortInocFile:LPSTR
        LOCAL lpszLongInocFile:LPSTR

IF bSYM_VXD
        LOCAL szOEMFile[MAX_NET_PATH_SIZE + 30]:BYTE
ENDIF

        mov     XAX, uEntrySize
        sub     XAX, sFile_Protect
        mov     uEntrySizeDifference, XAX

        mov     nStatus,INVALID_INPUT
        mov     Handle,0

        cmp     Flags,PROTECT_REPAIR
        je      J_Protect_Repair
        cmp     Flags,PROTECT_CREATE
        je      J_Protect_Add
        cmp     Flags,PROTECT_QUICK_INOC
        je      J_Protect_Add

        jmp     J_Done
;--------------------------------------
J_Protect_Add:
        mov     nStatus,FIL4_NOT_FOUND

        cld

        LES_DI  Entry
        mov     XCX,uEntrySize
        xor     al,al
        rep     stosb


        LDS_SI  lpszOEMFile
        cmp     [XSI+1], byte ptr ':'
        jne     Check_UNC_Long
        inc     XSI
        inc     XSI
        jmp     J_SetLong

Check_UNC_Long:

        INVOKE  IsUNCName, lpszOEMFile
        test    XAX,XAX
        jz      J_SetLong

        INVOKE  UNC_SkipServerVolumeName, lpszOEMFile
        cmp     XAX,-1
        je      J_Done

        mov     XSI,XAX

J_SetLong:
        mov     UINT PTR lpszLongInocFile,XSI
IF OS_16
        mov     WORD PTR lpszLongInocFile+2,ds
ENDIF

                                        ; copy file name to entry
        LDS_SI  lpszOEMShortFile
        cmp     [XSI+1], byte ptr ':'   ; Is there a drive letter?
        jne     Check_UNC
        inc     XSI                     ; If so, step over it.
        inc     XSI
        jmp     J_StartCopy

Check_UNC:

        INVOKE  IsUNCName, lpszOEMShortFile
        cmp     XAX, 0
        je      J_StartCopy             ; not UNC

        INVOKE  UNC_SkipServerVolumeName, lpszOEMShortFile
        cmp     XAX, -1                 ; error occured
        je      J_Done                  ; return

        mov     XSI, XAX                ; don't worry about DS, it's already set

J_StartCopy:
        mov     UINT PTR lpszShortInocFile,XSI
IF OS_16
        mov     WORD PTR lpszShortInocFile+2,ds
ENDIF
        LES_DI  Entry
J_CopyString:
        lodsb
        stosb
        or      al,al
        jnz     J_CopyString

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference          ; Skip past variable space
        mov     [XSI].File_Protect.Version,byte ptr VERSION_NUMBER
        add     XSI,File_Protect.Fileattr
        INVOKE  FIL4_STAT,lpszFile,vSDS XSI,Buffer
        or      ax,ax
        jnz     J_Done

        INVOKE  FIL4_OPEN,lpszFile,fREAD OR fSHARE_DENY_NONE
        cmp     XAX, -1
        jz      J_Done
        mov     Handle,XAX

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference          ; Skip past variable space
        add     XSI,File_Protect.Header
        INVOKE  FIL4_READ,Handle,vSDS XSI,35
        cmp     XAX, -1                 ; Error reading.
        jz      J_Done
        or      XAX,XAX                 ; Fail on 0 byte file, too.
        jz      J_Done

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_BEG
        INVOKE  FIL4_GET_START,Handle,Buffer,ADDR StartStruct
        or      ax,ax
        jnz     @F                      ; jmp if success
                                        ; otherwise,
                                        ; it's the case where fstart fails.
                                        ; Get the first 16 bytes to be used
                                        ; for CRC
        INVOKE  FIL4_SEEK,Handle,0,fSEEK_BEG
        INVOKE  FIL4_READ,Handle, Buffer, 16
        cmp     XAX, -1                 ; Error reading.
        jz      J_Done
        or      XAX,XAX                 ; Fail on 0 byte file, too.
        jz      J_Done

@@:
IF OS_16
        mov     bx,ax                   ; Save length
ELSE
        movzx   XBX,ax                  ; Save length
ENDIF

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference          ; Skip past variable space
        INVOKE  MEM4_CRC,0,Buffer,XBX   ; Generate CRC of full buffer
        mov     [XSI].File_Protect.CRCAllBuf,ax

        cmp     XBX,16                   ; Generate the number 1-16
        jle     J_DoStartCRC
        mov     XBX,16

J_DoStartCRC:
        LDS_SI  Entry
        add     XSI,uEntrySizeDifference          ; Skip past variable space
        INVOKE  MEM4_CRC,0,Buffer,XBX
        mov     [XSI].File_Protect.StartCRC,ax

IFNDEF  SYM_NLM                         ; this done to pass Novell certification
                                        ; NLM doesn't use slow inoculation

        cmp     Flags,PROTECT_QUICK_INOC
        je      J_Skip_LastK

        INVOKE  FIL4_SIZE,Handle

IF OS_16
        sub     ax,1024                 ; Make sure there's 1024 bytes
        sbb     dx,0
ELSE
        sub     eax,1024
ENDIF
        jnc     J_DoSeek

        xor     XAX,XAX
IF OS_16
        xor     XDX,XDX
ENDIF

J_DoSeek:
IF OS_16
        INVOKE  FIL4_SEEK,Handle,dx::ax,fSEEK_BEG
ELSE
        INVOKE  FIL4_SEEK,Handle,eax,fSEEK_BEG
ENDIF
        INVOKE  FIL4_CRC,Handle,0,1024,Buffer,1024
        mov     [XSI].File_Protect.CRCLastK,ax

J_Skip_LastK:

        ; If short OEM filename != OEM filename, then do CRC of OEM filename
        ; and store it.
        INVOKE  STR4_ICOMPARE,lpszLongInocFile,lpszShortInocFile
        or      XAX,XAX
        jnz     J_doneWithCreate

        ; Not the same.  CRC filename and save in structure.
IF bSYM_VXD
        INVOKE  STR4_COPY,lpszLongInocFile,ADDR szOEMFile
        INVOKE  STR4_UPPER,ADDR szOEMFile
        INVOKE  STR4_LENGTH,ADDR szOEMFile
        INVOKE  MEM4_CRC,0,ADDR szOEMFile,XAX
ELSE
        INVOKE  STR4_UPPER,lpszLongInocFile
        INVOKE  STR4_LENGTH,lpszLongInocFile
        INVOKE  MEM4_CRC,0,lpszLongInocFile,XAX
ENDIF
        mov     [XSI].File_Protect.wReserved2,ax

ENDIF                                   ; IFNDEF SYM_NLM

J_doneWithCreate:
        mov     nStatus,0
        jmp     J_Done

;--------------------------------------
J_Protect_Repair:

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference          ; Skip past variable space
        cmp     [XSI].File_Protect.Version,byte ptr VERSION_NUMBER
        jne     J_Done

IFNDEF  SYM_NLM                         ; this done to make us pass Novell certification, see dad
        INVOKE  INOC_REP_MAIN,lpszFile,Entry,uEntrySize,Buffer,Buffer2
ENDIF                                   ; IFNDEF NLM
        mov     nStatus,ax

;--------------------------------------
J_Done:
        cmp     Handle,0
        je      J_ReallyDone
        INVOKE  FIL4_CLOSE,Handle

;--------------------------------------
J_ReallyDone:
        mov     ax,nStatus
J_Ret:

        ret

FIL4_PROTECT ENDP
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; SkipServerVolumeName ( LPSTR UNCFileName );
;
; UNCFileName: UNC must start with double backslash
;              (i.e. \\server\vol\path\filename )
; return:
; a pointer to "\path\filename". Note that the return value includes
; a backslash
;
;   in 16-bit land, the returned pointer is dx:ax
;   in 32-bit land, the returned pointer is eax
;
; return XAX = -1 on error
;
;-----------------------------------------------------------------------------
UNC_SkipServerVolumeName PROC USES XCX SDS XSI,
        UNCFileName : LPSTR


        INVOKE STR4_LENGTH, UNCFileName  ; get the length of the string
        cmp    XAX, 0
        je     skip_error

        mov    XCX, XAX             ;

        LDS_SI UNCFileName
        inc    XSI                  ; skip first backslash

skip_server:
        sub    XCX, 1               ; error checking don't loop forever!
        jz     skip_error
        INVOKE DBCS_DBCSIsLeadByte, byte ptr [XSI] ; char is lead byte?
        cmp    XAX, FALSE
        je     @F                   ; if no, advance pointer by one byte
        inc    XSI                  ; advance pointer by one DBCS char
        inc    XSI                  ;  
        sub    XCX, 1               ; another 1 (one) is subtracted at
        jz     skip_error           ;    beginning of skip_server: section
        jmp    skip_server
@@:
        inc    XSI
        cmp    byte ptr [XSI], 05Ch ; backslash '\' ?
        jne    skip_server

skip_volume:
        sub    XCX, 1               ; error checking don't go forever
        jl     skip_error
        INVOKE DBCS_DBCSIsLeadByte, byte ptr [XSI] ; char is lead byte?
        cmp    XAX, FALSE
        je     @F                   ; if no, advance pointer by one byte
        inc    XSI                  ; advance pointer by one DBCS char
        inc    XSI                  ;  
        sub    XCX, 1               ; another 1 (one) is subtracted at
        jl     skip_error           ;    beginning of skip_volume: section
        jmp    skip_volume
@@:
        inc    XSI
        cmp    byte ptr [XSI], 05Ch ; backslash '\'?
        jne    skip_volume
                                    ; XSI point to '\path\filename'
        mov    XAX, XSI

IF OS_16
        mov    dx, ds
ENDIF
        jmp    skip_done
skip_error:
        mov    XAX, -1
skip_done:
        ret

UNC_SkipServerVolumeName ENDP

;-----------------------------------------------------------------------------
; UNC_GetServerVolumeName ( LPSTR UNCFileName );
;
; DX:AX  pointer to buffer holding "\\server\volume" name
;        note that there is no backslash after volume.
;
; XAX = 0 on error
;
;-----------------------------------------------------------------------------
UNC_GetServerVolumeName PROC USES XCX SDS XSI,
        lpUNCFileName : LPSTR,
        lpResult:LPSTR


        INVOKE  STR4_LENGTH, lpUNCFileName  ; get the length of the string
        cmp     XAX, 0
        jle     unc_get_error
        mov     XCX, XAX             ;

        INVOKE  STR4_COPY, lpUNCFileName, lpResult

        LDS_SI  lpResult
        inc     XSI

get_server:
        sub     XCX, 1
        jz      unc_get_error

        INVOKE  DBCS_DBCSIsLeadByte, byte ptr [XSI] ; char is lead byte?
        cmp     XAX, FALSE
        je      @F                   ; if no, advance pointer by one byte
        inc     XSI                  ; advance pointer by one DBCS char
        inc     XSI                  ;    
        sub     XCX, 1               ; another 1 (one) is subtracted at
        jz      unc_get_error        ;    beginning of get_server: section
        jmp     get_server
@@:
        inc     XSI
        cmp     byte ptr [XSI], 05Ch ; backslash '\'?
        jne     get_server

get_volume:
        sub     XCX, 1
        jl      unc_get_error

        INVOKE  DBCS_DBCSIsLeadByte, byte ptr [XSI] ; char is lead byte?
        cmp     XAX, FALSE
        je      @F                   ; if no, advance pointer by one byte
        inc     XSI                  ; advance pointer by one DBCS char
        inc     XSI                  ;
        sub     XCX, 1               ; another 1 (one) is subtracted at
        jl      unc_get_error        ;    beginning of get_volume: section
        jmp     get_volume
@@:                                     
        inc     XSI
        cmp     byte ptr [XSI], 05Ch ; backslash '\'?
        jne     get_volume

        mov     byte ptr [XSI], 0    ; NULL terminate the string
        mov     XAX, XSI

IF OS_16
        mov     dx, ds
ENDIF
        jmp     unc_get_done

unc_get_error:
        LDS_SI  lpResult
        mov     byte ptr [XSI], 0     ;
        XOR     XAX,XAX
unc_get_done:
        ret
UNC_GetServerVolumeName ENDP

;-----------------------------------------------------------------------------
; BOOL IsUNCName ( LPSTR lpFileName );
;
; Parameters:
;    lpFileName: check whether this path is UNC
;
; Return:
;    TRUE  (XAX= 1) if UNC
;    FALSE (XAX= 0) if not UNC
;-----------------------------------------------------------------------------
IsUNCName PROC USES XDX SDS XSI,
        lpFileName : LPSTR

        mov     XAX, 0

        LDS_SI  lpFileName
IF OS_32
        or      esi, 0
        je      NotUNC
ELSE
        mov     dx, ds
        or      dx, si
        jz      NotUNC
ENDIF
        cmp     byte ptr [XSI], 05Ch    ; look for double backslash
        jne     NotUNC

        cmp     byte ptr [XSI+1], 05Ch  ; one backslash is not enough,
        jne     NotUNC                  ; look for another

IsUNC:
        mov     XAX, 1
NotUNC:
        ret
IsUNCName ENDP
;-----------------------------------------------------------------------------

UniqueSegmentEnd
END

