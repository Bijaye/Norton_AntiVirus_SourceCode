;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/INCLUDE/VCS/REPAIRN.INv   1.8   10 Apr 1996 15:10:36   DALLEE  $
;;
;; Description:  Declares function prototypes, macros, equates and types
;;               for file and boot repair functions in CERTLIBN project.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/INCLUDE/VCS/REPAIRN.INv  $
;  
;     Rev 1.8   10 Apr 1996 15:10:36   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.7   08 Mar 1996 14:56:18   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.6   10 Nov 1995 16:30:36   GDZIECI
;  Removed second prototype for REP4_SYS_NORM.
;  
;     Rev 1.5   23 Aug 1994 20:26:06   RSTANEV
;  Added some prototypes so that compiles for VxD platform.
;  WARNING:  THIS FILE SHOULD BE MOVED TO VIRLIB and should not be used
;  by Quake!!!!!!!
;
;     Rev 1.4   26 May 1994 14:56:50   RSTANEV
;  Removed a prototype.
;
;     Rev 1.3   26 May 1994 09:46:04   RSTANEV
;  Restored repair function prototypes.
;
;     Rev 1.2   23 May 1994 19:44:10   RSTANEV
;  Enabled file repair functions.
;
;     Rev 1.1   11 May 1994 16:45:20   RSTANEV
;  Enabled some function prototypes for REP4_DCR.ASM.
;
;     Rev 1.0   05 May 1994 12:46:58   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

ObjectBootFile          EQU     32

REP_MBR_PHYSICAL        EQU     01
REP_MBR_SAVE_PHYSICAL   EQU     02
REP_MBR_EOFD            EQU     03
REP_MBR_GENERIC         EQU     04
REP_MBR_UNIQUE          EQU     05

REP_BOOT_PHYSICAL       EQU     11
REP_BOOT_SAVE_PHYSICAL  EQU     12
REP_BOOT_SAVE_LOGICAL   EQU     13
REP_BOOT_GENERIC        EQU     14
REP_BOOT_UNIQUE         EQU     15

REP_EXEC_CODE           EQU     20

bySecMask               EQU     01
byHeadMask              EQU     02
byCylMask               EQU     04

;----------------------------------------------------------------------------
; Repair Structs
;----------------------------------------------------------------------------
FIX  STRUCT

wAX      WORD  0                ; Data area for register storage
wBX      WORD  0                ;
wCX      WORD  0                ;
wDX      WORD  0                ;
wSI      WORD  0                ;
wDI      WORD  0                ;
wSizehi  WORD  0                ; Hi byte of file size
wSizelo  WORD  0                ; Lo ditto
wScanhi  WORD  0                ; Ptr for prepending removal
wScanlo  WORD  0                ; Lo ditto
wGraph   WORD  0                ; Repair size in paragraphs (512 bytes)
wModula  WORD  0                ; Repair size MOD 512
wPass    WORD  0                ; Counter
wHand    WORD  0                ; Handle for N30
wVIDS    WORD  0                ; Number of entries in N30

FIX  ENDS

sFIX  EQU  SIZE FIX

;----------------------------------------------------------------------------

RepairStruct  STRUCT

xN30      N30   <>              ; N30 entry structure
xFIX      FIX   <>              ; Repair
wMZ       WORD  "ZM"            ; MZ
wModlen   WORD  "lM"            ; 02h size mod 512
wParalen  WORD  "lP"            ; 04h size/512
wRelocs   WORD  "eR"            ; 06h reloc tables
wHedsize  WORD  "dH"            ; 08h len of header
wMin      WORD  "nM"            ; 0Ah minimum memory
wMax      WORD  "xM"            ; 0Ch maximum memory
w_ss      WORD  "sS"            ; 0Eh stack segment
w_sp      WORD  "pS"            ; 10h stack pointer
wChksum   WORD  "kC"            ; 12h check sum
w_ip      WORD  "pI"            ; 14h instruction pointer
w_cs      WORD  "sC"            ; 16h code segment

RepairStruct  ENDS

sREPAIR  =  SIZE RepairStruct

;----------------------------------------------------------------------------
;                        FILE REPAIR PROTOTYPES
;----------------------------------------------------------------------------

REP4_ME                 PROTO CODEDISTANCE,:WORD,:UINT,:LPSTR,:LPBYTE,:UINT,:LPSTR
REP4_CALLS              PROTO CODEDISTANCE,:UINT,:UINT,:LPBYTE
REP4_PREPENT            PROTO CODEDISTANCE,:UINT,:UINT,:LPBYTE
REP4_EXE_NORM           PROTO CODEDISTANCE,:UINT,:UINT,:LPBYTE
REP4_GENERIC            PROTO CODEDISTANCE,:UINT,:UINT,:LPBYTE
REP4_SYS_NORM           PROTO CODEDISTANCE,:UINT,:UINT,:LPBYTE
REP4_DECRYPT            PROTO CODEDISTANCE,:UINT,:LPBYTE
DETERMINE_XOR_BYTE      PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_XOR_BYTE        PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD
DETERMINE_ADD_BYTE      PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_ADD_BYTE        PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD
DETERMINE_XOR_WORD      PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_XOR_WORD        PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD,:WORD
DETERMINE_ADD_WORD      PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_ADD_WORD        PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD,:WORD
DETERMINE_ADD_OSCIL_WORD  PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_ADD_OSCIL_WORD  PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD,:WORD
DETERMINE_XOR_OSCIL_WORD  PROTO CODEDISTANCE,:LPBYTE,:LPBYTE
DECRYPT_XOR_OSCIL_WORD  PROTO CODEDISTANCE,:UINT,:LPBYTE,:WORD,:WORD

