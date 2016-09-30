;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/rep4_fil.asv   1.13   27 Jun 1996 17:31:58   DALLEE  $
;;
;; Description: Defines REP4_ME function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/rep4_fil.asv  $
;  
;     Rev 1.13   27 Jun 1996 17:31:58   DALLEE
;  LuigiPlus port:
;  Comments out DIR2 repairs for DX platform (code moving to NAVEX?).
;  
;     Rev 1.12   10 Apr 1996 15:10:48   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.11   11 Jan 1996 17:55:08   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.10   29 Nov 1995 14:25:38   DCHI
;  Changed calls of FIL4_GET_START to FIL4_NEW_GET_START and
;  Fixed REP4_ME which did not correctly error out on failure in SEEK
;  or READ.
;  
;     Rev 1.9   29 Jun 1995 02:14:32   SZIADEH
;  activated loading of EXTERNAL DAT files for bSYM_DOS platform.
;  
;     Rev 1.8   05 Apr 1995 13:48:12   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.7   05 Apr 1995 00:47:10   DALLEE
;  !@#%# Bug!!!
;  
;  W32 platform (and probably other 32-bit platforms) -- INVOKE macro when
;  casting a WORD to a DWORD argument translates it to PUSH 0, PUSH wArg.
;  This causes 8 bytes to be put on the stack, not the 4 expected!
;  
;  All this fun without even a compiler warning!
;  This instance of the problem was located in VERIFY_BYTE and was causing
;  the FIL4_SEEK to fail which in turn blew the CASCADE repairs.
;  
;  Someone (probably me...) will have to take the time to verify for all
;  CERTLIBN that we aren't depending on INVOKE to properly convert
;  WORD's to DWORD's for us.
;  
;     Rev 1.6   22 Aug 1994 18:38:04   DALLEE
;  Commented out SCN4_EXEC -- breaks .DX platform.
;  We're no longer using this update scheme (hopefully).
;  
;     Rev 1.5   17 Aug 1994 13:26:14   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.4   19 Jul 1994 16:25:04   CAREY
;  Changed si/di references to XSI/XDI to handle 32-bit correctly.
;  
;     Rev 1.3   26 May 1994 10:32:28   RSTANEV
;  Change of prototype in the comments.
;
;     Rev 1.2   24 May 1994 07:45:12   RSTANEV
;  Fixed a conflict between FILETIME and the variable wFileTime.
;
;     Rev 1.1   23 May 1994 16:31:08   RSTANEV
;  Fixed wrong parameter passing.
;
;     Rev 1.0   23 May 1994 09:13:10   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

VERIFY_BYTE     PROTO CODEDISTANCE,:UINT,:WORD,:BYTE
REP4_NASTY      PROTO CODEDISTANCE,:UINT,:LPBYTE,:DWORD,:LPSTR
REP4_CASCADE    PROTO CODEDISTANCE,:UINT,:LPBYTE,:DWORD
REP4_MALTESE    PROTO CODEDISTANCE,:UINT,:LPBYTE,:DWORD
REP4_TEQUILA    PROTO CODEDISTANCE,:UINT,:LPBYTE
REP4_TREMOR     PROTO CODEDISTANCE,:UINT,:LPBYTE
ZERO_AND_CUT    PROTO CODEDISTANCE,:UINT,:DWORD

IFDEF  DOS16
  IFNDEF SYM_DOSX
  REP4_DIR2       PROTO CODEDISTANCE,:LPSTR,:LPBYTE,:WORD
  FIX_DIR2_ENTRY  PROTO CODEDISTANCE,:LPSTR,:LPBYTE,:WORD
  ENDIF
ENDIF

NASTY_FAIL          EQU     0
NASTY_REPAIRED      EQU     1
NASTY_KEEP_TRYING   EQU     2

UniqueSegment

;----------------------------------------------------------------------------
; REP4_ME
;
; Receives handle open r/w. Does file read work.
;
; BOOL WINAPI REP4_ME ( WORD wVID, UINT wHostHandle,
;                       LPSTR lpDatabaseName, LPVOID lpBuffer,
;                       UINT uBuffer, LPSTR lpFileName );
;
; Input:
;       1) N30 entry number
;       2) Host handle
;       3) Ptr to database name (N30 name?)
;       4) Ptr to buffer
;       5) Size of buffer
;       6) Ptr to data (infected filename?)
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
;----------------------------------------------------------------------------
REP4_ME  PROC  CODEDISTANCE  USES  XBX XCX XDX SDS SES XSI XDI,
        VID:WORD,
        HostHandle:UINT,
        lpDatabase:LPSTR,
        lpBuffer:LPBYTE,
        uBuffer:UINT,
        lpFileName:LPSTR

        LOCAL dwRBUF:DWORD
        LOCAL dwVSIZE:DWORD
        LOCAL N30Handle:UINT
        LOCAL CRCMatch:WORD
