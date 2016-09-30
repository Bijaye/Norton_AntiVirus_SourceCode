;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/rep4_sph.asv   1.3   27 Jun 1996 17:38:38   DALLEE  $
;;
;; Description: Defines REP4_SAVE_PHYSICAL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/rep4_sph.asv  $
;  
;     Rev 1.3   27 Jun 1996 17:38:38   DALLEE
;  LuigiPlus port:
;  Treat 1.68M DMF floppies as 1.44 for known repairs.
;  
;     Rev 1.2   10 Apr 1996 15:10:50   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:26:18   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   25 May 1994 09:10:08   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_SAVE_PHYSICAL
;
; AX:DX WINAPI REP4_SAVE_PHYSICAL ( LPVOID lpN30, LPVOID lpBuffer,
;                                   BYTE byType );
;
; Input:
;       1) lpN30: N30 stuff
;       2) lpBuffer: buffer containing boot/mbr virus
;       3) byType:type of drive
; Output:
;        1) AX: -1 on error
;        2) DX: Cylinder
;           AL: Sector
;           Ah: Head
;----------------------------------------------------------------------------
REP4_SAVE_PHYSICAL  PROC  CODEDISTANCE  USES XBX SDS XSI,
        lpN30: LPBYTE,                  ; N30 stuff
        lpBuffer:LPBYTE,                ; buffer containing boot/mbr virus
        byType:BYTE                     ; type of drive

        LOCAL  uCylOff: WORD
        LOCAL  uSecOff: WORD
        LOCAL  uHeadOff:WORD
        LOCAL  uXSI:UINT

        LOCAL  bySecAdd:BYTE

        xor     XAX, XAX
        xor     XDX, XDX

        mov     uCylOff,ax
        mov     uSecOff,ax
        mov     uHeadOff,ax

        LDS_SI  lpN30

        mov     ax,[XSI].N30.HardLoc1   ; cylinder/sec offsets
        mov     dx,[XSI].N30.HardLoc2   ; head/add
        cmp     byType,0                ; Hard disk
        je      R_CONTINUE

        mov     ax,[XSI].N30.Flop_360   ; Try 360k
        cmp     byType,1
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_720   ; Try 720k
        cmp     byType,2
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_12    ; Try 1.2m
        cmp     byType,3
        je      R_GET_HEAD

        mov     ax,[XSI].N30.Flop_144   ; Try 1.44m
        cmp     byType,4
        je      R_GET_HEAD              ; Treat 1.68M DMF 1024 byte as 1.44M
        cmp     byType,6
        je      R_GET_HEAD              ; Treat 1.68M DMF 2048 byte as 1.44M
        cmp     byType,7
        ;je      R_GET_HEAD

        jne     R_ERROR

R_GET_HEAD:
        mov     dx,[XSI].N30.Flop_All

R_CONTINUE:
        mov     bySecAdd,dl             ; ADD field
        mov     BYTE PTR[uHeadOff],dh
        mov     BYTE PTR[uCylOff],ah
        mov     BYTE PTR[uSecOff],al

        mov     dx,[XSI].N30.SpecFlags  ; cylinder

        mov     ax,[XSI].N30.Restore
        test    al,bySecMask            ; bySecMask= 0000 0001b
        jz      @F
        add     uSecOff,0100h
@@:
        test    al,byHeadMask
        jz      @F
        add     uHeadOff,0100h          ; byHeadMask=0000 0010b

@@:
        test    al,byCylMask            ; byCylMask= 0000 0100b
        jz      @F
        add     uCylOff,0100h
@@:

        LDS_SI  lpBuffer

        mov     uXSI,XSI

        mov     ax,uCylOff
        add     XSI,XAX

        or      dx,dx                   ; read one or two bytes
        je      @F                      ; jmp if only one
        mov     dh, BYTE PTR [XSI+1]    ; flag was set. read hi byte
@@:
        mov     dl, BYTE PTR [XSI]      ; read low byte

        mov     XSI,uXSI

        mov     ax, uSecOff
        add     XSI, XAX
        xor     ax,ax
        mov     al, BYTE PTR[XSI]       ; sector

        push    uXSI
        pop     XSI

        movuw   XBX,uHeadOff
        add     XSI,XBX
        xor     XBX,XBX
        mov     bl,BYTE PTR[XSI]
        mov     ah,bl                   ; head
        add     al,bySecAdd             ; adjustment
                                        ; sanity check
        or      ax,ax
        jnz     R_EXIT
R_ERROR:
        mov     ax, -1
R_EXIT:
        ret

REP4_SAVE_PHYSICAL  ENDP

UniqueSegmentEnd
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
