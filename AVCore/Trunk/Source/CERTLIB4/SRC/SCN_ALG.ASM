; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIB4/VCS/scn_alg.asv   1.39   09 Dec 1996 11:53:18   JALLEE  $
;
; Filename:	
; Programmer:	
; Assembler:	MASM 6.0+
; Project:	NAV 3.0
;
; Contents:	
;
; Note:		
;		
; Copyright (C) 1992-1993 Symantec Corporation
;
; $Log:   S:/CERTLIB4/VCS/scn_alg.asv  $
;  
;     Rev 1.39   09 Dec 1996 11:53:18   JALLEE
;  Updated from Qak2 post 3.10.
;  
;     Rev 1.38   29 Nov 1994 13:09:54   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.35.1.23   10 Nov 1994 08:01:16   JMILLARD
;  fix problem with detecting seek errors
;  fix get_test to correctly save the file offset in 32-bit mode
;  add comment regarding ignoring the high word of file offset in DF_FOLLOWJUM
;  
;     Rev 1.35.1.22   17 Oct 1994 15:38:28   SZIADEH
;  fixed push/pop SDS since NLM complained!
;  
;     Rev 1.35.1.21   09 Oct 1994 19:45:48   SZIADEH
;  FirstLanding does is not necessarily equate to the offset (in the file) 
;  of the work buffer. The correct offset in the file is required for the
;  implementation of FollowJump(). Thus, a new component was added to 
;  STARTCODE structure called BufferOffset to keep track of such offset.
;  Files affected are: CTS4.INC CTS4.H FSTART.ASM SCN_ME.ASM SCN_ALG.ASM.
;  
;     Rev 1.35.1.20   07 Oct 1994 16:21:26   SZIADEH
;  In FollowJump, changed  STARTCODE to STARTSIZE.
;  
;     Rev 1.35.1.19   06 Oct 1994 17:15:56   CJKUO
;  Took out a second set of neg XCX and mov XDX,XCX from ifdef SDOS16
;  to fix TSR bug
;  
;     Rev 1.35.1.18   05 Oct 1994 10:31:02   SZIADEH
;  commented out "int 03" in the followjump code.
;  
;     Rev 1.35.1.17   15 Sep 1994 01:13:02   SZIADEH
;  added FollowJump and ScanMask functionality. 
;  
;     Rev 1.35.1.16   31 Aug 1994 14:35:12   JWELLS
;  Moved LES_DI out of IFDEF so pointer would be initialized for TSR.
;  
;     Rev 1.35.1.15   12 Jun 1994 15:02:14   CJKUO
;  Allow string search to use D1 as a skip byte
;  
;     Rev 1.35.1.14   11 Jun 1994 01:27:38   CJKUO
;  1.35.1.12, hope this works
;  
;     Rev 1.35.1.12   10 Jun 1994 16:16:30   CJKUO
;  Decryption routines, enables Natas detection
;  
;     Rev 1.35.1.11   08 Jun 1994 14:14:48   CJKUO
;  For 3.0.4 and after, skip VIRSC0x.DAT files.
;  
;     Rev 1.35.1.10   07 Jun 1994 17:24:42   CJKUO
;  Make regular version as functionally eq to TSR version as possible.
;  Works for TSR.
;  
;     Rev 1.35.1.9   06 Jun 1994 18:48:26   CJKUO
;  SMEG:Queeg TSR code, first try
;  
;     Rev 1.35.1.8   03 Jun 1994 19:36:46   CJKUO
;  SMEG detector (instead of Queeg specific)
;  Again, TSR doesn't work yet.
;  
;     Rev 1.35.1.7   25 May 1994 20:25:44   CJKUO
;  Fix Pathogen TSR detection
;  
;     Rev 1.35.1.6   25 May 1994 13:43:12   CJKUO
;  Pathogen TSR changes
;  
;     Rev 1.35.1.5   18 May 1994 18:45:50   CJKUO
;  Pathogen detection, plus encrypted search functions.
;  TSR will not work if Pathogen def is there!!!!!
;  
;     Rev 1.35.1.4   22 Dec 1993 11:47:16   SZIADEH
;  changed jl to jb in the SCANBACK function.
;  
;  
;     Rev 1.35.1.3   13 Dec 1993 10:07:14   JMILLARD
;  fix problem with using FIL4_SIZE return value in 32-bit mode
;  
;     Rev 1.35.1.2   06 Dec 1993 17:05:52   CJKUO
;  SEEKBACK routine now checks size of suspect file before seeking
;  
;     Rev 1.35.1.1   18 Nov 1993 09:34:26   CJKUO
;  Satan Bug false id.  Increase detection criteria to require at least 14
;  sets of illogical combinations.
;  
;     Rev 1.35.1.0   19 Oct 1993 07:25:44   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.35   12 Oct 1993 16:35:58   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.34.1.2   15 Sep 1993 20:26:42   CAREY
;  Changed SeekEOF(xxx) code so that we never read from the file if we seek
;  BEFORE the TOF.  This was crashing the banyan server on certain files.
;  
;     Rev 1.34.1.1   07 Sep 1993 13:09:30   CJKUO
;  Add 3 to XDI in GIRAFE detection so XDI-3 wouldn't go out of bounds
;  
;     Rev 1.34.1.0   03 Sep 1993 19:29:44   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.34   24 Aug 1993 18:12:40   CJKUO
;  CBWE doesn't exist and must be CBW followed by CWDE
;  
;     Rev 1.33   20 Aug 1993 17:29:02   CJKUO
;  Added DF_VOID and DF_USE_NEXT functions for ultimate expandability project
;  
;     Rev 1.32   19 Aug 1993 23:44:50   CJKUO
;  Add DF_EXEC function
;  
;     Rev 1.31   17 Aug 1993 20:47:18   CJKUO
;  FollowJmp1 function
;  
;     Rev 1.30   14 Aug 1993 16:42:02   BRAD
;  Updated from QAK0.
;  
;     Rev 1.19.1.11   13 Aug 1993 15:35:28   CAREY
;  Commented out verification code for tremor detection.  The verification code
;  was causing some mutant samples to be missed.  If any false ID situations
;  arise we can uncomment the code out and add a CRC for the mutants.
;  
;     Rev 1.19.1.10   12 Aug 1993 20:56:58   CJKUO
;  Enter LowNibble and VerifyDByte functions
;  
;     Rev 1.19.1.9   10 Aug 1993 18:20:48   CAREY
;  Sped up Girafe detection.
;  
;     Rev 1.28   10 Aug 1993 17:27:42   JMILLARD
;  check in Carey's Girafe detection
;  
;     Rev 1.27   10 Aug 1993 12:58:36   JMILLARD
;  fix 32-bit problems in Tremor/SatanBug
;  
;     Rev 1.26   09 Aug 1993 16:26:28   CAREY
;  Added Satan Bug detection as alg function.
;  
;     Rev 1.25   06 Aug 1993 17:07:16   CAREY
;  Enhanced tremor detection.
;  
;     Rev 1.24   06 Aug 1993 12:23:38   CAREY
;  Added more stringent Tremor detection.
;  
;     Rev 1.23   05 Aug 1993 17:53:36   CAREY
;   removed int 3
;  
;     Rev 1.22   05 Aug 1993 16:28:52   CAREY
;  Added tremor detection -> 0xDF, 0x06.
;  
;     Rev 1.21   04 Aug 1993 21:05:36   CAREY
;  Aded new df function to scan for strings
;  
;     Rev 1.19   30 Jul 1993 14:33:40   CJKUO
;  Don't use al after an invoke, fix previous checkin
;  
;     Rev 1.18   30 Jul 1993 14:12:50   CJKUO
;  Allow first byte to be DF and let that drop directly to wildcard matching
;  
;     Rev 1.17   22 Jul 1993 14:00:10   CGUC
;  Fixed prototype that failed in SDOS16 (tsr library)
;  
;     Rev 1.16   21 Jul 1993 23:18:08   JMILLARD
;  fix prototypes for wildcard and compare_strings for OS_32
;  
;     Rev 1.15   20 Jul 1993 21:12:10   CJKUO
;  Added FollowJmpE9 if used from header
;  
;     Rev 1.14   11 Jul 1993 20:44:58   CJKUO
;  SEEK_BAK should have been SEEK_END
;  
;     Rev 1.13   10 Jul 1993 12:05:24   CJKUO
;  Added functions.
;  
;     Rev 1.12   09 Jul 1993 15:06:52   CJKUO
;  Added first DF function.
;  Check into header
;  
;     Rev 1.11   09 Jul 1993 00:43:06   CJKUO
;  Allow DF functions
;  
;     Rev 1.10   06 Jul 1993 21:06:12   JMILLARD
;  save whole loop count, not just lsw
;  
;     Rev 1.9   01 Jul 1993 13:40:02   CJKUO
;  Changed logic to support +1 encryption
;  
;     Rev 1.8   21 May 1993 16:55:16   SCOURSEN
;  Fixed D0 wildcard, fixed D1-D9 wildcard
;  
;     Rev 1.6   18 May 1993 15:49:38   JMILLARD
;  further 32-bit fixes - this code is ugly - we need a better approach
;  
;     Rev 1.5   18 May 1993 15:21:24   JMILLARD
;  fix 32-bit problems with jump table
;  
;     Rev 1.4   15 May 1993 15:12:30   JWELLS
;  Completed high level wildcarding using jmp table.
;  
;     Rev 1.3   23 Mar 1993 17:32:06   JMILLARD
;  Fix problem in sliding stuff for Tequila per jwells
;  
;     Rev 1.2   17 Mar 1993 17:53:44   GUC_C
;  Updated from J.M
;  
;     Rev 1.2   16 Mar 1993 20:20:08   JMILLARD
;  fix problem with missing last signature - broken by 1-base change
;  
;     Rev 1.4   18 Jan 1993 14:55:44   JMILLARD
;  Fixed a 32-bit problem and some other things I ran into while
;  looking for 32-bit problems.
;  
;  1. Deleted inc,inc of XSI at beginning. fstart doesn't put the count
;     in the buffer.
;  
;  2. Always check counters for zero before entering a loop. Some defs are
;     zero, and this was crashing the server with some samples. I would
;     like to leave this in as insurance. Crashing the server is a big deal.
;     If a zero count is seen, it fails compare_strings or SlidePointer
;     and goes to the next definition.
;  
;  3. Change mask for wildcards from d0 to f0. This was picking up 0Fxh
;     bytes in definitions and using them as wild cards.
;  
;  4. Restore xor ah,ah at beginning of WildCard. I had made it xor XAX,XAX
;     for 32-bit reasons. This was clearly wrong, and Carlton took it out.
;     It is now back the way it was. The upper part of eax is zeroed earlier
;     in scn_alg.
;  
;     Rev 1.3   08 Jan 1993 13:32:46   JMILLARD
;  chkin in change from Carlton - see comment below
;
;     Rev 1.3   08 Jan 1993 11:10:00   CAG
;  Moved a xor XAX XAX to assure that AL dosn't clear.
;  
;     Rev 1.1   07 Jan 1993 22:35:20   JMILLARD
;  Fix 32-bit problems, we must ensure the high word of 32-bit loop
;  counters is zero - also a problem with adding XAX to a pointer, the
;  high word of eax wasn't guaranteed zero.
;
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
include CTS4.INC
include repair.inc

