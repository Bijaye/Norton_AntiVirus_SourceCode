;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/scn_alg.asv   1.25   09 Jul 1996 18:26:16   DCHI  $
;;
;; Description:  Defines SCN4_ALG function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/scn_alg.asv  $
;  
;     Rev 1.25   09 Jul 1996 18:26:16   DCHI
;  Put IFDEF NAVOEM around GIRAFE detection code.
;  
;     Rev 1.24   27 Jun 1996 17:46:44   DALLEE
;  LuigiPlus port:
;  Re-merged DECRYPT_ROTATE fix -- last merge caught an extra line.
;  
;     Rev 1.23   06 May 1996 12:09:12   DCHI
;  Modified ENTRYPT functions to work correctly on COM files.
;  
;     Rev 1.22   29 Nov 1995 16:01:04   DCHI
;  Modified SCN4_ALG to support INSCANF data greater than 64K.
;  
;     Rev 1.21   29 Nov 1995 14:47:16   DCHI
;  1. Replaced old algorithmic Execute command with new NAVEX Execute command.
;  2. Fixed bug with byte rotate encryption detection.
;  3. Added CheckEntryptImage() and CheckEntryptFilesize() functions.
;  4. Added NEG and NOT encryption detection.
;  5. Changed Queeg function in jump table to DF_VOID to return fail.
;  
;     Rev 1.20   10 Nov 1995 16:08:56   GDZIECI
;  Fixed protection violations on NetWare server platform.
;  
;     Rev 1.19   23 Jun 1995 12:34:04   SZIADEH
;  activated loading and execution of external dat files.
;  
;     Rev 1.18   21 Dec 1994 19:51:04   DALLEE
;  Commented out calls to SCN4_EXEC (again!) until someone uncomments the 
;  actual function and tells me it's working.
;  
;     Rev 1.17   18 Dec 1994 20:12:28   SZIADEH
;  incorporated 3.05 changes including 32-bit conversion.
;  
;     Rev 1.16   17 Dec 1994 00:10:54   SZIADEH
;  added another parameter to SCN_ALG.
;  
;     Rev 1.15   07 Sep 1994 15:05:56   RSTANEV
;  Fixed a problem with macro parameter passing in VxD platform.
;
;     Rev 1.14   07 Sep 1994 11:49:08   SZIADEH
;  changed movzx to movux in FOLLOWJUMP code.
;
;     Rev 1.13   06 Sep 1994 21:01:34   SZIADEH
;  added code to support Following a jump instruction.
;
;     Rev 1.12   22 Aug 1994 18:38:06   DALLEE
;  Commented out SCN4_EXEC -- breaks .DX platform.
;  We're no longer using this update scheme (hopefully).
;
;     Rev 1.11   19 Aug 1994 22:10:26   RSTANEV
;  Fixed a double-define.
;
;     Rev 1.10   17 Aug 1994 13:26:20   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.9   20 Jul 1994 13:24:42   CAREY
;  Changed CALL FAR PTR XCS:[XBX] to CALL DWORD PTR XCS:[XBX] so it doesn't
;  break 32 bit.
;
;     Rev 1.8   19 Jul 1994 18:12:54   CAREY
;  Fixed 32 bit problem with handle argument for SCN4_QUEEG.
;
;     Rev 1.7   19 Jul 1994 16:24:08   CAREY
;  Changed si/di references to XSI/XDI to handle 32-bit correctly.
;
;     Rev 1.6   06 Jul 1994 11:47:42   CAREY
;  Fixed bug in DOS with jump tables for Alg scanner.
;
;     Rev 1.5   30 Jun 1994 17:06:32   CAREY
;  Added one more change which was in the new scn_alg which Jimmy/Sami had
;  worked on.  It has to do with the version check of the DF_EXEC file.
;
;     Rev 1.4   30 Jun 1994 16:41:46   CAREY
;  Added new QUEEG functions as well as encrypted search and verified 32 bit
;  O.K.
;
;     Rev 1.3   23 May 1994 09:17:08   RSTANEV
;  Fixed some warnings.
;
;     Rev 1.2   20 May 1994 15:59:10   RSTANEV
;  Fixed bugs with scanning files with size on a 64K limit.
;
;     Rev 1.1   20 May 1994 08:30:28   RSTANEV
;  Fixed small problems here and there.
;
;     Rev 1.0   02 May 1994 18:28:18   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------
include CTSN.INC
include REPAIRN.INC

