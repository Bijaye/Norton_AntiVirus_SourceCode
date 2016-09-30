;-----------------------------------------------------------
; $Header:   S:/INCLUDE/VCS/1HALF.INv   1.4   08 Mar 1996 14:56:18   DALLEE  $
;
; FILE NAME:        1half.inc
;-----------------------------------------------------------
; $Log:   S:/INCLUDE/VCS/1HALF.INv  $
;  
;     Rev 1.4   08 Mar 1996 14:56:18   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   11 May 1995 20:42:24   SZIADEH
;  
;     Rev 1.2   11 May 1995 17:56:56   SZIADEH
;  added ABSDISKREC structure. it's used in getindat.asm.
;  
;     Rev 1.1   11 May 1995 12:08:58   SZIADEH
;  defined HDParamTable for use in GETINDAT.asm
;  
;     Rev 1.0   29 Nov 1994 18:28:10   DALLEE
;  Initial revision.
;  
;     Rev 1.0.1.2   31 Oct 1994 03:35:22   DALLEE
;  modifed the dec_word macro to use SUB instead of DEC. The second does not 
;  set the carry flag.
;-----------------------------------------------------------

SetupDisplay MACRO
    mov     ah,0Fh                  ; Get video mode
    int     10h                     ;
    mov     smode,al                ; Save mode
    mov     vpage,bh                ; Save page for use

    mov     ah, 03h
    int     10h

    mov     row, dh
    mov     col, dl

    ENDM
;-----------------------------------------------------------

locate  MACRO   row,col
        mov     ah, 2
        mov     bh, vpage
        mov     dh, row
        mov     dl, col
        int     10h

        ENDM
;-----------------------------------------------------------
dec_dword MACRO  dwValue
        sub     word ptr [dwValue], 1
        sbb     word ptr [dwValue+2], 0

	ENDM

;-----------------------------------------------------------
;ABSDISKREC STRUC
;   dn                BYTE   0       ; /* Drive number (0,1,0x80,0x81) */
;    startingHead      BYTE   0       ; /* First head (0-based)         */
;    startingTrack     WORD   0       ; /* First track (0-based)        */
;    startingSector    BYTE   0       ; /* First sector (1-based)       */
;    endingHead        BYTE   0       ; /* Ending head # (0-based)      */
;    endingTrack       WORD   0       ; /* Ending track # (0-based)     */
;    endingSector      BYTE   0       ; /* Ending sector # (1-based)    */
;    sectorsPerTrack   BYTE   0       ; /* Sectors on each track        */
;    totalTracks       WORD   0       ; /* Total tracks on disk         */
;    totalHeads        BYTE   0       ; /* Total heads on disk          */
;    head              BYTE   0       ; /* Current head (0-based)       */
;    track             WORD   0       ; /* Current track (0-based)      */
;    sector            BYTE   0       ; /* Current sector (1-based)     */
;    numSectors        BYTE   0       ; /* Number of sectors to use     */
;    padding           WORD   0       ; /* Put buffer on DWORD boundary */
;    buffer            LPBYTE 0       ; /* Ptr. to buffer to use        */
;    sectorsProcessed  BYTE   0       ; /* Number of sectors processed*/
;    CMOS_Type         BYTE   0       ; /* CMOS drive type              */
;                                     ; /* (Floppies only)              */
;                                     ; /*   1 - 360K                   */
;                                     ; /*   2 - 1.2M                   */
;                                     ; /*   3 - 720K                   */
;                                     ; /*   4 - 1.44M                  */
;    numDrives         BYTE   0       ; /* Num. drives (floppy or hard) */
;    sectorLenCode     BYTE   0       ; /* Sector length code       */
;                                     ; /* (NEC Floppy             or HD only)  */
;                                     ; /* code  :   0                1    2    3   */
;                                     ; /* length: 128              256  512 1024   */
;ABSDISKREC ENDS

;-----------------------------------------------------------

RESTORE_INFO STRUC
    StartCyl    WORD 0
    StartHead   BYTE 0
    StartSec    BYTE 0
    MaxCyl      WORD 0
    MaxHead     BYTE 0
    MaxSec      BYTE 0
    Key         WORD 0
    MbrOffset   WORD 0
RESTORE_INFO ENDS

sRESTORE_INFO       EQU  SIZE RESTORE_INFO

NAVSIG_OFFSET       EQU  10h
FLAG_OFFSET         EQU  13h
HEAD_SECTOR_OFFSET  EQU  1fah



if FARCALL
OneHalfRestoreDisk PROTO FAR,:LPBYTE

else

OneHalfRestoreDisk PROTO NEAR,:LPBYTE
endif