ifdef SDOS16
get_test PROTO NEAR, :LPSTR
wildcard PROTO NEAR,:WORD,:LPSTR, :LPSTR, :BYTE
compare_strings PROTO NEAR,:WORD,:LPSTR, :DWORD
SCN4_TREMOR     PROTO NEAR,:LPSTR,:WORD
SCN4_TREMOR_XOR PROTO NEAR,:LPSTR
SCN4_TREMOR_ADD PROTO NEAR,:LPSTR
SCN4_SATAN_BUG  PROTO NEAR,:LPSTR,:WORD
SCN4_GIRAFE     PROTO NEAR,:LPSTR
;SCN4_PATHOGEN   PROTO NEAR,:LPSTR,:WORD
;SMEG_SEARCH     PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_XOR_DELTA PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_ADD_DELTA PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_ROTATE  PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_NEG  PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_NOT  PROTO NEAR,:LPSTR,:LPSTR
SCN4_QUEEG      PROTO NEAR,:LPSTR,:WORD
TSR_PATHOGEN_BUFFER_LOCATION EQU     (STARTSIZE*2)
else

if FARCALL

get_test PROTO FAR, :LPSTR
wildcard PROTO FAR,:WORD,:LPSTR, :LPSTR, :BYTE
compare_strings PROTO FAR,:WORD,:LPSTR, :DWORD
SCN4_TREMOR     PROTO FAR,:LPSTR,:WORD
SCN4_TREMOR_XOR PROTO FAR,:LPSTR
SCN4_TREMOR_ADD PROTO FAR,:LPSTR
SCN4_SATAN_BUG  PROTO FAR,:LPSTR,:WORD
SCN4_GIRAFE     PROTO FAR,:LPSTR
;SCN4_PATHOGEN   PROTO FAR,:LPSTR,:WORD
;SMEG_SEARCH     PROTO FAR,:LPSTR,:LPSTR
DECRYPT_XOR_DELTA PROTO FAR,:LPSTR,:LPSTR
DECRYPT_ADD_DELTA PROTO FAR,:LPSTR,:LPSTR
DECRYPT_ROTATE  PROTO FAR,:LPSTR,:LPSTR
DECRYPT_NEG  PROTO FAR,:LPSTR,:LPSTR
DECRYPT_NOT  PROTO FAR,:LPSTR,:LPSTR
SCN4_QUEEG      PROTO FAR,:LPSTR,:WORD
SCN4_EXEC       PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR
else

if FARDATA

get_test PROTO FAR, :LPSTR
wildcard PROTO FAR,:WORD,:LPSTR, :LPSTR, :BYTE
compare_strings PROTO FAR,:WORD,:LPSTR, :DWORD
SCN4_TREMOR     PROTO FAR,:LPSTR,:WORD
SCN4_TREMOR_XOR PROTO FAR,:LPSTR
SCN4_TREMOR_ADD PROTO FAR,:LPSTR
SCN4_SATAN_BUG  PROTO FAR,:LPSTR,:WORD
SCN4_GIRAFE     PROTO FAR,:LPSTR
;SCN4_PATHOGEN   PROTO FAR,:LPSTR,:WORD
;SMEG_SEARCH     PROTO FAR,:LPSTR,:LPSTR
DECRYPT_XOR_DELTA PROTO FAR,:LPSTR,:LPSTR
DECRYPT_ADD_DELTA PROTO FAR,:LPSTR,:LPSTR
DECRYPT_ROTATE  PROTO FAR,:LPSTR,:LPSTR
DECRYPT_NEG  PROTO FAR,:LPSTR,:LPSTR
DECRYPT_NOT  PROTO FAR,:LPSTR,:LPSTR
SCN4_QUEEG      PROTO FAR,:LPSTR,:WORD
else

get_test PROTO NEAR, :LPSTR
wildcard PROTO NEAR,:WORD,:LPSTR, :LPSTR, :BYTE
compare_strings PROTO NEAR,:WORD,:LPSTR, :DWORD
SCN4_TREMOR     PROTO NEAR,:LPSTR,:WORD
SCN4_TREMOR_XOR PROTO NEAR,:LPSTR
SCN4_TREMOR_ADD PROTO NEAR,:LPSTR
SCN4_SATAN_BUG  PROTO NEAR,:LPSTR,:WORD
SCN4_GIRAFE     PROTO NEAR,:LPSTR
;SCN4_PATHOGEN   PROTO NEAR,:LPSTR,:WORD
;SMEG_SEARCH     PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_XOR_DELTA PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_ADD_DELTA PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_ROTATE  PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_NEG  PROTO NEAR,:LPSTR,:LPSTR
DECRYPT_NOT  PROTO NEAR,:LPSTR,:LPSTR
SCN4_QUEEG      PROTO NEAR,:LPSTR,:WORD
endif
endif
endif

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_ALG                              10/28/92                         JOE
;
;      Scan for a match using the ALGs
;
; UINT WINAPI SCN4_ALG(WORD Handle, LPSTR Buffer, WORD BufferSize, LPSTR Data);
;
; Input:
;       1)  Handle of program to scan
;       2)  Buffer to look at
;       3)  Size of buffer
;       4)  Scan data
;
; Output:
;       1)  No Virus = 0 or Virus ID=?
;
;-----------------------------------------------------------------------------
; UPDATE                               12/1/92				CAG
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;       32 bit, clean.
;
;-----------------------------------------------------------------------------
SCN4_ALG PROC USES XBX XCX XDX SDS SES XSI XDI,
        Handle:WORD,
        lpBuffer:LPSTR,
        uBuffer:WORD,
        lpData:LPSTR,
        dwStartOffset: DWORD

        LOCAL   wSigsFailed:WORD        ; # sigs from prev blks failed
        LOCAL   wSigsInBlk:WORD         ; # sigs in current blk
IF OS_16
        LOCAL   plpNextSigBuf:WORD      ; points to ptr to next sig buf
ELSE
        LOCAL   plpNextSigBuf:DWORD     ; points to ptr to next sig buf
ENDIF

        LDS_SI  lpBuffer                ; point to string

        mov     word ptr [XSI+300h], 0  ; zero out # of external scan requests

        mov     XBX,XSI                 ; bx saves start si

        LES_DI  lpData

IFDEF SDOS16
        xor     XCX,XCX
                                        ; Count = number of ALG sigs
        mov     cx,XES[XDI].HEADER.uNumALGs

        add     XDI,sHEADER             ; First ALG sig
ELSE
        xor     XAX,XAX
        mov     ax,XES[XDI].HEADER.uNumCRCs
        shl     XAX,1
        add     XDI,sHEADER             ; XDI pts to CRCs
        add     XDI,XAX                 ; XDI pts to next blk ptr

        mov     plpNextSigBuf,XDI       ; Save ptr to next blk ptr

        xor     ax,ax                   ; Initialize count of total number
        mov     wSigsFailed,ax          ; of sigs in prev blks failed to 0

;; The outer loop that loops through the blocks starts here

BlkLoop:
        mov     XDI,plpNextSigBuf

IF OS_16
        LES_DI  XES[XDI]
        mov     ax,es
        cmp     ax,0                    ; Is the ptr NULL?
ELSE
        xor     XAX,XAX
        mov     XDI,XES[XDI]
        cmp     XDI,0
ENDIF
        je      exit_all                ; If so, then exit failed

        mov     plpNextSigBuf,XDI       ; Save ptr to next blk ptr

        add     XDI,SIZE LPSTR          ; Advance past next blk ptr

        xor     XCX,XCX
        mov     cx,WORD PTR XES[XDI]    ; CX gets # sigs in this blk
        mov     wSigsInBlk,cx           ; Save # sigs in this blk
        add     XDI,2                   ; Advance past # sigs field
ENDIF

        xor     XAX,XAX

ALGLoop:

        ; *CAREY -> modified below

        mov     al,byte ptr XES[XDI+1]  ; 1st byte of SIG (*NOT* size byte)
        dec     al                      ; decrypt this byte
        cmp     al,0DFh                 ; allow special character for bypass
        je      J_GoAnyway
        cmp     al,byte ptr [XSI]       ; compare to scanned-file's byte
        jne     @F
J_GoAnyway:

        ; *CAREY -> modified above
	
        push    XCX                     ;save count

        INVOKE  compare_strings,Handle,lpBuffer, dwStartOffset  ;go compare more bytes
        pop     XCX                     ;restore count
        jc      exit_infected
@@:
        add XDI,SIZE_SIG    ;go to next sig
        loop	ALGLoop

IFDEF SDOS16
        xor     XAX, XAX                ;zero ax
        jmp     exit_all

exit_infected:

        LES_DI  lpData

        xor     XAX,XAX
                                        ;Get number of ALG sigs
        mov     ax, XES[XDI].HEADER.uNumALGs
        sub     XAX, XCX                ;Calculate virus number
        inc     XAX                     ; 1-base virus id
ELSE
        mov     ax,wSigsInBlk
        add     wSigsFailed,ax          ;Add to count of sigs tried
        jmp     BlkLoop                 ;Try next block

exit_infected:
        ; DCHI -> modified below for new id calculation method

        xor     XBX, XBX
        mov     bx, wSigsInBlk          ; Calculate virus number based
        sub     XBX, XCX                ; on current sig id in this blk
        xor     XAX, XAX
        mov     ax, wSigsFailed         ; plus the number of sigs
        add     XAX, XBX                ; from previous blks.
        inc     XAX                     ; 1-base virus id

        ; DCHI -> modified above
ENDIF

exit_all:
        ret

SCN4_ALG        ENDP
;----------------------------------------------------------------------------
; Compares strings
;
;       si--> String from file
;       di--> Detection sig
;
;----------------------------------------------------------------------------
compare_strings PROC USES XAX XSI XDI XBX,
        Handle:WORD,
        Buffer:LPSTR,
        dwStartOffset: DWORD

        LOCAL   byCurrentByte:BYTE

        xor     XCX, XCX
        mov     cl,byte ptr XES[XDI]    ; get length of signature
        dec     cl                      ; *CAREY -> decrypt

        or      cl,cl                   ; ensure count not 0
        jz      mismatch
        cmp     cl,1                    ; if the sig was only length 1,
        je      pre_allmatch            ;   we've already matched

;        cmp     al,0DFh                 ; al still has first def character
;       al might be destroyed depending on parameter list in INVOKE
        cmp     byte ptr XES[XDI+1],0E0h; Use E0 (undecoded DF)
        je      @F                      ; if first byte was DF, don't assume
                                        ;  first byte matched. 
        inc     XSI                     ; advance 1 byte in host file
@@:
        add     XDI, 2                  ; advance to second byte of sig
        mov     xdx, xdi                ;
        add     XDX, XCX                ; *CAREY -> DX points to end of sig
        dec     XDX

comploop:
        mov     al,byte ptr XES[XDI]    ; get next byte from sig
        dec     al                      ; *CAREY -> decrypt
	
        mov     ah,al                   ; ah for ANDing

        and     ah,0F0h                 ; test for wildcards
        cmp     ah,0D0h
        jne     normbyte                ; not a wildcard
   	
        mov     byCurrentByte, al

        INVOKE  wildcard,Handle,Buffer, ADDR dwStartOffset, byCurrentByte

        jnc     mismatch                ;carry was mismatch
        jmp     short wskip

normbyte:
        cmp     al,byte ptr[XSI]        ;test for match
        jne     mismatch
        inc     XSI

wskip:  
        inc     XDI
        cmp     XDI, XDX
        jb      comploop