option scoped

get_test                PROTO CODEDISTANCE,:LPSTR
wildcard                PROTO CODEDISTANCE,:UINT,:LPSTR,:LPSTR,:BYTE
compare_strings         PROTO CODEDISTANCE,:UINT,:LPSTR,:LPSTR
SCN4_TREMOR             PROTO CODEDISTANCE,:LPSTR,:UINT
SCN4_TREMOR_XOR         PROTO CODEDISTANCE,:LPSTR
SCN4_TREMOR_ADD         PROTO CODEDISTANCE,:LPSTR
SCN4_SATAN_BUG          PROTO CODEDISTANCE,:LPSTR,:UINT

IFDEF NAVOEM
SCN4_GIRAFE             PROTO CODEDISTANCE,:LPSTR
ENDIF

DECRYPT_XOR_DELTA       PROTO CODEDISTANCE,:LPSTR,:LPSTR
DECRYPT_ADD_DELTA       PROTO CODEDISTANCE,:LPSTR,:LPSTR
DECRYPT_ROTATE          PROTO CODEDISTANCE,:LPSTR,:LPSTR
DECRYPT_NEG             PROTO CODEDISTANCE,:LPSTR,:LPSTR
DECRYPT_NOT             PROTO CODEDISTANCE,:LPSTR,:LPSTR
SCN4_QUEEG              PROTO CODEDISTANCE,:LPSTR,:UINT

TSR_PATHOGEN_BUFFER_LOCATION EQU     (STARTSIZE*2)

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
SCN4_ALG PROC CODEDISTANCE USES XBX XCX XDX SDS SES XSI XDI,
        Handle:UINT,
        lpBuffer:LPSTR,
        uBuffer:UINT,
        lpData:LPSTR,
        dwStartOffset: LPSTR

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

IFDEF SYM_TSR
                                        ; Count = number of ALG sigs
        movuw   XCX,XES[XDI].HEADER.uNumALGs

        add     XDI,sHEADER             ; First ALG sig
ELSE

        movuw   XAX,XES[XDI].HEADER.uNumCRCs
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

        movuw   XCX,<WORD PTR XES[XDI]> ; CX gets # sigs in this blk
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
                                        ;go compare more bytes
        INVOKE  compare_strings,Handle,lpBuffer, dwStartOffset
        pop     XCX                     ;restore count
        jc      exit_infected
@@:
        add     XDI,SIZE_SIG            ;go to next sig
        loop	ALGLoop

IFDEF SYM_TSR
        xor     XAX, XAX                ;zero ax
        jmp     exit_all

exit_infected:

        LES_DI  lpData
                                        ;Get number of ALG sigs
        movuw   XAX, XES[XDI].HEADER.uNumALGs
        sub     XAX, XCX                 ;Calculate virus number
        inc     XAX                     ; 1-base virus id
ELSE
        mov     ax,wSigsInBlk
        add     wSigsFailed,ax          ;Add to count of sigs tried
        jmp     BlkLoop                 ;Try next block

exit_infected:
        ; DCHI -> modified below for new id calculation method

        movuw   XBX, wSigsInBlk         ; Calculate virus number based
        sub     XBX, XCX                ; on current sig id in this blk
        movuw   XAX, wSigsFailed        ; plus the number of sigs
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
compare_strings PROC CODEDISTANCE USES XAX XSI XDI XBX,
        Handle:UINT,
        Buffer:LPSTR,
        dwStartOffset: LPSTR

        LOCAL   byCurrentByte:BYTE

        xor     XCX, XCX
        mov     cl,byte ptr XES[XDI]    ; get length of signature
        dec     cl                      ; *CAREY -> decrypt

        or      cl,cl                   ; ensure count not 0
        jz      mismatch
        cmp     cl,1                    ; if the sig was only length 1,
        je      pre_allmatch            ;   we've already matched

        cmp     byte ptr XES[XDI+1],0E0h; Use E0 (undecoded DF)
        je      @F                      ; if first byte was DF, don't assume
                                        ;  first byte matched. 
        inc     XSI                     ; advance 1 byte in host file
@@:
        add     XDI, 2                  ; advance to second byte of sig
        mov     XDX, XDI                ;
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

        INVOKE  wildcard,Handle,Buffer,ADDR dwStartOffset,byCurrentByte

        jnc     mismatch                ;carry was mismatch
        jmp     SHORT wskip

