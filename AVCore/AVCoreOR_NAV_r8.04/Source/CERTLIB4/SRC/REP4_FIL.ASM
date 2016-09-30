include CTS4.INC
include repair.inc

UniqueSegment

;-----------------------------------------------------------------------------
; REP4_ME                            10/28/92                         JOE
;
;     Receives handle open r/w. Does file read work.
; $Header:   S:/CERTLIB4/VCS/rep4_fil.asv   1.37   09 Dec 1996 11:49:52   JALLEE  $
;
; UINT WINAPI REP4_ME(WORD wVID, WORD wHostHandle, 
;                     LPSTR lpDatabaseName, LPSTR lpBuffer,
;                     WORD uBuffer, LPSTR lpData);
;
; Input:
;       1) N30 entry number
;       2) Host handle
;       3) Ptr to database name.
;       4) Ptr to buffer
;       5) Size of buffer
;       6) Ptr to data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; UPDATE                               12/1/92                    CAG
;-----------------------------------------------------------------------------
; REWORKED                             1/21/93                    JOE
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIB4/VCS/rep4_fil.asv  $
;  
;     Rev 1.37   09 Dec 1996 11:49:52   JALLEE
;  Ported from Qak2
;  ;     Rev 1.31.1.3   27 Nov 1995 18:00:16   DCHI
;  ;  Fixed REP4_ME which did not error out correctly on failure in SEEK
;  ;  or READ.
;  ;  
;  ;     Rev 1.31.1.2   18 Sep 1995 17:23:34   DCHI
;  ;  Changed calls of FIL4_GET_START to FIL4_NEW_GET_START.
;  
;     Rev 1.36   29 Nov 1994 13:11:06   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.31.1.1   01 Nov 1993 15:39:44   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.31.1.0   19 Oct 1993 07:17:10   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.31   12 Oct 1993 16:27:44   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.30.1.3   07 Oct 1993 18:43:26   MFALLEN
;  Function accepting ANSI string in Windows.
;  
;     Rev 1.30.1.2   18 Sep 1993 19:29:14   CGUC
;  Added comment.
;  
;     Rev 1.30.1.1   13 Sep 1993 13:43:22   CAREY
;  Added DIR-II verification for samples that can't be repaired.
;  
;     Rev 1.30.1.0   03 Sep 1993 19:30:40   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.30   27 Aug 1993 17:35:54   CJKUO
;  Fixed bug to enable EXEC_CODE to work right.  Name was being passed at
;  address off by 1
;  
;     Rev 1.29   25 Aug 1993 22:14:50   CJKUO
;  Allow repair code to execute code, same as scan code
;  
;     Rev 1.28   24 Aug 1993 08:51:26   CGUC
;  Added test so we don't try to repair if the drive is writed protected.
;  This was a bug fix necessary for DISKREET.
;  
;     Rev 1.27   14 Aug 1993 16:38:12   BRAD
;  Updated from QAK0.
;  
;     Rev 1.22.2.5   12 Aug 1993 14:51:34   CJKUO
;  Added BOOK_FROM_BEGIN code for repairs
;  
;     Rev 1.22.2.4   06 Aug 1993 17:09:42   CAREY
;  Added tremor repair!
;  
;     Rev 1.25   04 Aug 1993 19:41:50   CAREY
;  ifdef'd out the dir-2 repair under windows & dos TSR since the UNVECTOR
;  function is not being linked in to windows code (and currently this code
;  will not run correctly under windows).
;  
;     Rev 1.24   03 Aug 1993 14:17:04   CAREY
;  fixed bug in CRC repair:  If a CRC entry existed which had the NO_CLEAN flag
;  set or invalid EXE or COM repair flags set, the routine would exit out with
;  error as opposed to trying another CRC.
;  
;     Rev 1.23   02 Aug 1993 18:37:22   CAREY
;  dir-2 repair works for dos 3.3 under DOS scanner.
;  
;     Rev 1.22   29 Jul 1993 16:16:06   CAREY
;  Fixed sliding problem.
;  
;     Rev 1.21   29 Jul 1993 15:02:26   CAREY
;  Fixed bug in the code that attempted multiple CRC repairs.
;  
;     Rev 1.20   28 Jul 1993 14:54:52   CJKUO
;  Checking code path of FIL4_OPEN, bypass close of file that wasn't opened.
;  
;     Rev 1.19   26 Jul 1993 12:11:00   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.18   22 Jul 1993 19:19:52   CAREY
;  Added PROTOs for NEAR
;  
;     Rev 1.17   22 Jul 1993 14:15:30   CAREY
;  Added Tequila repair, Cascade repair, and Maltese Amoeba repairs.
;  (by Carey, checked by Jimmy)
;  
;     Rev 1.16   19 Jul 1993 12:43:56   CJKUO
;  Fixed Carey's work and other stuff
;  
;     Rev 1.15   16 Jul 1993 23:20:06   dallee
;  Added cut_from_begin code.  This code now calls fstart before any repairs
;  are done in order to determine the first landing.  Later, if the first
;  landing bit is set, the host is cut at the specified location.
;  
;     Rev 1.14   15 Jul 1993 22:30:20   CJKUO
;  Add check for DECRYPT and call to REP4_DECRYPT
;  
;     Rev 1.13   12 Jul 1993 22:54:16   CJKUO
;  Save and restore date and time
;  
;     Rev 1.12   12 Jul 1993 19:18:02   RSTANEV
;  Fixed a bug in the search for BookMark when SLIDER is on.
;
;     Rev 1.11   10 May 1993 18:17:54   SZIADEH
;  Included "repair.inc"
;
;-----------------------------------------------------------------------------

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

