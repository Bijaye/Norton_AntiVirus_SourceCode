include CTS4.INC
include repair.inc

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_GENERIC                      1/21/93                             JOE
;
;     Does Generic repair on COMs and EXEs.
;
; $Header:   S:\certlib4\vcs\rep4_gen.asv   1.11   29 Nov 1994 13:04:50   DALLEE  $
;
; UINT WINAPI REP4_GENERIC(WORD HostHandle, WORD uBuffer,LPSTR lpBuffer);
;
; Input:
;       1) Handle
;       2) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_gen.asv  $
;  
;     Rev 1.11   29 Nov 1994 13:04:50   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.9.1.0   19 Oct 1993 07:24:08   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.9   12 Oct 1993 16:34:14   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.8.1.0   03 Sep 1993 19:30:32   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.8   14 Aug 1993 16:37:14   BRAD
;  Updated from QAK0.
;  
;     Rev 1.7.1.1   13 Aug 1993 20:12:10   CAREY
;  Aded support for new COM_DISCRETE flag.
;  
;     Rev 1.7.1.0   02 Aug 1993 09:13:44   SCOTTP
;  Branch base for version QAK0
;  
;     Rev 1.7   10 May 1993 19:06:54   SZIADEH
;  
;     Rev 1.6   10 May 1993 19:05:24   SZIADEH
;  Included "repair.inc"
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Local prototypes follow
;-----------------------------------------------------------------------------

if FARCALL
COPY_BYTES_TO_TOP   PROTO FAR,:WORD,:WORD,:DWORD,:WORD,:WORD
else
COPY_BYTES_TO_TOP   PROTO NEAR,:WORD,:WORD,:DWORD,:WORD,:WORD
endif

;-----------------------------------------------------------------------------


REP4_GENERIC    PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                uBuffer:WORD,           ; Size of work buffer
                lpBuffer:LPSTR          ; Ptr to my work buffer

                
        LDS_SI  lpBuffer

        mov     bx,[XSI].RepairStruct.xN30.wInfsize

        test    word ptr[XSI].RepairStruct.xN30.wControl2, COM_DISTINCT
        jz      NORMAL_GENERIC_REPAIR

        cmp     word ptr[XSI].RepairStruct.xN30.wHostJmp, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wHostJmp
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 0, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wJmpLen, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wJmpLen
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 1, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wSS_reg, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wSS_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 2, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wSP_reg, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wSP_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 3, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wCS_reg, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wCS_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 4, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wIP_reg, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wIP_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 5, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wHeader, word ptr 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wHeader
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 6, 1, bx
        or      ax, ax
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     word ptr[XSI].RepairStruct.xN30.wHeadLen, word ptr 0FFFFh
        jnz      @F

        mov     ax, 1                   ; no repairs to do, success!
        jmp     EXIT_GENERIC_REPAIR

@@:
        mov     dx,[XSI].RepairStruct.xN30.wHeadLen
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 7, 1, bx

        jmp     EXIT_GENERIC_REPAIR

NORMAL_GENERIC_REPAIR:

        ; set offset & length of bytes we want to copy back

        mov     cx,[XSI].RepairStruct.xN30.wJmpLen
        mov     dx,[XSI].RepairStruct.xN30.wHostJmp

        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 0, cx, bx

        ; COPY_BYTES_TO_TOP should set AX = 0 or 1 for failure/success

EXIT_GENERIC_REPAIR:

        ret   

REP4_GENERIC ENDP
;--------------------------------------------------------------------------

COPY_BYTES_TO_TOP   PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                wTOVOffset:WORD,        ; offset to bytes from top of virus
                dwTOFOffset:DWORD,      ; offset from TOF where to copy bytes
                wNumBytes:WORD,         ; how many bytes to copy
                wVirusSize:WORD         ; how big is the virus

        LOCAL   dwSeekDist: DWORD
        LOCAL   bRepair: BYTE

        xor     dx, dx                  ; HIGH WORD of dist from EOF to TOV
        mov     ax, wVirusSize          ; LOW WORD of dist from EOF to TOV
        sub     ax, wTOVOffset          ; new dist from EOF to repair bytes

        mov     word ptr dwSeekDist,   ax
        mov     word ptr dwSeekDist+2, dx

COPY_LOOP:

        INVOKE  FIL4_SEEK, HostHandle, dwSeekDist, fSEEK_END

        sub     word ptr dwSeekDist,   1  ; point to next byte to copy!

        INVOKE  FIL4_READ, HostHandle, ADDR bRepair, 1
        INVOKE  FIL4_SEEK, HostHandle, dwTOFOffset, fSEEK_BEG

        add     word ptr dwTOFOffset, 1   ; point to next destination

        INVOKE  FIL4_WRITE,HostHandle, ADDR bRepair, 1
        or      ax, ax
        jz      COPY_ERROR

        dec     wNumBytes
        jnz     COPY_LOOP

        mov     ax, 1


COPY_ERROR:                             ; AX will be ZERO HERE!

        ret

COPY_BYTES_TO_TOP   ENDP

;--------------------------------------------------------------------------
END