IF OS_16
        LOCAL wFileDate:WORD
        LOCAL wFileTime:WORD
ELSE
        LOCAL FileDateTime:DWORD
ENDIF
        LOCAL FirstLanding:DWORD
        LOCAL StartStruct:STARTCODE


        ;IFDEF   WIN16
        ;        INVOKE  GetAnsiFlag
        ;        or      ax, ax
        ;        jz      @F
        ;        INVOKE  ANSITOOEM, lpFilename, lpFilename
        ;        INVOKE  ANSITOOEM, lpDatabase, lpDatabase
        ;@@:
        ;ENDIF

        ; The VERY first thing we need to do is determine if the file
        ; is located on a write protected drive, we do this by seeking
        ; to then end of the file, writing 0 bytes, then seeking to
        ; the beginning of the file.

        ; This WILL NOT WORK! if a drive has a write-ahead cache.
        ; You need to do your work, THEN close the file
        ; and check the RETURN VALUE of CLOSE.

        ; This is left here for drives that DO NOT have a cache
        ; so the code will not change.

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END
        INVOKE  FIL4_WRITE,HostHandle,lpBuffer,0
        inc     XAX
        jz      R_EXIT

                                        ; The first thing we will do is
                                        ; obtain the first_landing of the
                                        ; infected file, just in case we
                                        ; need it later.

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_NEW_GET_START, HostHandle, lpBuffer, ADDR StartStruct

IF OS_16
        mov     ax,WORD PTR StartStruct.Firstlanding
        mov     WORD PTR FirstLanding, ax
        mov     ax,WORD PTR StartStruct.Firstlanding + 2
        mov     WORD PTR FirstLanding + 2, ax
ELSE
        mov     XAX,StartStruct.Firstlanding
        mov     FirstLanding,XAX
ENDIF

                                        ; now that we have the first landing,
                                        ; go on!

        LDS_SI  lpBuffer                ; XDS[XSI] pts to Buffer base

        lea     XDI,[XSI + sREPAIR]     ; XES[XDI] pts to read buffer
                                        ; + sREPAIR is for subsequent reads.

IF FALSE
        mov     [XSI].RepairStruct.xFIX.wSI,XSI
        mov     [XSI].RepairStruct.xFIX.wDI,XDI
        mov     [XSI].RepairStruct.xFIX.wDX,dx
        mov     ax,VID
        mov     [XSI].RepairStruct.xFIX.wAX,ax
        mov     bx,HostHandle
        mov     [XSI].RepairStruct.xFIX.wBX,bx
        mov     cx,uBuffer
        mov     [XSI].RepairStruct.xFIX.wCX,cx
ENDIF

                                        ; Set pointer to EOF N30

        INVOKE  FIL4_OPEN,lpDatabase,fREAD
        inc     XAX
        jz      R_EXIT
        dec     XAX
        mov     N30Handle,XAX

        ECHO Warning: Fix the repairs when DAT file size in on a 64K border.

        INVOKE  FIL4_SIZE,XAX           ; This is a workaround for a
        or      ax,ax                   ; problem when the DAT file size is
        jz      R_ERROR                 ; on a 64K border.

IF OS_32
        xor     XDX,XDX
ENDIF
        mov     XBX,sN30
        div     XBX

        mov     [XSI].RepairStruct.xFIX.wVIDS,ax

        INVOKE  SCN4_GET_ENTRY,N30Handle,VID,lpBuffer
        or      XAX,XAX
        jz      R_ERROR

R_2:

        test    [XSI].RepairStruct.xN30.wControl,NOCLEAN

        ; ASSUME: DS:SI points to the Repair Struct so we can properly
        ; obtain the next CRC value from the 96 byte entry and attempt
        ; another repair.

        jnz     R_ANOTHER_CRC

ECHO Warning: Add a check for enough buffer space for read routine below.
ECHO Currently hard wired for 512 bytes.

        INVOKE  FIL4_DATETIME, HostHandle, 0, 0, GET_INFO
