include CTS4.INC
include REPAIR.INC

if FARCALL
   SCN4_EXEC       PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR
else
   SCN4_EXEC       PROTO NEAR,:WORD,:LPSTR,:LPSTR,:LPSTR
endif

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_BOOT                                                           SAMIZ
;
;     Decides on repair routine and calls it.
;
; $Header:   S:\certlib4\vcs\rep4_boo.asv   1.16   29 Nov 1994 13:07:56   DALLEE  $
;
; UINT WINAPI REP4_BOOT(WORD, LPSTR , LPSTR, WORD, BYTE);
; Input:
;       1) VID          Virus Entry Number
;       2) lpDatafile   path name for VIRSCAN.DAT
;       3) lpBuffer     Work Buffer (Min 512+ sizeof(N30))
;       4) uBuffer      Size of Buffer (must be size of N30 record + 512 or greater)
;       5) byDrive      Absolute drive number (0,1,2, ...,7F, 80,81)
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_boo.asv  $
;  
;     Rev 1.16   29 Nov 1994 13:07:56   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.11.1.3   12 Dec 1993 15:37:40   SZIADEH
;  Removed all references to REP4_SAVE_LOGICAL().
;  
;     Rev 1.11.1.2   06 Dec 1993 17:19:32   SZIADEH
;  Added code to support execution of external code in boot detection
;  and repair provided via VIRSPxx.DAT files.
;  
;     Rev 1.11.1.1   01 Nov 1993 15:40:06   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.11.1.0   19 Oct 1993 07:26:34   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.11   12 Oct 1993 16:36:52   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.10.2.1   07 Oct 1993 18:42:52   MFALLEN
;  Function accepting ANSI string in Windows.
;  
;     Rev 1.10.2.0   03 Sep 1993 19:28:04   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.10   30 Jul 1993 18:36:30   SZIADEH
;  Added byType parameter to REP4_UNIQUE.
;  
;     Rev 1.9   26 Jul 1993 12:10:56   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.8   19 Jul 1993 14:15:30   SZIADEH
;  Generic MBR repair.
;  
;     Rev 1.7   28 Jun 1993 16:29:56   SZIADEH
;  Added disk type to parameter list in generic boot repair
;  
;     Rev 1.6   27 Jun 1993 21:01:20   SZIADEH
;  Call generic boot repair should original BR is not valid.
;  
;     Rev 1.5   21 Jun 1993 18:31:08   SZIADEH
;  call floppy generic repair.
;  
;     Rev 1.4   21 Jun 1993 16:58:34   SZIADEH
;  Modified code to work under WINDOWS...
;  
;     Rev 1.3   17 Jun 1993 15:22:54   SZIADEH
;  corrected the code that checks for the valid boot sector.
;  
;     Rev 1.2   14 Jun 1993 14:35:14   SZIADEH
;  Modified code to reduce size
;-----------------------------------------------------------------------------
REP4_BOOT       PROC USES XBX XCX XDX SDS SES XSI XDI,
                VID:WORD,               ; virus id (N30 entry number)
                lpDatafile:LPSTR,       ; database name
                lpBuffer:LPSTR,         ; Ptr to my work buffer
                uBuffer:WORD,           ; Size of work buffer
                byDrive:BYTE            ; This is drive info

                LOCAL byType:BYTE
                LOCAL RepairType:BYTE
                LOCAL byOrgSec:BYTE
                LOCAL byOrgHead:BYTE
                LOCAL HARD_BOOT:BYTE
                LOCAL uOrgCyl:WORD
                LOCAL lpN30:LPSTR
                LOCAL lpSEG_OFF:LPSTR
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, lpDatafile, lpDatafile
@@:
ENDIF

;--------------------------------------
        mov     XAX, 1024
        add     XAX, sN30
        cmp     XAX, uBuffer
        ja      R_ERROR                             ;must be at least 1024+sN30 bytes
;--------------------------------------
        push    word ptr lpBuffer                  ;reserve space fon N30 stuff
        pop     word ptr lpSEG_OFF                 ;at the start of the
        push    word ptr lpBuffer+2                ;work buffer
        pop     word ptr lpSEG_OFF+2               ;
        add     word ptr lpSEG_OFF, sN30