pre_allmatch:
        stc
        jmp     short allmatch		;Hot beans! We've got one!

mismatch:
        clc				;No match here.

allmatch:
        ret

compare_strings 	ENDP
;----------------------------------------------------------------------------
; WILDCARD SYSTEM                10/28/92                         JOE
;
; ENTER:	ds:si	points to next byte in string from file
;		es:di	points to wildcard byte in sig
;
; EXIT:		si	reset to new position for next pass
;		stc	assume match(es) and si reset       (=match)
;		clc	byte not found by SlidePointer PROC (=mismatch)
;
;-----------------------------------------------------------------------------
; Algor Sigs are stored in N20 (and in memory) as follows:
;
;		1st Byte, Count, Rest of sig, Null pads
;
;	Where Count is number of bytes in "rest of sig"
;	and "rest of sig" is stored in reverse order.
;
; Meaning of wildcards:
;				      
;	D1 - D9 and DC simply set si in the file string to a new location
;	and return carry as if a match is found.
;
;	D0, DA and DB scan for match through XX bytes. If found pointer is
;	set for next iteration of compare_strings routine and carry is set
;	as if all intervening bytes are matched. 
;
;	NOTE: If byte is not found in range, carry is clear to report a
;	      mismatch.
;
;-----------------------------------------------------------------------------
; Meaning of wildcard bytes
;
;	D0h		Quick slide (Check each of next 16 bytes for match)
;	D1h - D9h	Inc 1 to 9 bytes in file string and return
;	DAh,XXh		Advance through XX bytes looking for match
;	DBh,XXh		Back up to start of file then Advance XXh
;	DCh,XXh		Seek ahead XXh and look there for match
;	DDh - DFh	Reserved for now (Possible nibble wildcarding, etc.)
;
;----------------------------------------------------------------------------
;
; DS:SI points to virus buffer
; ES:DI points to the signature
; BL contains the current sig byte
;

WildCard	PROC USES XBX,
        Handle:WORD,
        Buffer:LPSTR,
        lpStartOffset:LPSTR,
        byByte:BYTE

        LOCAL   dwBase:DWORD


        push    XDX
; Buffer *is* XBX!  Remove saving and restoring of XBX other than the
; USES above.  --  CJK
        push    XBX                     ; save BX for later

        mov     al, byByte
        and     xax, 000fh              ; use low nibble of AL for index
J_JUMP_AGAIN:
        shl     ax, 1                   ; multiply offset by 2 (word)
IF OS_32
        shl     ax, 1                   ; multiply offset by 2 (dword)
        xor     XBX,XBX                 ; clear for 32-bit add
ENDIF
        mov     bx, ax
        add     XBX, offset JUMP_TABLE  ; BX points to our jump table now
        
IF OS_32
        jmp     dword ptr cs:[XBX]       ; jump to the code
ELSE
        jmp     word ptr cs:[XBX]       ; jump to the code
ENDIF

DA_CODE label near
        xor     XCX,XCX                 ; clear upper byte of CX
        inc     XDI                     ; point to argument to DA
        mov     cl, byte ptr XES[XDI]   ; place count into CL
        dec     cl                      ; *CAREY -> decrypt
        jmp     QUICK_SLIDE

DB_CODE  label near
        pop     XBX
        mov     XSI, XBX                ; reset to start of buffer
        pop     XDX
        jmp     EXIT_CATCH_NO_POP       ; already popped everything

DC_CODE label near
        inc     XDI                     ; advance to next byte in sig
        mov     al, XES[XDI]            ; place argument in AL, AH = 0
        dec     al                      ; *CAREY -> decrypt
        add     XSI, XAX                ; add this to SI, advancing the PTR
        jmp     EXIT_CATCH

DD_CODE  label near
        inc     XDI
        mov     al, XES[XDI]            ; place argument in AL, AH = 0
        dec     al                      ; *CAREY -> decrypt
        sub     XSI, XAX                ; sub this from SI, backtracking PTR
        jnc     EXIT_CATCH              ; if we go backwards too far, ...
preEXIT_FAIL:
DF_VOID label near
        jmp     EXIT_FAIL               ;   assuming 0 is beginning

DE_CODE  label near
        inc     XDI                     ; place pointer on nibble byte
        mov     ah, XES[XDI]            ; AH contains nibble byte
        dec     ah                      ; *CAREY -> decrypt
        mov     al, [XSI]               ; AL contains the byte from the host
        and     al, 0f0h                ; isolate high part of nibble
        cmp     al, ah                  ; if 0, then we have a match!
        jne     EXIT_FAIL
        inc     XSI
        jmp     EXIT_CATCH              ; return with carry set and inc SI

DF_CODE  label near
        inc     XDI                     ; place pointer on function byte
        xor     XAX,XAX
        mov     al, XES[XDI]            ; AL contains function byte
        dec     al                      ; *CAREY -> decrypt
        add     ax,10h
        cmp     ax,MAX_DF_FUNCS
        jae     EXIT_FAIL
        jmp     J_JUMP_AGAIN

DF_SCAN_HEADER  label near
        LDS_SI  Buffer                ;re-establish top of buffer
        add     XSI,STARTSIZE
        jmp     EXIT_CATCH

;----------------------------

DF_FOLLOWJUMP LABEL NEAR

      ;int    03h

      mov     word ptr [dwBase],     00h
      mov     word ptr [dwBase + 2], 00h

      push   XSI
ifndef NLM
      push   ds
endif

      ; calculate the BASE

      LDS_SI Buffer
      add    XSI, STARTSIZE

      cmp     [XSI],word ptr 'ZM'     ; Native EXE
      je      exe_type

      cmp     [XSI],word ptr 'MZ'     ; European EXE
      jne     not_exe

exe_type:
      mov     ax,[XSI+16h]            ; CS of header
      add     ax,[XSI+08h]            ; Number of paragraphs in header
      mov     dx,10h
      mul     dx                      ; DX:AX = base

      mov     word ptr [dwBase], ax
      mov     word ptr [dwBase + 2], dx

not_exe:

      LDS_SI  lpStartOffset

      mov     bx, word ptr [XSI]
      mov     cx, word ptr [XSI+2]

ifndef NLM
      pop     ds
endif
      pop     XSI

      mov     al, byte ptr [XSI - 1]
      cmp     al, 0ebh
      je      @F

      and     al, 0f0h
      cmp     al, 070h
      jne     do_2byte_jump

@@:
      mov     al, byte ptr [XSI]
      inc     XSI                       ; one byte jmp
      cbw

      jmp     @F

do_2byte_jump:
      mov     ax, word ptr [XSI]
      add     XSI, 2                    ; two bytes jmp

@@:
     ; cx:bx  current file offset
     ; ax     displacement
     ; dwBase

      mov     XDX, XSI
      LDS_SI  Buffer
      sub     XDX, XSI
      and     XDX, STARTSIZE - 1  ; offset of instruction in the buffer

                                  ; NOTE - the code doesn't use cx - the high
                                  ; word of the file offset - this works
                                  ; because it ends up creating in bx a 16-bit
                                  ; relative offset to the current file offset -
                                  ; this works even if it wraps across a
                                  ; 64-k boundary

      add     bx, dx              ; add offset, dx ,point at op of jmp instruction

      ; sub offset, dwBase    offset with in the segment

      mov     cx, word ptr [dwBase]
      sub     bx, cx
      add     bx, ax                    ; add offset, displacement destination of seek
      add     word ptr [dwBase], bx     ; mov dwTemp, dwBase
      adc     word ptr [dwBase+2], 0    ; add dwTemp, low-word(offset)
                                        ; seek to dwTemp

      INVOKE  FIL4_SEEK,Handle, dwBase, fSEEK_BEG
      INVOKE  get_test, lpStartOffset

      jmp     check_seek_result
;----------------------------

DF_FOLLOWJMP2ABS label near
        mov     ax,[XSI]
        sub     ax,100h
J_READ3RDBUF:
        xor     bx,bx
        INVOKE  FIL4_SEEK,Handle,bx::ax,fSEEK_BEG
        INVOKE  get_test, lpStartOffset

check_seek_result:

ifndef NLM
        and     ax, dx                  ; merge error results in 16-bit land
endif
        cmp     XAX,-1
        je      EXIT_FAIL

        LDS_SI  Buffer                ;re-establish top of buffer
        add     XSI,STARTSIZE*2
        INVOKE  FIL4_READ,Handle,vSDS XSI,STARTSIZE
        jmp     EXIT_CATCH

;----------------------------
;XDS:XSI   scanning buffer
;XES:XDI   virus signature
;signature is of the form: 0xDF, 0x14, bySCAN_LENGTH, SCAN_BYTE,BIT_MASK

DF_SCANMASK LABEL NEAR
    inc     XDI

    xor     XCX,XCX

    mov     cl, XES[XDI]                ; load up our length byte
    dec     cl                          ; decrypt

    inc     XDI                         ; point to next byte in sig

    mov     dx, XES[XDI]                ; retrieve next two bytes of func
    dec     dl                          ; DL = byte to scan for
    dec     dh                          ; DH = important bits (mask)
                                        ; note (DL & DH) == DL should be TRUE!
    inc     XDI                         ; now advanced to last byte sig...

loop_top:

    lodsb
    and     al, dh
    cmp     al, dl
    je      EXIT_CATCH

    loop    loop_top

    ; fail at this point

    jmp     EXIT_FAIL
;----------------------------

DF_ENTRYPOINT_IMAGE label near
        push    XSI                     ; Save pointer

        LDS_SI  Buffer
        add     XSI,STARTSIZE

        cmp     [XSI],word ptr 'ZM'     ; Native EXE
        je      ep_is_exe

        cmp     [XSI],word ptr 'MZ'     ; European EXE
        je      ep_is_exe

        pop     XSI                     ; Restore pointer
        jmp     DF_ENTRYPOINT_FILESIZE  ; Not EXE, so do seek

ep_is_exe:
        xor     XDX,XDX
        mov     dx,word ptr [XSI+4]     ; UB on length in 512-byte pages
        xor     XAX,XAX
        mov     ax,word ptr [XSI+2]     ; Length Mod 512

        pop     XSI                     ; Restore pointer

        and     XAX,XAX                 ; Was it an even multiple of pages?
        jz      @F
        dec     XDX                     ; Not even multiple
@@:

; Now DX contains length / 512 and AX contains length % 512

IF OS_16
        mov     bx,dx                   ; Convert from page notation
        mov     cl,9
        shl     bx,cl                   ;  to 32-bit linear
        or      ax,bx                   ;  filelength, storing result
        mov     cl,7
        shr     dx,cl                   ;  in DX:AX
ELSE
        shl     XDX,9                   ; Convert from page notation
        or      XAX,XDX                 ;  to 32-bit linear length in AX
ENDIF
        jmp     GetEntrypoint


DF_ENTRYPOINT_FILESIZE label near
        INVOKE  FIL4_SEEK,Handle,0,fSEEK_END    ; Get length from seek

GetEntrypoint:
        ; First save filelength into BX:CX or CX for OS_32

IF OS_16
        mov     bx,dx
        mov     cx,ax
ELSE
        mov     XCX,XAX
ENDIF
        push    XSI                     ; Save pointer

        LDS_SI  Buffer
        add     XSI,STARTSIZE

        cmp     [XSI],word ptr 'ZM'     ; Native EXE
        je      calc_exe_ep

        cmp     [XSI],word ptr 'MZ'     ; European EXE
        je      calc_exe_ep

