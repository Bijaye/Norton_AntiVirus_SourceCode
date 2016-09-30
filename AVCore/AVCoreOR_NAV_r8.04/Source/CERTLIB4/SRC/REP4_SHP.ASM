include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_HARD_SAVE_PHYSICAL                                           SAMIZ
;
;     Repairs HD where original is stored as absolute in virus (MusicBug)
; $Header:   S:\certlib4\vcs\rep4_shp.asv   1.4   29 Nov 1994 13:05:36   DALLEE  $
;
; UINT WINAPI REP4_HARD_SAVE_PHYSICAL(WORD uDrive, LPSTR lpBuffer);
;
; Input:
;       1) Drive Number with logical in high byte, absolute in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_shp.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:05:36   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:20:50   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:31:30   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:29:26   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   18 May 1993 01:33:24   SZIADEH
;  minor modifications.
;  
;  
;     Rev 1.0   10 May 1993 16:41:22   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_HARD_SAVE_PHYSICAL      PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR

                        LOCAL uCylOff:WORD
                        LOCAL uSecOff:WORD
                        LOCAL uHeadOff:WORD

                        LOCAL byCount:BYTE
                        LOCAL uCylinder:WORD
                        LOCAL bySector:BYTE
                        LOCAL byHead:BYTE
                        LOCAL byDrive:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpBuffer


        mov     byCount,1
        mov     ax, uDrive
        mov     byDrive,al              ; first physical hard disk
        xor     ax, ax


        mov     uCylOff, ax
        mov     uSecOff, ax
        mov     uHeadOff,ax

        mov     ax,word ptr[XSI].N30.HardLoc1
        mov     byte ptr [uCylOff], ah
        mov     byte ptr [uSecOff], al
        
        mov     ax,word ptr[XSI].N30.HardLoc2
        mov     byte ptr [uHeadOff],ah

        mov     bySector, al            ; ADD field


        mov     ax,[XSI].N30.Restore

        test    al, bySecMask           ; bySecMask= 0000 0001b
        jz      @F
        add     uSecOff, 0100h
@@:
        test    al, byHeadMask
        jz      @F
        add     uHeadOff, 0100h          ; byHeadMask=0000 0010b

@@:

        test    al, byCylMask           ; byCylMask= 0000 0100b
        jz      @F
        add     uCylOff, 00100h


@@:

        mov     ax ,word ptr[XSI].N30.SpecFlags; should I read 1 or 2 bytes
        mov     uCylinder, ax                  ; for cylinder

;-------------------------------------

        INVOKE  DIS4_READ_BOOTSECTOR, byDrive,lpBuffer ;read boot sector
        or      ax,ax
        jnz     R_ERROR
;-------------------------------------
        LDS_SI  lpBuffer

        push    XSI
                                           ;at this point ax must be zero

        mov     ax, uCylOff
        add     XSI , XAX
        xor     ax, ax                     ;
        cmp     uCylinder, 0
        je      @F
        mov     ah, byte ptr[XSI+1]        ; flag was set. read additional byte
@@:
        mov     al, byte ptr [XSI]         ; read low byte
        mov     uCylinder, ax

        pop     XSI
        push    XSI

        mov     ax, uSecOff
        add     XSI, XAX
        xor     ax,ax
        mov     al, byte ptr[XSI]
        add     bySector, al                    ; adjust sector

        pop     XSI
        mov     ax, uHeadOff
        add     XSI, XAX
        xor     ax,ax
        mov     al, byte ptr[XSI]
        mov     byHead, al

;-------------------------------------
R_WRITE:

        INVOKE DIS4_LOC_BOOTSECTOR, byDrive,lpBuffer  ;get address of boot sector
        cmp    ax, -1                           ; error
        je     R_ERROR

        mov   uHeadOff,ax                       ; head/sec
        mov   uCylOff,dx                        ; cylinder


        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR                         ;error

        LDS_SI  lpBuffer                        ;
        cmp     word ptr[XSI+510],0AA55h        ; Verify as BR
        jnz     R_ERROR
                                                ; set physical address for
        mov     ax, uCylOff                     ; the boot sector
        mov     uCylinder, ax                   ; cylinder
        mov     ax,uHeadOff                     ;
        mov     bySector, al                    ; sector
        mov     byHead, ah                      ; head

        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
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

REP4_HARD_SAVE_PHYSICAL      ENDP
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
; wBookMark          - Special Flags        NOTE2
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


NOTE2:

 default:
  One byte is read from offset cyl.

 Special case:

 set this field to: 0000 0000 0000 0001
 to read a WORD (2 bytes) from offset cyl.


 This can be used in the case of a large hard disk drive where the
 the original sector is stored toward the end of the disk, and the virus
 used 2 bytes to store the cylinder address. If this bit is not
 set, only one byte is read.