IF OS_16
        mov     wFileTime,ax
        mov     wFileDate,dx
ELSE
        mov     FileDateTime,XAX
ENDIF

        test    [XSI].RepairStruct.xN30.wControl2, EXEC_CODE
        jz      @F

IF bSYM_DOS
        ; exec code with handle, address of buffer to use which presently
        ; has Entry, StartStruct for FirstLanding, and the two byte name of
        ; the file to execute.

        push    XSI
                                        ; point at the name of the file
                                        ; to run.
        add     XSI,RepairStruct.xN30.wBookMark
                                        ; Add +1 encryption to the name, to
                                        ; match scan encryption. Low order
                                        ; byte must not be FF.  But then, it
                                        ; can't be since that would not be
                                        ; a legal filename character.
        add     [XSI],WORD PTR 101h
        dec     XSI
        INVOKE  SCN4_EXEC, HostHandle, lpBuffer, ADDR StartStruct, vSDS XSI
        pop     XSI

        or      XAX,XAX
        jz      R_ANOTHER_CRC
        cmp     XAX,1                   ; 1 if repaired OK
        je      RESET_DATE
        cmp     XAX,2                   ; Tried but failed and cannot retry
        je      R_ERROR
ELSE
        jmp     R_ANOTHER_CRC
ENDIF

@@:
                                        ; see if we want to calculate
                                        ; the virus size

        test    [XSI].RepairStruct.xN30.wControl2, BOOK_FROM_BEGIN
        jz      @F
        INVOKE  FIL4_SIZE,HostHandle

                                        ; Only deal with ax because wInfSize
                                        ; is only 16 bits

        ECHO Warning: Not keeping the full file size.

        sub     ax, WORD PTR FirstLanding
        mov     [XSI].RepairStruct.xN30.wInfSize,ax

@@:
        ; try all specific repairs

        INVOKE  REP4_NASTY, HostHandle, lpBuffer, FirstLanding, lpFileName

        cmp     XAX, NASTY_REPAIRED     ; file has been repaired!
        jz      RESET_DATE

        cmp     XAX, NASTY_KEEP_TRYING  ; no fixed-id repair found!
        jz      @F

        jmp     R_ERROR                 ; unable to repair

@@:

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG

        mov     XDX,XSI
        add     XDX,sN30 + sFIX         ; SDS:XDX pts to MZ read area
                                        ; (+ sFIX is for initial read.)
IF OS_16
        mov     WORD PTR dwRBUF + 2,ds
        mov     WORD PTR dwRBUF,XDX
ELSE
        mov     dwRBUF, XDX
ENDIF

        INVOKE  FIL4_READ,HostHandle,dwRBUF,512

        mov     ax,[XSI].RepairStruct.xN30.wControl
        cmp     WORD PTR [XSI].RepairStruct.wMZ,'ZM'
        je      @F
        cmp     WORD PTR [XSI].RepairStruct.wMZ,'MZ'
        jne     R_4
@@:
        and     ax,EXE_CLEAN           ;Has two EXE clean bits set
        jnz     R_6                    ;If 0 then no Repair

        ; failure repairing with this sig.  Try another!

        jmp     R_ANOTHER_CRC

R_4:
        and     ax,COM_CLEAN
        jnz     R_5                     ;If 0 then no Repair

        ; failure repairing with this sig.  Try another!

        jmp     R_ANOTHER_CRC

R_5:
        mov     ax,[XSI].RepairStruct.xN30.wControl
        test    ax,PREPENDING         ;See if Prepend bit is what's left.
        jnz     R5
        test    ax,IMPROVED           ;See if Improved bit is what's left.
        jz      R_6
R5:
        LES_DI  lpBuffer
        add     XDI,sN30 + sFIX
        jmp     R_BOOK_2

R_6:
        test    [XSI].RepairStruct.xN30.wControl,SLIDER
        jz      R_READ

        ; add 32 bytes to the virus size.  We are going to assume that the
        ; virus is 32 bytes longer than in the definition, and then start
        ; sliding for 64 bytes.  Thus the virus can be between +32 and -32
        ; bytes smaller than the recorded size.


        add     [XSI].RepairStruct.xN30.wInfSize,32

R_READ:

        movuw   XDX,[XSI].RepairStruct.xN30.wInfSize

IF OS_16
        mov     WORD PTR dwVSIZE,XDX
        mov     WORD PTR dwVSIZE + 2,0
