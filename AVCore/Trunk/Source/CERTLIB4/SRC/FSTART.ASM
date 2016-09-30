; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIB4/VCS/fstart.asv   1.29   09 Dec 1996 12:36:02   JALLEE  $
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
; $Log:   S:/CERTLIB4/VCS/fstart.asv  $
;  
;     Rev 1.29   09 Dec 1996 12:36:02   JALLEE
;  Oops, one more try to implement the changes noted in the previous rev.
;  
;     Rev 1.28   09 Dec 1996 11:42:06   JALLEE
;  Ported following changes from Qak2.
;  
;  ;     Rev 1.24.1.10   21 Aug 1996 10:23:16   CNACHEN
;  ;  Fixed fstart 256 byte buffer boundary bug.
;  ;  
;  ;     Rev 1.24.1.9   04 Dec 1995 11:39:28   DCHI
;  ;  Removed exclusion for files beginning with "PK".
;  ;  
;  ;     Rev 1.24.1.8   18 Sep 1995 17:23:18   DCHI
;  ;  Added FIL4_NEW_GET_START.
;  ;  
;  ;     Rev 1.24.1.7   12 Sep 1995 18:15:20   DCHI
;  ;  Removed PKLITE check and removed skipping of opcode bytes 0x00 and 0x33.
;  
;     Rev 1.27   29 Nov 1994 13:21:24   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.24.1.6   23 Nov 1994 10:24:22   JMILLARD
;  fix problem with fstart when getting an error on the last FIL4_READ
;  fix problem with handling an error return (-1) in the exit code
;  
;     Rev 1.24.1.5   17 Oct 1994 15:36:12   SZIADEH
;  
;     Rev 1.24.1.4   09 Oct 1994 19:40:02   SZIADEH
;  FirstLanding does is not necessarily equate to the offset (in the file) 
;  of the work buffer. The correct offset in the file is required for the
;  implementation of FollowJump(). Thus, a new component was added to 
;  STARTCODE structure called BufferOffset to keep track of such offset.
;  Files affected are: CTS4.INC CTS4.H FSTART.ASM SCN_ME.ASM SCN_ALG.ASM.
;  
;     Rev 1.24.1.3   08 Jul 1994 14:50:16   JMILLARD
;  fix problem with getting the FIL4_SEEK result from 2 registers in 32-bit mode
;  (it is all in EAX) - this broke because pathogen now uses Firstlanding (which
;  was always incorrect before - an bogus value for dx is loaded a few lines
;  earlier when then FIL4_SEEK is called
;  
;     Rev 1.24.1.2   11 May 1994 08:46:02   JMILLARD
;  fix problem with running the buffer pointer as 16 bits instead of 32. Thanks,
;  Rad!
;  
;     Rev 1.24.1.1   27 Dec 1993 16:25:46   SZIADEH
;  fixed the problem of failing to detect file greater that 512K. 
;  
;     Rev 1.24.1.0   19 Oct 1993 07:23:06   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.24   12 Oct 1993 16:33:28   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.23.1.3   06 Oct 1993 15:25:12   CJKUO
;  FB was listed twice in the ignore list
;  
;     Rev 1.23.1.2   08 Sep 1993 16:05:58   CJKUO
;  Assert cld, just in case
;  
;     Rev 1.23.1.1   08 Sep 1993 15:40:54   CJKUO
;  if the buffer read is less than 32, add 32 NULLs
;  
;  
;     Rev 1.23.1.0   03 Sep 1993 19:30:12   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.23   02 Sep 1993 16:55:06   CAREY
;  Fixed a bug created by the first landing code.
;  
;     Rev 1.22   01 Sep 1993 20:45:28   CAREY
;  Sped up detection by several microseconds by having verified coms
;  jump directly to the jump (e8) code.
;  
;     Rev 1.21   31 Aug 1993 13:29:06   CAREY
;  Fixed first landing bug.
;  
;     Rev 1.20   30 Aug 1993 20:37:32   CAREY
;  Correct PUSHRET code for Andrushkas
;  
;     Rev 1.19   22 Jul 1993 14:16:10   CAREY
;  Code to set FirstLanding correctly, for use mostly by specific repairs,
;  plus by CUT_FROM_BEGIN
;  
;     Rev 1.18   17 Jul 1993 19:58:34   DALLEE
;  LES_DI StartStruct before using XES[XDI] to write to FirstLanding.
;  
;     Rev 1.17   16 Jul 1993 23:14:02   CAREY
;  The fstart routine has been modified to consistently set the FirstLanding
;  field so the cut_from_begin function can be implemented.  I removed the
;  old code that set firstlanding in the VerifiedCom section of the code and
;  moved the code to set the firstlanding right after the file seek at label
;  j_reread.
;  
;     Rev 1.16   12 Jul 1993 17:32:26   CJKUO
;  Set ES:DI before comparing for IS_COM
;  
;     Rev 1.15   10 Jul 1993 15:18:34   CJKUO
;  After the rep movsw, was not reseting SI
;  
;     Rev 1.14   10 Jul 1993 13:11:26   CJKUO
;  Only save 64 bytes of header
;  
;     Rev 1.13   09 Jul 1993 16:05:04   CJKUO
;  Instead of reading into the secondary buffer, read into first but
;  copy into second...
;  
;     Rev 1.12   09 Jul 1993 12:54:58   CJKUO
;  First read is into Buffer+STARTSIZE, subsequent reads are into Buffer
;  
;     Rev 1.11   18 Jun 1993 16:54:22   CJKUO
;  Change to not follow more than 100 jumps
;  
;     Rev 1.10   19 May 1993 18:18:34   DDREW
;  Set UINT to WORD for hHandle
;  
;     Rev 1.9   12 May 1993 12:40:50   JWELLS
;  STARTSTRUCT FileType and FirstLanding initialized.
;  Minimum read required of FIL4_READ.
;  FIL4_READ return of -1 returned as 0 and STARTSTRUCT.FileType = IS_INVALID.
;  Code added to return correct FirstLanding for V2P group detection.
;  If ByteIn is 0, redundant file read skipped.
;  Code added to trace mov/push/ret start in viruses (CJKuo).
;  
;     Rev 1.8   24 Apr 1993 21:15:04   CJKUO
;  Guarantee FIL4_GET_START returns the valid length of buffer.
;  Invalidates return of 1 if Whale is found.
;  
;     Rev 1.7   21 Apr 1993 15:18:24   MFALLEN
;  Saving DS
;  
;     Rev 1.6   01 Apr 1993 12:55:42   JMILLARD
;  ensure ES is always right so we don't tromp memory - per David Allee 
;  
;     Rev 1.5   23 Mar 1993 16:46:16   JMILLARD
;  check in fix to skipping zip files
;  
;     Rev 1.4   17 Mar 1993 17:52:50   GUC_C
;  Updated from John M.
;  
;  
;     Rev 1.4   16 Mar 1993 20:19:06   JMILLARD
;  move lookup table to .data if nlm
;  
;     Rev 1.3   09 Mar 1993 12:04:00   WELLS_J
;  Added Whale, Lzexe, Pklite, Pkzip, and Invalid File returns.
;  
;  
;     Rev 1.3   26 Feb 1993 10:23:22   JMILLARD
;  fix problem with .sys - now correctly finds strategy entry point
;  larger questions remain about what we ought to be scanning
;  
;     Rev 1.2   08 Feb 1993 12:31:12   JMILLARD
;  check in Carlton's changes
;  
;     Rev 1.1   07 Jan 1993 22:58:48   JMILLARD
;  Fixed several problems found in the course of pursuing 32-bit problems.
;  
;  1. Fixed treatment of all signed jumps and calls. This required tracking
;     the apparent IP in order to decide whether to seek forward or backward
;     in the file
;  2. Handle an unusual case requiring sign-extension of the initial offset
;     into a .exe file when the file is very small and the initial virus IP
;     is > 0. An example is avpd ital_805.exe
;  3. Corrected 32-bit problem with not clearing the high word of ecx before
;     the TestByte check loop
;  4. Corrected problem with calculated length of TestByte table (1 too long) -
;     it was running off the end and including a garbage byte in the table.
;  5. Treat single-byte jumps as signed.
;  6. Restore si after each new read - si gets changed by the routine and needs
;     to start at the buffer start after a new read.
;  
;     Rev 1.0   07 Jan 1993 15:14:00   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


include CTS4.INC
UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_GET_START                       12/28/92                        CAG
;
;       Get the starting code of an executable program.
;
; DWORD WINPAI FIL4_GET_START(UINT Handle,LPSTR Buffer,STARTCODE Struct);
;
; Input:
;       1)  Handle of file
;       2)  Buffer to use (256 bytes) (STARTSIZE)
;       3)  Structure of EXE start
; Output:
;       1)  Buffer filled
;       2)  Structure filled
;       3)  (AX) = Number of bytes read, 0 for error.
;
;-----------------------------------------------------------------------------
FIL4_GET_START PROC USES XBX XCX SDS SES XSI XDI,
        Handle:WORD,
        Buffer:LPSTR,
        StartStruct:LPSTR

