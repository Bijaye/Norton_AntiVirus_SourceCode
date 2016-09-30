include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_SAVE_LOGICAL                                     SAMIZ
; $Header:   S:\certlib4\vcs\rep4_slo.asv   1.7   29 Nov 1994 13:04:40   DALLEE  $
;
; int WINAPI REP4_SAVE_LOGICAL (lpN30,lpBuffer,byDrive:BYTE,byType);
;
; Input:
;	 1) lpN30: N30 stuff
;	 2) lpBuffer: buffer containing boot/mbr virus
;    3) byDrive: Physical drive number
;    4) byType:type of drive
; Output:
;	 1) -1 on error
;	 2)  DX: Cylinder
;		 AL: Sector
;		 Ah: Head
; Note:
;   This function works IF AND ONLY IF the virus preserves the BPB.
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_slo.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:04:40   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:22:22   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:32:54   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:29:58   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   17 Jun 1993 15:58:12   BRADR
;  Now passing byDrive to DIS4_LOGICAL_TO_ABSOLUTE.
;  
;     Rev 1.3   17 Jun 1993 15:23:50   SZIADEH
;  uncommented DIS4_LOGICAL_TO_ABSOLUTE function call.
;  
;     Rev 1.2   17 Jun 1993 10:33:40   BRADR
;  Added commented out code to call DIS4_LOGICAL_TO_ABSOLUTE. This
;  function requires another buffer.
;  
;     Rev 1.1   16 Jun 1993 10:01:08   CGUC
;  Temp fix for ABS_TO_LOG
;  
;     Rev 1.0   14 Jun 1993 14:28:22   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_SAVE_LOGICAL      PROC USES XBX XCX SDS SES XSI XDI,
                        lpN30:LPSTR,
                        lpBuffer:LPSTR,
                        byDrive:BYTE,
                        byType:BYTE

                        LOCAL dwSector:DWORD
                        LOCAL bySecAdd:BYTE

                        LOCAL stBPB:BPB_struc

;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpN30


        mov     ax,word ptr[XSI].N30.HardLoc1               ; cylinder/sec offsets
        mov     dx,word ptr[XSI].N30.HardLoc2               ; 0/add
        cmp     byType, 0                                   ; Hard disk
        je      R_CONTINUE

        mov     ax,word ptr [XSI].N30.Flop_360              ; Try 360k
        cmp     byType,1
        je      R_GET_ADD

        mov     ax,word ptr[XSI].N30.Flop_720               ; Try 720k
        cmp     byType,2
        je      R_GET_ADD

        mov     ax,word ptr[XSI].N30.Flop_12                ; Try 1.2m
        cmp     byType,3
        je      R_GET_ADD

        mov     ax,word ptr[XSI].N30.Flop_144               ; Try 1.44m
        cmp     byType,4

        jne     R_ERROR
;-------------------------------------
R_GET_ADD:
        mov     dx,word ptr[XSI].N30.Flop_All
R_CONTINUE:
        mov     bySecAdd, dl            ; adjustment

        mov     word ptr dwSector,0
        mov     word ptr dwSector+2,0

        LDS_SI  lpBuffer                ;
        add     XSI,XAX                 ; Point si to word in virus
        mov     ax,word ptr[XSI]        ; store address of logical sector in ax

        cmp     [XSI].N30.SpecFlags, 1  ; should I keep the 2 bytes?
        je      @F                      ; YES
        xor     ah,ah                   ; No. kill hi byte
@@:
        add     al, bySecAdd            ; adjust field
        mov     word ptr dwSector,ax    ; store logical sector for read
                                        ; in low word
;---------------------------------------;

        INVOKE  DIS4_GET_BPB,byDrive,ADDR stBPB,lpBuffer
        cmp     ax,-1
        je      R_ERROR

        INVOKE  DIS4_LOGICAL_TO_ABSOLUTE,dwSector,byDrive,stBPB.SecPerTrack,stBPB.Heads
        or      ax,ax
        jz      R_ERROR

        jmp     R_EXIT
;--------------------------------------
R_ERROR:
        mov     ax, -1
;--------------------------------------
R_EXIT:
        ret

REP4_SAVE_LOGICAL       ENDP
;--------------------------------------
END

;-------------------------------------------------
;  CLEAN                MEANING          V_Logical
;  FIELD               FOR BOOT            hi/lo
;
;-------------------------------------------------
; wInfSize           - Hard Type             -
; wTagSize           - Flop Type             -
; wHostJmp           - Hard Location 1    offset
; wJmpLen            - Hard Location 2       -
; wSS_reg            - Flop 360           offset
; wSP_reg            - Flop 720           offset
; wCS_reg            - Flop 1.2           offset
; wIP_reg            - Flop 1.44          offset
; wHeader            - Flop 2.88          offset
; wHeadLen           - Flop All           0/add
; wBookMarkLocation  - Restore Word         -
; wBookMark          - Special Flags       NOTE
;-------------------------------------------------
NOTE:
 By default a BYTE is read in for the logical offset of the original boot
 sector. Set this flag in the case where the virus stores the Offset in
 a WORD.