calc_com_ep:
        cmp     byte ptr [XSI],0E9h
        je      jump_near
        cmp     byte ptr [XSI],0E8h
        je      call_near
        cmp     byte ptr [XSI],0EBh
        je      jump_short

IF OS_16
        xor     XDX,XDX                 ; Zero high word of entrypoint
ENDIF
        xor     XAX,XAX                 ; Entrypoint is at TOF
        jmp     CheckEntrypoint

jump_near:
call_near:
        xor     XAX,XAX
        mov     ax,word ptr [XSI+1]
        add     XAX,0103h               ; Add to instruction after jmp
        jmp     @F

jump_short:
        mov     al,byte ptr [XSI+1]
        cbw                             ; Convert to 16-bit signed value
        add     XAX,0102h               ; Add to instruction after jmp

@@:
        and     XAX,0FFFFh              ; Mask to lower 16 bits
        sub     XAX,0100h               ; Realign relative to TOF

IF OS_16
        sbb     dx,0
ENDIF
        jmp     CheckEntrypoint

calc_exe_ep:

        xor     XAX,XAX
        mov     ax,word ptr [XSI+16h]   ; CS of header
        add     ax,[XSI+08h]            ; # paragraphs in header
        mov     XDX,10h
        mul     XDX
IF OS_16
        add     ax,[XSI+14h]            ; DX:AX = Location of code
        adc     dx,0
        and     dx,000Fh
ELSE
        movuw   XDX,word ptr [XSI+14h]
        add     XAX,XDX                 ; AX = Location of code
        and     XAX,0FFFFFh
ENDIF

CheckEntrypoint:
        pop     XSI                     ; Restore pointer

IF OS_16
; Now DX:AX contains 32-bit filelength

        sub     cx,ax
        sbb     bx,dx
        mov     ax,cx                   ; Save size in ax

        and     bx,bx                   ; If the size is negative or
        jnz     EXIT_FAIL               ; >=64K, then fail
ELSE
; Now EAX contains 32-bit filelength
        sub     ecx,eax
        mov     eax,ecx                 ; Save size in ax

        and     ecx,0FFFF0000h          ; If the size is negative or
        jnz     EXIT_FAIL               ; >=64K, then fail
ENDIF

        inc     XDI
        xor     XDX,XDX
        mov     dx,word ptr XES[XDI]    ; DX gets lower bound

        dec     dl                      ; Decrypt
        dec     dh                      ; Decrypt

        cmp     XAX,XDX                 ; Is size less than lower bound?
        jl      EXIT_FAIL               ; If so, then fail.

        inc     XDI
        inc     XDI
        xor     XDX,XDX
        mov     dx,word ptr XES[XDI]    ; DX gets upper bound

        dec     dl                      ; Decrypt
        dec     dh                      ; Decrypt

        cmp     XAX,XDX                 ; Is size greater than upper bound?
        jg      EXIT_FAIL               ; If so, then fail.

        inc     XDI

        jmp     EXIT_CATCH              ; Size satisfies bounds.

;--------------------------------------------------------------------


DF_DIRECTJMP2ABS label near
        inc     XDI
        mov     ax,XES[XDI]
        dec     al
        dec     ah
        inc     XDI
        jmp     J_READ3RDBUF

DF_SEEKBACK label near
        inc     XDI                     ; Get past code
        mov     ax,XES[XDI]             ; get address passed to us
        dec     al                      ; decode
        dec     ah
        inc     XDI                     ; move one.  After we leave, another
                                        ;  one will be accounted for.
        push    ax
        INVOKE  FIL4_SIZE,Handle

        pop     bx
IF OS_32                                ; convert eax return to dx:ax
        mov     XDX,XAX
        shr     XDX,16                  ; MSB to LSB
ENDIF
        or      dx,dx
        jnz     @F
        cmp     ax,bx                   ; ax=16-bit file size, bx= seek back
        jb      EXIT_FAIL
@@:
        mov     ax,bx
        xor     bx,bx
        INVOKE  FIL4_SEEK,Handle,bx::ax,fSEEK_END
        INVOKE  get_test, lpStartOffset

ifndef NLM
        and     ax, dx                  ; merge error results in 16-bit land
endif
        cmp     XAX,-1
        je      EXIT_FAIL               ; start of the file *OR* have an error

        LDS_SI  Buffer                  ; re-establish top of buffer
        add     XSI,STARTSIZE*2         ; use the third buffer
        INVOKE  FIL4_READ,Handle,vSDS XSI,STARTSIZE
        jmp     EXIT_CATCH

DF_FOLLOWJMPHEADERE9 label near
        mov     ax,[XSI]                ; pick up JMP destination, relative
        add     ax,2h                   ; should be +3, but we've already
                                        ;  accounted for the E9
        mov     XBX,XSI                 ; save our location in buffer
        LDS_SI  Buffer                  ; get address of Buffer so we can
        sub     XBX,XSI                 ;  determine where we are.
        sub     XBX,STARTSIZE
        add     ax,bx                   ; We found out where we are, so add
                                        ;  to relative address.
        jmp     J_READ3RDBUF

DF_STRINGSEARCH label   near            ; this is our "string searching" func

; signatures MUST be 2 or more bytes long!

        inc     XDI                     ; point to length information
        mov     bx, XES[XDI]            ; BL = dist to search, BH = string len
        sub     bh,2                    ; The size to compare is ONE less than
                                        ; the actual string size since we
                                        ; ALWAYS compare the first byte.
        dec     bl                      ; decrypt
        add     di, 2                   ; point to first byte of sig

        mov     ah, XES[XDI]            ; AH = FIRST byte to search for
        inc     XDI                     ; point to next byte in signature
        dec     ah                      ; decrypt

ifdef NLM
        xor     XCX, XCX                ; clear CX out
endif
        mov     cl, bl                  ; CX = distance to search for stuff

STRING_SCAN:

        lodsb                           ; look for our first byte in buffer
        cmp     al,ah                   ; is search_byte == buffer_byte?
        jz      COMPARE_REST            ; yes! see if the rest is the same

STRING_LOOP:
        loop    STRING_SCAN             ; no, keep looking
        jmp     EXIT_FAIL

; at this point we have found the first byte and are pointing to the second
; byte that we want to sig

COMPARE_REST:

        push    si                      ; save SI for later
        push    di                      ; save DI for later
        mov     dx, ax
        mov     ah, bh                  ; AH = size of string to match


KEEP_COMPARING:

        mov     al, XES[XDI]            ; retrive next byte of sig
        cmp     al, 0D2h
        je      @F
        dec     al                      ; decrypt
        cmp     al, [XSI]               ; is it in the buffer?
        jnz     STRING_MORE             ; not found, keep searching
@@:
        inc     XSI                     ; found - point to next byte
        inc     XDI
        dec     ah
        jnz     KEEP_COMPARING

        add     sp, 4                   ; remove SI & DI from stack
        dec     XDI                     ; point to LAST byte of string

        jmp     EXIT_CATCH


STRING_MORE:
        mov     ax, dx
        pop     di
        pop     si
        jmp     STRING_LOOP


DF_TREMOR_DETECT    label   near

        INVOKE  SCN4_TREMOR, vSDS XSI, Handle
        or      ax, ax
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_SATAN_BUG_DETECT label   near

        INVOKE  SCN4_SATAN_BUG, vSDS XSI, Handle
        or      ax, ax
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

ifdef 0
DF_PATHOGEN_DETECT label   near

        push    XSI
        LDS_SI  Buffer                  ; re-establish top of buffer
ifdef SDOS16
        add     XSI,TSR_PATHOGEN_BUFFER_LOCATION ; Use Second half of 1K, plus 2
else
        add     XSI,STARTSIZE*4         ; Use 2K->4K
endif
        INVOKE  SCN4_PATHOGEN, vSDS XSI, Handle
        pop     XSI
        or      ax, ax
        jz      EXIT_FAIL
        jmp     EXIT_CATCH
endif

DF_QUEEG_DETECT label   near

        INVOKE  SCN4_QUEEG, Buffer, Handle
        or      ax, ax
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_GIRAFE_DETECT    label   near

        INVOKE  SCN4_GIRAFE, vSDS XSI
        or      ax, ax
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_LOW_NIBBLE label near
        inc     XDI                     ; place pointer on nibble byte
        mov     ah, XES[XDI]            ; AH contains nibble byte
        dec     ah                      ; decrypt
        mov     al, [XSI]               ; AL contains the byte from the host
        and     al, 0fh                 ; isolate low part of nibble
        cmp     al, ah                  ; if 0, then we have a match!
        jne     EXIT_FAIL
        inc     XSI
        jmp     EXIT_CATCH              ; return with carry set and inc SI

DF_DETECT_DX label near
        inc     XDI                     ; place pointer at next byte
        mov     ah, XES[XDI]            ; AH contains nibble byte
        dec     ah                      ; decrypt
        cmp     ah, [XSI]               ; [XSI] contains the byte from the host
        jne     EXIT_FAIL
        inc     XSI
        jmp     EXIT_CATCH

DF_FOLLOWJMP1 label near
        mov     al, [XSI]               ; get distance of Jcond
        cbw
ifdef NLM
        cwde