ELSE
        mov     dwVSIZE,XDX
ENDIF

        INVOKE  FIL4_SEEK,HostHandle,XDX,fSEEK_END

IF OS_16
        and     XAX,XDX
        cmp     XAX,0FFFFh
ELSE
        cmp     XAX,0FFFFFFFFh
ENDIF
        je      R_ANOTHER_CRC

        mov     XDX,XSI
        add     XDX,sREPAIR

IF OS_16
        mov     WORD PTR dwRBUF + 2,ds
        mov     WORD PTR dwRBUF,XDX
ELSE
        mov     dwRBUF,XDX
ENDIF

        INVOKE  FIL4_READ,HostHandle,dwRBUF,512

        or      XAX,XAX
        jnz     R_BOOKMARK
        jmp     R_ANOTHER_CRC

R_BOOKMARK:
        mov     ax,[XSI].RepairStruct.xN30.wControl
        test    ax,DECRYPT
        jz      @F
;                            Infected file, pointer to VIRSCAN.DAT entry
        INVOKE  REP4_DECRYPT,HostHandle,lpBuffer
        jc      R_ANOTHER_CRC           ; Did not find "bookmark"
        jmp     R_BOOK_FOUND
@@:
        mov     XCX,1                   ; CX = 1. No sliding takes place
        test    [XSI].RepairStruct.xN30.wControl,SLIDER
        jz      R_BOOK_1
        mov     XCX, 64                 ; CX = 64.  Slide 64 bytes


R_BOOK_1:

        LES_DI  lpBuffer
        add     XDI,sREPAIR
R_BOOK_2:
        movuw   XAX, [XSI].RepairStruct.xN30.wBookLocation
        test    ax, 08000h
        jnz     R_BOOK_FOUND

IF OS_16
        add     XDI,[XSI].RepairStruct.xN30.wBookLocation
ELSE
        movuw   XAX,[XSI].RepairStruct.xN30.wBookLocation
        add     XDI,XAX
ENDIF
        mov     ax,[XSI].RepairStruct.xN30.wBookMark
        xchg    ah,al
        mov     bx,[XSI].RepairStruct.xN30.wControl
        and     bx,SLIDER
        cld

R_BOOK_LOOP:

        cmp     XES[XDI],ax
        je      R_BOOK_FOUND
        cmp     bx,SLIDER
        je      @F
        jmp     R_ANOTHER_CRC           ; remember to check
                                        ; for more viruses!
@@:
        inc     XDI                     ; di -> XDI             -Carey

        ; we didn't find the bookmark yet.  Decrement the virus size by
        ; 1 and keep searching for it!

        dec     [XSI].RepairStruct.xN30.wInfSize
        loop    R_BOOK_LOOP
R_ANOTHER_CRC:

        ; Go back to N30 and scan ahead for another entry if CRC.

        mov     ax,[XSI].RepairStruct.xN30.wCRC
        mov     CRCMatch,ax             ; Store CRC to match
        or      ax,ax                   ; if zero leave

        jne     R_REDO
        jmp     R_ERROR
R_REDO:
        inc     VID                     ; inc entry number
        mov     ax,VID
        cmp     ax,[XSI].RepairStruct.xFIX.wVIDS  ; last entry?
        jne     @F
        jmp     R_ERROR
@@:
        INVOKE  SCN4_GET_ENTRY,N30Handle,VID,lpBuffer  ; get entry

        mov     ax,[XSI].RepairStruct.xN30.wCRC  ; CRCs match
        cmp     CRCMatch,ax
        jne     R_REDO                  ; Try again

        jmp     R_2

R_BOOK_FOUND:

                                        ; Set up and call Repairs

        INVOKE  REP4_CALLS,HostHandle,uBuffer,lpBuffer
        or      ax,ax
        je      R_ERROR

                                        ; Truncate file
                                        ; Now let's see if we want to
                                        ; truncate from the EOF-virus_size
                                        ; or from the first_landing point.
        test    [XSI].RepairStruct.xN30.wControl2, CUT_BEGIN
        jz      CUT_USING_VSIZE

                                        ; Compute dwVSIZE

        ECHO Warning: Wouldn't this cut destroy the file?

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END
        sub     ax,WORD PTR FirstLanding
        mov     WORD PTR dwVSIZE,ax

                                        ; High word is understood to be 0.

        mov     WORD PTR dwVSIZE + 2,0
        jmp     SHORT CUT_FILE