if FARCALL
VERIFY_BYTE     PROTO FAR,:WORD,:WORD,:BYTE
REP4_NASTY      PROTO FAR,:WORD,:LPSTR,:DWORD,:LPSTR
REP4_CASCADE    PROTO FAR,:WORD,:LPSTR,:DWORD
REP4_MALTESE    PROTO FAR,:WORD,:LPSTR,:DWORD
REP4_TEQUILA    PROTO FAR,:WORD,:LPSTR
REP4_TREMOR     PROTO FAR,:WORD,:LPSTR
ZERO_AND_CUT    PROTO FAR,:WORD,:DWORD
REP4_DIR2       PROTO FAR,:LPSTR,:LPSTR,:WORD
FIX_DIR2_ENTRY  PROTO FAR,:LPSTR,:LPSTR,:WORD
SCN4_EXEC       PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR     ; DOS only
else
VERIFY_BYTE     PROTO NEAR,:WORD,:WORD,:BYTE
REP4_NASTY      PROTO NEAR,:WORD,:LPSTR,:DWORD,:LPSTR
REP4_CASCADE    PROTO NEAR,:WORD,:LPSTR,:DWORD
REP4_MALTESE    PROTO NEAR,:WORD,:LPSTR,:DWORD
REP4_TEQUILA    PROTO NEAR,:WORD,:LPSTR
REP4_TREMOR     PROTO NEAR,:WORD,:LPSTR
ZERO_AND_CUT    PROTO NEAR,:WORD,:DWORD
REP4_DIR2       PROTO NEAR,:LPSTR,:LPSTR,:WORD
FIX_DIR2_ENTRY  PROTO NEAR,:LPSTR,:LPSTR,:WORD
endif

NASTY_REPAIRED      equ     1
NASTY_KEEP_TRYING   equ     2
NASTY_FAIL          equ     0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


REP4_ME         PROC USES XBX XCX XDX SDS SES XSI XDI,
                VID:WORD,               ; This is N30 entry number
                HostHandle:WORD,        ; Handle of infected file
                lpDatabase:LPSTR,       ; N30 name
                lpBuffer:LPSTR,         ; Ptr to my work buffer
                uBuffer:WORD,           ; Size of work buffer
                lpFileName:LPSTR        ; Ptr infected filename

                LOCAL dwRBUF:DWORD
                LOCAL dwVSIZE:DWORD
                LOCAL N30Handle:WORD
                LOCAL CRCMatch:WORD
                LOCAL Filedate:WORD
                LOCAL Filetime:WORD
                LOCAL FirstLanding:DWORD
                LOCAL StartStruct:STARTCODE


                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, lpFilename, lpFilename
        INVOKE  ANSITOOEM, lpDatabase, lpDatabase
@@:
ENDIF


; The VERY first thing we need to do is determine if the file
; is located on a write protected drive, we do this by seeking
; to then end of the file, writing 0 bytes, then seeking to 
; the beginning of the file.

; This WILL NOT WORK!  if a drive has a write-ahead cache.
; You need to do your work, THEN close the file
; and check the RETURN VALUE of CLOSE.

; This is left here for drives that DO NOT have a cache
; so the code will not change.

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END
        INVOKE  FIL4_WRITE,HostHandle,lpBuffer,0
        cmp     ax,-1
        jne     FileIsOkToWrite
        xor     ax,ax                   ; Unable to repair
        jmp     R_EXIT

FileIsOkToWrite:

; The first thing we will do is obtain the first_landing of the
; infected file, just in case we need it later.


        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_GET_START, HostHandle, lpBuffer, ADDR StartStruct

        mov     ax, word ptr StartStruct.Firstlanding
        mov     word ptr FirstLanding, ax
        mov     ax, word ptr StartStruct.Firstlanding+2
        mov     word ptr FirstLanding+2, ax

; now that we have the first landing, go on!

        xor     XAX,XAX
        xor     XDX,XDX

        LDS_SI  lpBuffer                 ; SDS[XSI] pts to Buffer base

        mov     XDI,XSI                  ; SES[XDI] pts to read buffer
        add     XDI,sREPAIR              ; This is for subsequent reads.

        mov     [XSI].RepairStruct.xFIX.wSI,si
        mov     [XSI].RepairStruct.xFIX.wDI,di
        mov     [XSI].RepairStruct.xFIX.wDX,dx
        mov     ax,VID
        mov     [XSI].RepairStruct.xFIX.wAX,ax
        mov     bx,HostHandle
        mov     [XSI].RepairStruct.xFIX.wBX,bx
        mov     cx,uBuffer
        mov     [XSI].RepairStruct.xFIX.wCX,cx

;--------------------------------------
; Set pointer to EOF N30

        INVOKE  FIL4_OPEN,lpDatabase,fREAD
        cmp     ax, -1
        jnz     R_0
        xor     ax,ax
        jmp     R_EXIT
;--------------------------------------------------------------------
R_0:
        mov     N30Handle,ax

        INVOKE  FIL4_SIZE,ax            ; dx,ax has size
        or      ax,ax
        jnz     R_1
        jmp     R_ERROR
;--------------------------------------------------------------------
R_1:
        mov     bx,sN30
        div     bx

        mov     [XSI].RepairStruct.xFIX.wVIDS,ax

        INVOKE  SCN4_GET_ENTRY,N30Handle,VID,lpBuffer

        or      ax,ax
        jnz     R_2
        jmp     R_ERROR

;--------------------------------------------------------------------
R_2:

        test    [XSI].RepairStruct.xN30.wControl,NOCLEAN
        jz      R_3

        ; ASSUME: DS:SI points to the Repair Struct so we can properly
        ; obtain the next CRC value from the 96 byte entry and attempt
        ; another repair.


        jmp     R_ANOTHER_CRC
;--------------------------------------------------------------------
;   ADD A CHECK FOR ENOUGH BUFFER SPACE FOR READ ROUTINE BELOW !!!
;   Currently hard wired for 512 bytes
;--------------------------------------------------------------------
R_3:

        INVOKE  FIL4_DATETIME,HostHandle, Filedate, Filetime,GET_INFO
        mov     Filedate,dx
        mov     Filetime,ax

        test    word ptr[XSI].RepairStruct.xN30.wControl2, EXEC_CODE
        jz      @F