endif
        add     XSI, XAX                ; add this to SI, get new location
        inc     XSI                     ;  add one to really get new location
        mov     ax,word ptr Buffer      ; make sure we're still in our
        cmp     si,ax                   ;  buffer space (don't GP)
        jb      EXIT_FAIL
        add     ax,3*STARTSIZE          ; we only use 3 buffers
        cmp     si,ax
        ja      EXIT_FAIL
        jmp     EXIT_CATCH


DF_EXEC label near

ifndef NONAVEX

; For TSR, for which NAVEX definitions should never be present,
; the definition just fails.

ifndef SDOS16

        inc     XDI                     ; go to WORD containing ID #
        mov     AX, XES[XDI]            ; fetch it and decrypt
        dec     AH
        dec     AL

        LDS_SI  Buffer                  ; re-establish top of buffer
        add     XSI, 100h*3             ; point to 3rd 256 byte buffer
        inc     word ptr [XSI]
        xor     XBX, XBX
        mov     bx, word ptr [XSI]      ; obtain next free index
        cmp     XBX, 511
        jge     @F

        shl     XBX, 1                  ; multiply by two to get the memory
        mov     [XBX+XSI], AX           ; location and store our ID #.

        inc     XDI                     ; point to HIGH byte of ID

endif   ; SDOS16

@@:
        jmp     EXIT_FAIL               ; fail the definition and go to the
                                        ; next one!
else    ; NONAVEX
; 8/15/96 jallee We no longer want virspxx.dat functionality (scn4_exec)
;                under the NONAVEX define.  Used only by NAVUPDATE.

        jmp     EXIT_FAIL

; 8/15/96 jallee Commented out below virspxx.dat functionality.
;                This affects Navupdat only. (certlib5)
;                Source not removed due to perceived PM instability.
;ifdef DOS16
;
;; If using VIRSC0x.DAT, this NAV version will skip these, the older ones
;; will execute them.
;        cmp     XES[XDI+1], byte ptr '1'
;        je      EXIT_FAIL
;;infected file, start of our buffer, where we are, where we are in the sig
;        INVOKE  SCN4_EXEC, Handle, Buffer, vSDS XSI, vSES XDI
;        or      ax, ax
;        jz      EXIT_FAIL
;        add     XDI,2
;        jmp     EXIT_CATCH
;else
;        jmp     EXIT_FAIL
;
;endif   ; DOS16
; 8/15/96 jallee

endif   ; NONAVEX


DF_USE_NEXT label near
        pop     XBX
        inc     XDI
        mov     al, byte ptr XES[XDI]   ; point to bytes to skip
        cbw
ifdef NLM
        cwde
endif
        add     XDI, XAX                ; Get us to length byte of next sig
        mov     al, byte ptr XES[XDI]   ; Get length of next sig
        ; no decrypt, the +1 encryption is what we want.
        cbw
ifdef NLM
        cwde
endif
        pop     XDX                     ; get it off stack
        mov     XDX,XDI
        add     XDX,XAX                 ; point to new end of sig
        add     XDI,3                   ; point to after fake VOID sig
        jmp     EXIT_CATCH_NO_POP       ; already popped everything

DF_ENCRYPTED_SEARCH label near
        inc     XDI                     ; Point to bytes to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        mov     DL,XES[XDI]
        dec     DL
        test    DL, 040h
        jz      @F
        INVOKE  DECRYPT_XOR_DELTA,vSDS XSI,vSES XDI
        jc      ENCRYPTED_FOUND
@@:
        test    DL, 020h
        jz      @F
        INVOKE  DECRYPT_ADD_DELTA,vSDS XSI,vSES XDI
        jc      ENCRYPTED_FOUND
@@:
        test    DL, 010h
        jz      @F
        INVOKE  DECRYPT_ROTATE,vSDS XSI,vSES XDI
        jc      ENCRYPTED_FOUND
@@:
        test    DL, 008h
        jz      @F
        INVOKE  DECRYPT_NEG,vSDS XSI,vSES XDI
        jc      ENCRYPTED_FOUND
@@:
        test    DL, 004h
        jz      ENCRYPTED_FOUND
        INVOKE  DECRYPT_NOT,vSDS XSI,vSES XDI
        jc      ENCRYPTED_FOUND

ENCRYPTED_FOUND:
        pushf
        test    DL, 080h
        jz      @F
        add     XDI,4                  ; Get past all the bytes 
@@:
        add     XDI,5                  ; Get past all the bytes 
        popf
        jnc     EXIT_FAIL
        jmp     EXIT_CATCH


D0_CODE label near
        mov     xcx, 16                 ; # of bytes for quick slide
                                        ; will be dec'd as soon as we get
                                        ; to QUICK_SLIDE

;        jmp     QUICK_SLIDE             ; perform the quick slide

QUICK_SLIDE:
        inc     XDI                     ; point DI to byte we're looking for
        cld                             ; clear direction flag
        mov     ah, XES[XDI]            ; ah contains the byte we're matching
        dec     ah                      ; *CAREY -> decrypt

LOOK_LOOP:

        lodsb                           ; AL contains next byte in host
        cmp     al, ah                  ; are the two the same???
        je      EXIT_CATCH
        loop    LOOK_LOOP

EXIT_FAIL:
        pop     XBX
        pop     XDX
        clc
        jmp     EXIT_ALL

D1_9_CODE label near

        shr     ax, 1                   ; restore AX to previous value
IF OS_32
        shr     ax, 1                   ; once more for 32 bit pointers
ENDIF

        add     XSI, XAX                ; advance pointer by AL bytes
;        jmp     EXIT_CATCH              ; got it

EXIT_CATCH:
        pop     XBX
        pop     XDX

EXIT_CATCH_NO_POP:
        stc
EXIT_ALL:
        ret

WildCard	ENDP

IF OS_16

JUMP_TABLE dw offset D0_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset D1_9_CODE
           dw offset DA_CODE
           dw offset DB_CODE
           dw offset DC_CODE
           dw offset DD_CODE
           dw offset DE_CODE
           dw offset DF_CODE
; Start DF functions
           dw offset DF_SCAN_HEADER
           dw offset DF_FOLLOWJMP2ABS
           dw offset DF_DIRECTJMP2ABS
           dw offset DF_SEEKBACK 
           dw offset DF_FOLLOWJMPHEADERE9
           dw offset DF_STRINGSEARCH
           dw offset DF_TREMOR_DETECT
           dw offset DF_SATAN_BUG_DETECT
           dw offset DF_GIRAFE_DETECT
           dw offset DF_LOW_NIBBLE
           dw offset DF_DETECT_DX
           dw offset DF_FOLLOWJMP1
           dw offset DF_EXEC
           dw offset DF_VOID
           dw offset DF_USE_NEXT
           dw offset DF_VOID
           dw offset DF_ENCRYPTED_SEARCH
;          dw offset DF_PATHOGEN_DETECT
           dw offset DF_VOID
           dw offset DF_VOID
           dw offset DF_FOLLOWJUMP
           dw offset DF_SCANMASK
           dw offset DF_ENTRYPOINT_IMAGE
           dw offset DF_ENTRYPOINT_FILESIZE
; End DF functions
MAX_DF_FUNCS      EQU     ($-JUMP_TABLE)/2

ELSE

JUMP_TABLE dd offset D0_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset D1_9_CODE
           dd offset DA_CODE
           dd offset DB_CODE
           dd offset DC_CODE
           dd offset DD_CODE
           dd offset DE_CODE
           dd offset DF_CODE
; Start DF functions
           dd offset DF_SCAN_HEADER
           dd offset DF_FOLLOWJMP2ABS
           dd offset DF_DIRECTJMP2ABS
           dd offset DF_SEEKBACK 
           dd offset DF_FOLLOWJMPHEADERE9
           dd offset DF_STRINGSEARCH
           dd offset DF_TREMOR_DETECT
           dd offset DF_SATAN_BUG_DETECT
           dd offset DF_GIRAFE_DETECT
           dd offset DF_LOW_NIBBLE
           dd offset DF_DETECT_DX
           dd offset DF_FOLLOWJMP1
           dd offset DF_EXEC
           dd offset DF_VOID
           dd offset DF_USE_NEXT
           dd offset DF_VOID
           dd offset DF_ENCRYPTED_SEARCH
;          dd offset DF_PATHOGEN_DETECT
           dd offset DF_VOID
           dd offset DF_VOID
           dd offset DF_FOLLOWJUMP
           dd offset DF_SCANMASK
           dd offset DF_ENTRYPOINT_IMAGE
           dd offset DF_ENTRYPOINT_FILESIZE
; End DF functions
MAX_DF_FUNCS      EQU     ($-JUMP_TABLE)/4

ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TREMOR detection!
; this function returns 0 if not found and 1 if found!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SCN4_TREMOR     PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        Handle:WORD

    LOCAL   Verify:DWORD


    LDS_SI  lpBuffer
    add     si, 2ah                     ; bytes start at FirstLanding + 2ch
                                        ; start looking a bit earlier in case
                                        ; we skipped past a few nops, etc @
                                        ; the first landing


    INVOKE  SCN4_TREMOR_XOR, vSDS XSI
    or      ax, ax
    jnz     MAYBE_FOUND_TREMOR

    INVOKE  SCN4_TREMOR_ADD, vSDS XSI
    or      ax, ax
    jz      NO_TREMOR

MAYBE_FOUND_TREMOR:

; !! The verification following this comment has been intentionally removed.
; IF we have any false ID situations, this can be added back again.  Currently
; it causes us to miss several germs.

;    INVOKE  FIL4_SEEK, Handle, 4, fSEEK_END
;    and     ax, dx
;    cmp     ax, 0ffffh
;    jz      NO_TREMOR

;    INVOKE  FIL4_READ, Handle, ADDR Verify, 4

    ; in order to verify tremor we:
    ; read the last two words.  ADD 0F172 to the last word and XOR with the
    ; second to last word.  if the result is DEAD, we have it!

;    mov     bx, word ptr Verify+2
;    add     bx, 0f172h
;    mov     ax, word ptr Verify
;    xor     ax, bx
;    cmp     ax, 0DEADh
;    jnz     NO_TREMOR

SET_FOUND_TREMOR:

    mov     ax, 1
    jmp     FOUND_TREMOR


NO_TREMOR:

    xor     ax, ax

FOUND_TREMOR:

    ret

SCN4_TREMOR     ENDP




SCN4_TREMOR_XOR  PROC USES SDS XSI,
        lpBuffer:LPSTR

    LDS_SI  lpBuffer
    mov     XCX, 03h

    ; signature = 080h, 03eh, 0d3h, 0c0h, 001h, 074h, 0f5h, 02eh, 089h, 036h
    ; this signature always starts on an XOR/ADD boundary

XOR_LOOP:

    mov     ax, [XSI]                   ; AX = next word to scan
    xor     ax, 03e80h                  ; AX = base
    mov     bx, [XSI+2]                 ; BX = following word
    xor     bx, 00cd3h                  ; BX = base + delta
    sub     bx, ax                      ; BX = delta
    mov     di, ax
    add     di, bx
    add     di, bx                      ; CX = XOR value for next word

    mov     DX, [XSI+4]                 ; DX = DS:[XSI+4]
    xor     DX, DI                      ; decrypt!
    cmp     DX, 07401h                  ; compare with next part of sig
    jnz     TRY_XOR_XOR

    add     DI, BX                      ; add in delta
    mov     DX, [XSI+6]                 ; DX = DS:[XSI+6]
    xor     DX, DI                      ; decrypt!
    cmp     DX, 02ef5h                  ; compare with next part of sig
    jnz     TRY_XOR_XOR

    add     DI, BX                      ; add in delta
    mov     DX, [XSI+8]                 ; DX = DS:[XSI+8]
    xor     DX, DI                      ; decrypt!
    cmp     DX, 03689h                  ; compare with next part of sig
    jnz     TRY_XOR_XOR

    jmp     XOR_FOUND_TREM              ; tremor found!


TRY_XOR_XOR:

    ; here we check to see if they did the following type of decryption:
    ;
    ; XOR [SI], BX
    ; XOR BX, 1234H
    ;
    ; This will give an oscillating key.

    mov     DX, [XSI+4]                 ; DX = DS:[XSI+4]
    xor     DX, AX                      ; decrypt
    cmp     DX, 07401h                  ; compare with next part of sig
    jnz     XOR_END_OF_LOOP

    mov     di, ax
    add     di, bx

    mov     DX, [XSI+6]                 ; DX = DS:[XSI+6]
    xor     DX, DI                      ; decrypt
    cmp     DX, 02ef5h                  ; compare with next part of sig
    jnz     XOR_END_OF_LOOP

    mov     DX, [XSI+8]                 ; DX = DS:[XSI+8]
    xor     DX, AX                      ; decrypt
    cmp     DX, 03689h                  ; compare with next part of sig
    jnz     XOR_END_OF_LOOP

    jmp     XOR_FOUND_TREM

XOR_END_OF_LOOP:

    inc     XSI                         ; point to next byte!

IF OS_16
    loop    XOR_LOOP
ELSE
    dec     XCX                         ; loop is too far in 32-bit mode
    jnz     XOR_LOOP    
ENDIF

    xor     ax, ax                      ; not found!
    jmp     EXIT_XOR_TREM


XOR_FOUND_TREM:
    mov     ax, 1

EXIT_XOR_TREM:

    ret

SCN4_TREMOR_XOR     ENDP





SCN4_TREMOR_ADD  PROC,
        lpBuffer:LPSTR

    LDS_SI  lpBuffer
    mov     XCX, 03h

    ; signature = 080h, 03eh, 0d3h, 0c0h, 001h, 074h, 0f5h, 02eh, 089h, 036h
    ; this signature always starts on an XOR/ADD boundary

ADD_LOOP:

    mov     ax, [XSI]                   ; AX = next word to scan
    sub     ax, 03e80h                  ; AX = base key value if on target
    mov     bx, [XSI+2]                 ; BX = following word
    sub     bx, 00cd3h                  ; BX = base + delta
    sub     bx, ax                      ; BX = delta
    mov     di, ax
    add     di, bx
    add     di, bx                      ; CX = ADD value for next word

    mov     DX, [XSI+4]                 ; DX = DS:[XSI]
    sub     DX, DI                      ; decrypt!
    cmp     DX, 07401h                  ; compare with next part of sig
    jnz     TRY_ADD_XOR

    add     DI, BX                      ; add in delta
    mov     DX, [XSI+6]                 ; DX = DS:[XSI]
    sub     DX, DI                      ; decrypt!
    cmp     DX, 02ef5h                  ; compare with next part of sig
    jnz     TRY_ADD_XOR

    add     DI, BX                      ; add in delta
    mov     DX, [XSI+8]                 ; DX = DS:[XSI]
    sub     DX, DI                      ; decrypt!
    cmp     DX, 03689h                  ; compare with next part of sig
    jnz     TRY_ADD_XOR

    jmp     ADD_FOUND_TREM              ; tremor found!

TRY_ADD_XOR:

    ; here we check to see if they did the following type of decryption:
    ;
    ; ADD [SI], BX
    ; XOR BX, 1234H
    ;
    ; This will give an oscillating key.

    mov     DX, [XSI+4]                 ; DX = DS:[XSI+4]
    sub     DX, AX                      ; decrypt
    cmp     DX, 07401h                  ; compare with next part of sig
    jnz     ADD_END_OF_LOOP

    mov     di, ax
    add     di, bx

    mov     DX, [XSI+6]                 ; DX = DS:[XSI+6]
    sub     DX, DI                      ; decrypt
    cmp     DX, 02ef5h                  ; compare with next part of sig
    jnz     ADD_END_OF_LOOP

    mov     DX, [XSI+8]                 ; DX = DS:[XSI+8]
    sub     DX, AX                      ; decrypt
    cmp     DX, 03689h                  ; compare with next part of sig
    jnz     ADD_END_OF_LOOP

    jmp     ADD_FOUND_TREM

ADD_END_OF_LOOP:

    inc     XSI                         ; point to next byte!

IF OS_16
    loop    ADD_LOOP
ELSE
    dec     XCX                         ; loop is too far in 32-bit mode
    jnz     ADD_LOOP    
ENDIF

    xor     ax, ax                      ; not found!
    jmp     EXIT_ADD_TREM


ADD_FOUND_TREM:
    mov     ax, 1

EXIT_ADD_TREM:

    ret

SCN4_TREMOR_ADD     ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Satan bug detection
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SATAN_BYTES     label byte
                db  026h, 02eh, 03eh, 042h, 043h, 045h, 047h, 04ah, 04bh
                db  04dh, 090h, 0f5h, 0f8h, 0f9h, 0fbh, 0fch
NUM_SATAN_BYTES EQU     ($-SATAN_BYTES)


; 0 acts as a terminator for prefix bytes

PREFIX_BYTES    label byte
                db  026h, 02eh, 03eh, 042h, 043h, 045h, 047h, 04ah, 04bh
                db  04dh, 090h, 0f5h, 0f8h, 0f9h, 0fbh, 0fch, 0

SCN4_SATAN_BUG      PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        HostHandle:WORD

    LOCAL   wBadCount : WORD
    LOCAL   wFirstTwo : WORD

    mov     wBadCount, 0            ; initialize count of bad prefixes

    lea     XSI, PREFIX_BYTES

IF OS_16
    mov     ax, SCS                 ; never fool with segment regs (selectors)
    mov     SDS, ax                 ; in 32-bit mode
ENDIF

PREFIX_LOOP:

    lodsb                           ; AL = next prefix to scan for
    or      al, al                  ; no more prefixes?
    jz      TEST_SATAN_COUNT        ; see if we have enough invalid prefixes
    LES_DI  lpBuffer                ; point to bytes we're scanning
    mov     XCX, 255                ; scan up to 255 bytes (& clear top of ECX)

KEEP_SCANNING:

    repnz   scasb                   ; go a-lookin'
    jcxz    PREFIX_LOOP             ; all done scanning for current prefix

    ; at this point we have found a prefix! Yay!

    mov     bx, ax                  ; store AL away for later
    mov     dx, cx                  ; store CX away for later
    mov     al, XES[XDI]            ; see if byte following prefix is invalid

IF OS_16
    push    SES                     ; save for later
ENDIF

    push    XDI

IF OS_16
    mov     cx, SCS                 ; never fool with segment regs (selectors)
    mov     SES, cx                 ; in 32-bit mode
ENDIF

    lea     XDI, SATAN_BYTES        ; point to our bytes
    mov     cx, NUM_SATAN_BYTES
    repnz   scasb                   ; found it?
    jcxz    VALID_PREFIX

    inc     wBadCount               ; got a bad one!

    add     sp, 4                   ; remove ES and DI from stack

    jmp     PREFIX_LOOP             ; found one, try another prefix

VALID_PREFIX:

    pop     XDI

IF OS_16
    pop     SES
ENDIF

    mov     ax, bx                  ; restore contents of AL
    mov     cx, dx                  ; restore count in buffer

    jmp     KEEP_SCANNING

TEST_SATAN_COUNT:

    cmp     wBadCount, 14           ; one of each type of prefix violation
    jl      NO_SATAN_BUG

    LDS_SI  lpBuffer                ; one more verification
    mov     bx, 256                 ; scan first 256 bytes for invalid codes
    xor     dx, dx                  ; init count of satan-bug-byte count

IF OS_16
    SMOV    SES, SCS                ; ES = CS
ENDIF

COUNT_SATAN_BYTE_LOOP:

    lodsb                           ; obtain byte from buffer
    lea     di, SATAN_BYTES         ; point ES:DI to our satan bytes
    mov     cx, NUM_SATAN_BYTES     
    repnz   scasb                   ; scan for this byte in our list
    jcxz    END_SATAN_BYTE_LOOP

    inc     dx                      ; one more satan-bug byte

END_SATAN_BYTE_LOOP:

    dec     bx
    jnz     COUNT_SATAN_BYTE_LOOP   ; keep checking...

    cmp     dx, 50                  ; > 50/256 bytes -> satan-bug
    jl      NO_SATAN_BUG

    INVOKE  FIL4_SEEK, HostHandle, 0, fSEEK_BEG
    INVOKE  FIL4_READ, HostHandle, ADDR wFirstTwo, 2

    cmp     wFirstTwo, 04d5ah
    jz      SATAN_EXEC
    cmp     wFirstTwo, 05a4dh
    jz      SATAN_EXEC
    cmp     byte ptr wFirstTwo, 0e9h
    jnz     NO_SATAN_BUG

SATAN_EXEC:
    mov     ax, 1
    jmp     FOUND_SATAN_BUG

NO_SATAN_BUG:

    xor     ax, ax

FOUND_SATAN_BUG:

    ret

SCN4_SATAN_BUG      ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Girafe (Encrypted using TPE 1.X)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; these signatures are encrypted by +1 encryption!  (both are taken from EVEN
; boundaries of the virus decryption)

GIRAFE_BYTES    label   byte
    db      0e9h, 001h, 001h, 05fh, 084h, 0efh, 004h, 0c0h, 001h, 002h

SCN4_GIRAFE     PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR

    LES_DI  lpBuffer                    ; point to first byte in virus buffer

; The test for the POP AX is now being done in the sig portion... :'(
;    cmp     XES[XDI], byte ptr 50h      ; This must be a PUSH AX for girafe
;    jnz     NO_GIRAFE


    mov     XCX, 200                    ; scan up to 200 bytes for POP AX
    cld                                 ; reset direction flag
    add     XDI,3                       ; so that XDI-3 (later) won't GP.

GIRAFE_POP_SCAN:

    mov     al, 58h                     ; now we want to scan for a POP AX
    repnz   scasb                       ; go looking
    or      cx, cx                      ; have we hit the end of the buffer?
    jnz     GIRAFE_FOUND_POP            ; found a pop - verify!
    jmp     NO_GIRAFE

GIRAFE_FOUND_POP:

    ; when we find a POP AX (if it is the correct one) it will look like:
    ; E2/E0/75/7F XX        LOOP/JMP BACK
    ; 58                    POP AX

    mov     ah, XES[XDI-3]              ; find the loop byte

    cmp     ah, 0e2h                    ; found a loop byte, continue verifying
    jz      VERIFY_GIRAFE
    cmp     ah, 0e0h                    ; found the loop byte
    jz      VERIFY_GIRAFE
    cmp     ah, 075h                    ; ditto
    jz      VERIFY_GIRAFE
    cmp     ah, 07fh                    ; ditto
    jnz     GIRAFE_POP_SCAN             ; keep looking for more POP AX's

    ; fall thru on 7f & others to VERIFY_GIRAFE...

VERIFY_GIRAFE:

    test    XES[XDI-2], byte ptr 80h    ; is this a JNZ/LOOP *BACK*???
    jz      GIRAFE_POP_SCAN             ; NO!  Keep looking!

IF OS_16
    SMOV    SDS, SCS                    ; DS = CS
ENDIF

    lea     XSI, GIRAFE_BYTES           ; point to our 1st signature!

    INVOKE  DETERMINE_XOR_BYTE,vSES XDI,vSDS XSI
    jc      @F
    jmp     GIRAFE_FOUND
@@:
    INVOKE  DETERMINE_ADD_BYTE,vSES XDI,vSDS XSI
    jc      @F
    jmp     GIRAFE_FOUND

@@:
    INVOKE  DETERMINE_XOR_WORD,vSES XDI,vSDS XSI
    jc      @F
    jmp     GIRAFE_FOUND
@@:
    INVOKE  DETERMINE_ADD_WORD,vSES XDI,vSDS XSI
    jc      @F
    jmp     GIRAFE_FOUND

@@:
    jmp     GIRAFE_POP_SCAN


NO_GIRAFE:

    xor     XAX, XAX
    jmp     GIRAFE_RETURN

GIRAFE_FOUND:

    mov     XAX, 1

GIRAFE_RETURN:

    ret

SCN4_GIRAFE     ENDP

ifdef 0
PATHOGEN_REG_PATTERN label byte
    db 053h, 04Dh, 045h, 047h, 020h, 076h, 030h, 02Eh, 031h, 0E8h, 09Ch

PATHOGEN_XOR_PATTERN label byte
    db 01Eh, 008h, 002h, 067h, 056h, 046h, 01Eh, 01Fh, 0D9h, 074h, 09Fh

PATHOGEN_ADD_PATTERN label byte
    db 006h, 008h, 0FEh, 027h, 0AAh, 046h, 002h, 0FDh, 049h, 04Ch, 099h

PATHOGEN_XORALT_PATTERN label byte
    db 05Ch, 0E7h, 086h, 0C4h, 01Eh, 0BCh, 03Fh, 02Eh, 044h, 006h, 074h

SCN4_PATHOGEN      PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        HostHandle:WORD

SMEG_READSIZE equ 512

    INVOKE  FIL4_SEEK, HostHandle, 680h, fSEEK_END
    INVOKE  FIL4_READ, HostHandle, lpBuffer, SMEG_READSIZE

    mov XCX,SMEG_READSIZE/2-1
    LDS_SI  lpBuffer                ;point to string

DECRYPT_PATHOGEN:
    lodsb
    mov AH,AL
ifdef SDOS16
    xor al,[XSI]
    mov [XSI-1],al

    xor ah,[XSI+0FFh]
    mov [XSI+SMEG_READSIZE/2-1],ah
else
    mov BX,AX

    xor al,[XSI]
    mov [XSI-1],al

    sub bl,[XSI]
    mov [XSI+SMEG_READSIZE-1],bl

    xor ah,[XSI+0FFh]
    mov [XSI+SMEG_READSIZE*2-1],ah

    mov al,[XSI]
    neg al
    neg bh
    xor al,bh
    mov [XSI+SMEG_READSIZE*3-1],al
endif

    loop DECRYPT_PATHOGEN

IF OS_16
    SMOV    SDS, SCS                    ; DS = CS
ENDIF

; SMEG_SEARCH returns 0 if not found, 1 if found
;                     carry           no carry

    lea     XSI, PATHOGEN_XOR_PATTERN
    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
    jc      PATH_RET

    lea     XSI, PATHOGEN_REG_PATTERN
    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
    jc      PATH_RET

ifdef SDOS16
    LES_DI  lpBuffer                  
    add     XDI,SMEG_READSIZE/2
    lea     XSI, PATHOGEN_XORALT_PATTERN
    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
    jc      PATH_RET

    INVOKE  FIL4_SEEK, HostHandle, 680h, fSEEK_END
    INVOKE  FIL4_READ, HostHandle, lpBuffer, SMEG_READSIZE/2

    mov XCX,SMEG_READSIZE/2-1
    LDS_SI  lpBuffer                ;point to string

DECRYPT_PATHOGEN_AGAIN:
    lodsb
    mov AH,AL
    sub al,[XSI]
    mov [XSI-1],al

    mov al,[XSI]
    neg al
    neg ah
    xor al,ah
    mov [XSI+SMEG_READSIZE/2-1],al

    loop DECRYPT_PATHOGEN_AGAIN

    push cs
    pop  ds

    lea     XSI, PATHOGEN_ADD_PATTERN
    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
    jc      PATH_RET

    LES_DI  lpBuffer                  
    add     XDI,SMEG_READSIZE/2
    lea     XSI, PATHOGEN_XOR_PATTERN
    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI

else
    LES_DI  lpBuffer                  
    add     XDI,SMEG_READSIZE*3
;    lea     XSI, PATHOGEN_XOR_PATTERN   same as above
    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
    jc      PATH_RET

    LES_DI  lpBuffer                  
    add     XDI,SMEG_READSIZE
    lea     XSI, PATHOGEN_ADD_PATTERN
    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
    jc      PATH_RET

;   LES_DI  lpBuffer                  
;   add     XDI,SMEG_READSIZE*2
;      becomes
    add     XDI,SMEG_READSIZE

    lea     XSI, PATHOGEN_XORALT_PATTERN
    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
endif

PATH_RET:
    ret

SCN4_PATHOGEN      ENDP

SMEG_SEARCH      PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        Pattern:LPSTR

    LES_DI  lpBuffer                  
    LDS_SI  Pattern                  
     lodsb		; AL holds first byte of pattern.
     mov XCX,SMEG_READSIZE/2-1
Smeg_continue_search:
     repnz scasb
     jcxz Smeg_search_nf
     push XDI
     push XCX
     mov XCX,10
     push XSI
     repz cmpsb
     pop XSI
     pop XCX
     pop XDI
     jnz Smeg_continue_search
     stc		;Found!
     mov     XAX,1
     jmp Smeg_return
Smeg_search_nf:
     XOR     XAX, XAX     ; AX = 0 is NOT_FOUND
;     clc                 ; XOR clears carry
Smeg_return:
     ret
SMEG_SEARCH      ENDP
endif

ifdef 0
QUEEG_REG_PATTERN label byte
; SMEG v0.2SV
    db 053h, 04Dh, 045h, 047h, 020h, 076h, 030h, 02Eh, 032h, 053h, 056h

QUEEG_ADD_PATTERN label byte
    db 04Dh, 065h, 00Dh, 044h, 035h, 090h, 0A5h, 067h, 005h, 04Ch, 045h

QUEEG_XORALT_PATTERN label byte
    db 057h, 0ABh, 073h, 044h, 0F5h, 090h, 0A5h, 0EBh, 01Dh, 054h, 045h

;xor     55 A5 7D 44 CB 90 BB E9 1F 54 47
;negxor  57 AB 73 44 F5 90 A5 EB 1D 54 45
;sub     4D 65 0D 44 35 90 A5 67 05 4C 45
;?decxor  1F 09 03 66 69 45 01 1F 62 04 EE
endif

SCN4_QUEEG      PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        HostHandle:WORD

        LOCAL FirstLanding:DWORD
        LOCAL StartStruct:STARTCODE
ifdef SDOS16
        LOCAL PassCount:WORD
        LOCAL CountDs:WORD
endif

; Scan buffer for 0Dxh bytes.  Only continue if more than 15 in buffer.

        LDS_SI  lpBuffer                  
        mov     XCX, STARTSIZE
        xor     XBX,XBX
ifdef SDOS16
        mov     PassCount, BX
;       mov     CountDs, BX
endif
QUEEG_Look_for_Ds:
        lodsb
        and     al, 0F0h
        cmp     al, 0D0h
        jne     QUEEG_Continue_count_Ds
        inc     XBX
        cmp     XBX,15
        jg      QUEEG_Got_enough_Ds

QUEEG_Continue_count_Ds:
        loop    QUEEG_Look_for_Ds
        jmp     QUEEG_NOT_FOUND

; In the TSR, where size is more important, just call FIL4_GET_START.
; Time cost in the TSR is not as noticible.
QUEEG_Got_Enough_Ds:
ifndef SDOS16
        LDS_SI  lpBuffer                  
        add     XSI,STARTSIZE
        cmp     [XSI], byte ptr 0E9h
        jne     QUEEG_Find_FirstLanding
        mov     ax, [XSI+1]
        add     ax, 3
        mov     word ptr FirstLanding, ax
        mov     word ptr FirstLanding+2, 0
        jmp     QUEEG_Read_Buffer
endif

QUEEG_Find_FirstLanding:
        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_GET_START, HostHandle, lpBuffer, ADDR StartStruct

        mov     ax, word ptr StartStruct.Firstlanding
        mov     word ptr FirstLanding, ax
        mov     ax, word ptr StartStruct.Firstlanding+2
        mov     word ptr FirstLanding+2, ax

        test    StartStruct.Filetype,IS_BCOM OR IS_VCOM OR IS_EXE
        jz      QUEEG_NOT_FOUND

QUEEG_Read_Buffer:
        INVOKE  FIL4_SEEK,HostHandle,FirstLanding,fSEEK_BEG
        LES_DI  lpBuffer                  ; This line was moved out of the 
                                          ; ifdef statement below. --Joe

ifdef SDOS16
        inc     PassCount
        INVOKE  FIL4_READ,HostHandle,lpBuffer,1024
        mov     XCX,1024-64               ; Less 64 because of subsequent SCASB
else
;===========================================================================
; The following line was moved above the ifdef so the pointer would be
; initialized in the TSR as it is in the scanner. -- Joe
;        LES_DI  lpBuffer                  

        add     XDI,STARTSIZE*2
        INVOKE  FIL4_READ,HostHandle,vSES XDI,2048-64 ; Less 64 to match TSR
        mov     XCX,2048-64-64            ; Less 64 because of subsequent SCASB
endif

        MOV     AX, 0E975h                ; Looking for the JNZ
                                          ; E9 for subsequent use
QUEEG_Look_for_JNZ:
        repnz   scasb
        jcxz    QUEEG_Maybe_NOT_FOUND

; Found JNZ, Make sure forward jump only a short distance

;       test    XES[XDI], byte ptr 0C0h   ; We want the high two bits to be off
        test    [XDI], byte ptr 0C0h   ; We want the high two bits to be off
        jnz     QUEEG_Look_for_JNZ

; Found valid JNZ, look for preceeding bytes, either: AND rX,same, DEC rX,
; or SUB rX,0001

;       mov     BX, XES[XDI-3]
        mov     BX, [XDI-3]
        mov     DX, BX
        and     DH, 0FCh                ; Look for 48, 49, 4A, or 4B
        cmp     DH, 048h
        je      QUEEG_Look_for_E9
; No DEC, look for SUB rX,0001
        cmp     BX, 1
        je      QUEEG_Look_for_E9
; look for AND
        cmp     BL, 023h                ; AND codes: 23 followed by C0,C9,D2,DB
        jne     QUEEG_Look_for_JNZ
        cmp     BH, 0C0h
        je      QUEEG_Look_for_E9
        cmp     BH, 0C9h
        je      QUEEG_Look_for_E9
        cmp     BH, 0D2h
        je      QUEEG_Look_for_E9
        cmp     BH, 0DBh
        jne     QUEEG_Look_for_JNZ
QUEEG_Look_for_E9:
        push    XDI
        push    XCX
        xor     XCX,XCX
        mov     CL,[XDI]
        add     CX,2
        xchg    AH,AL                   ; Make E9 the search target
QUEEG_Loop_for_E9:
        repnz   scasb
        jcxz    QUEEG_E9_NOT_FOUND
;       cmp     XES[XDI+1], byte ptr 0
        cmp     [XDI+1], byte ptr 0
        jne     QUEEG_Loop_for_E9
;       test    XES[XDI], byte ptr 080h ; We want the high bit to be off
;       test    [XDI], byte ptr 080h ; We want the high bit to be off
        jz      QUEEG_Maybe_got_one
        jmp     QUEEG_Loop_for_E9
; Have to put it here to get it within range of the jcxz
QUEEG_Maybe_NOT_FOUND:
ifdef SDOS16
        mov     AX, PassCount
        cmp     AX, 2
        je      QUEEG_NOT_FOUND
        cmp     AX, 1
        je      QUEEG_TSR_Not_in_FirstK
endif
QUEEG_NOT_FOUND:
        XOR     XAX, XAX     ; AX = 0 is NOT_FOUND
        jmp     QUEEG_RET
QUEEG_E9_NOT_FOUND:
        pop     XCX
        pop     XDI
        xchg    AH,AL                   ; Make 75 (JNZ) the search target
        jmp     QUEEG_Look_for_JNZ
QUEEG_Maybe_got_one:

        pop     XCX
        pop     XDI
ifdef SDOS16
QUEEG_TSR_Not_in_FirstK:
endif
        LDS_SI  lpBuffer                  
ifdef SDOS16
        sub     XCX, 1024-64
else
        add     XSI,STARTSIZE*2
        sub     XCX, 2048-64-64
endif
        neg     XCX
        mov     XDX,XCX
ifdef SDOS16
        cmp     PassCount,1
        je      @F
        shr     DX, 4
        sub     DX, CountDs
        add     DX, 64
else
        shr     XDX,4
endif
@@:
        xor     XBX,XBX
QUEEG_Look_for_Ds_again:
        lodsb
        and     al, 0F0h
        cmp     al, 0D0h
        jne     QUEEG_Continue_count_Ds_again
        inc     XBX
        cmp     XBX,XDX
        jg      QUEEG_Got_enough_Ds_again

QUEEG_Continue_count_Ds_again:
        loop    QUEEG_Look_for_Ds_again
ifdef SDOS16
        cmp     PassCount,1
        jne     QUEEG_NOT_FOUND
        cmp     BX, 64
        jl      QUEEG_NOT_FOUND
        mov     CountDs, BX
        mov     AX, 1024-64
        add     word ptr FirstLanding, AX
        adc     word ptr FirstLanding+2, CX ; CX has 0
        jmp     QUEEG_Read_Buffer
endif
        jmp     QUEEG_NOT_FOUND
QUEEG_Got_enough_Ds_again:
        mov     XAX,1
ifdef 0
jmp QUEEG_RET

; Calculate location we want to get buffer...
; 2048 is the maximum, minus bytes left in search for E9, minus bytes left
; in search for JNZ, plus E9 offset, + 3 (E9 offset adjustment).
; Add it to FirstLanding.
        mov     XAX, 2048+3
        sub     XAX, XCX
        pop     XCX
        sub     XAX, XCX
;       add     AX, XES[XDI]
        add     AX, [XDI]
        pop     XDI
        add     word ptr FirstLanding,AX
        adc     word ptr FirstLanding+2,0
        INVOKE  FIL4_SEEK,HostHandle,FirstLanding,fSEEK_BEG
        LES_DI  lpBuffer                  
        add     XDI,STARTSIZE*2
        INVOKE  FIL4_READ,HostHandle,vSES XDI,SMEG_READSIZE

; Prepare for decryption stuff...

DECRYPT_QUEEG:
        lodsb
        mov AH,AL
        mov BX,AX
        mov dl,[XSI+0FFh]

        xor al,dl
        mov [XSI-1],al

        sub ah,dl
        mov [XSI+SMEG_READSIZE*2-1],ah

        neg bl
        neg dl
        xor bl,dl
        mov [XSI+SMEG_READSIZE-1],bl

        dec bh
        xor bh,[XSI]
        mov [XSI+SMEG_READSIZE*3-1],bh

        loop DECRYPT_QUEEG
endif

QUEEG_RET:
ifdef SDOS16
        cmp     PassCount, word ptr 0
        je      QUEEG_TSR_Ret
        push    XAX
        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_GET_START, HostHandle, lpBuffer, ADDR StartStruct
        pop     XAX
QUEEG_TSR_Ret:
endif
        ret

SCN4_QUEEG      ENDP

DECRYPT_XOR_DELTA PROC USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw
ifdef NLM
        cwde
endif
        inc     XDI
        inc     XDI
        test    dl, 080h
        jz      XOR_BYTE
        mov     BX, XES[XDI]            ; First word to look for
        dec     BL
        dec     BH
        xor     BX, [XSI]               ; Key is now in BX

        mov     CX, XES[XDI+2]          ; Second word to look for
        dec     CL
        dec     CH
        add     XSI, XAX
        xor     CX, [XSI]               ; Second XOR Key is now in CX
        sub     CX, BX                  ; Delta is now in CX

        add     BX, CX
        add     BX, CX                  ; BX now has the next key

        add     XSI, XAX
        push    BX
        xor     BX, [XSI]
        inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+4], BX          ; This should be the same.
        pop     BX
        jne     XOR_NOT_FOUND

        add     BX, CX                  ; BX now has the next key
        add     XSI, XAX
        xor     BX, [XSI]
        inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+6], BX          ; This should be the same.
        je      XOR_FOUND