MAXDEPTH        EQU     100
        LOCAL   Depth:WORD
        LOCAL   Retval:WORD
        LOCAL   BytesRead:WORD
        LOCAL   ByteIn:WORD             ; offset in buffer
        LOCAL   MyIP:WORD               ; current IP
        LOCAL   WhaleFlag:WORD
        LOCAL   SetFirst:WORD           ; have we set the first landing yet?

        mov     SetFirst, 0             ; we have not hit the first landing yet

        LDS_SI  Buffer
        LES_DI  StartStruct

        mov     ByteIn,0
        mov     Depth,word ptr 0

; Initialize structure returns to zero.
        mov     XES[XDI].STARTCODE.Filetype,0


        ; make sure both HIGH & LOW word's of first landing are zero.

        ;int     03h

        mov     word ptr XES[XDI].STARTCODE.Firstlanding,0
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,0

        mov     word ptr XES[XDI].STARTCODE.BufferOffset,0
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,0

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE

;        cmp     ax,-1                   ; DOS read error
;        je      J_INVALID               ; Return 0 and INVALID

        cmp     ax,1                    ; Minimum read of 1 bytes
        jl      J_INVALID               ; is needed for testing.
                                        ; jl also covers -1 error return

        mov     BytesRead,ax            ; Number of bytes read(EOB)
        mov     Retval,ax

        LES_DI  Buffer
        add     XDI,STARTSIZE
        cld
        mov     XCX,32                  ; save 64 bytes of header
        rep     movsw
        LES_DI  StartStruct
        sub     XSI,64                  ; restore SI back to head of buffer