normbyte:
        cmp     al,BYTE PTR [XSI]       ;test for match
        jne     mismatch
        inc     XSI

wskip:  
        inc     XDI
        cmp     XDI, XDX
        jb      comploop
pre_allmatch:
        stc
        jmp     SHORT allmatch          ;Hot beans! We've got one!

mismatch:
        clc                             ;No match here.

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

WildCard    PROC CODEDISTANCE USES XBX,
        Handle:UINT,
        Buffer:LPSTR,
        lpStartOffset:LPSTR,
        byByte:BYTE

        LOCAL   dwBase:DWORD


        push    XDX
                                        ; Buffer *is* XBX!  Remove saving and
                                        ; restoring of XBX other than the
                                        ; USES above.  --  CJK
        push    XBX                     ; save BX for later

        mov     al, byByte
        and     XAX, 000fh              ; use low nibble of AL for index
J_JUMP_AGAIN:
IF OS_16
        shl     XAX, 1                   ; multiply offset by 2 (word)
ELSE
        shl     XAX, 2                   ; multiply offset by 2 (dword)
ENDIF
        mov     XBX, XAX
        add     XBX, offset JUMP_TABLE  ; BX points to our jump table now
        
IF OS_16
        jmp     NEAR PTR XCS[XBX]       ; jump to the code
ELSEIFDEF SYM_NLM
        jmp     DWORD PTR cs:[XBX]
ELSE
        jmp     DWORD PTR XCS[XBX]
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

      mov     word ptr [dwBase],     00h
      mov     word ptr [dwBase + 2], 00h

      push   XSI
if OS_16
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

if OS_16
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

IF OS_16
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
        movuw   XDX,<word ptr [XSI+4]>  ; UB on length in 512-byte pages
        movuw   XAX,<word ptr [XSI+2]>  ; Length Mod 512

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
        movuw   XAX,<word ptr [XSI+1]>
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

        movuw   XAX,<word ptr [XSI+16h]> ; CS of header
        add     ax,[XSI+08h]            ; # paragraphs in header
        mov     XDX,10h
        mul     XDX
IF OS_16
        add     ax,[XSI+14h]            ; DX:AX = Location of code
        adc     dx,0
        and     dx,000Fh
ELSE
        movuw   XDX,<word ptr [XSI+14h]>
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
        movuw   XDX,<word ptr XES[XDI]> ; DX gets lower bound

        dec     dl                      ; Decrypt
        dec     dh                      ; Decrypt

        cmp     XAX,XDX                 ; Is size less than lower bound?
        jl      EXIT_FAIL               ; If so, then fail.

        inc     XDI
        inc     XDI
        movuw   XDX,<word ptr XES[XDI]> ; DX gets upper bound

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
        push    XAX
        INVOKE  FIL4_SIZE,Handle

        pop     XBX
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

IF OS_16
        and     ax, dx                  ; merge error results in 16-bit land
ENDIF
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
        add     XDI, 2                  ; point to first byte of sig

        mov     ah, XES[XDI]            ; AH = FIRST byte to search for
        inc     XDI                     ; point to next byte in signature
        dec     ah                      ; decrypt

        xor     XCX, XCX                ; clear CX out
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

        rpush   XSI,XDI                 ; save XSI & XDI for later
        mov     XDX, XAX
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

        add     sp, sizeXSI + sizeXDI   ; remove SI & DI from stack
        dec     XDI                     ; point to LAST byte of string

        jmp     EXIT_CATCH


STRING_MORE:
        mov     XAX, XDX
        rpop    XDI,XSI
        jmp     STRING_LOOP


DF_TREMOR_DETECT    label   near

        INVOKE  SCN4_TREMOR, vSDS XSI, Handle
        or      XAX,XAX
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_SATAN_BUG_DETECT label   near

        INVOKE  SCN4_SATAN_BUG, vSDS XSI, Handle
        or      XAX,XAX
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_QUEEG_DETECT label   near

        INVOKE  SCN4_QUEEG, Buffer, Handle
        or      XAX, XAX
        jz      EXIT_FAIL
        jmp     EXIT_CATCH

DF_GIRAFE_DETECT    label   near

IFDEF NAVOEM
        INVOKE  SCN4_GIRAFE, vSDS XSI
        or      XAX, XAX
        jz      EXIT_FAIL
        jmp     EXIT_CATCH
ELSE
        xor     XAX, XAX
        jmp     EXIT_FAIL
