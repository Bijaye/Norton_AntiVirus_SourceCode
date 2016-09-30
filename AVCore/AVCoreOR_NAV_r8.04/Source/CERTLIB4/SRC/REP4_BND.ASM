include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_PART_EOFD                                                   SAMIZ
;
;     Repairs MBR from "end of disk" infections like Flip & Tequila.
; $Header:   S:\certlib4\vcs\rep4_bnd.asv   1.6   29 Nov 1994 13:04:56   DALLEE  $
;
; UINT WINAPI REP4_PART_EOFD(LPSTR,LPSTR, BYTE);
; Input:
;      lpN30: Virus entry information in Database
;      lpBuffer: Work buffer. 1024 bytes minimum
;      byDrive:  Physical drive (0, 1, ..., 7f, 080, 81)
; Output:
;    TRUE  sucessful
;    FALSE otherwise
;
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bnd.asv  $
;  
;     Rev 1.6   29 Nov 1994 13:04:56   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.0   19 Oct 1993 07:18:44   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:30:00   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.2.0   03 Sep 1993 19:28:44   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   21 Jun 1993 17:09:58   SZIADEH
;  Modified code to work under WINDOWS.
;  
;     Rev 1.2   14 Jun 1993 14:34:46   SZIADEH
;  Modified code to reduce size.
;-----------------------------------------------------------------------------
REP4_PART_EOFD      PROC USES XBX XCX SDS SES XSI XDI,
                        lpN30:LPSTR,
                        lpBuffer:LPSTR,
                        lpSEG_OFF:LPSTR,
                        byDrive:BYTE

                        LOCAL uCylinder:WORD
                        LOCAL bySector:BYTE
                        LOCAL byHead:BYTE

                        LOCAL Add_Sect:BYTE
                        LOCAL Fix_Boot:BYTE
                        LOCAL Fix_Part:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-----------------------------------------------------------------------------
; Get add_to_sector value from clean info in buffer

        LDS_SI  lpN30

        mov     ax,word ptr[XSI].N30.HardLoc1
        mov     Add_Sect,al

        mov     ax,word ptr[XSI].N30.Restore
        mov     Fix_Part,ah
        mov     Fix_Boot,al
;-----------------------------------------------------------------------------
; lpBuffer contain MBR virus
; Calculate End Sector and add value to point to original MBR sector

        LDS_SI  lpBuffer                        ;Look at buffer
        mov     bySector, 03Fh                  ;bySector=00111111b

        mov     al,byte ptr[XSI+1C3h]           ;Get End: head
        mov     byHead,al                       ;Store Head

        mov     ax,word ptr[XSI+1C4h]           ;Get End: Cyl,Sector
        and     bySector, al
        mov     bl, Add_Sect
        add     bySector,bl                     ;Add to Sector

        and     al, 0c0h                        ;kill first 6 bits (sector)
        mov     cl,6
        shr     al,cl
        xchg    ah,al

        mov     uCylinder,ax                   ;shift to the right
        
R_WRITE:
        INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

        cmp     word ptr[XSI+510],0AA55h        ; Verify as BR
        jnz     R_ERROR

;-----------------------------------------------------------------------------
; Fix Ending Sector value.

        LDS_SI  lpBuffer
        cmp     Fix_Part, 0
        je      @F

        xor     ah,ah
        mov     al,Fix_Part
        add     word ptr[XSI+1C4h],ax
@@:
        INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,0,1,0,byDrive
        or      ax,ax
        jnz     R_ERROR
;-----------------------------------------------------------------------------
; If Boot modified, Read the Boot sector and fix total sector value.

R_BOOT:
        cmp     Fix_Boot,0                      ;Don't fix boot if 0
        je      R_DONE

        INVOKE  DIS4_LOC_BOOTSECTOR,byDrive,lpSEG_OFF  ;get address of boot sector
        cmp     ax, -1                           ; error
        je      R_ERROR

        mov     byHead,ah                        ; head
        mov     bySector,al
        mov     uCylinder,dx                     ; cylinder

        INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

        LDS_SI  lpBuffer                        ;Look at MBR in buffer
        cmp     byte ptr[XSI],0E9h              ;Verify as Boot sector
        je      @F

        cmp     byte ptr[XSI],0EBh              ;Verify as Boot sector
        jne     R_EXIT

@@:

        cmp     word ptr[XSI+510],0AA55h        ; Verify signature
        jnz     R_ERROR

        mov     al,Fix_Boot                     ;Fix Total sector value
        add     byte ptr[XSI+13],al

        INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

R_DONE:
        mov     ax,1
        jmp     R_EXIT
;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_PART_EOFD       ENDP
;--------------------------------------
END


;----------------------------------------------------
;  CLEAN                MEANING             EOFD
;  FIELD               FOR BOOT             hi/lo
;                                          (offset)
;----------------------------------------------------
; wInfSize           - Hard Type             -
; wTagSize           - Flop Type             -
; wHostJmp           - Hard Location 1      0/add
; wJmpLen            - Hard Location 2       -
; wSS_reg            - Flop 360              -
; wSP_reg            - Flop 720              -
; wCS_reg            - Flop 1.2              -
; wIP_reg            - Flop 1.44             -
; wHeader            - Flop 2.88             -
; wHeadLen           - Flop All              -
; wBookMarkLocation  - Restore Word        par/boo
; wBookMark          - Special Flags         -
;----------------------------------------------------