; Determine type of program (COM,EXE,SYS,Bulgarian,Unknown)
        xor     XAX,XAX
        xor     XDX,XDX

        mov     MyIP,100h               ; default start IP

        cmp     [XSI],word ptr 'ZM'     ; Native EXE
        je      J_EXE

        cmp     [XSI],word ptr 'MZ'     ; European EXE
        je      J_EXE

        cmp     [XSI],word ptr 0E94Dh   ; Bulgarian marked COM
        je      J_BCOM

        cmp     [XSI],byte ptr 0E9h     ; Verified COM
        je      J_VCOM

        cmp     [XSI],word ptr -1       ; Test if it is a DEVICE Driver
        jne     J_COM
        cmp     [XSI+2],word ptr -1     ; Test if it is a DEVICE Driver
        jne     J_COM

        jmp     J_SYS

J_COM:
        mov     XES[XDI].STARTCODE.Filetype,IS_COM
        jmp     J_START

J_BCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_BCOM
        jmp     J_START

J_VCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_VCOM
;-------------------------------
; Whale Test for COMs

        cmp     BytesRead, STARTSIZE    ; must have at least 5 bytes to verify!
        jl      J_E8

        mov     ax,[XSI+4]              ; Get identity word
        xor     ax,1501h                ; xor with 'FI' and 'SH'
        add     ax,[XSI+2]              ; add second and
        add     ax,[XSI]                ; first words
        or      ax,ax                   ; If zero then Whale
        jz      J_Moby

        jmp     J_E8

J_Moby:
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        jmp     J_OUT

J_SYS:
        mov     XES[XDI].STARTCODE.Filetype,IS_SYS
        xor     ax,ax                   ; drivers start at 0
        mov     MyIP,ax                 ; set up initial ip
        mov     ax,[XSI+6]              ; address to jump to
        jmp     J_FJ3                   ; and treat as jump to continue

J_EXE:
        cmp     word ptr[XSI+14h],1     ; IP = 1 in Whale
        jne     J_EXE1
        mov     WhaleFlag,1
J_EXE1:
        mov     XES[XDI].STARTCODE.Filetype,IS_EXE

        cmp     [XSI+1Ch],word ptr 'ZL'                 ; LZEXE
        jne     @F
        mov     XES[XDI].STARTCODE.Filetype,IS_LZEXE
@@:        
        mov     ax,[XSI+16h]            ; CS of header
        add     ax,[XSI+08h]            ; Number of paragraphs in header
        mov     dx,10h
        mul     dx                      ; DX:AX = Location of code
        add     ax,[XSI+14h]            ; IP DX:AX = Location of code
        adc     dx,0

        ; The and instruction is used in the case where a wrap around
        ; occures. As in ital_805 where cs=fff1 and ip=100h
        ; In all other cases, dx:ax is the correct offset of the programs
        ; entry point.

        and     dx, 000fh               ; clear high nibbles.
        sub     ax,BytesRead
        sbb     dx,0
        mov     cx,dx
        mov     dx,ax

        mov     ax,[XSI+14h]            ; save starting IP
        mov     MyIP,ax

        jmp     J_ReRead

