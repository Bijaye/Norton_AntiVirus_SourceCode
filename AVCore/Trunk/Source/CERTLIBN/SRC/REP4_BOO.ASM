;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_BOO.ASv   1.11   10 Apr 1996 15:10:44   DALLEE  $
;;
;; Description: Defines REP4_BOOT function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_BOO.ASv  $
;  
;     Rev 1.11   10 Apr 1996 15:10:44   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.10   05 Feb 1996 20:15:46   RSTANEV
;  Fixed SYM_NTK platform.
;
;     Rev 1.9   11 Jan 1996 16:21:30   JWORDEN
;  Port changes from Quake 8
;
;     Rev 1.8.1.1   18 Jul 1995 20:48:04   SZIADEH
;  Added code to properly calculate the uCylinder value for large
;  drives prior to reading/writing the sector out.
;
;     Rev 1.8.1.0   12 Jul 1995 18:58:08   MPYE
;  Branch base for version QAK8
;
;     Rev 1.8   07 Jul 1995 21:15:56   RSTANEV
;  Fixed a big problem with incorrectly prototyped and incorrectly accessed
;  REP4_BOOT_LoadN30Callback.
;
;     Rev 1.7   03 Mar 1995 13:39:22   RSTANEV
;  Duh.
;
;     Rev 1.6   03 Mar 1995 13:03:22   RSTANEV
;  Now loading N30 records in a standard way...
;
;     Rev 1.5   22 Aug 1994 18:37:26   DALLEE
;  Commented out SCN4_EXEC -- breaks .DX platform.
;  We're no longer using this update scheme (hopefully).
;
;     Rev 1.4   17 Aug 1994 13:25:56   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.3   15 Aug 1994 19:07:30   SZIADEH
;  Added @@ for the forward jump in the floppy boot repair section.
;
;     Rev 1.2   26 May 1994 08:33:32   RSTANEV
;  Fixed a reference to unknown parameter.
;
;     Rev 1.1   25 May 1994 16:35:10   RSTANEV
;  Fixed a parameter passing error.
;
;     Rev 1.0   25 May 1994 14:54:30   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

IF bSYM_VXD OR bSYM_WIN32 OR bSYM_NLM OR bSYM_NTK

  LoadN30Proto  TYPEDEF  PROTO  NEAR,:LPBYTE,:UINT,:LPBYTE
  LoadN30ProtoPtr  TYPEDEF  PTR  LoadN30Proto

  PUBLIC REP4_BOOT_LoadN30Callback
  PUBLIC REP4_BOOT_LoadN30Context

  REP4_BOOT_LoadN30Callback  LoadN30ProtoPtr  0
  REP4_BOOT_LoadN30Context   DD               0

ENDIF

;----------------------------------------------------------------------------
; REP4_BOOT
;
; Decides on repair routine and calls it.
;
; BOOL WINAPI REP4_BOOT ( WORD VID, LPSTR lpDatafile, LPVOID lpBuffer,
;                         UINT uBufferSize, BYTE byDrive );
;
; Input:
;       1) VID          Virus Entry Number
;       2) lpDatafile   path name for VIRSCAN.DAT
;       3) lpBuffer     Work Buffer (Min 512+ sizeof(N30))
;       4) uBuffer      Size of Buffer (must be size of N30 record + 512 or greater)
;       5) byDrive      Absolute drive number (0,1,2,...,7F,80,81)
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;----------------------------------------------------------------------------
        ECHO Warning: Fix the checks for function results (AL vs AX vs XAX).

REP4_BOOT  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        VID:WORD,                       ; virus id (N30 entry number)
        lpDatafile:LPSTR,               ; database name
        lpBuffer:LPBYTE,                ; Ptr to my work buffer
        uBuffer:UINT,                   ; Size of work buffer
        byDrive:BYTE                    ; This is drive info

        LOCAL   byType:BYTE
        LOCAL   RepairType:BYTE
        LOCAL   byOrgSec:BYTE
        LOCAL   byOrgHead:BYTE
        LOCAL   HARD_BOOT:BYTE
        LOCAL   uOrgCyl:WORD
        LOCAL   lpN30:LPBYTE

        cmp     uBuffer,sN30 + 1024
        jbe     R_ERROR