ifdef WIN16
        mov     ax, word ptr lpBuffer+2            ;
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax,ax
        jne     @F

        jmp     R_ERROR
@@:
        mov     word ptr lpBuffer+2, ax
endif

;--------------------------------------
        push    word ptr lpBuffer                  ;reserve space fon N30 stuff
        pop     word ptr lpN30                     ;at the start of the
        push    word ptr lpBuffer+2                ;work buffer
        pop     word ptr lpN30+2                   ;

        add     word ptr lpBuffer, sN30             ;new start work buffer
;--------------------------------------
        INVOKE  FIL4_OPEN,lpDatafile,fREAD          ;open file for read
        cmp     ax, -1
        jnz     @F                                  ;no errors, continue

        jmp     R_ERROR                             ;get out!
@@:
        mov     bx,ax                               ;save file handle

        INVOKE  SCN4_GET_ENTRY,bx,VID,lpN30         ;get virus entry
        push    ax                                  ;save result of read
        INVOKE  FIL4_CLOSE,bx                       ;close file
        pop     ax                                  ;restore result of read

        or      ax,ax                               ;return zero on error
        jnz     @F
        jmp     R_ERROR

;--------------------------------------
@@:
        LDS_SI  lpN30

        mov     byType, 0                           ;
        mov     HARD_BOOT,0
        cmp     byDrive,080h                        ;
        jb      REPAIR_BOOT                         ;

REPAIR_HARD:

        mov     bx, word ptr[XSI].RepairStruct.xN30.HardType
        cmp     bx, 0
        je      R_ERROR

        mov     RepairType, bl

        INVOKE  DIS4_READ_MBR, byDrive, lpSEG_OFF   ;read MBR
        cmp     ax,-1
        jz      R_ERROR

        cmp     bx, REP_MBR_PHYSICAL
        je      LAB_PHYSICAL

        cmp     bx, REP_MBR_SAVE_PHYSICAL
        je      LAB_SAVE_PHYSICAL

        cmp     bx, REP_MBR_GENERIC
        je      LAB_MBR_GENERIC

        cmp     bx, REP_MBR_EOFD
        je      LAB_MBR_EOFD

        cmp     bx, REP_MBR_UNIQUE
        je      LAB_UNIQUE

        cmp     bx, REP_EXEC_CODE
        je      LAB_EXEC_CODE

        ;else it has to be a hard disk boot repair
        mov     HARD_BOOT, 1                        ; set hard boot
        XOR     XAX,XAX
        jmp     @F

REPAIR_BOOT:

        mov     bx, word ptr[XSI].RepairStruct.xN30.FlopType
        cmp     bx, 0
        je      R_ERROR

        mov     RepairType, bl
        INVOKE  DIS4_ANALYZE_FLOP,byDrive,lpSEG_OFF, lpBuffer
        or      ax,ax
        jnz     @F

        jmp     R_ERROR

@@:
        mov     byType, al                          ; zero is hard disk
        INVOKE  DIS4_READ_BOOTSECTOR, byDrive,lpSEG_OFF
        cmp     ax,-1
        jz      R_ERROR

        cmp     bx, REP_BOOT_PHYSICAL
        je      LAB_PHYSICAL

        cmp     bx, REP_BOOT_SAVE_PHYSICAL
        je      LAB_SAVE_PHYSICAL

        cmp     bx, REP_BOOT_GENERIC
        je      LAB_BOOT_GENERIC

        cmp     bx, REP_BOOT_UNIQUE
        je      LAB_UNIQUE

        cmp     bx, REP_EXEC_CODE
        je      LAB_EXEC_CODE

        jmp     R_ERROR

;----------------------------------------------------------------------------
;                         JMP Table
; At this point, lpBuffer contains the infected Boot/MBR sector.
;----------------------------------------------------------------------------

LAB_PHYSICAL:

    INVOKE  REP4_PHYSICAL, lpN30, byType
    cmp     ax, -1
    jne     R_WRITE

    jmp     LAB_BOOT_GENERIC        ;try generic boot repair