ifdef DOS16
        ; exec code with handle, address of buffer to use which presently
        ; has Entry, StartStruct for FirstLanding, and the two byte name of
        ; the file to execute.

        push    XSI
        ; point at the name of the file to run
        add     XSI,RepairStruct.xN30.wBookMark
        ; Add +1 encryption to the name, to match scan encryption.
        ; Low order byte must not be FF.  But then, it can't be since that
        ; would not be a legal filename character.
        add     [XSI],word ptr 101h
        dec     XSI
        INVOKE  SCN4_EXEC,HostHandle, lpBuffer, ADDR StartStruct, vSDS XSI
        pop     XSI

        or      ax,ax
        jz      R_ANOTHER_CRC
        cmp     ax,1                    ; 1 if repaired OK
        je      RESET_DATE
        cmp     ax,2                    ; Tried but failed and cannot retry
        je      R_ERROR
else
        jmp     R_ANOTHER_CRC
endif

@@:
        ; see if we want to calculate the virus size

        test    word ptr[XSI].RepairStruct.xN30.wControl2, BOOK_FROM_BEGIN
        jz      @F
        INVOKE  FIL4_SIZE,HostHandle    ; dx,ax has size

        ; Only deal with ax because wInfSize is only 16 bits
        sub     ax, word ptr FirstLanding
        mov     word ptr[XSI].RepairStruct.xN30.wInfSize,ax

@@:
        ; try all specific repairs

        INVOKE  REP4_NASTY, HostHandle, lpBuffer, FirstLanding, lpFileName

        cmp     ax, NASTY_REPAIRED      ; file has been repaired!
        jz      RESET_DATE

        cmp     ax, NASTY_KEEP_TRYING   ; no fixed-id repair found!
        jz      @F

        jmp     R_ERROR                 ; unable to repair

@@:

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG

        mov     dx,si
        add     dx,sN30                 ; SDS:XDX pts to MZ read area
        add     dx,sFIX                 ; This is for initial read.

        mov     ax,ds
        mov     word ptr dwRBUF+2,ax
        mov     word ptr dwRBUF,dx

        INVOKE  FIL4_READ,HostHandle,dwRBUF,512

;--------------------------------------
        mov     ax,word ptr[XSI].RepairStruct.xN30.wControl
        cmp     word ptr[XSI].RepairStruct.wMZ,'ZM'
        je      @F
        cmp     word ptr[XSI].RepairStruct.wMZ,'MZ'
        jne     R_4
@@:
        and     ax,EXE_CLEAN           ;Has two EXE clean bits set
        cmp     ax,0                   ;If equal then no Repair
        jne     R_6

        ; failure repairing with this sig.  Try another!

        jmp     R_ANOTHER_CRC
;--------------------------------------
R_4:
        and     ax,COM_CLEAN
        cmp     ax,0                    ;If equal then no Repair
        jne     R_5

        ; failure repairing with this sig.  Try another!

        jmp     R_ANOTHER_CRC
;--------------------------------------
R_5:
        mov     ax,word ptr[XSI].RepairStruct.xN30.wControl
        test    ax,PREPENDING         ;See if Prepend bit is what's left.
        jnz     R5
        test    ax,IMPROVED           ;See if Improved bit is what's left.
        jz      R_6
R5:
        LES_DI  lpBuffer
        add     di,sN30
        add     di,sFIX
        jmp     R_BOOK_2
;--------------------------------------
R_6:
        test    word ptr[XSI].RepairStruct.xN30.wControl,SLIDER
        jz      R_READ

        ; add 32 bytes to the virus size.  We are going to assume that the
        ; virus is 32 bytes longer than in the definition, and then start
        ; sliding for 64 bytes.  Thus the virus can be between +32 and -32
        ; bytes smaller than the recorded size.


        add     [XSI].RepairStruct.xN30.wInfSize,32
;--------------------------------------
R_READ:

        mov     dx,word ptr[XSI].RepairStruct.xN30.wInfSize
        mov     word ptr dwVSIZE,dx
        mov     word ptr dwVSIZE+2,0

        INVOKE  FIL4_SEEK,HostHandle,dwVSIZE,fSEEK_END

        mov     dx,si
        add     dx,sREPAIR

        mov     ax,ds
        mov     word ptr dwRBUF+2,ax
        mov     word ptr dwRBUF,dx

        INVOKE  FIL4_READ,HostHandle,dwRBUF,512

        or      ax,ax
        jnz     R_BOOKMARK
        jmp     R_ERROR

;--------------------------------------
R_BOOKMARK:
        mov     ax,word ptr[XSI].RepairStruct.xN30.wControl
        test    ax,DECRYPT
        jz      @F
;                            Infected file, pointer to VIRSCAN.DAT entry
        INVOKE  REP4_DECRYPT,HostHandle,lpBuffer
        jc      R_ANOTHER_CRC           ; Did not find "bookmark"
        jmp     R_BOOK_FOUND
@@:
        mov     cx, 1                   ; CX = 1. No sliding takes place
        test    word ptr[XSI].RepairStruct.xN30.wControl,SLIDER
        jz      R_BOOK_1
        mov     cx, 64                  ; CX = 64.  Slide 64 bytes


R_BOOK_1:

        LES_DI  lpBuffer
        add     di,sREPAIR
R_BOOK_2:
        mov     ax,word ptr[XSI].RepairStruct.xN30.wBookMark
        xchg    ah,al
        add     di,word ptr[XSI].RepairStruct.xN30.wBookLocation
        mov     bx,word ptr[XSI].RepairStruct.xN30.wControl
        and     bx,SLIDER
        cld
;--------------------------------------
R_BOOK_LOOP:

        cmp     word ptr XES[XDI],ax
        je      R_BOOK_FOUND
        cmp     bx,SLIDER
        je      @F
        jmp     R_ANOTHER_CRC                        ; remember to check
                                                     ; for more viruses!
@@:
        inc     di

        ; we didn't find the bookmark yet.  Decrement the virus size by
        ; 1 and keep searching for it!

        dec     [XSI].RepairStruct.xN30.wInfSize
        loop    R_BOOK_LOOP