XOR_NOT_FOUND:
        clc
        jmp     XOR_RET

XOR_BYTE:
        mov     BL, XES[XDI]            ; First byte to look for
        dec     BL
        xor     BL, [XSI]               ; Key is now in BL

        mov     CL, XES[XDI+1]          ; Second byte to look for
        dec     CL
        add     XSI, XAX
        xor     CL, [XSI]               ; Second XOR Key is now in CL
        sub     CL, BL                  ; Delta is now in CL

        add     BL, CL
        add     BL, CL                  ; BL now has the next key

        add     XSI, XAX
        mov     BH, BL                  ; Save present key
        xor     BL, [XSI]
        inc     BL                      ; Make it match +1 encryption.
        cmp     XES[XDI+2], BL          ; This should be the same.
        jne     XOR_NOT_FOUND

        add     BH, CL                  ; BH now has the next key
        add     XSI, XAX
        xor     BH, [XSI]
        inc     BH                      ; Make it match +1 encryption.
        cmp     XES[XDI+3], BH          ; This should be the same.
        jne     XOR_NOT_FOUND
XOR_FOUND:
        stc
XOR_RET:
        ret

DECRYPT_XOR_DELTA ENDP

DECRYPT_ADD_DELTA PROC USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw
ifdef NLM
        cwde