;-------------------------------
J_START:

J_TestAgain:
;-------------------------------



        mov     ah,MAXPAD               ; Look for PADDING bytes first
        cld                             ; clear for scan
        SMOV    SES,SCS
J_TB:
        lea     XDI,TestByteList
        mov     XCX,TestByteListLength
        mov     al,[XSI]
        repne   scasb
        jne     J_TB0
        dec     ah                      ; Padding bytes
        jz      J_DONE
        inc     XSI
        inc     ByteIn
        inc     MyIP
        mov     cx,BytesRead
        cmp     ByteIn,cx
        jb      J_TB
        jmp     J_FillBuffer
J_TB0:

;-------------------------------
; Look for JUMP/CALL
        xor     XAX,XAX
        cmp     [XSI],byte ptr 0EBh
        je      J_EB
        cmp     [XSI],byte ptr 0E8h
        je      J_E8
        cmp     [XSI],byte ptr 0E9h
        je      J_E8
        LES_DI  StartStruct
        cmp     XES[XDI].STARTCODE.Filetype,IS_COM

        jne     J_DONE                          ; Skip ret trace if not COM

;--------------------------------------- New code to trace initial
;                                        mov/push/ret virus system

        cmp     [XSI+4], byte ptr 0C3h          ; Either RET
        je      J_PUSHRET
        cmp     [XSI+4],word ptr 0C3FAh         ; or CLI RET
        jne     J_DONE

J_PUSHRET:
        mov     al,[XSI]                        ; XOR the MOV and PUSH
        mov     ah,[XSI+3]                      ;  together to make sure
        xor     al,ah                           ;  it uses the same reg.
        cmp     al, byte ptr 0E8h               ;  Save XSI+3 in ah.
        jne     J_DONE                          
        and     ah, byte ptr 0F8h               ; Wipe bottom 3 bits
        cmp     ah, byte ptr 050h               ; Make sure it was a PUSH
        jne     J_DONE                            
        mov     ax, [XSI+1]                     ; Get landing and convert
        sub     ax,100h                         ; absolute to relative.

        ; since we're doing an absolute jump from TOF, we want to zero
        ; our buffer start and bytes-in

        mov     MyIP, 100h                      ; reset IP (non-relative jmp)
        LDS_SI  Buffer                          ; definitely do a re-read
        mov     ByteIn, 0                       ; 0 bytes into buffer.

        jmp     J_FJ3                           

; So the validation is: find the RET, verify the PUSH right before the
; RET, verify a MOV instruction to the same register.  The only thing I
; don't check is buffer out-of-bounds but then I don't think this
; circumstance could be fulfilled with random data. -- CJKuo
;

J_E8:
        mov     ax,[XSI+1]
        add     ax,3
        jmp     J_FJ3
J_EB:
        mov     al,[XSI+1]
        cbw                             ; this is a signed offset!
        add     ax,2
J_FJ3:
        inc     Depth                   ; jumping another level deeper
        cmp     Depth,MAXDEPTH          ; allow only MAXDEPTH levels
        ja      J_ERROR
        add     ax,MyIP                 ; new IP and test direction of seek
        mov     bx,ax                   
        xor     cx,cx                   ; assume positive seek
        sub     ax,MyIP                 ; back to relative, test direction
        mov     MyIP,bx                 ; and set new IP
        jae     F2                      ; forward
        mov     cx,-1                   ; negative seek
if OS_32                                ; sign extend ax if needed
        cwde                            ; make offset 32-bit
endif                                   ; XAX signed offset in segment
F2:                                     
        add     XSI,XAX                 ; new memory offset (possibly)
        add     ByteIn,ax               ; offset to seek
        js      J_Backward
        mov     ax,BytesRead
        cmp     ByteIn,ax

        pushf                           ; save flags from compare

        mov     ax, ByteIn              ; send this over to J_TestAgain for
                                        ; first landing calculation! *CAREY*


        cmp     SetFirst, 0             ; is this the first landing?
        jnz     @F

        ; we should only be setting FirstLanding here in the case where a JMP
        ; destination was within our original buffer.  In this case, the SEEK
        ; by J_ReRead is not performed until later (or never).  If SetFirst is
        ; 0 and we are setting the first landing here, it will mean that we
        ; jumped from the JB at J_FJ3 instead of doing the standard SEEK.  AX
        ; will contain the low word of the first landing, and the high word
        ; will be zero.

        LES_DI  StartStruct
        mov     word ptr XES[XDI].STARTCODE.Firstlanding, ax
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,0

        mov     SetFirst, 1             ; no longer the first landing