ENDIF

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
IF OS_32
        cwde
ENDIF
        add     XSI, XAX                ; add this to SI, get new location
        inc     XSI                     ;  add one to really get new location
        mov     XAX, UINT PTR Buffer    ; make sure we're still in our
        cmp     XSI, XAX                ;  buffer space (don't GP)
        jb      EXIT_FAIL
        add     XAX, 3 * STARTSIZE      ; we only use 3 buffers
        cmp     XSI, XAX
        ja      EXIT_FAIL
        jmp     EXIT_CATCH

DF_EXEC label near

        inc     XDI                     ; go to WORD containing ID #
        mov     AX, XES[XDI]            ; fetch it and decrypt
        dec     AH
        dec     AL

        LDS_SI  Buffer                  ; re-establish top of buffer
        add     XSI, 100h*3             ; point to 3rd 256 byte buffer
        inc     word ptr [XSI]
        movuw   XBX,<word ptr [XSI]>    ; obtain next free index
        cmp     XBX, 511
        jge     @F

        shl     XBX, 1                  ; multiply by two to get the memory
        mov     [XBX+XSI], AX           ; location and store our ID #.

        inc     XDI                     ; point to HIGH byte of ID
@@:
        jmp     EXIT_FAIL               ; fail the definition and go to the
                                        ; next one!

DF_USE_NEXT label near
        pop     XBX
        inc     XDI
        mov     al, byte ptr XES[XDI]   ; point to bytes to skip
        cbw
IF OS_32
        cwde
ENDIF
        add     XDI, XAX                ; Get us to length byte of next sig
        mov     al, byte ptr XES[XDI]   ; Get length of next sig
                                        ; no decrypt, the +1 encryption is
                                        ; what we want.
        cbw
IF OS_32
        cwde
ENDIF
        pop     XDX                     ; get it off stack
        mov     XDX,XDI
        add     XDX,XAX                 ; point to new end of sig
        add     XDI,3                   ; point to after fake VOID sig
        jmp     EXIT_CATCH_NO_POP       ; already popped everything

DF_ENCRYPTED_SEARCH label near
        inc     XDI                     ; Point to bytes to look for
                                        ; 0xDF, 0x10, byteControlFlags,
                                        ; byte(EveryNBytes)-1, 4 or 8 bytes of
                                        ; sig.
                                        ; ControlFlags: ABCDEFGH
                                        ;            A=Byte (0) or Word (1)
                                        ;            B=Xor/Delta
                                        ;            C=Add/Delta
                                        ;            D=Rotate
                                        ;            E=NEG
                                        ;            F=NOT

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
        mov     XCX, 16                 ; # of bytes for quick slide
                                        ; will be dec'd as soon as we get
                                        ; to QUICK_SLIDE

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

IF OS_16
        shr     XAX, 1                  ; restore XAX to previous value
ELSE
        shr     XAX, 2                  ; restore XAX to previous value
ENDIF

        add     XSI, XAX                ; advance pointer by AL bytes

EXIT_CATCH:
        pop     XBX
        pop     XDX

EXIT_CATCH_NO_POP:
        stc
EXIT_ALL:
        ret

WildCard	ENDP

;----------------------------------------------------------------------------
;  JUMP_TABLE definition
;----------------------------------------------------------------------------
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

;----------------------------------------------------------------------------
;
; TREMOR detection!
;
; This function returns UINT 0 if not found and 1 if found!
;
;----------------------------------------------------------------------------
SCN4_TREMOR  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        Handle:UINT

        LOCAL   Verify:DWORD


        LDS_SI  lpBuffer
        add     XSI, 2ah                ; bytes start at FirstLanding + 2ch
                                        ; start looking a bit earlier in case
                                        ; we skipped past a few nops, etc @
                                        ; the first landing


        INVOKE  SCN4_TREMOR_XOR, vSDS XSI
        or      XAX, XAX
        jnz     MAYBE_FOUND_TREMOR

        INVOKE  SCN4_TREMOR_ADD, vSDS XSI
        or      XAX, XAX
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
        mov     XAX,1
        jmp     SHORT FOUND_TREMOR

NO_TREMOR:
        xor     XAX,XAX

FOUND_TREMOR:

        ret

SCN4_TREMOR  ENDP

;----------------------------------------------------------------------------

SCN4_TREMOR_XOR  PROC  CODEDISTANCE  USES SDS XSI,
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

        xor     XAX,XAX                     ; not found!
        jmp     EXIT_XOR_TREM


XOR_FOUND_TREM:
        mov     XAX,1

EXIT_XOR_TREM:

        ret

SCN4_TREMOR_XOR     ENDP

;----------------------------------------------------------------------------

SCN4_TREMOR_ADD  PROC  CODEDISTANCE,
        lpBuffer:LPSTR

        LDS_SI  lpBuffer
        mov     XCX,03h

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

        mov     XDI, XAX
        add     XDI, XBX

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

        xor     XAX,XAX                    ; not found!
        jmp     EXIT_ADD_TREM


ADD_FOUND_TREM:
        mov     XAX,1

EXIT_ADD_TREM:

        ret

SCN4_TREMOR_ADD     ENDP

;----------------------------------------------------------------------------
;
; Satan bug detection
;
;----------------------------------------------------------------------------

IFDEF SYM_NLM
UniqueSegmentEnd
UniqueDataSegment
ENDIF
SATAN_BYTES     label byte
                db  026h, 02eh, 03eh, 042h, 043h, 045h, 047h, 04ah, 04bh
                db  04dh, 090h, 0f5h, 0f8h, 0f9h, 0fbh, 0fch
NUM_SATAN_BYTES EQU     ($-SATAN_BYTES)


; 0 acts as a terminator for prefix bytes

PREFIX_BYTES    label byte
                db  026h, 02eh, 03eh, 042h, 043h, 045h, 047h, 04ah, 04bh
                db  04dh, 090h, 0f5h, 0f8h, 0f9h, 0fbh, 0fch, 0
IFDEF SYM_NLM
UniqueDataSegmentEnd
UniqueSegment
ENDIF

SCN4_SATAN_BUG  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        HostHandle:UINT

        LOCAL   wBadCount : WORD
        LOCAL   wFirstTwo : WORD

        mov     wBadCount, 0            ; initialize count of bad prefixes

        lea     XSI, PREFIX_BYTES

        smov    ds,cs

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

        rpush   SES,XDI                 ; save for later

        smov    es,cs
        lea     XDI, SATAN_BYTES        ; point to our bytes
        mov     XCX, NUM_SATAN_BYTES
        repnz   scasb                   ; found it?
        jcxz    VALID_PREFIX

        inc     wBadCount               ; got a bad one!

        add     sp, sizeXDI + sizeSES   ; remove ES and DI from stack

        jmp     PREFIX_LOOP             ; found one, try another prefix

VALID_PREFIX:

        rpop    XDI,SES

        mov     ax, bx                  ; restore contents of AL
        mov     cx, dx                  ; restore count in buffer

        jmp     KEEP_SCANNING

TEST_SATAN_COUNT:

        cmp     wBadCount, 14           ; one of each type of prefix violation
        jl      NO_SATAN_BUG

        LDS_SI  lpBuffer                ; one more verification
        mov     XBX, 256                ; scan first 256 bytes for invalid codes
        xor     XDX, XDX                ; init count of satan-bug-byte count

        smov    es,cs

COUNT_SATAN_BYTE_LOOP:

        lodsb                           ; obtain byte from buffer
        lea     XDI,SATAN_BYTES         ; point ES:DI to our satan bytes
        mov     XCX,NUM_SATAN_BYTES
        repnz   scasb                   ; scan for this byte in our list
        jcxz    END_SATAN_BYTE_LOOP

        inc     XDX                     ; one more satan-bug byte

END_SATAN_BYTE_LOOP:

        dec     XBX
        jnz     COUNT_SATAN_BYTE_LOOP   ; keep checking...

        cmp     XDX, 50                 ; > 50/256 bytes -> satan-bug
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
        mov     XAX,1
        jmp     FOUND_SATAN_BUG

NO_SATAN_BUG:

        xor     XAX,XAX

FOUND_SATAN_BUG:

        ret

SCN4_SATAN_BUG      ENDP

;----------------------------------------------------------------------------
;
; Girafe (Encrypted using TPE 1.X)
;
;----------------------------------------------------------------------------

; these signatures are encrypted by +1 encryption!  (both are taken from EVEN
; boundaries of the virus decryption)

IFDEF NAVOEM

IFDEF SYM_NLM
UniqueSegmentEnd
UniqueDataSegment
ENDIF
GIRAFE_BYTES    label   byte
        db      0e9h, 001h, 001h, 05fh, 084h, 0efh, 004h, 0c0h, 001h, 002h
IFDEF SYM_NLM
UniqueDataSegmentEnd
UniqueSegment
ENDIF

SCN4_GIRAFE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LES_DI  lpBuffer                    ; point to first byte in virus buffer

        mov     XCX, 200                    ; scan up to 200 bytes for POP AX
        cld                                 ; reset direction flag
        add     XDI,3                       ; so that XDI-3 (later) won't GP.

GIRAFE_POP_SCAN:

        mov     al, 58h                 ; now we want to scan for a POP AX
        repnz   scasb                   ; go looking
        jcxz    NO_GIRAFE               ; if we have hit the end of the buffer

                                        ; when we find a POP AX (if it is the correct one) it will look like:
                                        ; E2/E0/75/7F XX        LOOP/JMP BACK
                                        ; 58                    POP AX

        mov     ah, XES[XDI-3]          ; find the loop byte

        cmp     ah, 0e2h                ; found a loop byte, continue verifying
        jz      VERIFY_GIRAFE
        cmp     ah, 0e0h                ; found the loop byte
        jz      VERIFY_GIRAFE
        cmp     ah, 075h                ; ditto
        jz      VERIFY_GIRAFE
        cmp     ah, 07fh                ; ditto
        jnz     GIRAFE_POP_SCAN         ; keep looking for more POP AX's

        ; fall thru on 7f & others to VERIFY_GIRAFE...

VERIFY_GIRAFE:

        test    XES[XDI-2], BYTE PTR 80h; is this a JNZ/LOOP *BACK*???
        jz      GIRAFE_POP_SCAN         ; NO!  Keep looking!

        smov    ds,cs

        lea     XSI, GIRAFE_BYTES       ; point to our 1st signature!

        INVOKE  DETERMINE_XOR_BYTE,vSES XDI,vSDS XSI
        jnc     GIRAFE_FOUND

        INVOKE  DETERMINE_ADD_BYTE,vSES XDI,vSDS XSI
        jnc     GIRAFE_FOUND

        INVOKE  DETERMINE_XOR_WORD,vSES XDI,vSDS XSI
        jnc     GIRAFE_FOUND

        INVOKE  DETERMINE_ADD_WORD,vSES XDI,vSDS XSI
        jnc     GIRAFE_FOUND

        jmp     GIRAFE_POP_SCAN

NO_GIRAFE:

        xor     XAX, XAX
        jmp     GIRAFE_RETURN

GIRAFE_FOUND:

        mov     XAX,1

GIRAFE_RETURN:

        ret

SCN4_GIRAFE     ENDP

ENDIF

;---------------------------------------------------------------
;---------------------------------------------------------------
SCN4_QUEEG  PROC CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        HostHandle:UINT

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; The following code has been commented out and is maintained as a reference
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ifdef 0
;jmp QUEEG_RET
;
;; Calculate location we want to get buffer...
;; 2048 is the maximum, minus bytes left in search for E9, minus bytes left
;; in search for JNZ, plus E9 offset, + 3 (E9 offset adjustment).
;; Add it to FirstLanding.
;        mov     XAX, 2048+3
;        sub     XAX, XCX
;        pop     XCX
;        sub     XAX, XCX
;;       add     AX, XES[XDI]
;        add     AX, [XDI]
;        pop     XDI
;        add     word ptr FirstLanding,AX
;        adc     word ptr FirstLanding+2,0
;        INVOKE  FIL4_SEEK,HostHandle,FirstLanding,fSEEK_BEG
;        LES_DI  lpBuffer
;        add     XDI,STARTSIZE*2
;        INVOKE  FIL4_READ,HostHandle,vSES XDI,SMEG_READSIZE
;
;; Prepare for decryption stuff...
;
;DECRYPT_QUEEG:
;        lodsb
;        mov AH,AL
;        mov BX,AX
;        mov dl,[XSI+0FFh]
;
;        xor al,dl
;        mov [XSI-1],al
;
;        sub ah,dl
;        mov [XSI+SMEG_READSIZE*2-1],ah
;
;        neg bl
;        neg dl
;        xor bl,dl
;        mov [XSI+SMEG_READSIZE-1],bl
;
;        dec bh
;        xor bh,[XSI]
;        mov [XSI+SMEG_READSIZE*3-1],bh
;
;        loop DECRYPT_QUEEG
;endif
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; End of commented section
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Encrypted search code
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DECRYPT_XOR_DELTA PROC CODEDISTANCE USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
                                        ; Since EveryNBytes was put here as -1,
                                        ; no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw

IF OS_32
        cwde
ENDIF
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
        push    XBX
        xor     BX, [XSI]
        inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+4], BX          ; This should be the same.
        pop     XBX
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