R_ANOTHER_CRC:
;--------------------------------------------------------------------------
; Go back to N30 and scan ahead for another entry if CRC.

        mov     ax,word ptr[XSI].RepairStruct.xN30.wCRC
        mov     CRCMatch,ax                             ; Store CRC to match
        or      ax,ax                                   ; if zero leave

        jne     R_REDO
        jmp     R_ERROR
R_REDO:
        inc     VID                                     ; inc entry number
        mov     ax,VID
        cmp     ax,[XSI].RepairStruct.xFIX.wVIDS        ; last entry?
        jne     @F
        jmp     R_ERROR
@@:
        INVOKE  SCN4_GET_ENTRY,N30Handle,VID,lpBuffer   ; get entry

        mov     ax,word ptr[XSI].RepairStruct.xN30.wCRC ; CRCs match
        cmp     CRCMatch,ax
        jne     R_REDO                                  ; Try again

        jmp     R_2
;--------------------------------------
R_BOOK_FOUND:

;--------------------------------------
; Set up and call Repairs

        INVOKE  REP4_CALLS,HostHandle,uBuffer,lpBuffer
        or      ax,ax
        je      R_ERROR

;--------------------------------------
; Truncate file
;
; Now let's see if we want to truncate from the EOF-virus_size or from the
; first_landing point.

        test    word ptr[XSI].RepairStruct.xN30.wControl2, CUT_BEGIN
        jz      CUT_USING_VSIZE

        ; Compute dwVSIZE

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END
        sub     ax,word ptr FirstLanding
        mov     word ptr dwVSIZE,ax

        ; High word is understood to be 0.

        mov     word ptr dwVSIZE+2,0
        jmp     CUT_FILE

CUT_USING_VSIZE:

        ; do the standard virus cut from EOF

        mov     ax,word ptr[XSI].RepairStruct.xN30.wInfSize
        add     ax,word ptr[XSI].RepairStruct.xN30.wTagSize
        mov     word ptr dwVSIZE,ax
        mov     word ptr dwVSIZE+2,0


CUT_FILE:
        INVOKE  ZERO_AND_CUT, HostHandle, dwVSIZE


RESET_DATE:

        INVOKE  FIL4_DATETIME,HostHandle, Filedate, Filetime,SET_INFO

        mov     ax,1
        jmp     R_CLOSE_EXIT

;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_CLOSE_EXIT:
        push    ax
        INVOKE  FIL4_CLOSE,N30Handle
        pop     ax
R_EXIT:
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, lpFilename, lpFilename
        INVOKE  OEMTOANSI, lpDatabase, lpDatabase
@@:
        pop     ax
ENDIF
        ret

REP4_ME       ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


REP4_NASTY      PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                lpBuffer:LPSTR,         ; Ptr to database entry
                FirstLanding:DWORD,     ; dword offset of virus entry
                lpFileName:LPSTR        ; filename of infected file


        LDS_SI  lpBuffer

        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_CASCADE/2
        jne     @F
        INVOKE  REP4_CASCADE, HostHandle, lpBuffer, FirstLanding
        jmp     EXIT_NASTY

@@:

        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_MALTESE/2
        jne     @F
        INVOKE  REP4_MALTESE, HostHandle, lpBuffer, FirstLanding
        jmp     EXIT_NASTY

@@:

        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_TEQUILA/2
        jne     @F
        INVOKE  REP4_TEQUILA, HostHandle, lpBuffer
        jmp     EXIT_NASTY

@@:

        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_TREMOR/2
        jne     @F
        INVOKE  REP4_TREMOR, HostHandle, lpBuffer
        jmp     EXIT_NASTY

@@:

; Only do DIR2 repair for DOS scanner!

ifdef   DOS16
ifndef  SDOS16
ifndef  WIN16

        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_DIR2/2
        jne     @F
        INVOKE  REP4_DIR2, lpFileName, lpBuffer, HostHandle
        jmp     EXIT_NASTY

@@:

endif
endif
endif

        ; if we get here the entry has no specific repair even though it
        ; has a fixed id.  Keep trying repair with normal methods.

        mov     ax, NASTY_KEEP_TRYING
        jmp     NASTY_RETURN

EXIT_NASTY:

        or      ax, ax
        jz      R_NASTY_FAILURE
        mov     ax, NASTY_REPAIRED
        jmp     NASTY_RETURN

R_NASTY_FAILURE:

        mov     ax, NASTY_FAIL

NASTY_RETURN:

        ret

REP4_NASTY      ENDP


; Sample cascade decryption loop follows
;
; 2E48:013D E80000        CALL    0140
; 2E48:0140 5B            POP BX
; 2E48:0141 81EB3101      SUB BX,0131
; 2E48:0145 2E            CS:
; 2E48:0146 F6872A0101    TEST    BYTE PTR [BX+012A],01
; 2E48:014B 740F          JZ  015C
; 2E48:014D 8DB74D01      LEA SI,[BX+014D]
; 2E48:0151 BC8206        MOV SP,0682
; 2E48:0154 3134          XOR [SI],SI
; 2E48:0156 3124          XOR [SI],SP
; 2E48:0158 46            INC SI
; 2E48:0159 4C            DEC SP
; 2E48:015A 75F8          JNZ 0154



REP4_CASCADE    PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                lpBuffer:LPSTR,         ; Ptr to database entry
                dwFirstLanding:DWORD    ; offset of first landing in file


        LOCAL   wSPStartVal: WORD
        LOCAL   wVirSize: WORD
        LOCAL   wDecryptOff: WORD
        LOCAL   wDist: WORD
        LOCAL   bAndVal: BYTE

; Standard default settings (1704 case)
        mov     wSPStartVal, 0685h      ; CASC-1704 initial SP value
        mov     wDecryptOff, 022h       ; start of decrypt buffer
        mov     wVirSize, 1704          ; & size
        mov     wDist, 0bh              ; distance to repair bytes
        mov     bAndVal, 01h            ; is this sample encrypted?

        INVOKE  VERIFY_BYTE, HostHandle, 1700, 0fah
        or      ax, ax
        jz      @F
        mov     wSPStartVal, 0682h      ; CASC-1701 initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
        mov     wVirSize, 1701          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?

        ; now see if we're dealing with a Jo-Jo variant!
        ; The jojo-1703 variant (actually 1701 bytes long) has a SP value
        ; of 20 instead of the usual 682.

        INVOKE  VERIFY_BYTE, HostHandle, 1676, 020h
        or      ax, ax
        jz      REP_STANDARD_CASC       ; Nope, just the normal 1701 variety

        mov     wSPStartVal, 020h       ; CASC- Jo Jo initial SP value

        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1703, 0fah
        or      ax, ax
        jz      @F
