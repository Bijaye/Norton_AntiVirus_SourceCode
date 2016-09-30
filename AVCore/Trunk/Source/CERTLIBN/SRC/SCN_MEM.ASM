; Copyright 1993 Symantec Corporation                                  
;****************************************************************************
; 
; $Header:   S:/CERTLIBN/VCS/scn_mem.asv   1.29   10 Oct 1996 18:23:40   DALLEE  $  
; 
; Description: Memory scan routines
; 
; Contains: 
; 
; See Also: 
; 
;****************************************************************************
; $Log:   S:/CERTLIBN/VCS/scn_mem.asv  $  
;  
;     Rev 1.29   10 Oct 1996 18:23:40   DALLEE
;  Fixed problem with mem scan exiting after 1st sig if no progress callback.
;  Also cleaned up the code that checks for the progress callback.
;  
;     Rev 1.28   27 Jun 1996 23:49:02   TCASHIN
;  We broken the stack in ScanMemoryBuffer if the progress callback 
;  returned 0.
;  
;     Rev 1.27   17 Jun 1996 10:03:16   MZAREMB
;  Now skips the callback processing if lpgCallBack is NULL.
;  
;     Rev 1.26   14 Jun 1996 16:02:34   TCASHIN
;  Check for progress callback in 16-bit wasn't right.
;  
;  
;     Rev 1.25   10 Jun 1996 09:18:14   TCASHIN
;  Memory progress callback didn't work right in 16-bit.
;  
;     Rev 1.24   02 May 1996 10:17:26   TCASHIN
;  Check the results of the progress callback.
;  
;     Rev 1.23   17 Apr 1996 14:08:48   TCASHIN
;  Fix a bug in progress callback stuff for 16-bit.
;  
;     Rev 1.22   16 Apr 1996 10:34:22   TCASHIN
;  Removed NAVOEM ifdefs and fixed null progress callback test.
;  
;     Rev 1.21   09 Apr 1996 19:18:26   TCASHIN
;  Added progress callback to ScanMemoryBuffer. Is in effect only
;  when NAVOEM is defined.
;  
;     Rev 1.20   10 Nov 1995 16:10:14   GDZIECI
;  For NLM: relinquish control in ScanMemoryCD() and ScanMemoryBuffer().
;  
;     Rev 1.19   25 Jan 1995 16:16:42   BRAD
;  Exit, if buffer size is 0
;  
;     Rev 1.18   24 Jan 1995 17:11:20   BRAD
;  Made changes so it can scan memory in 1 giant blast
;  
;     Rev 1.17   28 Dec 1994 16:48:56   SZIADEH
;  move "skip sig header" inside the loop in ScanCD
;  
;     Rev 1.16   28 Dec 1994 16:01:20   SZIADEH
;  replace SIZE_MEM with uSigLength
;  
;     Rev 1.15   22 Dec 1994 03:16:38   JMILLARD
;  
;  add uSigHeaderSize to ScanMemoryCD
;  use uSigHeaderSize consistently
;  
;     Rev 1.14   21 Dec 1994 23:20:48   SZIADEH
;  fix bugs
;  convert to use UINT
;  make all code float between 16 and 32 correctly
;  this is an interim check-in, one more will follow
;  
;     Rev 1.13   09 Dec 1994 20:30:12   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.16   31 Mar 1994 14:07:36   BRAD
;  Use new SYM_platform defines
;  
;     Rev 1.15   21 Mar 1994 01:03:22   DALLEE
;  Changed prototypes of LOOK413 and LOOKALL back to PROC NEAR.
;  These are only called from a table of near pointers.
;  
;     Rev 1.14   11 Feb 1994 15:20:32   MARKL
;  Qak 2 changes moved to trunk
;  
;     Rev 1.13.1.7   02 Feb 1994 22:48:30   BRAD
;  NLM code wasn't being IFDEFed.
;  
;     Rev 1.13.1.6   25 Jan 1994 09:42:40   JMILLARD
;  break up memory scans into blocks to meet time slice requirements
;  
;     Rev 1.13.1.5   10 Jan 1994 00:41:02   JMILLARD
;  add some relinquish control to slow memory scan
;  
;     Rev 1.13.1.4   13 Dec 1993 10:08:18   JMILLARD
;  install CheckSpecial code for 32-bit mode
;  fix 32-bit problems in CheckSpecial subfunctions
;  add CountValidPages and FindFirstValidPage for 32-bit mode
;  
;     Rev 1.13.1.3   30 Nov 1993 18:21:40   DALLEE
;  Bug in previous changes.
;  
;     Rev 1.13.1.2   18 Nov 1993 09:35:14   CJKUO
;  Don't look at A000:0 (when checking last segment, quit LEN bytes before
;  end of sengment).
;  
;     Rev 1.13.1.1   22 Oct 1993 15:02:04   JMILLARD
;  relinquish control more frequently for NLM
;  
;     Rev 1.13.1.0   19 Oct 1993 07:21:40   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.13   12 Oct 1993 16:32:16   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.12.2.1   08 Oct 1993 15:47:56   JMILLARD
;  fix problem in CheckPageValid with returning a WORD when the caller
;  may expect a DWORD
;  
;     Rev 1.12.2.0   03 Sep 1993 19:28:24   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.12   01 Aug 1993 16:53:34   JMILLARD
;  fix problem with wrong flag to check paging enable
;  
;     Rev 1.11   28 Jul 1993 14:50:22   JMILLARD
;  fix some assemble errors from the 16-bit guys
;****************************************************************************

include CTSN.INC
include AVAPI.INC