DECRYPT_ADD_DELTA PROC CODEDISTANCE USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw

IF OS_32
        cwde
ENDIF
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
        push    XBX
        add     BX, [XSI]
        ;inc     BL                      ; Make it match +1 encryption.
        inc     BH
        cmp     XES[XDI+4], BX          ; This should be the same.
        pop     XBX
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

DECRYPT_ROTATE PROC CODEDISTANCE USES XBX XCX XDX SDS SES XSI XDI,
                Buf:LPSTR,              ; Buffer to look in
                Sig:LPSTR               ; Signature to look for
; byteControlFlags, byte(EveryNBytes)-1, 4 or 8 bytes of sig
; ControlFlags: ABCDEFGH
;               A=Byte (0) or Word (1)
;               B=Xor/Delta
;               C=Add/Delta
;               D=Rotate

        LDS_SI  Buf
        LES_DI  Sig
        mov     DX, XES[XDI]            ; DL<-Flags, DH<-EveryNBytes-1
        dec     DL
        ; Since EveryNBytes was put here as -1, no need for dec DH
        mov     AL, DH                  ; AX will have "EveryNBytes"
        cbw

IF    OS_32
        cwde
ENDIF

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
if OS_32
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
if OS_32
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
get_test proc CODEDISTANCE uses XSI SDS,
     lpStartOffset: LPSTR
                           
      LDS_SI  lpStartOffset