LAB_SAVE_PHYSICAL:

    INVOKE  REP4_SAVE_PHYSICAL, lpN30, lpBuffer, byType
    cmp     ax, -1
    jne     R_WRITE

    jmp     LAB_BOOT_GENERIC        ;try generic boot repair

LAB_MBR_EOFD:

    INVOKE  REP4_PART_EOFD, lpN30, lpBuffer, lpSEG_OFF, byDrive
    or      ax,ax
    jne     R_DONE

    jmp     LAB_MBR_GENERIC

LAB_UNIQUE:

    INVOKE  REP4_UNIQUE, lpN30, lpBuffer, lpSEG_OFF, byDrive, byType
    or      ax, ax                               ; zero if error occured
    jne     R_DONE

    jmp     R_ERROR

LAB_EXEC_CODE:
    xor     ax,ax
ifdef DOS16

;copy path\VIRSCAN.DAT to start of buffer

    LDS_SI lpDatafile
    LES_DI lpBuffer
    mov     cx, MAX_PATH_SIZE+1
    rep     movsb

    LDS_SI  lpN30
    cmp     byDrive,080h                        ;
    jb      do_floppy
    add     XSI, RepairStruct.xN30.HardLoc1     ; get 2 bytes file name
    jmp     short over_floppy
do_floppy:
    add     XSI, RepairStruct.xN30.Flop_All     ; get 2 bytes file name
over_floppy:
    add     word ptr XDS[XSI], 0101h            ; encrypt
    dec     XSI                                 ; adjust si
    INVOKE  SCN4_EXEC,  VID, lpBuffer, lpN30, vSDS XSI
endif
    or ax,ax
    jne R_DONE

    jmp R_ERROR


LAB_BOOT_GENERIC:

    INVOKE  REP4_BOOT_GENERIC, lpBuffer, lpSEG_OFF, byDrive, byType
    or      ax, ax
    jne     R_DONE
;
;Fall through in case it is MBR repair.
;REP4_MBR_GENERIC returns an error if the the byDrive is other than 0x80
;

LAB_MBR_GENERIC:
    INVOKE REP4_MBR_GENERIC, lpBuffer, lpSEG_OFF, byDrive, RepairType
    or     ax,ax
    jne    R_DONE

    jmp    R_ERROR


;----------------------------------------------------------------------------
;dx = cyl
;al = sec
;ah = head
;

R_WRITE:

    mov     byOrgSec, al
    mov     byOrgHead, ah
    mov     uOrgCyl, dx

    xor     XDX, XDX                             ;CYL=DX=0 for write,
    mov     bx, 1                                ;dx:ax first physical sector
                                                 ;which is 0,0,1
    cmp     HARD_BOOT,0                          ;Am I fixing Hard boot?
    je      @F                                   ;NO! jmp

    ;get location of boot sector on hard disk
    INVOKE  DIS4_LOC_BOOTSECTOR, byDrive, lpSEG_OFF;cyl=dx, sec=al, head=ah
    cmp     ax, -1                                 ;
    je      R_ERROR                                ;
    mov     bx,ax                                  ;bx=head,sec
                                                   ;
@@:                                                ;
    INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,uOrgCyl,byOrgSec,byOrgHead,byDrive;uCylinder,bySector,byHead,byDrive
    or      ax,ax
    jnz     LAB_BOOT_GENERIC                       ; Give generic a try...
                                                   ;
    LDS_SI  lpBuffer                               ;
    cmp     word ptr[XSI+510],0AA55h               ; Verify as BR
    jnz     LAB_BOOT_GENERIC                       ; Give generic a try...

    ;Invoke verify function if it becomes necessary.

    INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,dx,bl,bh,byDrive                        ;uCylinder,bySector,byHead,byDrive
    or      ax,ax
    jnz     R_ERROR

;--------------------------------------
R_DONE:
    mov     ax, 1
    jmp     short R_EXIT
;--------------------------------------
R_ERROR:
    xor     ax,ax
;--------------------------------------
R_EXIT:
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, lpDatafile, lpDatafile
@@:
        pop     ax
ENDIF

    ret
REP4_BOOT   ENDP
END