UniqueDataSegment

EXTERN          C lpgCallBack:DWORD

UniqueDataSegmentEnd

UniqueSegment

;-----------------------------------------------------------------------
; general functions for scanning buffers under any platform
;
;-----------------------------------------------------------------------
;WORD ScanMemoryBuffer  ( LPSTR, WORD, LPSTR, WORD , WORD , WORD );
; @parameters:
;      lpScanBuffer:  Pointer to the buffer to scan
;      uBufferSize :  Number of bytes to scan
;      lpMemSig    :  Memory Signatures
;      wSigCount   :  Number of Memory Signatures
;      bySkip      :  Number of Bytes to SKIP at the start of each
;                     signatures.
;
; @return:
;      AX = 0  No virus found
; otherwise,
;      AX = Virus Id
;===========================================================================
ScanMemoryBuffer  PROC USES SES SDS XSI XDI XDX XCX XBX,
         lpScanBuffer:LPBYTE,
         uBufferSize:UINT,
         lpMemSig:LPBYTE,
         uSigCount:UINT,
         uSigLength:UINT,
         uSigHeader:UINT

    ;--------------------------------

    LOCAL   uTotalSigs:UINT            ; Progress callback expects a DWORD

    mov     XAX, uSigCount             ; Save the total number of sigs
    mov     uTotalSigs, XAX            ; make sure there are some sigs

    cmp     XAX, 0
    jz      look_no_virus    

    cld
    mov     XCX, uBufferSize    
    cmp     XCX, 0                      ; If no memory to scan, skip
    jnz     begin_search 
    jmp     look_no_virus               ; exit out

begin_search:
    mov     XAX, uSigLength             ; scan up to
    sub     XCX, XAX
    inc     XCX                         ; - 1
    mov     uBufferSize, XCX            ;

    LDS_SI  lpMemSig

start_sig:
    add     XSI, uSigHeader             ; skip VID and FF xx bytes
    LES_DI  lpScanBuffer                ; reset start of buffer
    mov     XCX, uBufferSize            ; set counter to buffer size
                                        ; di points to current position in
                                        ;    buffer
                                        ; si points to start of sig
                                        ; cx remaining bytes to scan
search_buffer:
    mov     al, byte ptr [XSI]          ; get byte from current sig
    dec     al                          ; decrypt
    repne   scasb                       ; search for this byte in scan buffer
    JXCXZ   get_next_sig                ; no match, get next sig
    mov     XDX, XDI                    ; save current position in buffer
    mov     XBX, XCX                    ; remaining bytes to scan

    push    XSI                         ; save start of signature
    inc     XSI                         ; skip the byte already matched

    mov     XCX, uSigLength             ; signature length
    dec     XCX                         ; account for the byte already matched

look_nextbyte:
    lodsb                               ; get next byte from signature
    dec     al                          ; decode
    scasb                               ; scan memory
    loope   look_nextbyte               ; compare the rest of the signature

    pop     XSI                         ; restore start of sig

    je      look_virus_found            ; signature found!!!

    mov     XDI, XDX                    ; restore current location is search buff
    mov     XCX, XBX                    ; restore counter

    jmp     search_buffer               ;

get_next_sig:
IFDEF SYM_NLM
    pushad
    INVOKE  RelinquishControl           ; relinquish server
    popad
ENDIF


IF OS_16
    push    DS                          ; Save sig pointer
ENDIF
    push    XSI

IF OS_16                                ; See if we have a callback pointer
    MOV     SI, SEG lpgCallBack
    MOV     DS, SI
ENDIF
    LDS_SI  <DWORD PTR lpgCallBack>
    or      XSI,XSI
    jnz     CheckForProgress
IF OS_16
    mov     AX, DS
    or      AX,AX
    jnz     CheckForProgress
ENDIF
    mov     XAX, 1                      ; Set XAX as if callback said continue
    jmp     next_sig


CheckForProgress:                       ; See if we have a progress callback
    mov     XAX, UINT PTR [XSI].CALLBACKREV1.Progress
    or      XAX, XAX
    jnz     do_progress
IF OS_16
    mov     ax, word ptr [XSI].CALLBACKREV1.Progress+2
    or      ax, ax
    jnz     do_progress
ENDIF
    mov     XAX, 1                      ; Set XAX as if callback said continue
    jmp     next_sig


do_progress:
    mov     XAX, uTotalSigs
    sub     XAX, uSigCount

    INVOKE  [XSI].CALLBACKREV1.Progress,AX,uTotalSigs

next_sig:
    pop     XSI                         ; Restore sig pointer
IF OS_16
    pop     DS
ENDIF

    or      XAX,XAX                     ; If the callback returned zero, return
    jz      look_no_virus               ; now with no virus found.

    dec     uSigCount                   ; one less signature
    jz      look_no_virus               ; jump if zero sigs remaining

    add     XSI, uSigLength             ; skip rest of bytes
    jmp     start_sig

look_virus_found:
    sub     XSI, uSigHeader             ; start of memory record

    XOR     XAX,XAX     
    mov     ax, word ptr [XSI]          ; get virus id

    jmp     look_done

look_no_virus:
    mov     XAX, 0

look_done:
    ret                                 ; return to caller

ScanMemoryBuffer ENDP

;========================================================================
;========================================================================

ScanMemoryCD  PROC USES SES SDS XSI XDI XDX XCX XBX,
         lpScanBuffer:LPBYTE,
         uBufferSize:UINT,
         lpMemSigs:LPBYTE,
         uSigCount:UINT,
         uSigLength:UINT,
         uSigHeader:UINT

    ;=============================