IF OS_32
      mov     [XSI], eax
ELSE
      mov     word ptr [XSI], ax
      mov     word ptr [XSI+2], dx
ENDIF

      ret

get_test endp

UniqueSegmentEnd

END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; this is here for future reference
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ifdef 0
;PATHOGEN_REG_PATTERN label byte
;    db 053h, 04Dh, 045h, 047h, 020h, 076h, 030h, 02Eh, 031h, 0E8h, 09Ch
;
;PATHOGEN_XOR_PATTERN label byte
;    db 01Eh, 008h, 002h, 067h, 056h, 046h, 01Eh, 01Fh, 0D9h, 074h, 09Fh
;
;PATHOGEN_ADD_PATTERN label byte
;    db 006h, 008h, 0FEh, 027h, 0AAh, 046h, 002h, 0FDh, 049h, 04Ch, 099h
;
;PATHOGEN_XORALT_PATTERN label byte
;    db 05Ch, 0E7h, 086h, 0C4h, 01Eh, 0BCh, 03Fh, 02Eh, 044h, 006h, 074h
;
;SCN4_PATHOGEN      PROC USES XBX XCX XDX SDS SES XSI XDI,
;        lpBuffer:LPSTR,
;        HostHandle:WORD
;
;SMEG_READSIZE equ 512
;
;    INVOKE  FIL4_SEEK, HostHandle, 680h, fSEEK_END
;    INVOKE  FIL4_READ, HostHandle, lpBuffer, SMEG_READSIZE
;
;    mov XCX,SMEG_READSIZE/2-1
;    LDS_SI  lpBuffer                ;point to string
;
;DECRYPT_PATHOGEN:
;    lodsb
;    mov AH,AL
;ifdef SDOS16
;    xor al,[XSI]
;    mov [XSI-1],al
;
;    xor ah,[XSI+0FFh]
;    mov [XSI+SMEG_READSIZE/2-1],ah
;else
;    mov BX,AX
;
;    xor al,[XSI]
;    mov [XSI-1],al
;
;    sub bl,[XSI]
;    mov [XSI+SMEG_READSIZE-1],bl
;
;    xor ah,[XSI+0FFh]
;    mov [XSI+SMEG_READSIZE*2-1],ah
;
;    mov al,[XSI]
;    neg al
;    neg bh
;    xor al,bh
;    mov [XSI+SMEG_READSIZE*3-1],al
;endif
;
;    loop DECRYPT_PATHOGEN
;
;IF OS_16
;    SMOV    SDS, SCS                    ; DS = CS
;ENDIF
;
;; SMEG_SEARCH returns 0 if not found, 1 if found
;;                     carry           no carry
;
;    lea     XSI, PATHOGEN_XOR_PATTERN
;    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
;    jc      PATH_RET
;
;    lea     XSI, PATHOGEN_REG_PATTERN
;    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
;    jc      PATH_RET
;
;ifdef SDOS16
;    LES_DI  lpBuffer
;    add     XDI,SMEG_READSIZE/2
;    lea     XSI, PATHOGEN_XORALT_PATTERN
;    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
;    jc      PATH_RET
;
;    INVOKE  FIL4_SEEK, HostHandle, 680h, fSEEK_END
;    INVOKE  FIL4_READ, HostHandle, lpBuffer, SMEG_READSIZE/2
;
;    mov XCX,SMEG_READSIZE/2-1
;    LDS_SI  lpBuffer                ;point to string
;
;DECRYPT_PATHOGEN_AGAIN:
;    lodsb
;    mov AH,AL
;    sub al,[XSI]
;    mov [XSI-1],al
;
;    mov al,[XSI]
;    neg al
;    neg ah
;    xor al,ah
;    mov [XSI+SMEG_READSIZE/2-1],al
;
;    loop DECRYPT_PATHOGEN_AGAIN
;
;    push cs
;    pop  ds
;
;    lea     XSI, PATHOGEN_ADD_PATTERN
;    INVOKE  SMEG_SEARCH,lpBuffer,vSDS XSI
;    jc      PATH_RET
;
;    LES_DI  lpBuffer
;    add     XDI,SMEG_READSIZE/2
;    lea     XSI, PATHOGEN_XOR_PATTERN
;    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
;
;else
;    LES_DI  lpBuffer
;    add     XDI,SMEG_READSIZE*3
;    lea     XSI, PATHOGEN_XOR_PATTERN   same as above
;    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
;    jc      PATH_RET
;
;    LES_DI  lpBuffer
;    add     XDI,SMEG_READSIZE
;    lea     XSI, PATHOGEN_ADD_PATTERN
;    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
;    jc      PATH_RET
;
;;   LES_DI  lpBuffer
;;   add     XDI,SMEG_READSIZE*2
;;      becomes
;    add     XDI,SMEG_READSIZE
;
;    lea     XSI, PATHOGEN_XORALT_PATTERN
;    INVOKE  SMEG_SEARCH,vSES XDI,vSDS XSI
;endif
;
;PATH_RET:
;    ret
;
;SCN4_PATHOGEN      ENDP
;
;
;SMEG_SEARCH      PROC USES XBX XCX XDX SDS SES XSI XDI,
;        lpBuffer:LPSTR,
;        Pattern:LPSTR
;
;    LES_DI  lpBuffer
;    LDS_SI  Pattern
;     lodsb      ; AL holds first byte of pattern.
;     mov XCX,SMEG_READSIZE/2-1
;Smeg_continue_search:
;     repnz scasb
;     jcxz Smeg_search_nf
;     push XDI
;     push XCX
;     mov XCX,10
;     push XSI
;     repz cmpsb
;     pop XSI
;     pop XCX
;     pop XDI
;     jnz Smeg_continue_search
;     stc        ;Found!
;     mov     XAX,1
;     jmp Smeg_return
;Smeg_search_nf:
;     XOR     XAX, XAX     ; AX = 0 is NOT_FOUND
;;     clc                 ; XOR clears carry
;Smeg_return:
;     ret
;SMEG_SEARCH      ENDP
;endif
;
;ifdef 0
;QUEEG_REG_PATTERN label byte
;; SMEG v0.2SV
;    db 053h, 04Dh, 045h, 047h, 020h, 076h, 030h, 02Eh, 032h, 053h, 056h
;
;QUEEG_ADD_PATTERN label byte
;    db 04Dh, 065h, 00Dh, 044h, 035h, 090h, 0A5h, 067h, 005h, 04Ch, 045h
;
;QUEEG_XORALT_PATTERN label byte
;    db 057h, 0ABh, 073h, 044h, 0F5h, 090h, 0A5h, 0EBh, 01Dh, 054h, 045h
;
;;xor     55 A5 7D 44 CB 90 BB E9 1F 54 47
;;negxor  57 AB 73 44 F5 90 A5 EB 1D 54 45
;;sub     4D 65 0D 44 35 90 A5 67 05 4C 45
;;?decxor  1F 09 03 66 69 45 01 1F 62 04 EE
;endif

