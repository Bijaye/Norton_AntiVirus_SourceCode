include CTS4.INC
include REPAIR.INC                    

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_SAVE_PHYSICAL                 5/4/93                        SAMIZ
;
;     Repairs floppy by using saved PHYSICAL sector method.
;  $Header:   S:\certlib4\vcs\rep4_fsp.asv   1.5   29 Nov 1994 13:05:40   DALLEE  $
;
; UINT WINAPI REP4_FLOP_SAVE_PHYSICAL(WORD uDrive, LPSTR lpBuffer, WORD uType);
;
; Input:
;       1) Drive Number with logical in high byte, physical in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_fsp.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:05:40   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:21:04   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:31:46   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.0   03 Sep 1993 19:29:32   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   18 May 1993 01:30:28   SZIADEH
;  minor midfications.
;  
;     Rev 1.0   10 May 1993 16:24:10   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_FLOP_SAVE_PHYSICAL       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR,
                        uType:WORD


                        LOCAL uCylOff:WORD
                        LOCAL uSecOff:WORD
                        LOCAL uHeadOff:WORD

                        LOCAL bySector:BYTE
                        LOCAL uCylinder:WORD
                        LOCAL byHead:BYTE
                        LOCAL byDrive:BYTE
                        LOCAL byCount:BYTE
                        LOCAL bySecAdd:BYTE

;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------

        mov     byCount,1
        mov     ax,uDrive
        mov     byDrive,al                     ; Store physical drive number

        xor     ax,ax
        mov     uCylOff, ax
        mov     uSecOff, ax
        mov     uHeadOff,ax

        LDS_SI  lpBuffer

        mov     ax,[XSI].N30.wSS_reg    ; Try 360k
        cmp     uType,1
        je      R_WRITE

        mov     ax,[XSI].N30.wSP_reg    ; Try 720k
        cmp     uType,2
        je      R_WRITE

        mov     ax,[XSI].N30.wCS_reg    ; Try 1.2m
        cmp     uType,3
        je      R_WRITE

        mov     ax,[XSI].N30.wIP_reg    ; Try 1.44m
        cmp     uType,4
        jne     R_ERROR

R_WRITE:

        mov     byte ptr[uCylOff], ah
        mov     byte ptr[uSecOff], al

        mov     ax,[XSI].N30.Flop_All
        mov     byte ptr[uHeadOff], ah
        mov     bySecAdd, al             ; ADD field



        mov     ax,[XSI].N30.Restore

        test    al, bySecMask             ; bySecMask= 0000 0001b
        jz      @F
        add     uSecOff, 0100h
@@:
        test    al, byHeadMask
        jz      @F
        add     uHeadOff, 0100h            ; byHeadMask=0000 0010b

@@:

        test    al, byCylMask             ; byCylMask= 0000 0100b
        jz      @F
        add     uCylOff, 0100h


@@:

;-------------------------------------
        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,byCount,0,1,0,byDrive
        or      ax,ax
        jnz     R_ERROR
;-------------------------------------
        LDS_SI  lpBuffer

        push    XSI
                                           ;at this point ax must be zero
        mov     ax, uCylOff
        add     XSI , XAX
        xor     ax,ax
        mov     al, byte ptr [XSI]
        mov     uCylinder, ax

        pop     XSI
        push    XSI

        mov     ax, uSecOff
        add     XSI, XAX
        xor     ax,ax
        mov     al, byte ptr[XSI]
        add     bySector, al

        pop     XSI
        mov     ax, uHeadOff
        add     XSI, XAX
        xor     ax,ax
        mov     al, byte ptr[XSI]
        mov     byHead, al

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

;-------------------------------------
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,byCount,0,1,0,byDrive
        or      ax,ax
        jnz     R_ERROR

        mov     ax,1
        jmp     R_EXIT
;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_FLOP_SAVE_PHYSICAL       ENDP
;--------------------------------------
END


;----------------------------------------------------
;  CLEAN                MEANING           V_Physical
;  FIELD               FOR BOOT             hi/lo
;                                          (offset)
;----------------------------------------------------
; wInfSize           - Hard Type             -
; wTagSize           - Flop Type             -
; wHostJmp           - Hard Location 1     cyl/sec
; wJmpLen            - Hard Location 2     head/add
; wSS_reg            - Flop 360            cyl/sec
; wSP_reg            - Flop 720            cyl/sec
; wCS_reg            - Flop 1.2            cyl/sec
; wIP_reg            - Flop 1.44           cyl/sec
; wHeader            - Flop 2.88           cyl/sec
; wHeadLen           - Flop All            head/add
; wBookMarkLocation  - Restore Word         NOTE1
; wBookMark          - Special Flags         -
;----------------------------------------------------


NOTE1:
 Can be used in situations where the offset for any of the fields
 does not fit within 1 byte.

 if sec does not fit with in 1 byte, use this value:
    0000 0000 0000 0001

 if head does not fit with in 1 byte, use this value:
    0000 0000 0000 0010

 if cyl does not fit with in 1 byte, use this value:
    0000 0000 0000 0100

 Above values may be ORed.