;        mov     wSPStartVal, 0685h      ; CASC-1704 initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
;        mov     wVirSize, 1704          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        ; this variant of 1704 starts with 089h instead of the usual 0fah

        INVOKE  VERIFY_BYTE, HostHandle, 1703, 089h
        or      ax, ax
        jz      @F
;        mov     wSPStartVal, 0685h      ; CASC-1704 initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
;        mov     wVirSize, 1704          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 6257, 0fah
        or      ax, ax
        jz      @F
        mov     wSPStartVal, 017D2h     ; CASC-Formiche initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
        mov     wVirSize, 6258          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1705, 0fah
        or      ax, ax
        jz      @F
        mov     wSPStartVal, 0688h      ; CASC-1706 initial SP value
        mov     wDecryptOff, 021h       ; start of decrypt buffer
        mov     wVirSize, 1706          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1618, 0fah
        or      ax, ax
        jz      @F
        mov     wSPStartVal, 0634h      ; CASC-1621 initial SP value
        mov     wDecryptOff, 01fh       ; start of decrypt buffer
        mov     wVirSize, 1621          ; & size
        mov     wDist, 08h              ; distance to repair bytes
        mov     bAndVal, 0ffh           ; non-zero means encrypted

        ; 1621 is slightly different from the rest of the samples in that
        ; it checks to see if it's decrypted by looking 2 bytes before the
        ; first landing.  Adjust the first landing accordingly.

        sub     word ptr dwFirstLanding, 1
        sbb     word ptr dwFirstLanding+2, 0

        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1660, 0fah
        or      ax, ax
        jz      @F
        mov     wSPStartVal, 065ah      ; CASC-1661 initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
        mov     wVirSize, 1661          ; & size
        mov     wDist, 06h              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:



        jmp     EXIT_CASC_ERROR


REP_STANDARD_CASC:

        mov     ax, word ptr dwFirstLanding
        mov     dx, word ptr dwFirstLanding + 2
        sub     ax, 1
        sbb     dx, 0

        INVOKE  FIL4_SEEK, HostHandle, dx::ax,fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, lpBuffer, 100h
        
        LES_DI  lpBuffer                ; DI -> byte indicating decryption
        mov     si, di                  ; SI = DI for encryption test
        inc     di                      ; DI -> 1st instruction of decryptor

        add     di, wDecryptOff         ; DI -> to first byte to be decrypted
        mov     dx, di                  ; DX = DI for later

        mov     al, bAndVal             ; is this virus encrypted?
        and     al, XES[XSI]            ; 1 or non-zero (1621) means encryped
        jz      CASC_COPY_BYTES         ; not encr, just copy the bytes back.

        mov     si, word ptr dwFirstLanding
        add     si, 100h
        add     si, wDecryptOff         ; SI contains initial SI decrypt key

        mov     bx, wSPStartVal         ; BX contains initial SP decrypt key

        mov     cx, 40h                 ; decrypt 64 bytes

STANDARD_DECR_LOOP:

        xor     es:[di], si             ; XOR with SI key
        xor     es:[di], bx             ; XOR with SP key
        inc     si                      ; increment SI key value
        dec     bx                      ; decrement SP key value
        inc     di                      ; point to next byte!
        loop    STANDARD_DECR_LOOP

CASC_COPY_BYTES:

        mov     di, dx                  ; DI -> first byte that was decrypted
        add     di, wDist               ; DI -> first 3 bytes of host COM

        ; copy first 3 bytes back!

        INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 3

        ; zero and cut the virus now!

        xor     dx, dx
        mov     ax, wVirSize

        INVOKE  ZERO_AND_CUT, HostHandle, dx::ax

        mov     ax, 1                   ; success

        jmp     EXIT_CASC_REPAIR


EXIT_CASC_ERROR:

        xor     ax, ax

EXIT_CASC_REPAIR:

        ret

REP4_CASCADE    ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


REP4_MALTESE    PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                lpBuffer:LPSTR,         ; Ptr to database entry
                dwFirstLanding:DWORD    ; offset of first landing in file

        LOCAL   wSig: WORD              ; to hold first 2 bytes of host
        LOCAL   wSize[2] : WORD         ; to hold EXE file size for repairs


        ; first save away the first two bytes of the host so we can determine
        ; whether we need com or exe repair

        INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, ADDR wSig, 2

        ; we need oodles of bytes to correctly do the repairs

        INVOKE  FIL4_SEEK, HostHandle, dwFirstLanding,fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, lpBuffer, 400h

        LES_DI  lpBuffer

        mov     XCX, 300h               ; 768 bytes should be enough
        
MALT_SCAN:

        mov     ax, XES[XDI]            ; load a word up
        xor     ax, 05350h              ; xor with PS to find key value in AX
        mov     bx, XES[XDI+2]          ; load next word
        xor     bx, ax                  ; do we have a match?
        cmp     bx, 05251h              ; yes!
        jz      MALT_REPLACE            ; put stuff back where it should be

        inc     di                      ; advance to next byte & try again
        loop    MALT_SCAN

        jmp     MALT_ERROR              ; unable to repair this sample!


MALT_REPLACE:                           ; key value is in AX at this point

        cmp     wSig, 'MZ'              ; EXE file?
        jz      MALT_EXE
        cmp     wSig, 'ZM'
        jnz     MALT_COM                ; COM file?

MALT_EXE:

        mov     si, di                  ; save for later!

        sub     di, 01ah                ; point to SS of original EXE
        jnc     @F
        mov     di,si
        inc     di
        jmp     MALT_SCAN