CUT_USING_VSIZE:

                                        ; do the standard virus cut from EOF

        mov     ax,[XSI].RepairStruct.xN30.wInfSize
        add     ax,[XSI].RepairStruct.xN30.wTagSize
        mov     WORD PTR dwVSIZE,ax
        mov     WORD PTR dwVSIZE + 2,0


CUT_FILE:
        INVOKE  ZERO_AND_CUT, HostHandle, dwVSIZE


RESET_DATE:

IF OS_16
        INVOKE  FIL4_DATETIME, HostHandle, wFileDate, wFileTime, SET_INFO
ELSE
        INVOKE  FIL4_DATETIME, HostHandle, WORD PTR FileDateTime + 2, WORD PTR FileDateTime, SET_INFO
ENDIF

        mov     XAX,TRUE
        jmp     R_CLOSE_EXIT

R_ERROR:
        xor     XAX,XAX

R_CLOSE_EXIT:
        push    XAX
        INVOKE  FIL4_CLOSE,N30Handle
        pop     XAX
R_EXIT:

        ;IFDEF   WIN16
        ;        push    ax
        ;
        ;        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        ;        or      ax, ax                  ;
        ;        jz      @F                      ; NO
        ;
        ;        INVOKE  OEMTOANSI, lpFilename, lpFilename
        ;        INVOKE  OEMTOANSI, lpDatabase, lpDatabase
        ;@@:
        ;        pop     ax
        ;ENDIF

        ret

REP4_ME  ENDP

REP4_NASTY  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,        ; Handle of infected file
        lpBuffer:LPBYTE,        ; Ptr to database entry
        FirstLanding:DWORD,     ; DWORD offset of virus entry
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

IFDEF DOS16
  IFNDEF SYM_DOSX
        cmp     [XSI].RepairStruct.xN30.wFixedID, FID_DIR2/2
        jne     @F
        INVOKE  REP4_DIR2, lpFileName, lpBuffer, HostHandle
        jmp     EXIT_NASTY

@@:
  ENDIF
ENDIF

                                        ; If we get here the entry has
                                        ; no specific repair even though it
                                        ; has a fixed id.  Keep trying
                                        ; repair with normal methods.
        mov     XAX, NASTY_KEEP_TRYING
        jmp     NASTY_RETURN

EXIT_NASTY:

        or      XAX,XAX
        jz      R_NASTY_FAILURE
        mov     XAX, NASTY_REPAIRED
        jmp     NASTY_RETURN

R_NASTY_FAILURE:

        mov     XAX, NASTY_FAIL

NASTY_RETURN:

        ret

REP4_NASTY  ENDP

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

REP4_CASCADE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,        ; Handle of infected file
        lpBuffer:LPBYTE,        ; Ptr to database entry
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
        or      XAX,XAX
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
        or      XAX,XAX
        jz      REP_STANDARD_CASC       ; Nope, just the normal 1701 variety

        mov     wSPStartVal, 020h       ; CASC- Jo Jo initial SP value

        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1703, 0fah
        or      XAX,XAX
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
        or      XAX,XAX
        jz      @F
;        mov     wSPStartVal, 0685h      ; CASC-1704 initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
;        mov     wVirSize, 1704          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 6257, 0fah
        or      XAX,XAX
        jz      @F
        mov     wSPStartVal, 017D2h     ; CASC-Formiche initial SP value
;        mov     wDecryptOff, 022h       ; start of decrypt buffer
        mov     wVirSize, 6258          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1705, 0fah
        or      XAX,XAX
        jz      @F
        mov     wSPStartVal, 0688h      ; CASC-1706 initial SP value
        mov     wDecryptOff, 021h       ; start of decrypt buffer
        mov     wVirSize, 1706          ; & size
;        mov     wDist, 0bh              ; distance to repair bytes
;        mov     bAndVal, 01h            ; is this sample encrypted?
        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1618, 0fah
        or      XAX,XAX
        jz      @F
        mov     wSPStartVal, 0634h      ; CASC-1621 initial SP value
        mov     wDecryptOff, 01fh       ; start of decrypt buffer
        mov     wVirSize, 1621          ; & size
        mov     wDist, 08h              ; distance to repair bytes
        mov     bAndVal, 0ffh           ; non-zero means encrypted

        ; 1621 is slightly different from the rest of the samples in that
        ; it checks to see if it's decrypted by looking 2 bytes before the
        ; first landing.  Adjust the first landing accordingly.