IF OS_16
        mov     ax,WORD PTR lpBuffer
        mov     bx,WORD PTR lpBuffer + 2

        mov     WORD PTR lpN30,ax
        mov     WORD PTR lpN30 + 2,bx

        add     ax, sN30
        mov     WORD PTR lpBuffer,ax
ELSE
        mov     XAX,lpBuffer
        mov     lpN30,XAX
        add     XAX,sN30
        mov     lpBuffer,XAX
ENDIF

IF bSYM_VXD OR bSYM_WIN32 OR bSYM_NLM OR bSYM_NTK

        cmp     REP4_BOOT_LoadN30Callback,0
        je      NoN30Callback
        movuw   XAX,VID
        INVOKE  [REP4_BOOT_LoadN30Callback], REP4_BOOT_LoadN30Context, XAX, lpN30
        test    eax,eax
        jnz     N30Loaded
NoN30Callback:

ENDIF

        INVOKE  FIL4_OPEN,lpDatafile,fREAD ;open file for read
        cmp     XAX,-1
        je      R_ERROR

        mov     XBX,XAX                 ; save file handle

        INVOKE  SCN4_GET_ENTRY,XBX,VID,lpN30 ; get virus entry
        push    XAX                     ; save result of read
        INVOKE  FIL4_CLOSE,XBX          ; close file
        pop     XAX                     ; restore result of read

        or      ax,ax                   ; return zero on error
        jz      R_ERROR
N30Loaded:
        LDS_SI  lpN30

        mov     byType, 0
        mov     HARD_BOOT,FALSE
        cmp     byDrive,080h
        jb      REPAIR_BOOT

REPAIR_HARD:

        mov     bx,[XSI].RepairStruct.xN30.HardType
        or      bx,bx
        jz      R_ERROR

        mov     RepairType,bl

        INVOKE  DIS4_READ_MBR,byDrive,lpBuffer ; read MBR
        inc     XAX
        jz      R_ERROR


        mov     bl, RepairType
        cmp     bl, REP_MBR_PHYSICAL
        je      LAB_PHYSICAL

        cmp     bl, REP_MBR_SAVE_PHYSICAL
        je      LAB_SAVE_PHYSICAL

        cmp     bl, REP_MBR_GENERIC
        je      LAB_MBR_GENERIC

        cmp     bl, REP_MBR_EOFD
        je      LAB_MBR_EOFD

        cmp     bl, REP_MBR_UNIQUE
        je      LAB_UNIQUE

        cmp     bl, REP_EXEC_CODE
        je      LAB_EXEC_CODE

        ; else it has to be a hard disk boot repair
        mov     HARD_BOOT,TRUE          ; set hard boot
        xor     XAX,XAX
        jmp     @F

REPAIR_BOOT:

        mov     bx,[XSI].RepairStruct.xN30.FlopType
        or      bx,bx
        jz      R_ERROR

        mov     RepairType, bl
        INVOKE  DIS4_ANALYZE_FLOP,byDrive,lpBuffer
        or      al,al
        jz      R_ERROR

@@:
        mov     byType, al              ; zero is hard disk
        INVOKE  DIS4_READ_BOOTSECTOR,byDrive,lpBuffer
        inc     XAX
        jz      R_ERROR

        mov     bl, RepairType

        cmp     bl, REP_BOOT_PHYSICAL
        je      LAB_PHYSICAL

        cmp     bl, REP_BOOT_SAVE_PHYSICAL
        je      LAB_SAVE_PHYSICAL

        cmp     bl, REP_BOOT_GENERIC
        je      LAB_BOOT_GENERIC

        cmp     bl, REP_BOOT_UNIQUE
        je      LAB_UNIQUE

        cmp     bl, REP_EXEC_CODE
        je      LAB_EXEC_CODE

        jmp     R_ERROR

;----------------------------------------------------------------------------
;                         JMP Table
; At this point, lpBuffer contains the infected Boot/MBR sector.
;----------------------------------------------------------------------------

LAB_PHYSICAL:

        INVOKE  REP4_PHYSICAL,lpN30,byType
        cmp     ax,-1
        jne     R_WRITE

        jmp     LAB_BOOT_GENERIC        ; try generic boot repair