;----------------------------------------------------------------------------
;                          BOOT REPAIR
;----------------------------------------------------------------------------

;DIS4_NEW_BOOT           PROTO CODEDISTANCE,:BYTE,:WORD,:LPSTR
DIS4_ANALYZE_FLOP       PROTO CODEDISTANCE,:BYTE,:LPBYTE

REP4_BOOT               PROTO CODEDISTANCE,:WORD,:LPSTR,:LPBYTE,:UINT,:BYTE
REP4_PHYSICAL           PROTO CODEDISTANCE,:LPBYTE,:BYTE
REP4_SAVE_PHYSICAL      PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:BYTE
REP4_UNIQUE             PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:BYTE,:BYTE
REP4_BOOT_GENERIC       PROTO CODEDISTANCE,:LPBYTE,:BYTE,:BYTE
REP4_PART_EOFD          PROTO CODEDISTANCE,:LPBYTE,:LPBYTE,:BYTE
REP4_MBR_GENERIC        PROTO CODEDISTANCE,:LPBYTE,:BYTE, :BYTE

;----------------------------------------------------------------------------
;                  Aliases for structure pointers
;----------------------------------------------------------------------------

HardType   TEXTEQU  <wInfSize>
FlopType   TEXTEQU  <wTagSize>
HardLoc1   TEXTEQU  <wHostJmp>
HardLoc2   TEXTEQU  <wJmpLen>
Flop_360   TEXTEQU  <wSS_reg>
Flop_720   TEXTEQU  <wSP_reg>
Flop_12    TEXTEQU  <wCS_reg>
Flop_144   TEXTEQU  <wIP_reg>
Flop_288   TEXTEQU  <wHeader>
Flop_All   TEXTEQU  <wHeadlen>
Restore    TEXTEQU  <wBookLocation>
SpecFlags  TEXTEQU  <wBookMark>

;----------------------------------------------------------------------------------------------------
;  CLEAN                MEANING           V_Physical    Physical     V_Logical     EOFD      GENERIC
;  FIELD               FOR BOOT             hi/lo        hi/lo         hi/lo       hi/lo
;                                          (offset)     (offset)                  (offset)
;----------------------------------------------------------------------------------------------------
; wInfSize           - Hard Type             -            -              -          -            -
; wTagSize           - Flop Type             -            -              -          -            -
; wHostJmp           - Hard Location 1     cyl/sec      cylinder      offset       0/add         -
; wJmpLen            - Hard Location 2     head/add     head/sec         -          -            -
; wSS_reg            - Flop 360            cyl/sec      cyl/sec       offset        -            -
; wSP_reg            - Flop 720            cyl/sec      cyl/sec       offset        -            -
; wCS_reg            - Flop 1.2            cyl/sec      cyl/sec       offset        -            -
; wIP_reg            - Flop 1.44           cyl/sec      cyl/sec       offset        -            -
; wHeader            - Flop 2.88           cyl/sec      cyl/sec       offset        -            -
; wHeadLen           - Flop All            head/add     head/0        0/add         -            -
; wBookMarkLocation  - Restore Word         NOTE1          -             -        par/boo        -
; wBookMark          - Special Flags        NOTE2          -           NOTE3        -            -
;----------------------------------------------------------------------------------------------------
;------------------------------------------------------------------------------------------
;V_PHYSICAL:
;        Offsets within the virus for: CYL, HEAD, and SEC of the original
;        sector.
;        ADD can be used for adjusments.
;
;NOTE1:
; Can be used in situations where the offset for any of the fields
; does not fit within 1 byte.
;
; if sec does not fit with in 1 byte, use this value:
;    0000 0000 0000 0001
;
; if head does not fit with in 1 byte, use this value:
;    0000 0000 0000 0010
;
; if cyl does not fit with in 1 byte, use this value:
;    0000 0000 0000 0100
;
; Above values may be ORed.
;
;
;NOTE2: (HARD DISK only)
;
; Default:
;    One byte is read from offset cyl.
;
; Special case:
;    set this field to: 0000 0000 0000 0001
;    to read a WORD (2 bytes) from offset cyl.
;
;    This can be used in the case of a large hard disk drive where the
;    the original sector is stored toward the end of the disk, and the virus
;    used 2 bytes to store the cylinder address. If this bit is not
;    set, only one byte is read.
;-----------------------------------------------------------------------------------------------------
;
;PHYSCIAL:
;        CYL,HEAD, SEC are the address of the original sector.
;        ADD can be used for adjustments.
;-----------------------------------------------------------------------------------------------------
;V_LOGICAL:
;        Offset with in the virus to find logical address of original sector.
;        ADD can be used for adjusments.
; NOTE3:
;   Default: BYTE is read. Set flags to 1 to read a WORD.
;-----------------------------------------------------------------------------------------------------
;EOFD:
;  Hard Disk: Applies to affected partiton only.
;    ADD:
;       Value to add to END_SECTOR field in the INFECTED MBR to get to
;       the original MBR. (Normally 1)
;    PAR:
;       Value to add to END_SECTOR field in ORIGINAL MBR. This is the
;       number of sectors the virus occupies. (== Virus Size in Sectors)
;    BOO:
;       Value to add to TOTAL_SECTORS field in the boot sector. This is the
;       number of sectors the virus subtracted from this field to so that
;       the viral code is not accessable. (== Virus Size in Sectors)
;       0 if the field did not change.
;
;-----------------------------------------------------------------------------------------------------
;GENERIC:
;   Floppy:
;     Generic Boot Code will be written to sector 0,0,1. The code may or may
;     not be bootable.
;   Hard:
;     Not yet available.
;-----------------------------------------------------------------------------------------------------
