; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\fprot.asv   1.15   29 Nov 1994 13:05:24   DALLEE  $
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
; $Log:   S:\certlib4\vcs\fprot.asv  $
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
include CTS4.INC

VERSION_NUMBER EQU 3
UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_PROTECT                         12/23/92                        CAG
;
;       Create or repair a PROTECTED file.
;
; WORD WINAPI FIL4_PROTECT(LPSTR Filename,WORD Flag, LPSTR Entry,LPSTR 
;                          Buffer,LPSTR Buffer256);
;
; Input:
;       1)  Filename =  ptr to what the name should be, will use
;                       as is, with or without directory.
;       2)  Create entry/or repair (PROTECT_CREATE/PROTECT_REPAIR)
;       3)  Where the entry will be created.
;       4)  Work buffer, minimum size = 1024
;       5)  Work bugger, minimum size = 256
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
        Filename:LPSTR,
        Flags:WORD,
        Entry:LPSTR,
        Buffer:LPSTR,
        Buffer256:LPSTR

        LOCAL nStatus:WORD
        LOCAL OldDTA:DWORD
        LOCAL Handle:WORD
        LOCAL OldPTR:DWORD
        LOCAL StartStruct:STARTCODE

                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, Filename, Filename
@@:
ENDIF

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
        macClearString Entry,sFile_Protect
;       macCopyString Filename,Entry    ; Cannot use macro any more

        LDS_SI  Filename
        cmp     [XSI+1], byte ptr ':'   ; Is there a drive letter?
        jne     J_NoDrive_Copy
        inc     XSI                     ; If so, step over it.
        inc     XSI
J_NoDrive_Copy:
        LES_DI  Entry
J_CopyString:
        lodsb
        stosb
        or      al,al
        jnz     J_CopyString

        LDS_SI  Entry
        mov     [XSI].File_Protect.Version,byte ptr VERSION_NUMBER
        add     XSI,File_Protect.Fileattr
        INVOKE  FIL4_STAT,Filename,vSDS XSI,Buffer
        or      ax,ax
        jnz     J_Done

        INVOKE  FIL4_OPEN,Filename,fREAD OR fSHARE_DENY_NONE
        cmp     ax, -1
        jz      J_Done
        mov     Handle,ax

        LDS_SI  Entry
        add     XSI,File_Protect.Header
        INVOKE  FIL4_READ,Handle,vSDS XSI,35
        or      ax,ax
        jz      J_Done

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_BEG
        INVOKE  FIL4_GET_START,Handle,Buffer,ADDR StartStruct
        or      ax,ax
        jz      J_Done
        mov     bx,ax                   ; Save length

        LDS_SI  Entry
        INVOKE  MEM4_CRC,0,Buffer,bx    ; Generate CRC of full buffer
        mov     [XSI].File_Protect.CRCAllBuf,ax

        cmp     bx,16                   ; Generate the number 1-16
        jle     J_DoStartCRC
        mov     bx,16

J_DoStartCRC:
        LDS_SI  Entry
        INVOKE  MEM4_CRC,0,Buffer,bx
        mov     [XSI].File_Protect.StartCRC,ax

IFNDEF  NLM                             ; this done to pass Novell certification
                                        ; NLM doesn't use slow inoculation

        cmp     Flags,PROTECT_QUICK_INOC
        je      J_Skip_LastK

        INVOKE  FIL4_SIZE,Handle

        sub     ax,1024                 ; Make sure there's 1024 bytes
        sbb     dx,0
        jnc     J_DoSeek

        xor     ax,ax
        xor     dx,dx

J_DoSeek:
        INVOKE  FIL4_SEEK,Handle,dx::ax,fSEEK_BEG
        INVOKE  FIL4_CRC,Handle,0,1024,Buffer,1024
        mov     [XSI].File_Protect.CRCLastK,ax

J_Skip_LastK:

ENDIF
        mov     nStatus,0
        jmp     J_Done

;--------------------------------------
J_Protect_Repair:

        LDS_SI  Entry
        cmp     [XSI].File_Protect.Version,byte ptr VERSION_NUMBER
        jne     J_Done

IFNDEF  NLM                             ; this done to make us pass Novell certification, see dad
        INVOKE  INOC_REP_MAIN,Filename,Entry,Buffer,Buffer256
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
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, Filename, Filename
@@:
        pop     ax
ENDIF

        ret

FIL4_PROTECT ENDP
;-----------------------------------------------------------------------------
END
