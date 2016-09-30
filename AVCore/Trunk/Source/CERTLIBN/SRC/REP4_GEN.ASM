;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_GEN.ASv   1.3   10 Apr 1996 15:10:48   DALLEE  $
;;
;; Description: Defines REP4_GENERIC function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_GEN.ASv  $
;  
;     Rev 1.3   10 Apr 1996 15:10:48   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Aug 1994 13:26:16   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   23 May 1994 16:55:36   RSTANEV
;  Added file header.
;
;     Rev 1.0   23 May 1994 16:53:00   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_GENERIC
;
; Does Generic repair on COMs and EXEs.
;
; BOOL WINAPI REP4_GENERIC ( UINT HostHandle, UINT uBuffer, LPVOID lpBuffer );
;
; Input:
;       1) Handle
;       2) Size of buffer
;       3) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; Local prototypes follow
;----------------------------------------------------------------------------

COPY_BYTES_TO_TOP   PROTO CODEDISTANCE,:UINT,:WORD,:DWORD,:WORD,:WORD

;----------------------------------------------------------------------------

REP4_GENERIC  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        uBuffer:UINT,                   ; Size of work buffer
        lpBuffer:LPBYTE                 ; Ptr to my work buffer

        LDS_SI  lpBuffer

        mov     bx,[XSI].RepairStruct.xN30.wInfsize

        test    [XSI].RepairStruct.xN30.wControl2, COM_DISTINCT
        jz      NORMAL_GENERIC_REPAIR

        cmp     [XSI].RepairStruct.xN30.wHostJmp, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wHostJmp
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 0, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wJmpLen, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wJmpLen
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 1, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wSS_reg, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wSS_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 2, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wSP_reg, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wSP_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 3, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wCS_reg, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wCS_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 4, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wIP_reg, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wIP_reg
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 5, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wHeader, WORD PTR 0FFFFh
        je      @F

        mov     dx,[XSI].RepairStruct.xN30.wHeader
        INVOKE  COPY_BYTES_TO_TOP, HostHandle, dx, 6, 1, bx
        or      XAX,XAX
        jz      EXIT_GENERIC_REPAIR

@@:

        cmp     [XSI].RepairStruct.xN30.wHeadLen, WORD PTR 0FFFFh
        jnz     @F

        mov     XAX,TRUE                ; no repairs to do, success!
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

REP4_GENERIC  ENDP

;----------------------------------------------------------------------------

COPY_BYTES_TO_TOP  PROC  CODEDISTANCE  USES XBX XDX SDS SES,
        HostHandle:UINT,                ; Handle of infected file
        wTOVOffset:WORD,                ; offset to bytes from top of virus
        dwTOFOffset:DWORD,              ; offset from TOF where to copy bytes
        wNumBytes:WORD,                 ; how many bytes to copy
        wVirusSize:WORD                 ; how big is the virus

        LOCAL   dwSeekDist: DWORD
        LOCAL   bRepair: BYTE

        movuw   XAX,wVirusSize          ; LOW WORD of dist from EOF to TOV
        sub     ax,wTOVOffset           ; new dist from EOF to repair bytes

IF OS_16
        mov     WORD PTR dwSeekDist,ax
        mov     WORD PTR dwSeekDist + 2,0
ELSE
        mov     dwSeekDist,XAX
ENDIF

COPY_LOOP:

        INVOKE  FIL4_SEEK, HostHandle, dwSeekDist, fSEEK_END

        dec     WORD PTR dwSeekDist     ; point to next byte to copy!

        INVOKE  FIL4_READ, HostHandle, ADDR bRepair, 1
        INVOKE  FIL4_SEEK, HostHandle, dwTOFOffset, fSEEK_BEG

        inc     WORD PTR dwTOFOffset    ; point to next destination

        INVOKE  FIL4_WRITE, HostHandle, ADDR bRepair, 1
        or      XAX,XAX
        jz      COPY_ERROR

        dec     wNumBytes
        jnz     COPY_LOOP

        mov     XAX,TRUE

COPY_ERROR:                             ; AX will be ZERO HERE!

        ret

COPY_BYTES_TO_TOP  ENDP

UniqueSegmentEnd
END