;   int     03h

    mov     XAX, uSigCount              ; make sure there are some sigs
    cmp     XAX, 0
    jz      scancd_no_virus    

    LES_DI  lpScanBuffer
    mov     XCX, uBufferSize
    JXCXZ   scancd_no_virus             ; If no memory to scan, skip

    mov     XAX, uSigLength       ; scan up to
    sub     XCX, XAX              ; uBufferSize - Length of memsig
    inc     XCX                   ; - 1

    cld

scan_look_for_cd:                 ;
    mov    al, 0cdh               ;
    repne  scasb                  ;

    JXCXZ  scancd_no_virus         ;
                                  ;
    mov    XBX, XCX               ; save bytes remaining in scan buffer

    LDS_SI lpMemSigs              ; sigs buffer
    mov    XCX, uSigCount         ; number of sigs

scan_sig:
    add    XSI,uSigHeader         ; skip virus id
    mov    al, byte ptr [XSI]     ; get first byte in sig current sig
    dec    al                     ; decrypt byte
    cmp    al, byte ptr es:[XDI]  ;
    jne    scan_next_sig

    mov    XDX, XDI               ; save pointer to memory under scan

    push   XSI                    ; save start of current sig
    push   XCX                    ; save sigs counter

    mov    XCX, uSigLength
scan_rest_of_sig:
    lodsb
    dec    al
    scasb
    loope  scan_rest_of_sig

    pop    XCX
    pop    XSI
    mov    XDI, XDX

    jne    scan_next_sig

    sub    XSI, uSigHeader        ; skip back to virus id

    xor    XAX, XAX
    mov    ax, word ptr [XSI]     ; and get it

    jmp    scancd_virus_found     ; leave

scan_next_sig:
IFDEF SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
ENDIF
    add    XSI, uSigLength        ; skip to next sig

    dec    XCX                    ; Replace LOOP.  Much faster
    jnz    scan_sig

    mov    XCX, XBX

    jmp    scan_look_for_cd


scancd_no_virus:
    mov    XAX, 0
scancd_virus_found:
    ret
ScanMemoryCD ENDP


;========================================================================

UniqueSegmentEnd
END



;========================================================================
;========================================================================
;========================================================================

;
;       local prototypes
;

if OS_32

CheckMemoryBlock PROTO NEAR,lpData:LPSTR, lpStartAddress:LPSTR, dwLen:DWORD
CheckPageValid   PROTO NEAR,lpPage:LPSTR
CountValidPages  PROTO NEAR,lpPage:LPSTR, dwMaxBytes:DWORD
FindFirstValidPage PROTO NEAR,lpPage:LPSTR, dwMaxBytes:DWORD

PAGE_SIZE       EQU     01000h          ; memory management page size
SCAN_BLOCK_SIZE EQU     04000h          ; arbitrary size for server friendly
                                        ; scanning

;       page table equ's

PAGING_ENABLE   EQU     080000000h      ; PG bit in cr0
PAGE_MASK       EQU     03ffh           ; 10-bit mask for PAGE and DIR fields

PF_FLAG_SIZE    EQU     12
PF_FLAG_MASK    EQU     0fffff000h

PF_FLAG_PRESENT EQU     000000001h      ; page present flag

PAGE_OFFSET_SIZE EQU    12
PAGE_DIR_SIZE   EQU     10
PAGE_PAGE_SIZE  EQU     10

endif


;-----------------------------------------------------------------------------
; SCN4_MEMORY                          11/3/92                         JOE
;
;       Scan memory for known viruses.
;
; UINT WINAPI SCN4_MEMORY(LPSTR Data, BYTE byScanHighMemory);
;
; Input:
;       1)  Data to scan with.
; Output:
;       1)  No Virus = 0 or Virus ID=?
; Notes: 	
;       Sigs are stored in reverse order (15 bytes long).
;       Virus number preceeds Sig (1 word long).
;       Calling module will need to look up virus in N30.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
; UPDATE                               12/1/92				CAG
; UPDATE (WIN_SEGTOSEL)		       12/10/92         	        JOE
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;       32 bit, Clean
;
;-----------------------------------------------------------------------------
SCN4_MEMORY PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpData:LPSTR,
        byScanHighMemory:BYTE

if OS_16
	LOCAL SegES:WORD
        LOCAL wSegToStopOn:WORD

        ;--------------------------------------------------
        ; See if we should scan upto 640K or 1 Meg.
        ;--------------------------------------------------
        cmp     byScanHighMemory, 0     ; Scan High Memory?
        je      dontScanHigh            ; No.  Just scan upto 640K (A000h)

	xor 	ax,ax                   ; Scan upto 1M.  Test for rap
        jmp     short startScan

dontScanHigh:
        mov     ax,0A000h               ; Only scan upto 640K

        ;--------------------------------------------------
        ; Memory Scan Loop
        ;--------------------------------------------------
startScan:
        mov     wSegToStopOn,ax         ; Save either 640K or 1 Meg
        xor     ax,ax                   ; Start at segment 0
	mov	SegES,ax
	xor	bx,bx

MemLoopa:
ifdef WIN16
	INVOKE	MEM4_SEGTOSEL,ax
endif
      	mov	es,ax
        mov     cx,8000h                ; End half way into segment

        LDS_SI  lpData
        xor     XBX,XBX
        mov     bx,[XSI].HEADER.uNumALGs; Number of sigs to test

        add     XSI,sHEADER             ; Location of first sig
        xor     xdi,xdi                 ; Must initialize di to 0
                                        ; for each segement.
	cld