@@:
        xor     XES[XDI], ax            ; decrypt SS
        xor     XES[XDI+2], ax          ; decrypt SP
        sub     XES[XDI], word ptr 10h  ; -10h since maltese adds 10h to SS

        push    ax

        ; write SS and SP to file!

        INVOKE  FIL4_SEEK,  HostHandle, 14, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 4

        pop     ax

        mov     di, si                  ; restore value of DI
        sub     di, 0159h               ; point to IP of original EXE
        jnc     @F
        mov     di,si
        inc     di
        jmp     MALT_SCAN

@@:
        xchg    al, ah                  ; swap so our XOR's alligned

        xor     XES[XDI], ax            ; decrypt IP
        xor     XES[XDI+2], ax          ; & CS of original COM

        sub     XES[XDI+2], word ptr 10h; -10h since maltese adds 10h to CS

        ; write IP & CS into exe header.  (seek to 20 and write 4 bytes)

        INVOKE  FIL4_SEEK,  HostHandle, 20, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 4

        mov     dx,word ptr dwFirstLanding+2
        mov     ax,word ptr dwFirstLanding

        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        cmp     dx,0                    ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)

@@:

        mov     wSize, word ptr dx      ; number of bytes in last page
        mov     wSize+2, word ptr ax    ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSize, 4

        jmp     MALT_CUT

MALT_COM:

        sub     di, 01eh                ; DI-01EH -> first 3 bytes of host
        jnc     @F
        add     di, 01fh
        jmp     MALT_SCAN

@@:
        xor     XES[XDI], ax            ; decrypt!
        xor     XES[XDI+2], ax          ; decrypt!

        ; write the host's first 3 bytes back to the TOF

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 3

MALT_CUT:

        ; we have the entry point in dwFirstLanding.  Calculate the virus
        ; size by subtracting the first landing from the infected file size

        INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_END
        sub     ax, word ptr dwFirstLanding
        sbb     dx, word ptr dwFirstLanding+2

        INVOKE  ZERO_AND_CUT, HostHandle, dx::ax

        mov     ax, 1
        jmp     MALT_EXIT

MALT_ERROR:

        xor     ax, ax                  ; error repairing infected file


MALT_EXIT:
        ret

REP4_MALTESE    ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TEQUILA_VIRSIZE EQU 2468

REP4_TEQUILA    PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                lpBuffer:LPSTR          ; Ptr to database entry

        LOCAL wSize[2]: WORD


        ; Tequila encrypts the hosts's information by xoring with the
        ; decryptor bytes.  Thus, the key size is upwards of 36 bytes
        ; long.
        ; The decryptor can be found 68 bytes back from the EOF and
        ; the repair information can be found 2468 bytes back from the EOF
        ; at the top of the virus.
        ; Tequila can either decrypt itself by adding the decryptor or by
        ; xoring with it.  Thus we must try both avenues.

        ; place virus code which contains repair info at offset 0 of lpBuffer

        INVOKE  FIL4_SEEK,HostHandle,TEQUILA_VIRSIZE,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,lpBuffer, 36

        LES_DI  lpBuffer

        mov     si, di
        add     di, 36

        INVOKE  FIL4_SEEK,HostHandle,68,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,vSES XDI, 36

        mov     XCX, 24                 ; Count we need to decrypt

        cld

TEQUILA_XOR_LOOP:

        mov     al, XES[XDI]
        xor     XES[XSI], al
        inc     si
        inc     di
        loop    TEQUILA_XOR_LOOP

        sub     si, 2                   ; -> bytes 22 & 23, should be "We"
        cmp     XES[XSI], word ptr 06557h
        jne     TRY_TEQUILA_ADD         ; try adding decryptor instead
        jmp     TEQUILA_REPAIR

TRY_TEQUILA_ADD:

        ; re-read in our bytes to re-decrypt

        INVOKE  FIL4_SEEK,HostHandle,TEQUILA_VIRSIZE,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,lpBuffer, 36

        LES_DI  lpBuffer

        mov     si, di
        add     di, 36

        mov     XCX, 24

TEQUILA_ADD_LOOP:

        mov     al, XES[XDI]
        add     XES[XSI], al
        inc     si
        inc     di
        loop    TEQUILA_ADD_LOOP

        sub     si, 2                   ; -> bytes 22 & 23 (should be "We")
        cmp     XES[XSI], word ptr 06557h
        jnz     TEQUILA_ERROR           ; Couldn't repair this one...

TEQUILA_REPAIR:

        sub     XSI,22                  ; reset SI to start of buffer
;        LES_SI  lpBuffer

        sub     XES[XSI+2], word ptr 10h; tequila adds +10h to CS & write CS/IP

        INVOKE  FIL4_SEEK,HostHandle,20,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,lpBuffer, 4

        add     si, 4                   ; point to SS of original file & write

        sub     XES[XSI], word ptr 10h  ; tequila adds +10h to SS

        INVOKE  FIL4_SEEK,HostHandle,14,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,vSES XSI, 2

        ; now it's time to cut and such!

        ; to find the new file size, seek to EOF - vir-size.  DX:AX = orig size

        INVOKE  FIL4_SEEK,HostHandle,TEQUILA_VIRSIZE,fSEEK_END

        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        cmp     dx,0                    ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)

@@:

        mov     wSize, word ptr dx      ; number of bytes in last page
        mov     wSize+2, word ptr ax    ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSize, 4

        INVOKE  ZERO_AND_CUT, HostHandle, TEQUILA_VIRSIZE

        mov     ax, 1
        jmp     TEQUILA_EXIT

TEQUILA_ERROR:

        xor     ax, ax                  ; error repairing infected file


TEQUILA_EXIT:

        ret

REP4_TEQUILA    ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