IF OS_16
        sub     word ptr dwFirstLanding, 1
        sbb     word ptr dwFirstLanding + 2, 0
ELSE
        dec     dwFirstLanding
ENDIF

        jmp     REP_STANDARD_CASC

@@:

        INVOKE  VERIFY_BYTE, HostHandle, 1660, 0fah
        or      XAX,XAX
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

IF OS_16
        mov     ax, WORD PTR dwFirstLanding
        mov     dx, WORD PTR dwFirstLanding + 2
        sub     ax, 1
        sbb     dx, 0

        INVOKE  FIL4_SEEK, HostHandle, dx::ax, fSEEK_BEG
ELSE
        mov     XAX,dwFirstLanding
        dec     XAX

        INVOKE  FIL4_SEEK, HostHandle, XAX, fSEEK_BEG
ENDIF

        INVOKE  FIL4_READ, HostHandle, lpBuffer, 100h

        LES_DI  lpBuffer                ; DI -> byte indicating decryption
        mov     XSI,XDI                 ; SI = DI for encryption test
        inc     XDI                     ; DI -> 1st instruction of decryptor

IF OS_16
        add     XDI,wDecryptOff         ; DI -> to first byte to be decrypted
ELSE
        movuw   XAX,wDecryptOff
        add     XDI,XAX
ENDIF

        mov     XDX,XDI                 ; DX = DI for later

        mov     al, bAndVal             ; is this virus encrypted?
        and     al, XES[XSI]            ; 1 or non-zero (1621) means encryped
        jz      CASC_COPY_BYTES         ; not encr, just copy the bytes back.

        mov     si, WORD PTR dwFirstLanding
        add     si, 100h
        add     si, wDecryptOff         ; SI contains initial SI decrypt key

        mov     bx, wSPStartVal         ; BX contains initial SP decrypt key

        mov     XCX,40h                 ; decrypt 64 bytes

STANDARD_DECR_LOOP:

        xor     XES[XDI], si            ; XOR with SI key
        xor     XES[XDI], bx            ; XOR with SP key
        inc     si                      ; increment SI key value
        dec     bx                      ; decrement SP key value
        inc     XDI                     ; point to next byte!
        loop    STANDARD_DECR_LOOP

CASC_COPY_BYTES:

        mov     XDI,XDX                 ; DI -> first byte that was decrypted
IF OS_16
        add     XDI,wDist               ; DI -> first 3 bytes of host COM
ELSE
        movuw   XAX,wDist
        add     XDI,XAX
ENDIF

        ; copy first 3 bytes back!

        INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 3

        ; zero and cut the virus now!
        movuw   XAX, wVirSize
        INVOKE  ZERO_AND_CUT, HostHandle, XAX

        mov     XAX,TRUE                ; success

        jmp     EXIT_CASC_REPAIR


EXIT_CASC_ERROR:

        xor     XAX,XAX

EXIT_CASC_REPAIR:

        ret

REP4_CASCADE  ENDP


REP4_MALTESE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,        ; Handle of infected file
        lpBuffer:LPBYTE,        ; Ptr to database entry
        dwFirstLanding:DWORD    ; offset of first landing in file

        LOCAL   wSig: WORD              ; to hold first 2 bytes of host
        LOCAL   wSize: DWORD            ; to hold EXE file size for repairs

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
        mov     bx, XES[XDI + 2]        ; load next word
        xor     bx, ax                  ; do we have a match?
        cmp     bx, 05251h              ; yes!
        jz      MALT_REPLACE            ; put stuff back where it should be

        inc     XDI                     ; advance to next byte & try again
        loop    MALT_SCAN

        jmp     MALT_ERROR              ; unable to repair this sample!


MALT_REPLACE:                           ; key value is in AX at this point

        cmp     wSig,'MZ'               ; EXE file?
        jz      MALT_EXE
        cmp     wSig,'ZM'
        jnz     MALT_COM                ; COM file?

MALT_EXE:

        mov     XSI,XDI                 ; save for later!

        sub     XDI,01ah                ; point to SS of original EXE
        jnc     @F
        mov     XDI,XSI
        inc     XDI
        jmp     MALT_SCAN