endif
        inc     XDI
        inc     XDI
        test    dl, 080h
        jz      ADD_BYTE
        mov     BX, XES[XDI]            ; First word to look for
        ;dec     BL
        dec     BH
        sub     BX, [XSI]               ; Key is now in BX

        mov     CX, XES[XDI+2]          ; Second word to look for
        ;dec     CL
        dec     CH
        add     XSI, XAX
        sub     CX, [XSI]               ; Second ADD Key is now in CX
        sub     CX, BX                  ; Delta is now in CX

        add     BX, CX
        add     BX, CX                  ; BX now has the next key

        add     XSI, XAX
        push    BX
        add     BX, [XSI]
        ;inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+4], BX          ; This should be the same.
        pop     BX
        jne     ADD_NOT_FOUND

        add     BX, CX                  ; BX now has the next key
        add     XSI, XAX
        add     BX, [XSI]
        ;inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+6], BX          ; This should be the same.
        je      ADD_FOUND
ADD_NOT_FOUND:
        clc
        jmp     ADD_RET

ADD_BYTE:
        mov     BL, XES[XDI]            ; First byte to look for
        ;dec     BL
        sub     BL, [XSI]               ; Key is now in BL

        mov     CL, XES[XDI+1]          ; Second byte to look for
        ;dec     CL
        add     XSI, XAX
        sub     CL, [XSI]               ; Second ADD Key is now in CL
        sub     CL, BL                  ; Delta is now in CL

        add     BL, CL
        add     BL, CL                  ; BL now has the next key

        add     XSI, XAX
        mov     BH, BL                  ; Save present key
        add     BL, [XSI]
        ;inc     BL                      ; Make it match +1 encryption.
        cmp     XES[XDI+2], BL          ; This should be the same.
        jne     ADD_NOT_FOUND

        add     BH, CL                  ; BH now has the next key
        add     XSI, XAX
        add     BH, [XSI]
        ;inc     BH                      ; Make it match +1 encryption.
        cmp     XES[XDI+3], BH          ; This should be the same.
        jne     ADD_NOT_FOUND