@@:

        popf                            ; restore flags!

        jnc     J_Backward

        ; new behavior: always reload if we're following a jump C&D 8/19

        mov     cx, -1
        mov     dx, ByteIn
        sub     dx, BytesRead
        jmp     J_ReRead

J_Backward:
        mov     dx,ByteIn               ; current position
        sub     dx,BytesRead            ; rememer we are past this point now                            
        sbb     cx,0

J_ReRead:

        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_FOR


        LES_DI  StartStruct
ifdef NLM
        mov     XES[XDI].STARTCODE.BufferOffset,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset,  ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        cmp     SetFirst, 0             ; is this the first landing?
        jnz     @F

ifdef NLM                               ; 32-bit return is in one register, 
                                        ; not two
        mov     XES[XDI].STARTCODE.Firstlanding,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.Firstlanding,  ax
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,dx
endif


        mov     SetFirst, 1             ; no longer the first landing

@@:

;-------------------------------
J_FillBuffer:
        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     ax,-1
        je      J_INVALID
        cmp     ax,0
        je      J_INVALID
        mov     Retval,ax
        mov     BytesRead,ax            ; May be needed by final seek

        LDS_SI  Buffer                  ; recover SI which may have changed
        cmp     WhaleFlag,0
   
;-------------------------------
; Whale Test for EXEs

        je      J_GOON
        cmp     byte ptr[XSI],0E9h
        jne     J_GOON
        cmp     word ptr[XSI+1],023B8h
        jne     J_GOON
        cmp     word ptr[XSI+3],05A4Dh
        jne     J_GOON
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        mov     ax,1
        jmp     J_OUT

J_GOON:
        mov     WhaleFlag,0
        mov     BytesRead,ax
        mov     ByteIn,0
        jmp     J_TestAgain

;-------------------------------
J_DONE:
        cmp     ByteIn,0
        je      J_OUT
        mov     dx,BytesRead
        sub     dx,ByteIn
        xor     cx,cx
        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_BAK

                                        ;added in order to implement
                                        ;FollowJump(). Sami

        LES_DI  StartStruct
ifdef NLM
        mov     XES[XDI].STARTCODE.BufferOffset, XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset, ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     ax,-1                   ; correctly handle an error, otherwise
        je      J_INVALID               ; I end up returning 0xffff bytes
                                        ; instead of an error (0)
        mov     Retval,ax
        jmp     J_OUT

J_INVALID:      ;Handles zero bytes read
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_INVALID

J_ERROR:                                ; note that this doesn't actually
        mov     Retval,0                ; return anything the caller will
                                        ; see as an error.

J_OUT:
ifdef NLM
        xor     XAX,XAX                 ; to ret a DWORD value, clear x reg
ENDIF
        mov     ax, Retval
        cmp     ax, 32
        jae     J_Go                    ; used to be jge - but that fails if
                                        ; if ax is 0xffff - the error return,
                                        ; then we write at one byte in front of
                                        ; the buffer in 16-bit, and at 0xffff 
                                        ; after the buffer in 32-bit mode. This
                                        ; used to only be a possible problem on
                                        ; the FIL4_READ 20 lines earlier - that 
                                        ; has also been fixed to correctly 
                                        ; return 0, not -1 from fstart
        cld
        LES_DI  Buffer
        add     XDI,XAX
        xor     XAX,XAX
        mov     XCX,16
        rep     stosw
        mov     ax, Retval
J_Go:
        ret

FIL4_GET_START ENDP

;-----------------------------------------------------------------------------
; FIL4_NEW_GET_START
;
; Get the starting code of an executable program.  Doesn't skip 0x00 and 0x33.
;
; DWORD WINPAI FIL4_NEW_GET_START(UINT Handle,LPSTR Buffer,STARTCODE Struct);
;
; Input:
;       1)  Handle of file
;       2)  Buffer to use (256 bytes) (STARTSIZE)
;       3)  Structure of EXE start
; Output:
;       1)  Buffer filled
;       2)  Structure filled
;       3)  (AX) = Number of bytes read, 0 for error.
;
;-----------------------------------------------------------------------------
FIL4_NEW_GET_START PROC USES XBX XCX SDS SES XSI XDI,
        Handle:WORD,
        Buffer:LPSTR,
        StartStruct:LPSTR

