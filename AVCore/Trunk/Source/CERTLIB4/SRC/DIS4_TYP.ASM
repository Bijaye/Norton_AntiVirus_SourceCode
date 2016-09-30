include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_ANALYZE_FLOP                1/30/93                      JOE
;
;     Analyzes floppy type by looking for FATs.
; $Header:   S:\certlib4\vcs\dis4_typ.asv   1.7   29 Nov 1994 13:10:02   DALLEE  $
;
; UINT WINAPI REP4_ANALYZE_TYPE(BYTE byDrive, LPSTR lpSEG_OFF);
;
; Input:
;       1) Drive Number, logical as BYTE.
;       2) Ptr to 512k (minimum) buffer
;
; Output:
;       1) FALSE(0) Failed on reads or unknown type.
;       2) Diskette Type verified as:
;
;               1 = 360k
;               2 = 720k
;               3 = 1.2m
;               4 = 1.44m
;               5 = 2.88m
;
;       3) Buffer has first directory sector in it for bootable testing.
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dis4_typ.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:10:02   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.0   19 Oct 1993 07:27:12   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:23:00   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.2.0   03 Sep 1993 19:27:08   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   24 Jun 1993 19:49:42   SZIADEH
;  Second FAT is found at sector 4 (not 3) in the case of a 360K.
;  
;     Rev 1.1   21 Jun 1993 16:58:00   SZIADEH
;  Modified code to work under WINDOWS...
;  
;     Rev 1.0   10 May 1993 16:29:18   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
DIS4_ANALYZE_FLOP       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        byDrive:BYTE,
                        lpSEG_OFF:LPSTR,
                        lpSEL_OFF:LPSTR

                        LOCAL Sector:BYTE
                        LOCAL MediaID:BYTE
                        LOCAL TypeIs:WORD

;-------------------------------------
        xor     XAX,XAX                 ; Read Logical Sector 1 (1st FAT)
        INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,0,2,0,byDrive ;1st FAT byCount, uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jz      @F
        jmp     R_ERROR                 ; Exit on read error
@@:
      
ifdef WIN16
        LDS_SI  lpSEL_OFF               ; WIN Set ds:si to buffer
else
        LDS_SI  lpSEG_OFF               ; DOS
endif

        mov     al,byte ptr[si]         ; Get and store
        mov     MediaID,al              ; Media ID byte

        cmp     MediaID,0FDh            ; Is it 360k?
        je      IS_360

        cmp     MediaID,0F9h            ; Is it 720k or 1.2m?
        je      IS_720

        cmp     MediaID,0F0h            ; Is it 1.44m?
        je      IS_144

        cmp     MediaID,0FFh            ; Is it 2.88m?
        je      IS_288

        jmp     R_ERROR                 ; Type non-standard or corrupted.

IS_360:
        mov     Sector,4                ; Second FAT at logical sector 3
        mov     TypeIs,byte ptr 1
        jmp     R_READ

IS_720:
        mov     Sector,5                ; Second FAT at logical sector 4
        mov     TypeIs,byte ptr 2
        jmp     R_READ
        
IS_144:
        mov     Sector,11               ; Second FAT at logical sector 10
        mov     TypeIs,byte ptr 4
        jmp     R_READ

IS_288:
        mov     Sector,11               ; Second FAT at logical sector 10
        mov     TypeIs,byte ptr 5
R_READ:

        INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,0,Sector,0,byDrive ;
        or      ax,ax
        jnz     R_ERROR                 ; Exit on read error

        mov     al,MediaID              ; Get Media ID from 1st FAT
        cmp     byte ptr[si],al         ; Cmp Media ID from 2nd FAT.
        jz      R_OK
        cmp     TypeIs,2                ; If 720k check now try 1.2m        
        jnz     R_ERROR                 ;
        mov     Sector,9                ; Second FAT at logical sector 8
        mov     TypeIs,byte ptr 3       ; TypeIs <- 1.2
        jmp     R_READ                  ; try again

;--------------------------------------
R_OK:
        mov     ax,TypeIs               ; Put type into ax for return.
        jmp     R_EXIT
;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

DIS4_ANALYZE_FLOP       ENDP
;--------------------------------------
END