VERIFY_BYTE     PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                wOffsetFromEof:WORD,    ; Seek back from EOF this far
                bVerifyByte:BYTE        ; byte we're trying to match

        LOCAL   bFoundByte:BYTE

        xor     dx, dx
        mov     ax, wOffsetFromEof

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_END

        and     ax, dx                  ; check for errors
        cmp     ax, 0FFFFh              ; were both AX and DX FFFF?
        je      @F

        INVOKE  FIL4_READ,HostHandle,ADDR bFoundByte, 1
        cmp     ax,1
        jne     @F

        mov     al, bVerifyByte
        cmp     al, bFoundByte
        jne     @F

        mov     ax, 1                   ; AX = 1, found!
        jmp     EXIT_VERIFY_BYTE
@@:
        xor     ax, ax                  ; AX = 0 means ERROR

EXIT_VERIFY_BYTE:
        ret

VERIFY_BYTE     ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ZERO_AND_CUT    PROC USES XBX XCX XDX,
                HostHandle:WORD,        ; Handle of infected file
                dwVirSize:DWORD         ; Seek back from EOF this far


        LOCAL   wZeroBuffer : WORD

        mov     wZeroBuffer, 0

ifdef NLM
        xor     XCX,XCX
endif

        mov     cx,word ptr dwVirSize
        shr     cx,1

        INVOKE  FIL4_SEEK,HostHandle,dwVirSize,fSEEK_END; EOHost

ZERO_VIRUS_LOOP:

        INVOKE  FIL4_WRITE,HostHandle,ADDR wZeroBuffer,2; Overwrite virus
        loop    ZERO_VIRUS_LOOP

        INVOKE  FIL4_SEEK,HostHandle,dwVirSize,fSEEK_END; EOHost
        INVOKE  FIL4_WRITE,HostHandle,ADDR wZeroBuffer,0; Truncate file!

        ret

ZERO_AND_CUT        ENDP


ifdef   DOS16
ifndef  SDOS16
ifndef  WIN16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; These are CS local variables for the int 13 handler that's installed.
;; This code should ONLY be run in the dos stand-alone scanner to do dir-2
;; repairs.  Using this in windows would probably be disasterous!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LOCAL_BX        dw  0
LOCAL_CX        dw  0
LOCAL_DX        dw  0
LOCAL_ES        dw  0
lpOld13         dd  0


REP4_DIR2       PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFileName:LPSTR,       ; Full Path of file we're repairing
                lpBuffer:LPSTR,         ; Ptr to my work buffer
                HostHandle:WORD         ; file handle (we need to close it!)

        LOCAL   wMagicNum: WORD
        LOCAL   dwOldDTA:DWORD
        LOCAL   wEntryReturnCode:WORD

        ; obtain the magic number from the dir-2 virus

        INVOKE  FIL4_SEEK, HostHandle, 033fh, fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, ADDR wMagicNum, 2

        ; close the file

        INVOKE  FIL4_CLOSE, HostHandle

        ; reset all disk buffers

        mov     ah, 0dh
        int     21h

        ; save the original int 13h vector & vector it to us now!

        mov     dx, offset EXT_13_ENTRY
        INVOKE  SYS4_VECTOR, 13h, cs :: dx, ADDR lpOld13

        ; save original DTA

        INVOKE  FIL4_CHANGE_DTA, lpBuffer
        mov     word ptr dwOldDTA, ax
        mov     word ptr dwOldDTA+2,dx

        ; find our file.  the last int 13 call will be the one we're
        ; interested in.  use FF as attribute so we catch readonly/hidden too


        INVOKE  FIL4_FIND_FIRST, lpFileName, 0ffh

        ; restore old DTA

        INVOKE  FIL4_CHANGE_DTA, dwOldDTA

        ; point int 13h to where it was originally

        mov     dx, offset EXT_13_ENTRY
        INVOKE  SYS4_UNVECTOR, 13h, cs :: dx, ADDR lpOld13

        INVOKE  FIX_DIR2_ENTRY, lpFileName, lpBuffer, wMagicNum

        mov     wEntryReturnCode, ax    ; remember if we were successful!

        ; now re-open the file.  ASSUME file handle is the same!
        ; reset all disk buffers before opening though

        mov     ah, 0dh
        int     21h

        
        INVOKE  FIL4_OPEN, lpFileName, fREAD_WRITE

        cmp     ax, HostHandle
        jz      RETURN_WITH_HANDLE

        INVOKE  FIL4_CLOSE, ax
        xor     ax, ax                  ; failure
        jmp     EXIT_REP4_DIR2

RETURN_WITH_HANDLE:

        mov     ax, wEntryReturnCode    ; success!

EXIT_REP4_DIR2:

        ret

EXT_13_ENTRY:

        ; save all of the info

        mov     CS:LOCAL_BX, bx
        mov     CS:LOCAL_CX, cx
        mov     CS:LOCAL_DX, dx
        mov     CS:LOCAL_ES, es

        jmp     dword ptr CS:lpOld13

REP4_DIR2       ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


FIX_DIR2_ENTRY  PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFileName:LPSTR,       ; Ptr to path & filename of file
                lpBuffer:LPSTR,         ; work buffer
                wMagicNum: WORD         ; our magic number!

    LOCAL   wTempDi: WORD               ; points to start of FCB filename
    LOCAL   wTempSi: WORD               ; points to start of directory entry

    cld

    LES_DI  lpFileName                  ; find terminating 0
    xor     al, al
    mov     cx, 128
    repnz   scasb

    dec     di
    mov     cx, 13

FIND_BACK_SLASH:

    cmp     XES[XDI], byte ptr '\'
    jz      FOUND_BACK
    dec     di
    loop    FIND_BACK_SLASH

FOUND_BACK:

    inc     di
    mov     cx, 13

    mov     si, di
    mov     ds, word ptr lpFileName+2
    LES_DI  lpBuffer

    rep     movsb

    mov     ax, 02900h                  ; parse filename to FCB
    LDS_SI  lpBuffer                    ; DS:SI point to our filename
    LES_DI  lpBuffer                    ; ES:DI point to FCB contents
    add     di, 13                      ; right after our asciiz filename
    int     21h                         ; ES:DI points to the FCB!

    inc     di                          ; point to filename in FCB

    mov     ax, CS:LOCAL_ES             ; point DS:SI to directory sector
    mov     ds, ax
    mov     si, CS:LOCAL_BX

    mov     wTempDi, di                 ; store away.  ES:DI -> FCB filename
    mov     wTempSi, si                 ; store away.  DS:SI -> cur dir entry
    xor     dx, dx                      ; when this reaches 16 we're done!