MAXDEPTH        EQU     100
        LOCAL   Depth:WORD
        LOCAL   Retval:WORD
        LOCAL   BytesRead:WORD
        LOCAL   ByteIn:WORD             ; offset in buffer
        LOCAL   MyIP:WORD               ; current IP
        LOCAL   WhaleFlag:WORD
        LOCAL   SetFirst:WORD           ; have we set the first landing yet?

        mov     SetFirst, 0             ; we have not hit the first landing yet

        LDS_SI  Buffer
        LES_DI  StartStruct

        mov     ByteIn,0
        mov     Depth,word ptr 0

; Initialize structure returns to zero.
        mov     XES[XDI].STARTCODE.Filetype,0


        ; make sure both HIGH & LOW word's of first landing are zero.

        ;int     03h

        mov     word ptr XES[XDI].STARTCODE.Firstlanding,0
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,0

        mov     word ptr XES[XDI].STARTCODE.BufferOffset,0
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,0

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE

;        cmp     ax,-1                   ; DOS read error
;        je      J_INVALID               ; Return 0 and INVALID

        cmp     ax,1                    ; Minimum read of 1 bytes
        jl      JN_INVALID              ; is needed for testing.
                                        ; jl also covers -1 error return

        mov     BytesRead,ax            ; Number of bytes read(EOB)
        mov     Retval,ax

        LES_DI  Buffer
        add     XDI,STARTSIZE
        cld
        mov     XCX,32                  ; save 64 bytes of header
        rep     movsw
        LES_DI  StartStruct
        sub     XSI,64                  ; restore SI back to head of buffer

; Determine type of program (COM,EXE,SYS,Bulgarian,Unknown)
        xor     XAX,XAX
        xor     XDX,XDX

        mov     MyIP,100h               ; default start IP

        cmp     [XSI],word ptr 'ZM'     ; Native EXE
        je      JN_EXE

        cmp     [XSI],word ptr 'MZ'     ; European EXE
        je      JN_EXE

        cmp     [XSI],word ptr 0E94Dh   ; Bulgarian marked COM
        je      JN_BCOM

        cmp     [XSI],byte ptr 0E9h     ; Verified COM
        je      JN_VCOM

        cmp     [XSI],word ptr -1       ; Test if it is a DEVICE Driver
        jne     JN_COM
        cmp     [XSI+2],word ptr -1     ; Test if it is a DEVICE Driver
        jne     JN_COM

        jmp     JN_SYS

JN_COM:
        mov     XES[XDI].STARTCODE.Filetype,IS_COM
        jmp     JN_START

JN_BCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_BCOM
        jmp     JN_START

JN_VCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_VCOM
;-------------------------------
; Whale Test for COMs

        cmp     BytesRead, STARTSIZE    ; must have at least 5 bytes to verify!
        jl      JN_E8

        mov     ax,[XSI+4]              ; Get identity word
        xor     ax,1501h                ; xor with 'FI' and 'SH'
        add     ax,[XSI+2]              ; add second and
        add     ax,[XSI]                ; first words
        or      ax,ax                   ; If zero then Whale
        jz      JN_Moby

        jmp     JN_E8

JN_Moby:
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        jmp     JN_OUT

JN_SYS:
        mov     XES[XDI].STARTCODE.Filetype,IS_SYS
        xor     ax,ax                   ; drivers start at 0
        mov     MyIP,ax                 ; set up initial ip
        mov     ax,[XSI+6]              ; address to jump to
        jmp     JN_FJ3                  ; and treat as jump to continue

JN_EXE:
        cmp     word ptr[XSI+14h],1     ; IP = 1 in Whale
        jne     JN_EXE1
        mov     WhaleFlag,1
JN_EXE1:
        mov     XES[XDI].STARTCODE.Filetype,IS_EXE

        cmp     [XSI+1Ch],word ptr 'ZL'                 ; LZEXE
        jne     @F
        mov     XES[XDI].STARTCODE.Filetype,IS_LZEXE
@@:        
        mov     ax,[XSI+16h]            ; CS of header
        add     ax,[XSI+08h]            ; Number of paragraphs in header
        mov     dx,10h
        mul     dx                      ; DX:AX = Location of code
        add     ax,[XSI+14h]            ; IP DX:AX = Location of code
        adc     dx,0

        ; The and instruction is used in the case where a wrap around
        ; occures. As in ital_805 where cs=fff1 and ip=100h
        ; In all other cases, dx:ax is the correct offset of the programs
        ; entry point.

        and     dx, 000fh               ; clear high nibbles.
        sub     ax,BytesRead
        sbb     dx,0
        mov     cx,dx
        mov     dx,ax

        mov     ax,[XSI+14h]            ; save starting IP
        mov     MyIP,ax

        jmp     JN_ReRead