SegLoopa:
	mov	al,0CDh                 ; Looking for INT
	repne	scasb                   ; Fly til finding
	JXCXZ	NextSega                ; cx 0 means segment done

	call	CheckSigs               ; Otherwise we have a CD
	jc	SM_Exita                ; We have a match or
	jmp	SegLoopa                ; Look for next CD

NextSega:
	mov	ax,SegES                ; Retrieve current seg
        add     ax,800h                 ; add 800h  (32k)
	mov	SegES,ax                ; Save

        cmp     wSegToStopOn,ax         ; Are we done searching?
        jne     MemLoopa                ; No.  Search next block

; ****  xor     ax,ax                   ; Done scanning. No virus found

;
;       CheckSpecial will return ax = 0 if no viruses, otherwise virus number
;
        LDS_SI  lpData                  ; SDS:XSI = dat file
        mov     XCX,[XSI].HEADER.uNumALGs; XCX = number of signatures in table
        add     XSI,sHEADER             ; SDS:XSI = offset of first signature
        mov     di,wSegToStopOn         ; di = segment to stop real mode search
        call    CheckSpecial            ; check for special viruses
SM_Exita:

endif

if OS_32
        LOCAL dwCountSigChecks:DWORD
        LOCAL dwEndAddress:DWORD
        LOCAL dwFirstPage:DWORD
        LOCAL dwLastPage:DWORD

        mov     XAX, 1
	mov	dwCountSigChecks,XAX    ; used to relinquish control periodically

        ;--------------------------------------------------
        ; See if we should scan upto 640K or 1 Meg.
        ;--------------------------------------------------
        cmp     byScanHighMemory, 0     ; Scan High Memory?
        je      dontScanHigh            ; No.  Just scan upto 640K (A000h)

        mov     XCX,0100000h            ; end of 1-MB
        jmp     short startScan

dontScanHigh:
        mov     XCX,00a0000h            ; end of 640 k

        ;--------------------------------------------------
        ; Memory Scan Loop
        ;--------------------------------------------------
startScan:
        mov     dwEndAddress, XCX

        xor     XDI,XDI                 ; current start page
        mov     dwLastPage, XDI
        
NextPageSet:
        xor     XAX,XAX                 ; clear page valid
        mov     XDI,dwLastPage          ; and find the next set of pages

        ; locate next start page loop

FirstPageLoop:
        cmp     XDI,XCX                 ; at stop scan address?
        jae     NoValidPage             ; yes, all done, nothing found

        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jnz     @F                      ; yes

        add     XDI,PAGE_SIZE           ; no, skip to next page
        jmp     FirstPageLoop

        ; locate start page complete

@@:
        mov     dwFirstPage,XDI

        add     XDI,PAGE_SIZE           ; set up end page
        
EndPageLoop:

        cmp     XDI,XCX                 ; at stop scan address?
        jae     @F                      ; yes, all done

        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jz      @F                      ; no, done with this set of pages

        add     XDI,PAGE_SIZE           ; yes, skip to next page
        jmp     EndPageLoop

@@:
        mov     dwLastPage, XDI

        mov     XAX, XDI                ; calcuate size of block
        sub     XAX, dwFirstPage
        sub     XAX, 010h               ; backup from end of last page

        INVOKE  CheckMemoryBlock,lpData,dwFirstPage,XAX

        or      ax,ax                   ; any virus found?
        jnz     SM_Exita                ; yes, all done

        jmp     NextPageSet

NoValidPage:

;
;       CheckSpecial will return ax = 0 if no viruses, otherwise virus number
;
        LDS_SI  lpData                  ; SDS:XSI = dat file
        mov     cx,[XSI].HEADER.uNumALGs; XCX = number of signatures in table
        movzx   XCX,cx                  ; to dword
        add     XSI,sHEADER             ; SDS:XSI = offset of first signature
        mov     XDI,dwEndAddress        ; address to stop real mode search
        call    CheckSpecial            ; check for special viruses

SM_Exita:

endif

        ret


SCN4_MEMORY	ENDP





if OS_32

;-----------------------------------------------------------------------------
; CheckMemoryBlock
;
;       Scan block of memory for known viruses.
;
; UINT WINAPI CheckMemoryBlock(LPSTR StartAddress, DWORD dwLen);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
CheckMemoryBlock PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpData:LPSTR, lpStartAddress:LPSTR, dwLen:DWORD

        LOCAL dwCountSigChecks:DWORD
        LOCAL dwRemainingScanCount:DWORD

        mov     XAX, 1
        mov     dwCountSigChecks,XAX    ; used to relinquish control periodically

        mov     XCX, dwLen              ; count for scan loop
        mov     dwRemainingScanCount, XCX

        mov     XDI,lpStartAddress

        LDS_SI  lpData
        xor     XBX,XBX
        mov     bx,[XSI].HEADER.uNumALGs

        add     XSI,sHEADER

CMB_BlockLoop:
        xor     XAX, XAX                ; assume no more blocks to do
        mov     XCX, dwRemainingScanCount

        cmp     XCX, SCAN_BLOCK_SIZE
        jbe     @F                      ; only one block to do, all done

        mov     XAX, XCX                ; multiple blocks, set up first one
        sub     XAX, SCAN_BLOCK_SIZE
        mov     XCX, SCAN_BLOCK_SIZE

@@:
        mov     dwRemainingScanCount, XAX

        xor     ax,ax                   ; maybe done scanning - set no virus

        cmp     XCX, 0                  ; all done?
        je      CMB_Exitb

