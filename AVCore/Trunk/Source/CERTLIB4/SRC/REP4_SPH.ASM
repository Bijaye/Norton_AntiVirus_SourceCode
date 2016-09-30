include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
;REP4_SAVE_PHYSICAL                                 SAMIZ
;
; int WINAPI REP4_SAVE_PHYSICAL (LPSTR lpN30, LPSTR lpBuffer, BYTE byType)
;
; $Header:   S:\certlib4\vcs\rep4_sph.asv   1.5   29 Nov 1994 13:05:28   DALLEE  $
;
; int WINAPI REP4_SAVE_PHYSICAL(LPSTR,LPSTR,BYTE);
; Input:
;	 1) lpN30: N30 stuff
;	 2) lpBuffer: buffer containing boot/mbr virus
;    3) byType:type of drive
; Output:
;	 1) -1 on error
;	 2)  DX: Cylinder
;		 AL: Sector
;		 Ah: Head
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_sph.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:05:28   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:22:34   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:33:04   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.0   03 Sep 1993 19:30:02   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   25 Jun 1993 18:55:08   SZIADEH
;  Moved the instruction "mov    word ptr[uHeadOff], dh" to its proper place!
;  
;     Rev 1.1   21 Jun 1993 18:27:44   SZIADEH
;  Added code to check for the validity of sector value.
;  
;     Rev 1.0   14 Jun 1993 14:28:24   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_SAVE_PHYSICAL   PROC USES XBX XCX SDS SES XSI XDI,
                lpN30: LPSTR,           ; N30 stuff
                lpBuffer:LPSTR,         ; buffer containing boot/mbr virus
                byType:BYTE             ; type of drive

                LOCAL  uCylOff: WORD
                LOCAL  uSecOff: WORD
                LOCAL  uHeadOff:WORD
                LOCAL  uXSI:WORD
                LOCAL  bySecAdd:BYTE
;-----------------------------------------------------------------------------
        xor     XAX, XAX
        xor     XDX, XDX

        mov     uCylOff, ax
        mov     uSecOff, ax
        mov     uHeadOff,ax
;------------------------------
        LDS_SI  lpN30

        mov     ax,word ptr[XSI].N30.HardLoc1       ; cylinder/sec offsets
        mov     dx,word ptr[XSI].N30.HardLoc2       ; head/add
        cmp     byType, 0                               ; Hard disk
        je      R_CONTINUE

        mov     ax,word ptr [XSI].N30.Flop_360          ; Try 360k
        cmp     byType,1
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_720                   ; Try 720k
        cmp     byType,2
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_12                    ; Try 1.2m
        cmp     byType,3
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_144                   ; Try 1.44m
        cmp     byType,4

        jne     R_ERROR
;------------------------------
R_GET_HEAD:
        mov     dx,word ptr[XSI].N30.Flop_All

R_CONTINUE:
        mov     bySecAdd, dl                            ; ADD field
        mov     byte ptr[uHeadOff], dh
        mov     byte ptr[uCylOff], ah
        mov     byte ptr[uSecOff], al

        mov     dx, word ptr[XSI].N30.SpecFlags     ;cylinder

        mov     ax,[XSI].N30.Restore
        test    al, bySecMask                           ; bySecMask= 0000 0001b
        jz      @F
        add     uSecOff, 0100h
@@:
        test    al, byHeadMask
        jz      @F
        add     uHeadOff, 0100h                         ; byHeadMask=0000 0010b

@@:

        test    al, byCylMask                           ; byCylMask= 0000 0100b
        jz      @F
        add     uCylOff, 0100h
@@:
;------------------------------------------;
        LDS_SI  lpBuffer                   ;
                                           ;
        push    XSI                        ;
        pop     uXSI                       ;
                                           ;
        mov     ax, uCylOff                ;
        add     XSI , XAX                  ;
                                           ;
        cmp     dx, 0                      ;read one or two bytes
        je      @F                         ;jmp if only one
        mov     dh, byte ptr[XSI+1]        ;flag was set. read hi byte
@@:                                        ;
        mov     dl, byte ptr [XSI]         ;read low byte
                                           ;
        push    uXSI                       ;
        pop     XSI                        ;
                                           ;
        mov     ax, uSecOff                ;
        add     XSI, XAX                   ;
        xor     ax,ax                      ;
        mov     al, byte ptr[XSI]          ;sector
                                           ;
        push    uXSI                       ;
        pop     XSI                        ;
                                           ;
        mov     bx, uHeadOff               ;
        add     XSI,XBX                    ;
        xor     XBX,XBX                    ;
        mov     bl, byte ptr[XSI]          ;
        mov     ah,bl                      ;head
        add     al, bySecAdd               ;adjustment
;------------------------------------------;
;Sanity check...
;
        or      ax,ax
        jz      R_ERROR

        jmp     short R_EXIT

R_ERROR:
        mov     ax, -1
R_EXIT:
        ret

REP4_SAVE_PHYSICAL  ENDP
END
;-----------------------------------------------------------------------------
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