@@:
        xor     XES[XDI], ax            ; decrypt SS
        xor     XES[XDI + 2], ax        ; decrypt SP
        sub     XES[XDI], WORD PTR 10h  ; -10h since maltese adds 10h to SS

        push    XAX

        ; write SS and SP to file!

        INVOKE  FIL4_SEEK,  HostHandle, 14, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 4

        pop     XAX

        mov     XDI,XSI                 ; restore value of DI
        sub     XDI,0159h               ; point to IP of original EXE
        jnc     @F
        mov     XDI,XSI
        inc     XDI
        jmp     MALT_SCAN

@@:
        xchg    al, ah                  ; swap so our XOR's alligned

        xor     XES[XDI], ax            ; decrypt IP
        xor     XES[XDI+2], ax          ; & CS of original COM

        sub     XES[XDI+2], WORD PTR 10h; -10h since maltese adds 10h to CS

        ; write IP & CS into exe header.  (seek to 20 and write 4 bytes)

        INVOKE  FIL4_SEEK,  HostHandle, 20, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, vSES XDI, 4

        mov     dx,word ptr dwFirstLanding+2
        mov     ax,word ptr dwFirstLanding

        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        or      dx,dx                   ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)

@@:
        mov     WORD PTR wSize, dx      ; number of bytes in last page
        mov     WORD PTR wSize + 2, ax  ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSize, 4

        jmp     MALT_CUT

MALT_COM:

        sub     XDI,01eh                ; DI-01EH -> first 3 bytes of host
        jnc     @F
        add     XDI,01fh
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
IF OS_16
        sub     ax, WORD PTR dwFirstLanding
        sbb     dx, WORD PTR dwFirstLanding + 2
        INVOKE  ZERO_AND_CUT, HostHandle, dx::ax
ELSE
        sub     XAX, dwFirstLanding
        INVOKE  ZERO_AND_CUT, HostHandle, XAX
ENDIF

        mov     XAX,TRUE
        jmp     MALT_EXIT

MALT_ERROR:

        xor     XAX,XAX                 ; error repairing infected file

MALT_EXIT:
        ret

REP4_MALTESE  ENDP

TEQUILA_VIRSIZE  EQU  2468

REP4_TEQUILA  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        lpBuffer:LPBYTE                 ; Ptr to database entry

        LOCAL wSize:DWORD

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

        mov     XSI,XDI
        add     XDI,36

        INVOKE  FIL4_SEEK,HostHandle,68,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,vSES XDI, 36

        mov     XCX, 24                 ; Count we need to decrypt

        cld

TEQUILA_XOR_LOOP:

        mov     al, XES[XDI]
        xor     XES[XSI], al
        inc     XSI
        inc     XDI
        loop    TEQUILA_XOR_LOOP

        sub     XSI,2                   ; -> bytes 22 & 23, should be "We"
        cmp     XES[XSI], WORD PTR 06557h
        jne     TRY_TEQUILA_ADD         ; try adding decryptor instead
        jmp     TEQUILA_REPAIR

TRY_TEQUILA_ADD:

        ; re-read in our bytes to re-decrypt

        INVOKE  FIL4_SEEK,HostHandle,TEQUILA_VIRSIZE,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,lpBuffer, 36

        LES_DI  lpBuffer

        mov     XSI,XDI
        add     XDI,36

        mov     XCX,24

TEQUILA_ADD_LOOP:

        mov     al, XES[XDI]
        add     XES[XSI], al
        inc     XSI
        inc     XDI
        loop    TEQUILA_ADD_LOOP

        sub     XSI,2                   ; -> bytes 22 & 23 (should be "We")
        cmp     XES[XSI], WORD PTR 06557h
        jnz     TEQUILA_ERROR           ; Couldn't repair this one...

TEQUILA_REPAIR:

        sub     XSI,22                  ; reset SI to start of buffer
;        LES_SI  lpBuffer

        sub     XES[XSI+2], WORD PTR 10h; tequila adds +10h to CS & write CS/IP

        INVOKE  FIL4_SEEK,HostHandle,20,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,lpBuffer, 4

        add     XSI,4                   ; point to SS of original file & write

        sub     XES[XSI], WORD PTR 10h  ; tequila adds +10h to SS

        INVOKE  FIL4_SEEK,HostHandle,14,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,vSES XSI, 2

        ; now it's time to cut and such!

        ; to find the new file size, seek to EOF - vir-size.  DX:AX = orig size

        INVOKE  FIL4_SEEK,HostHandle,TEQUILA_VIRSIZE,fSEEK_END

IF OS_32
        ror     XAX,10h
        mov     dx,ax
        ror     XAX,10h