CMB_SegLoopb:
ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif

        cld
        mov     al,0CDh
        repne   scasb
        JXCXZ   CMB_BlockLoop

        inc     dwCountSigChecks

ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif
        call    CheckSigs
        jc      CMB_Exitb

        jmp     CMB_SegLoopb

CMB_Exitb:

ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif
        ret


CheckMemoryBlock        ENDP




;-----------------------------------------------------------------------------
; CheckPageValid
;
;       Check Page Valid
;
; UINT WINAPI CheckPageValid(LPSTR StartAddress);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
CheckPageValid PROC USES XBX XCX XSI,
        lpPageAddress:LPSTR

        mov     cx,  0ffh               ; assume page ok

        mov     eax, cr0                ; test paging enabled
        test    eax, PAGING_ENABLE
        jz      CPV_Done                ; no, then the page is ok

        ; extract the page table indices from the address

        mov     eax, lpPageAddress      
        shr     eax, PAGE_OFFSET_SIZE   ; drop page offset part of address
        mov     ebx, eax        
        and     ebx, PAGE_MASK          ; page PAGE value
        shl     ebx, 2                  ; dw index

        shr     eax, PAGE_PAGE_SIZE     ; page DIR value
        shl     eax, 2                  ; dw index

        mov     esi, cr3                ; get page directory base address
        add     esi, eax                ; physical address of page directory

        mov     eax, [esi]              ; get page directory entry
        or      eax, eax
        jz      CPV_BadPage             ; no entry, page bad

        and     eax, PF_FLAG_MASK       ; convert entry to address by 
                                        ; pitching flags

        add     eax, ebx                ; offset to requested entry
        mov     esi, eax
        mov     eax, [esi]              ; get page table entry

        test    eax, PF_FLAG_PRESENT    ; page present?
        jnz     CPV_Done                ; yes, ok flag already set

CPV_BadPage:
	xor	cx,cx                   ; page invalid

CPV_Done:
        movzx   XAX,cx                  ; return platform sized result
        ret


CheckPageValid        ENDP








;-----------------------------------------------------------------------------
; CountValidPages
;
;       Check Pages Valid - return number of bytes to scan
;
; DWORD WINAPI CountValidPages(LPSTR StartAddress, DWORD dwMaxBytes);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
CountValidPages PROC USES XBX XCX XSI XDI,
        lpPageAddress:LPSTR,
        dwMaxBytes:DWORD

        mov     XDI, lpPageAddress
        mov     XCX, dwMaxBytes
        xor     XBX,XBX                 ; clear total byte count

@@:
        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jz      @F                      ; no, end of page set
        add     XDI,1000h               ; yes, count page
        add     XBX,1000h               ; count bytes to return
        sub     XCX,1000h       
        jg      @B                      ; test next page

@@:
        cmp     XBX, dwMaxBytes         ; check overflow
        jb      @F
        mov     XBX,dwMaxBytes
@@:
        mov     XAX,XBX                 ; return valid bytes
        ret

CountValidPages        ENDP


;-----------------------------------------------------------------------------
; FindFirstValidPage
;
;       Find first valid page - return bytes to start of first valid page
;
; DWORD WINAPI FindFirstValidPage(LPSTR StartAddress, DWORD dwMaxBytes);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
FindFirstValidPage PROC USES XBX XCX XSI XDI,
        lpPageAddress:LPSTR,
        dwMaxBytes:DWORD

        mov     XDI, lpPageAddress
        mov     XCX, dwMaxBytes
        xor     XBX,XBX                 ; clear total byte count

@@:
        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jnz     @F                      ; yes, valid page found
        add     XDI,1000h               ; no, skip page
        add     XBX,1000h               ; count bytes to return
        sub     XCX,1000h       
        jg      @B                      ; test next page

@@:
        cmp     XBX, dwMaxBytes         ; check overflow
        jb      @F
        mov     XBX,dwMaxBytes
@@:
        mov     XAX,XBX                 ; return invalid bytes
        ret

FindFirstValidPage        ENDP



endif




;-----------------------------------------------------------------------------
; Check sigs against CD string found
;-----------------------------------------------------------------------------
CheckSigs PROC USES XBX XCX XDI XSI
	
J_CKLoop:
	mov	XDX,XSI
	mov	XCX,SIZE_MEM
        inc     XSI                             ;move past id word
        inc     XSI
        push    XDI                             ;save ptr to int code

	cmp	byte ptr [XSI],0
	je      J_NextSig
J_CSLoop:
	lodsb
        dec     al
	cmp	byte ptr es:[XDI],al
	jne     J_NextSig
	inc	XDI
        dec     XCX
        jnz     J_CSLoop

        pop     XDI                             ;virus found, restore int code
        mov     XSI,XDX
                                                ; At this point we want to
                                                ; return the virus id # so
                                                ; we can look up the name
                                                ; in the database.  We
                                                ; are ASSUMING that IN THE
                                                ; DATABASE, the virus id #'s
                                                ; are 1-based, not 0-based.
                                                ; this means that we don't
                                                ; need to INC AX to obtain
                                                ; a 1-based virus id #.
        ; inc   AX                              ; <--------------+


        mov     ax,word ptr[XSI]
        stc
	jmp	short J_Ck_Out

J_NextSig:
        pop     XDI
	mov	XSI,XDX
        dec     bx
        or      bx,bx                           ; needed to set carry flag
        jz      J_CK_Out
	add	XSI,SIZE_MEM + 2                ;Sig plus id word
        jmp     J_CKLoop
J_Ck_Out:
	ret

CheckSigs	ENDP