;-------------------------------
JN_START:

JN_TestAgain:
;-------------------------------



        mov     ah,MAXPAD               ; Look for PADDING bytes first
        cld                             ; clear for scan
        SMOV    SES,SCS
JN_TB:
        lea     XDI,TestByteNewList
        mov     XCX,TestByteNewListLength
        mov     al,[XSI]
        repne   scasb
        jne     JN_TB0
        dec     ah                      ; Padding bytes
        jz      JN_DONE
        inc     XSI
        inc     ByteIn
        inc     MyIP
        mov     cx,BytesRead
        cmp     ByteIn,cx
        jb      JN_TB
        jmp     JN_FillBuffer
JN_TB0:

;-------------------------------
; Look for JUMP/CALL
        xor     XAX,XAX
        cmp     [XSI],byte ptr 0EBh
        je      JN_EB
        cmp     [XSI],byte ptr 0E8h
        je      JN_E8
        cmp     [XSI],byte ptr 0E9h
        je      JN_E8
        LES_DI  StartStruct
        cmp     XES[XDI].STARTCODE.Filetype,IS_COM

        jne     JN_DONE                         ; Skip ret trace if not COM

;--------------------------------------- New code to trace initial
;                                        mov/push/ret virus system

        cmp     [XSI+4], byte ptr 0C3h          ; Either RET
        je      JN_PUSHRET
        cmp     [XSI+4],word ptr 0C3FAh         ; or CLI RET
        jne     JN_DONE

JN_PUSHRET:
        mov     al,[XSI]                        ; XOR the MOV and PUSH
        mov     ah,[XSI+3]                      ;  together to make sure
        xor     al,ah                           ;  it uses the same reg.
        cmp     al, byte ptr 0E8h               ;  Save XSI+3 in ah.
        jne     JN_DONE
        and     ah, byte ptr 0F8h               ; Wipe bottom 3 bits
        cmp     ah, byte ptr 050h               ; Make sure it was a PUSH
        jne     JN_DONE
        mov     ax, [XSI+1]                     ; Get landing and convert
        sub     ax,100h                         ; absolute to relative.

        ; since we're doing an absolute jump from TOF, we want to zero
        ; our buffer start and bytes-in

        mov     MyIP, 100h                      ; reset IP (non-relative jmp)
        LDS_SI  Buffer                          ; definitely do a re-read
        mov     ByteIn, 0                       ; 0 bytes into buffer.

        jmp     JN_FJ3

; So the validation is: find the RET, verify the PUSH right before the
; RET, verify a MOV instruction to the same register.  The only thing I
; don't check is buffer out-of-bounds but then I don't think this
; circumstance could be fulfilled with random data. -- CJKuo
;

JN_E8:
        mov     ax,[XSI+1]
        add     ax,3
        jmp     JN_FJ3
JN_EB:
        mov     al,[XSI+1]
        cbw                             ; this is a signed offset!
        add     ax,2
JN_FJ3:
        inc     Depth                   ; jumping another level deeper
        cmp     Depth,MAXDEPTH          ; allow only MAXDEPTH levels
        ja      JN_ERROR
        add     ax,MyIP                 ; new IP and test direction of seek
        mov     bx,ax                   
        xor     cx,cx                   ; assume positive seek
        sub     ax,MyIP                 ; back to relative, test direction
        mov     MyIP,bx                 ; and set new IP
        jae     NF2                     ; forward
        mov     cx,-1                   ; negative seek
if OS_32                                ; sign extend ax if needed
        cwde                            ; make offset 32-bit
endif                                   ; XAX signed offset in segment
NF2:
        add     XSI,XAX                 ; new memory offset (possibly)
        add     ByteIn,ax               ; offset to seek
        js      JN_Backward
        mov     ax,BytesRead
        cmp     ByteIn,ax

        pushf                           ; save flags from compare

        mov     ax, ByteIn              ; send this over to J_TestAgain for
                                        ; first landing calculation! *CAREY*


        cmp     SetFirst, 0             ; is this the first landing?
        jnz     @F

        ; we should only be setting FirstLanding here in the case where a JMP
        ; destination was within our original buffer.  In this case, the SEEK
        ; by J_ReRead is not performed until later (or never).  If SetFirst is
        ; 0 and we are setting the first landing here, it will mean that we
        ; jumped from the JB at J_FJ3 instead of doing the standard SEEK.  AX
        ; will contain the low word of the first landing, and the high word
        ; will be zero.

        LES_DI  StartStruct
        mov     word ptr XES[XDI].STARTCODE.Firstlanding, ax
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,0

        mov     SetFirst, 1             ; no longer the first landing