ENDIF

        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        or      dx,dx                   ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)
@@:
        mov     WORD PTR wSize, dx      ; number of bytes in last page
        mov     WORD PTR wSize + 2, ax  ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR wSize, 4

        INVOKE  ZERO_AND_CUT, HostHandle, TEQUILA_VIRSIZE

        mov     XAX,TRUE
        jmp     TEQUILA_EXIT

TEQUILA_ERROR:

        xor     XAX,XAX                 ; error repairing infected file

TEQUILA_EXIT:

        ret

REP4_TEQUILA  ENDP

VERIFY_BYTE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        wOffsetFromEof:WORD,            ; Seek back from EOF this far
        bVerifyByte:BYTE                ; byte we're trying to match

        LOCAL   bFoundByte:BYTE

        movuw   XAX, wOffsetFromEof
        INVOKE  FIL4_SEEK,HostHandle,XAX,fSEEK_END
IF OS_16
        and     ax, dx                  ; check for errors
        cmp     ax, 0FFFFh              ; were both AX and DX FFFF?
ELSE
        inc     XAX
ENDIF
        je      @F

        INVOKE  FIL4_READ,HostHandle,ADDR bFoundByte, 1
        cmp     XAX,1
        jne     @F

        mov     al, bVerifyByte
        cmp     al, bFoundByte
        jne     @F

        mov     XAX,TRUE                ; AX = 1, found!
        jmp     SHORT EXIT_VERIFY_BYTE
@@:
        xor     XAX,XAX                 ; AX = 0 means ERROR

EXIT_VERIFY_BYTE:
        ret

VERIFY_BYTE  ENDP

ZERO_AND_CUT  PROC  CODEDISTANCE  USES XBX XCX XDX,
        HostHandle:UINT,                ; Handle of infected file
        dwVirSize:DWORD                 ; Seek back from EOF this far

        LOCAL   wZeroBuffer : WORD

        mov     wZeroBuffer, 0

IF OS_16
        mov     cx,WORD PTR dwVirSize
ELSE
        mov     XCX,dwVirSize
ENDIF
        shr     XCX,1

        INVOKE  FIL4_SEEK,HostHandle,dwVirSize,fSEEK_END; EOHost

ZERO_VIRUS_LOOP:

        INVOKE  FIL4_WRITE,HostHandle,ADDR wZeroBuffer,2; Overwrite virus
        loop    ZERO_VIRUS_LOOP

        INVOKE  FIL4_SEEK,HostHandle,dwVirSize,fSEEK_END; EOHost
        INVOKE  FIL4_WRITE,HostHandle,ADDR wZeroBuffer,0; Truncate file!

        ret

ZERO_AND_CUT  ENDP

IFDEF DOS16
  IFNDEF SYM_DOSX
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

REP4_DIR2  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpFileName:LPSTR,               ; Full Path of file we're repairing
        lpBuffer:LPBYTE,                ; Ptr to my work buffer
        HostHandle:UINT                 ; file handle (we need to close it!)

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


FIX_DIR2_ENTRY  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
                lpFileName:LPSTR,       ; Ptr to path & filename of file
                lpBuffer:LPBYTE,        ; work buffer
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

  ENDIF
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Tremor repair follows!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

REP4_TREMOR  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        lpBuffer:LPBYTE                 ; Ptr to database entry

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

        mov     XAX,TRUE
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
IF OS_32
        ror     XAX,10h
        mov     dx,ax
        ror     XAX,10h
ENDIF
        ; DX:AX = file size after virus is remvoed (- 4000 bytes)


        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        or      dx,dx                   ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)
@@:

        mov     WORD PTR dwSize,dx      ; number of bytes in last page
        mov     WORD PTR dwSize + 2,ax  ; number of pages in file

        ; fix EXE header's file size entries.

        INVOKE  FIL4_SEEK,  HostHandle, 2, fSEEK_BEG
        INVOKE  FIL4_WRITE, HostHandle, ADDR dwSize, 4

TREMOR_TRUNC:

        ; get rid of the virus body!

        INVOKE  ZERO_AND_CUT, HostHandle, 4000

        mov     XAX,TRUE                ; success, return!
        jmp     REPAIR_TREMOR

NO_REPAIR_TREMOR:

        xor     XAX,XAX

REPAIR_TREMOR:

        ret

REP4_TREMOR  ENDP

UniqueSegmentEnd
END