;-----------------------------------------------------------------------------
;  CheckSpecial  PROC  USES XBX XCX XDX XDI XSI XBP SDS SES
;
;  Entry:
;       SDS:XSI = pointer to the table with virus signatures
;       XCX     = number of entries in the table
;       di      = segment to stop the search in real mode
;  Exit:
;       ax      = 0 if no viruses found; otherwise signature's ID
;
;  This procedure walks through the memory signatures and searches for
;  the "special" ones.  They begin with (BYTE) 0, and then (BYTE) Fctn.
;  When such signature is found, a particular scan function is called.
;
;-----------------------------------------------------------------------------
CheckSpecial  PROC  USES XCX XDX XBX XSI XDI XBP SDS SES
        cld                             ; increment si on next lodsx
K_CKLoop:
        lodsw                           ; ax = signature's ID number
        mov     dx,ax                   ; save signature's ID number
        lodsw                           ; al = 0 if special, ah = function ID
        xchg    al,ah                   ; ax = special function ID
        dec     ax                      ; ax = special function ID (0-aligned!)

ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif
        cmp     ax,SPECIAL_FUNC_COUNT   ; is it a special function?
        jae     K_NextSig               ; nope, go to next signature

        push    XCX
        push    XDX
        push    XSI
        push    ds
        push    XDI
IF OS_16
        shl     ax,1                    ; the pointer is WORD
        mov     bx,ax                   ; bx = pointer in JUMP_TABLE
        call    WORD PTR [cs:JUMP_TABLE + bx] ; call the check function
ELSE ; OS_32
        shl     ax,2                    ; the pointer is DWORD
        movzx   ebx,ax                  ; ebx = pointer in JUMP_TABLE
        call    DWORD PTR [JUMP_TABLE + ebx] ; call the check function
ENDIF
        pop     XDI
        pop     ds
        pop     XSI
        pop     XDX
        pop     XCX
        mov     ax,dx                   ; ax = signature's ID number
        jc      K_CKEnd                 ; gotcha!
K_NextSig:
        add     XSI,SIZE_MEM - 2        ; the first two bytes are already read!
        dec     XCX                     ; Replaced LOOP
        jnz     K_CKLoop                ; finish the signature walk-through
        xor     ax,ax                   ; return 0 - memory clean!
K_CKEnd:
        ret                             ; go back to the caller!
CheckSpecial  ENDP

;-----------------------------------------------------------------------------
;
;  Jump table for the special functions
;
;-----------------------------------------------------------------------------
IF OS_16

JUMP_TABLE      DW  OFFSET  LOOK413
                DW  OFFSET  LOOKALL
SPECIAL_FUNC_COUNT  =  ( $ - OFFSET JUMP_TABLE ) SHR 1

ELSE ; OS_32

JUMP_TABLE      DD  OFFSET  LOOK413
                DD  OFFSET  LOOKALL
SPECIAL_FUNC_COUNT  =  ( $ - OFFSET JUMP_TABLE ) SHR 2

ENDIF

;******************************************************************************
;  Special functions:
;
;  Entry:
;       SDS:XSI = pointer to the last (SIZE_MEM - 2) bytes of a special entry
;                 in the table with signatures.
;       di      = segment to stop search in real mode
;       DF      = 0
;
;  Exit:
;       CF      = set if the function had found a virus
;       CF      = 0 if no virus was found
;
;  Each function can use & destroy any of the 80x86 registers.
;
;******************************************************************************

;------------------------------------------------------------------------------
;  LOOK413 is scanning the memory starting [0:413]*40h and ending at 9FFF:000F.
;  If the value of 0:413 is greater than or equal to 280h (640K), then the
;  memory will not be scanned and the procedure will return with cf cleared.
;  If the value at 0:413 is less than 640K, then memory will be scanned against
;  the signature at SDS:XSI.  The size of the signature is SIZE_MEM-2.
;  LOOK413 will return cf set if the signature was found and cf 0 if not.
;------------------------------------------------------------------------------

LOOK413  PROC NEAR USES XBX XCX XDX XDI XSI

IF OS_16

        xor     ax,ax
IFDEF WIN16
	INVOKE	MEM4_SEGTOSEL,ax
ENDIF
        mov     es,ax                   ; es = BIOS' data segment
        mov     dx,WORD PTR es:[0413h]  ; dx = top of conventional memory
        cmp     dx,640d                 ; is it 640K?
        jae     LOOK413End              ; yep, memory size is OK ( CF = 0!!! )
        mov     cl,6                    ; used for multiplying by 40h
        shl     dx,cl                   ; dx = top segment reported by BIOS
        mov     XBP,XSI                 ; save the sig pointer there
;
;       XDS:XBP = pointer to a signature to scan for ( size = SIZE_MEM - 2 )
;       DX      = starting segment to scan ( rounded to 1K )
;
;        sub     dx,40h                  ; align it for the type of cycle
LOOK413SegmentLoop:
;        add     dx,40h                  ; dx = segment of next 1K block
        cmp     dx,0A000h               ; is it the top of memory?
        jae     LOOK413End              ; no viruses (Note: cf = 0!)

IFDEF WIN16
        push    dx
        INVOKE  MEM4_SEGTOSEL,dx
        mov     es,ax                   ; es = segment to search at
        pop     dx                      ; Just in case INT 31 changes and 
                                        ;  decides to blow away dx
ELSE
        mov     es,dx                   ; es = segment to search at