ADD_FOUND:
        stc
ADD_RET:
        ret

DECRYPT_ADD_DELTA ENDP

DECRYPT_ROTATE PROC USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw
ifdef NLM
        cwde
endif
        inc     XDI
        inc     XDI
        mov     dh,4
        test    dl, 080h
        jz      ROTATE_BYTE
rolw_outer:
        push    XAX                     ; Save EveryNBytes
        mov     ax, [XSI]
        mov     bx, XES[XDI]            ; Correct word
        dec     BL
        dec     BH
        mov     XCX,15
rolw_inner:                             ; 15, 13, 10, 6, 1, 11, 4, 12, 
        rol     bx, cl                  ; 3, 9, 14, 2, 5, 7, 8
        cmp     bx,ax
        je      @F
        loop    rolw_inner
        pop     XAX
        jmp     ROTATE_NOT_FOUND
@@:
        pop     XAX
        add     XSI, XAX
        inc     XDI
        inc     XDI
        dec     dh
        jnz     rolw_outer
        jmp     ROTATE_FOUND
   
ROTATE_NOT_FOUND:
        clc
        jmp     ROTATE_RET
ROTATE_BYTE:
rolb_outer:
        mov     bl, [XSI]
        mov     bh, XES[XDI]            ; Correct word
        dec     BH
        mov     XCX,7
rolb_inner:                             ; 7, 5, 2, 6, 1, 3, 4
        rol     bh,cl
        cmp     bh,bl
        je      @F
        loop    rolb_inner
        jmp     ROTATE_NOT_FOUND
@@:
        add     XSI, XAX
        inc     XDI
        dec     dh
        jnz     rolb_outer
ROTATE_FOUND:
        stc

ROTATE_RET:
        ret

DECRYPT_ROTATE ENDP

DECRYPT_NEG PROC USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        LDS_SI  Buf
        LES_DI  Sig

        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw
ifdef NLM
        cwde
endif
        mov     XCX,XAX                 ; CX now has "EveryNBytes"

        inc     XDI
        inc     XDI
        mov     dh,4
        test    dl, 080h
        jz      NEG_BYTE
negw_loop:
        mov     ax, word ptr [XSI]
        mov     bx, word ptr XES[XDI]   ; Correct word
        dec     BL
        dec     BH
        neg     bx
        cmp     bx,ax
        je      @F
        jmp     NEG_NOT_FOUND
@@:
        add     XSI, XCX
        inc     XDI
        inc     XDI
        dec     dh
        jnz     negw_loop
        jmp     NEG_FOUND
   
NEG_NOT_FOUND:
        clc
        jmp     NEG_RET
NEG_BYTE:
negb_loop:
        mov     bl, [XSI]
        mov     bh, XES[XDI]            ; Correct word
        dec     bh
        neg     bh
        cmp     bh,bl
        je      @F
        jmp     NEG_NOT_FOUND
@@:
        add     XSI, XCX
        inc     XDI
        dec     dh
        jnz     negb_loop
NEG_FOUND:
        stc

NEG_RET:
        ret

DECRYPT_NEG ENDP

DECRYPT_NOT PROC USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; 0xDF, 0x10, byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate
;               E=NEG
;               F=NOT

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw
ifdef NLM
        cwde
endif
        mov     XCX,XAX                 ; CX now has "EveryNBytes"

        inc     XDI
        inc     XDI
        mov     dh,4
        test    dl, 080h
        jz      NOT_BYTE
notw_loop:
        mov     ax, word ptr [XSI]
        mov     bx, word ptr XES[XDI]   ; Correct word
        dec     BL
        dec     BH
        not     bx
        cmp     bx,ax
        je      @F
        jmp     NOT_NOT_FOUND
@@:
        add     XSI, XCX
        inc     XDI
        inc     XDI
        dec     dh
        jnz     notw_loop
        jmp     NOT_FOUND
   
NOT_NOT_FOUND:
        clc
        jmp     NOT_RET
NOT_BYTE:
notb_loop:
        mov     bl, [XSI]
        mov     bh, XES[XDI]            ; Correct word
        dec     bh
        not     bh
        cmp     bh,bl
        je      @F
        jmp     NOT_NOT_FOUND
@@:
        add     XSI, XCX
        inc     XDI
        dec     dh
        jnz     notb_loop
NOT_FOUND:
        stc

NOT_RET:
        ret

DECRYPT_NOT ENDP

;----------------------------
get_test proc uses xsi sds,
     lpStartOffset: LPSTR


      LDS_SI  lpStartOffset

ifdef NLM
      mov     [XSI], eax
else
      mov     word ptr [XSI], ax
      mov     word ptr [XSI+2], dx
endif

      ret

get_test endp

END