@@:

        popf                            ; restore flags!

        jnc     JN_Backward

        ; new behavior: always reload if we're following a jump C&D 8/19

        mov     cx, -1
        mov     dx, ByteIn
        sub     dx, BytesRead
        jmp     JN_ReRead

JN_Backward:
        mov     dx,ByteIn               ; current position
        sub     dx,BytesRead            ; rememer we are past this point now                            
        sbb     cx,0

JN_ReRead:

        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_FOR


        LES_DI  StartStruct
ifdef NLM
        mov     XES[XDI].STARTCODE.BufferOffset,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset,  ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        cmp     SetFirst, 0             ; is this the first landing?
        jnz     @F

ifdef NLM                               ; 32-bit return is in one register, 
                                        ; not two
        mov     XES[XDI].STARTCODE.Firstlanding,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.Firstlanding,  ax
        mov     word ptr XES[XDI].STARTCODE.Firstlanding+2,dx
endif


        mov     SetFirst, 1             ; no longer the first landing

@@:

;-------------------------------
JN_FillBuffer:
        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     ax,-1
        je      JN_INVALID
        cmp     ax,0
        je      JN_INVALID
        mov     Retval,ax
        mov     BytesRead,ax            ; May be needed by final seek

        LDS_SI  Buffer                  ; recover SI which may have changed
        cmp     WhaleFlag,0
   
;-------------------------------
; Whale Test for EXEs

        je      JN_GOON
        cmp     byte ptr[XSI],0E9h
        jne     JN_GOON
        cmp     word ptr[XSI+1],023B8h
        jne     JN_GOON
        cmp     word ptr[XSI+3],05A4Dh
        jne     JN_GOON
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        mov     ax,1
        jmp     JN_OUT

JN_GOON:
        mov     WhaleFlag,0
        mov     BytesRead,ax
        mov     ByteIn,0
        jmp     JN_TestAgain

;-------------------------------
JN_DONE:
        cmp     ByteIn,0
        je      JN_OUT
        mov     dx,BytesRead
        sub     dx,ByteIn
        xor     cx,cx
        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_BAK

                                        ;added in order to implement
                                        ;FollowJump(). Sami

        LES_DI  StartStruct
ifdef NLM
        mov     XES[XDI].STARTCODE.BufferOffset, XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset, ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     ax,-1                   ; correctly handle an error, otherwise
        je      JN_INVALID              ; I end up returning 0xffff bytes
                                        ; instead of an error (0)
        mov     Retval,ax
        jmp     JN_OUT

JN_INVALID:      ;Handles zero bytes read
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_INVALID

JN_ERROR:                               ; note that this doesn't actually
        mov     Retval,0                ; return anything the caller will
                                        ; see as an error.

JN_OUT:
ifdef NLM
        xor     XAX,XAX                 ; to ret a DWORD value, clear x reg
ENDIF
        mov     ax, Retval
        cmp     ax, 32
        jae     JN_Go                   ; used to be jge - but that fails if
                                        ; if ax is 0xffff - the error return,
                                        ; then we write at one byte in front of
                                        ; the buffer in 16-bit, and at 0xffff 
                                        ; after the buffer in 32-bit mode. This
                                        ; used to only be a possible problem on
                                        ; the FIL4_READ 20 lines earlier - that 
                                        ; has also been fixed to correctly 
                                        ; return 0, not -1 from fstart
        cld
        LES_DI  Buffer
        add     XDI,XAX
        xor     XAX,XAX
        mov     XCX,16
        rep     stosw
        mov     ax, Retval
JN_Go:
        ret

FIL4_NEW_GET_START ENDP

IFDEF NLM         
        .data                          ; NLM prefers code and data separate
ENDIF

TestByteList    Label   byte
        db      0F5h,0F8h,0F9h,0FBh,0FCh,033h,046h,04Bh,05Eh,090h,042h,092h
        db      0FAh,0FDh,09Fh,09Eh,058h,059h,05Ah,05Dh,04Dh,000h

TestByteListLength      EQU     ($-TestByteList)

TestByteNewList Label   byte
        db      0F5h,0F8h,0F9h,0FBh,0FCh,046h,04Bh,05Eh,090h,042h,092h
        db      0FAh,0FDh,09Fh,09Eh,058h,059h,05Ah,05Dh,04Dh

TestByteNewListLength   EQU     ($-TestByteNewList)


;-----------------------------------------------------------------------------
END