ENDIF

        mov     XCX,401h                ; cx = search a 1K block, also offset
        add     dx,40h                  ; is this the last seg to check
        cmp     dx,0A000h
        jne     @F
        sub     XCX, SIZE_MEM-1         ; Shorten end to check
                                        ; Remember to account for 8001(1!)
@@:
        xor     XDI,XDI                 ; es:di = starting offset
LOOK413OffsetLoop:
        mov     XSI,XBP                 ; ds:si = pointer to signature
        lodsb                           ; get a byte from the signature
        dec     al                      ; decode it
        repne   scasb                   ; scan the memory
        JXCXZ   LOOK413SegmentLoop      ; signature not found
        mov     XBX,XCX                 ; save counter
        mov     XCX,SIZE_MEM - 3        ; XCX = bytes of sig left
        push    XDI                     ; no other place to save it
LOOK413SigLoop:
        lodsb                           ; get a byte from signature
        dec     al                      ; decode it
        scasb                           ; scan memory
        loope   LOOK413SigLoop          ; compare the rest of the signature
        pop     XDI                     ; restore destination address
        mov     XCX,XBX                 ; restore memory counter
        jne     LOOK413OffsetLoop       ; only the beginning was the same
        stc                             ; signature found!!!

ELSE ; OS_32

        LOCAL dwRemainingScanCount:DWORD
        LOCAL dwSignatureOffset:DWORD

        mov     dwSignatureOffset,XSI   ; save for signature checks

                                        ; check page 0 accessible
                                        ; and get 0x40:0x13 location
        mov     XDI,200h                ; default 80000 if page 0 invalid
        xor     XAX,XAX
        INVOKE  CheckPageValid,XAX      ; test page
        or      XAX,XAX                 ; page valid?
        jz      @F                      ; no, use default
        mov     XDI,413h
        mov     di,WORD PTR [XDI]       ; yes, get end of memory 
@@:
        cmp     di,640d                 ; is the memory more than 640K?
        jae     LOOK413End              ; skip search ( cf=0! )
        shl     edi,0Ah                 ; convert Ks to bytes ( first to scan )

        mov     XCX,0A0000h             ; final address
        sub     XCX,XDI                 ; count for number of mem bytes to scan

        INVOKE  FindFirstValidPage,XDI,XCX ; find start page
        add     XDI,XAX                 ; skip to start page
        sub     XCX,XAX       
        jnz     @F                      ; something to scan
                                        ; no valid pages
        xor     XAX,XAX                 ; no viruses
        jmp     LOOK413End
        
@@:
        INVOKE  CountValidPages,XDI,XCX ; count valid pages
        mov     XCX,XAX                 ; adjust end count accordingly
        or      XCX,XCX                 ; make sure some are valid
        jnz     @F                      ; ready to go
                                        ; no valid pages
        xor     XAX,XAX                 ; no viruses
        jmp     LOOK413End
        
@@:
        sub     XCX,SIZE_MEM - 3        ; can't scan last partial block
        mov     dwRemainingScanCount, XCX

LOOK413BlockLoop:
        xor     XAX, XAX                ; assume no more blocks to do
        mov     XCX, dwRemainingScanCount

        cmp     XCX, SCAN_BLOCK_SIZE
        jbe     @F                      ; only one block to do, all done

        mov     XAX, XCX                ; multiple blocks, set up first one
        sub     XAX, SCAN_BLOCK_SIZE
        mov     XCX, SCAN_BLOCK_SIZE

@@:
        mov     dwRemainingScanCount, XAX

        xor     XAX,XAX                 ; no viruses

        cmp     XCX, 0                  ; all done?
        je      LOOK413End

LOOK413OffsetLoop:
ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif
        mov     XSI,dwSignatureOffset   ; pointer to start of sig
        lodsb                           ; get first byte from signatures
        dec     al                      ; decode it
        repne   scasb                   ; search for first sig byte
        clc                             ; assume no viruses found

ifdef SYM_NLM
        pushf
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
        popf
endif
        JXCXZ   LOOK413BlockLoop        ; no viruses in this block
        mov     XBX,XDI                 ; save the new memory offset
        mov     XDX,XCX                 ; save the new counter
        mov     XCX,SIZE_MEM - 3        ; counter for sig bytes left
LOOK413SigLoop:
        lodsb                           ; get next sig byte
        dec     al                      ; decode it
        scasb                           ; compare it to memory
        loope   LOOK413SigLoop          ; still the same, examine next byte
        mov     XDI,XBX                 ; restore target memory offset
        mov     XCX,XDX                 ; restore bytes left
        jne     LOOK413OffsetLoop       ; comparison failed, scan next
        stc                             ; virus found!
ENDIF

LOOK413End:
        ret                             ; return to caller
LOOK413  ENDP

;------------------------------------------------------------------------------
;  LOOKALL is scanning the memory starting 0 and ending at 0A0000 / 0FFFFF.
;------------------------------------------------------------------------------
LOOKALL  PROC NEAR USES XBX XCX XDX XDI XSI

IF OS_16

        xor     dx,dx                   ; starting segment for search
        mov     XBP,XSI                 ; save the offset of definition

LOOKALLSegmentLoop:

        push    di                      ; save segment to stop search at
        push    dx                      ; save segment to search

IFDEF WIN16
        push    dx
        INVOKE  MEM4_SEGTOSEL,dx
        mov     es,ax                   ; es = segment to search at
        pop     dx                      ; Just in case INT 31 changes and 
                                        ;  blows away DX
ELSE
        mov     es,dx                   ; es = segment to search at