LOOK_AGAIN:

    mov     cx, 11                      ; 8 + 3 (file + ext) bytes to compare
    repz    cmpsb                       ; compare
    jne     ENTRY_NOT_FOUND             ; mismatch

    mov     si, wTempSi                 ; DS:SI -> beginning of dir entry

    mov     ax, wMagicNum
    mov     cl, dl                      ; DL is count, CL is # to rotate now!
    rol     ax, cl                      ; rotate AX by specified count

    mov     bx, [si+014h]               ; place original sector pointer in BX
    or      bx, bx                      ; is this file infected?
    jz      FIX_DIR2_ERROR              ; NO - exit!

    xor     bx, ax
    mov     [si+014h], word ptr 0       ; replace encrypted ptr with 0!

    mov     [si+01ah], bx               ; point first cluster to correct place

    mov     si, 5                       ; try up to 5 times to write sector!

TRY_WRITE_AGAIN:

    mov     bx, cs:LOCAL_BX
    mov     cx, cs:LOCAL_CX
    mov     dx, cs:LOCAL_DX
    mov     ax, cs:LOCAL_ES
    mov     es, ax
    mov     ax, 0301h
    int     13h

    jnc     WRITE_OK

    dec     si
    jnz     TRY_WRITE_AGAIN

    jmp     FIX_DIR2_ERROR              ; error writing to sector


WRITE_OK:

    mov     ax, 1                       ; success!
    jmp     EXIT_FIX_DIR2


ENTRY_NOT_FOUND:
    mov     di, wTempDi
    mov     si, wTempSi
    add     si, 32                      ; size of a directory entry
    mov     wTempSi, si                 ; store away new SI
    inc     dx                          ; point to next entry
    cmp     dx, 16                      ; have we hit the end?
    jz      FIX_DIR2_ERROR              ; yes - errrrror!
    jmp     LOOK_AGAIN


FIX_DIR2_ERROR:

    xor     ax, ax

EXIT_FIX_DIR2:

    ret

FIX_DIR2_ENTRY  ENDP

endif
endif
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Tremor repair follows!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


REP4_TREMOR     PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                lpBuffer:LPSTR          ; Ptr to database entry

        LOCAL   wHeaderCS: WORD
        LOCAL   wTweakSS: WORD
        LOCAL   wTweakCS: WORD
        LOCAL   wSig: WORD
        LOCAL   dwSize: DWORD

        ; decrypt the virus!

        INVOKE  REP4_DECRYPT, HostHandle, lpBuffer
        jc      NO_REPAIR_TREMOR

        ; com or exe repair?

        INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, ADDR wSig, 2
        cmp     wSig, 04D5ah
        jz      TREMOR_EXE_REPAIR
        cmp     wSig, 05a4Dh
        jz      TREMOR_EXE_REPAIR

TREMOR_COM_REPAIR:

        ; only repair bytes at 101 and 102.  E9 XX XX  <-- put back X's
        ; tremor only infects COM files beginning with E9, and leaves the E9
        ; at the TOF, so there's no need to put it back!

        INVOKE  FIL4_SEEK, HostHandle, 168, fSEEK_END
        INVOKE  FIL4_READ, HostHandle, ADDR wSig, 2

        INVOKE  FIL4_SEEK, HostHandle, 1, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSig, 2

        mov     ax, 1
        jmp     TREMOR_TRUNC


TREMOR_EXE_REPAIR:

        ; obtain the CS tweak value!

        INVOKE  FIL4_SEEK, HostHandle, 148, fSEEK_END
        INVOKE  FIL4_READ, HostHandle, ADDR wTweakCS, 2

        ; obtain the SS tweak value!

        INVOKE  FIL4_SEEK, HostHandle, 159, fSEEK_END
        INVOKE  FIL4_READ, HostHandle, ADDR wTweakSS, 2

        ; obtain the virus's CS value

        INVOKE  FIL4_SEEK, HostHandle, 22, fSEEK_BEG
        INVOKE  FIL4_READ, HostHandle, ADDR wHeaderCS, 2

        ; FIX CS value first!

        mov     ax, wHeaderCS
        sub     ax, wTweakCS
        mov     wTweakCS, ax

        ; write the host's CS value

        INVOKE  FIL4_SEEK, HostHandle, 22, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wTweakCS, 2

        ; FIX CS value next!

        mov     ax, wHeaderCS
        sub     ax, wTweakSS
        mov     wTweakSS, ax

        ; write the host's SS value

        INVOKE  FIL4_SEEK, HostHandle, 14, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wTweakSS, 2

        ; now write SP back!

        INVOKE  FIL4_SEEK, HostHandle, 154, fSEEK_END
        INVOKE  FIL4_READ, HostHandle, ADDR wSig, 2

        INVOKE  FIL4_SEEK, HostHandle, 16, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSig, 2

        ; now write IP back!

        INVOKE  FIL4_SEEK, HostHandle, 144, fSEEK_END
        INVOKE  FIL4_READ, HostHandle, ADDR wSig, 2

        INVOKE  FIL4_SEEK, HostHandle, 20, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSig, 2

        ; change file size in header now!

        INVOKE  FIL4_SEEK, HostHandle, 4000, fSEEK_END

        ; DX:AX = file size after virus is remvoed (- 4000 bytes)


        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        cmp     dx,0                    ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)

@@:

        mov     word ptr dwSize, dx     ; number of bytes in last page
        mov     word ptr dwSize+2, ax   ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR dwSize, 4

TREMOR_TRUNC:

        ; get rid of the virus body!

        INVOKE  ZERO_AND_CUT, HostHandle, 4000

        mov     ax, 1                   ; success, return!
        jmp     REPAIR_TREMOR



NO_REPAIR_TREMOR:

        xor     ax, ax


REPAIR_TREMOR:

        ret

REP4_TREMOR     ENDP




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

END