LAB_SAVE_PHYSICAL:

        INVOKE  REP4_SAVE_PHYSICAL,lpN30,lpBuffer,byType
        cmp     ax,-1
        jne     R_WRITE

        jmp     LAB_BOOT_GENERIC        ; try generic boot repair

LAB_MBR_EOFD:

        INVOKE  REP4_PART_EOFD,lpN30,lpBuffer,byDrive
        or      ax,ax
        jnz     R_DONE

        jmp     LAB_MBR_GENERIC

LAB_UNIQUE:

        INVOKE  REP4_UNIQUE,lpN30,lpBuffer,byDrive,byType
        or      ax,ax                   ; zero if error occured
        jnz     R_DONE

        jmp     R_ERROR

LAB_EXEC_CODE:

;&? Kills .DX platform.  -DALLEE 08/22/94
; IFDEF SYM_DOS
IF 0
                                        ; copy path\VIRSCAN.DAT to start
                                        ; of buffer
        LDS_SI  lpDatafile
        LES_DI  lpBuffer
        mov     XCX,MAX_PATH_SIZE + 1
        rep     movsb

        LDS_SI  lpN30
        cmp     byDrive,080h
        jb      do_floppy
        add     XSI, RepairStruct.xN30.HardLoc1 ; get 2 bytes file name
        jmp     SHORT over_floppy
do_floppy:
        add     XSI, RepairStruct.xN30.Flop_All ; get 2 bytes file name
over_floppy:
        add     WORD PTR XDS[XSI], 0101h        ; encrypt
        dec     XSI                             ; adjust si
        INVOKE  SCN4_EXEC,VID,lpBuffer,lpN30,vSDS XSI
        or      ax,ax
        jnz     R_DONE
ENDIF
        jmp     R_ERROR


LAB_BOOT_GENERIC:

        INVOKE  REP4_BOOT_GENERIC,lpBuffer,byDrive,byType
        or      ax,ax
        jnz     R_DONE

        ; Fall through in case it is MBR repair.
        ; REP4_MBR_GENERIC returns an error if the the byDrive
        ; is other than 0x80

LAB_MBR_GENERIC:
        INVOKE REP4_MBR_GENERIC,lpBuffer,byDrive,RepairType
        or     ax,ax
        jnz    R_DONE
        jmp    R_ERROR

R_WRITE:

                                        ; dx = cyl
                                        ; al = sec
                                        ; ah = head
        mov     byOrgHead, ah

        mov     byOrgSec, al
        and     byOrgSec, 03fh          ; kill last two bits

        mov     cl, 6                   ;
        shr     al, cl                  ; get last 2 bits of cyl value
        mov     dh, al                  ; merge the two hi bits
        mov     uOrgCyl,dx              ; dx has 10-bit cyl value

        xor     XDX,XDX                 ; CYL=DX=0 for write,
        mov     bx, 1                   ; dx:ax first physical sector
                                        ; which is 0,0,1
        cmp     HARD_BOOT,FALSE         ; Am I fixing Hard boot?
        je      @F                      ; NO! jmp

                                        ; get location of boot sector
                                        ; on hard disk
        INVOKE  DIS4_LOC_BOOTSECTOR,byDrive,lpBuffer
        cmp     ax,-1
        je      R_ERROR
        mov     bx,ax                   ; bx=head,sec

@@:
        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uOrgCyl,byOrgSec,byOrgHead,byDrive
        or      XAX,XAX
        jnz     LAB_BOOT_GENERIC        ; Give generic a try...

        LDS_SI  lpBuffer
        cmp     WORD PTR[XSI+510],0AA55h; Verify as BR
        jnz     LAB_BOOT_GENERIC        ; Give generic a try...

        ; Invoke verify function if it becomes necessary.

        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,dx,bl,bh,byDrive
        or      ax,ax
        jnz     R_ERROR

R_DONE:
        mov     XAX,TRUE
        jmp     SHORT R_EXIT
R_ERROR:
        xor     XAX,XAX
R_EXIT:
        ret

REP4_BOOT  ENDP

UniqueSegmentEnd
END