ENDIF

        mov     XCX,8001h               ; search 1/2 segment
        add     dx,800h                 ; is this the last seg to check
        cmp     dx,di
        jne     @F
        sub     XCX, SIZE_MEM-1         ; Shorten end to check
                                        ; Remember to account for 8001(1!)
@@:
        xor     XDI,XDI                 ; start at offset 0
LOOKALLOffsetLoop:
        mov     XSI,XBP                 ; get offset of signature
        lodsb                           ; get a signature byte
        dec     al                      ; decode it
        repne   scasb                   ; search for it in the memory
        JXCXZ   LOOKALLNextSegment      ; go to scan the next segment
        mov     XBX,XCX                 ; save counter
        mov     XDX,XDI                 ; save memory address
        mov     XCX,SIZE_MEM - 3        ; XCX = bytes of sig left
LOOKALLSigLoop:
        lodsb                           ; get next signature byte
        dec     al                      ; decode it
        scasb                           ; compare it to the memory
        loope   LOOKALLSigLoop          ; do until equal
        mov     XCX,XBX                 ; restore counter
        mov     XDI,XDX                 ; restore memory address
        jne     LOOKALLOffsetLoop       ; continue search in this segment
        stc                             ; virus found
        pop     dx                      ; just stack cleanup
        pop     di                      ; just stack cleanup
        jmp     SHORT LOOKALLEnd        ; quit
LOOKALLNextSegment:
        pop     dx                      ; restore current segment
        pop     di                      ; restore target segment
        add     dx,800h                 ; point to next segment
        cmp     dx,di                   ; have we reached it?
        jne     LOOKALLSegmentLoop      ; not yet, scan next
;*** no need of CLC because JNE in front!

ELSE  ; OS_32

        LOCAL dwRemainingScanCount:DWORD
        LOCAL dwSignatureOffset:DWORD

        mov     dwSignatureOffset,XSI   ; save for signature checks

        movzx   XCX,di                  ; XCX = paragraphs to scan
        shl     XCX,4                   ; convert para->bytes
        or      XCX,XCX                 ; is it particular address or whole mem?
        jnz     LOOKALLParticularAddr   ; address specified
        mov     XCX,100000h             ; final offset
LOOKALLParticularAddr:
        xor     XDI,XDI                 ; XBX = starting offset

;
;       find first valid starting page
;
        INVOKE  FindFirstValidPage,XDI,XCX ; find start page
        add     XDI,XAX                 ; skip to start page
        sub     XCX,XAX       
        jnz     @F                      ; something to scan
                                        ; no valid pages
        xor     XAX,XAX                 ; no viruses
        jmp     LOOKALLEND
        
@@:
        INVOKE  CountValidPages,XDI,XCX ; count valid pages
        mov     XCX,XAX                 ; adjust end count accordingly
        or      XCX,XCX                 ; make sure some are valid
        jnz     @F                      ; ready to go
                                        ; no valid pages
        xor     XAX,XAX                 ; no viruses
        jmp     LOOKALLEND
        
@@:
        sub     XCX,SIZE_MEM - 3        ; can't scan last partial block
        mov     dwRemainingScanCount, XCX

LOOKALLBlockLoop:
        xor     XAX, XAX                ; assume no more blocks to do
        mov     XCX, dwRemainingScanCount

        cmp     XCX, SCAN_BLOCK_SIZE
        jbe     @F                      ; only one block to do, all done

        mov     XAX, XCX                ; multiple blocks, set up first one
        sub     XAX, SCAN_BLOCK_SIZE
        mov     XCX, SCAN_BLOCK_SIZE

@@:
        mov     dwRemainingScanCount, XAX

        xor     XAX,XAX                 ; no viruses

        cmp     XCX, 0                  ; all done?
        je      LOOKALLEnd

LOOKALLOffsetLoop:
ifdef SYM_NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif
        mov     XSI,dwSignatureOffset   ; pointer to start of sig
        lodsb                           ; get a byte from signatures
        dec     al                      ; decode it
        repne   scasb                   ; search in memory
        clc                             ; assume no signature found

ifdef SYM_NLM
        pushf
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
        popf
endif
        jne     LOOKALLBlockLoop        ; no virus in this block
        mov     XBX,XCX                 ; save counter
        mov     XDX,XDI                 ; save current offset
        mov     XCX,SIZE_MEM - 3        ; size of signature
LOOKALLSigLoop:
        lodsb                           ; get a byte from signatures
        dec     al                      ; decode it
        scasb                           ; compare to memory
        loope   LOOKALLSigLoop          ; compare next byte from sig
        mov     XDI,XDX                 ; restore current offset
        mov     XCX,XBX                 ; restore counter
        stc                             ; viruses found if ZF = 1
        jne     LOOKALLOffsetLoop       ; no signature, scan next offset
ENDIF

LOOKALLEnd:
        ret
LOOKALL  ENDP


;------------------------------------------------------------------------------
IF 0
DUMMY  PROC  NEAR
        clc                             ; no virus!
        ret                             ; return to caller
DUMMY  ENDP
ENDIF

END



IF 0
;------------- this one is shorter, but a bit slower!
        mov     XBX,SIZE_MEM - 2        ; bx = bytes of signature left
LOOK413SigLoop:
        dec     XBX                     ; decrement the counter
        jz      LOOK413VirusFound       ; gotcha!
        lodsb                           ; get a byte from signature
        dec     al                      ; decode it
        scasb                           ; compare it to the memory
        je      LOOK413SigLoop          ; this byte is the same, check the rest
        jmp     LOOK413OffsetLoop       ; not the same signature, go to next offs
LOOK413VirusFound:
        stc                             ; signature found !
;-------------
ENDIF

