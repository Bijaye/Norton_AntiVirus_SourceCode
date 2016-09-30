;; Copyright 1995 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/PAM.DAT/VCS/found.asv   1.11   16 Jan 1998 01:24:56   JWILBER  $
;;
;; Description:
;;
;;   Contains ASM source code for the repair foundation.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/PAM.DAT/VCS/found.asv  $
;  
;     Rev 2.0   1 Feb 1999              Andy Raybould
;  Added autoverv verification & repair as Int EB
;
;     Rev 1.11   16 Jan 1998 01:24:56   JWILBER
;  Moved equates for TOF_OFFSET and LOAD_OFFSET to found.inc.
;
;     Rev 1.10   23 Sep 1997 23:24:46   JWILBER
;  Removed trailing spaces.
;
;     Rev 1.9   26 Dec 1996 15:41:06   AOONWAL
;  No change.
;
;     Rev 1.8   12 Jul 1996 09:54:26   CNACHEN
;  Changed CMP CX,0 JNZ Error to CMP CX, FFFF JE Error so foundatation repairs
;  work when stopper signature 1-31 are found.
;
;     Rev 1.7   03 Jul 1996 11:22:18   CRENERT
;  Fixed JC->JS bug while computing truncation point from IP.
;
;     Rev 1.6   03 Jan 1996 14:45:22   DCHI
;  Fixed three bugs:
;  1. Make sure upper twelve bits of AX are zero on success.
;  2. For COM files, if first byte is not E9 and EP is not used, do not error out.
;  3. Do not do any truncation operations (including updating of EXE header
;     image size fields) if no TRUNCATE_ flags specified.
;
;     Rev 1.5   02 Jan 1996 18:04:12   DCHI
;  Moved repair error constants from found.asm to found.inc.
;  Eliminated redundant REP_ERR_NO_COPY constants.
;
;     Rev 1.4   29 Dec 1995 16:17:30   DCHI
;  Added Simple Automatic Repair INT E9.
;
;     Rev 1.3   28 Dec 1995 13:23:30   DCHI
;  Added foundation repair code for appending viruses.
;
;     Rev 1.2   23 Oct 1995 10:58:12   CNACHEN
;  Changed ARPL @ INT21 vector to be an ARPL 090h, 0ffh
;
;     Rev 1.1   20 Oct 1995 17:11:28   CNACHEN
;  removed .386 directive...
;
;     Rev 1.0   20 Oct 1995 15:03:42   CNACHEN
;  Initial revision.
;
;;---------------------------------------------------------------------------

.286
.MODEL TINY

include found.inc

CSEG    SEGMENT 'CODE'

        ORG     0h

        assume  cs:cseg, ds:cseg, es:cseg, ss:cseg

ImageStart:

        dw      0                       ; 00
        dw      0                       ; 01
        dw      0                       ; 02
        dw      0                       ; 03
        dw      0                       ; 04
        dw      0                       ; 05
        dw      0                       ; 06
        dw      0                       ; 07
        dw      0                       ; 08
        dw      0                       ; 09
        dw      0                       ; 0A
        dw      0                       ; 0B
        dw      0                       ; 0C
        dw      0                       ; 0D
        dw      0                       ; 0E
        dw      0                       ; 0F
        dw      0                       ; 10
        dw      0                       ; 11
        dw      0                       ; 12
        dw      0                       ; 13
        dw      0                       ; 14
        dw      0                       ; 15
        dw      0                       ; 16
        dw      0                       ; 17
        dw      0                       ; 18
        dw      0                       ; 19
        dw      0                       ; 1A
        dw      0                       ; 1B
        dw      0                       ; 1C
        dw      0                       ; 1D
        dw      0                       ; 1E
        dw      0                       ; 1F
        dw      0                       ; 20
        dw      offset int_21           ; 21
        dw      0                       ; 22
        dw      0                       ; 23
        dw      0                       ; 24
        dw      0                       ; 25
        dw      0                       ; 26
        dw      0                       ; 27
        dw      0                       ; 28
        dw      0                       ; 29
        dw      0                       ; 2A
        dw      0                       ; 2B
        dw      0                       ; 2C
        dw      0                       ; 2D
        dw      0                       ; 2E
        dw      0                       ; 2F
        dw      0                       ; 30
        dw      0                       ; 31
        dw      0                       ; 32
        dw      0                       ; 33
        dw      0                       ; 34
        dw      0                       ; 35
        dw      0                       ; 36
        dw      0                       ; 37
        dw      0                       ; 38
        dw      0                       ; 39
        dw      0                       ; 3A
        dw      0                       ; 3B
        dw      0                       ; 3C
        dw      0                       ; 3D
        dw      0                       ; 3E
        dw      0                       ; 3F
        dw      0                       ; 40
        dw      0                       ; 41
        dw      0                       ; 42
        dw      0                       ; 43
        dw      0                       ; 44
        dw      0                       ; 45
        dw      0                       ; 46
        dw      0                       ; 47
        dw      0                       ; 48
        dw      0                       ; 49
        dw      0                       ; 4A
        dw      0                       ; 4B
        dw      0                       ; 4C
        dw      0                       ; 4D
        dw      0                       ; 4E
        dw      0                       ; 4F
        dw      0                       ; 50
        dw      0                       ; 51
        dw      0                       ; 52
        dw      0                       ; 53
        dw      0                       ; 54
        dw      0                       ; 55
        dw      0                       ; 56
        dw      0                       ; 57
        dw      0                       ; 58
        dw      0                       ; 59
        dw      0                       ; 5A
        dw      0                       ; 5B
        dw      0                       ; 5C
        dw      0                       ; 5D
        dw      0                       ; 5E
        dw      0                       ; 5F
        dw      0                       ; 60
        dw      0                       ; 61
        dw      0                       ; 62
        dw      0                       ; 63
        dw      0                       ; 64
        dw      0                       ; 65
        dw      0                       ; 66
        dw      0                       ; 67
        dw      0                       ; 68
        dw      0                       ; 69
        dw      0                       ; 6A
        dw      0                       ; 6B
        dw      0                       ; 6C
        dw      0                       ; 6D
        dw      0                       ; 6E
        dw      0                       ; 6F
        dw      0                       ; 70
        dw      0                       ; 71
        dw      0                       ; 72
        dw      0                       ; 73
        dw      0                       ; 74
        dw      0                       ; 75
        dw      0                       ; 76
        dw      0                       ; 77
        dw      0                       ; 78
        dw      0                       ; 79
        dw      0                       ; 7A
        dw      0                       ; 7B
        dw      0                       ; 7C
        dw      0                       ; 7D
        dw      0                       ; 7E
        dw      0                       ; 7F
        dw      0                       ; 80
        dw      0                       ; 81
        dw      0                       ; 82
        dw      0                       ; 83
        dw      0                       ; 84
        dw      0                       ; 85
        dw      0                       ; 86
        dw      0                       ; 87
        dw      0                       ; 88
        dw      0                       ; 89
        dw      0                       ; 8A
        dw      0                       ; 8B
        dw      0                       ; 8C
        dw      0                       ; 8D
        dw      0                       ; 8E
        dw      0                       ; 8F
        dw      0                       ; 90
        dw      0                       ; 91
        dw      0                       ; 92
        dw      0                       ; 93
        dw      0                       ; 94
        dw      0                       ; 95
        dw      0                       ; 96
        dw      0                       ; 97
        dw      0                       ; 98
        dw      0                       ; 99
        dw      0                       ; 9A
        dw      0                       ; 9B
        dw      0                       ; 9C
        dw      0                       ; 9D
        dw      0                       ; 9E
        dw      0                       ; 9F
        dw      0                       ; A0
        dw      0                       ; A1
        dw      0                       ; A2
        dw      0                       ; A3
        dw      0                       ; A4
        dw      0                       ; A5
        dw      0                       ; A6
        dw      0                       ; A7
        dw      0                       ; A8
        dw      0                       ; A9
        dw      0                       ; AA
        dw      0                       ; AB
        dw      0                       ; AC
        dw      0                       ; AD
        dw      0                       ; AE
        dw      0                       ; AF
        dw      0                       ; B0
        dw      0                       ; B1
        dw      0                       ; B2
        dw      0                       ; B3
        dw      0                       ; B4
        dw      0                       ; B5
        dw      0                       ; B6
        dw      0                       ; B7
        dw      0                       ; B8
        dw      0                       ; B9
        dw      0                       ; BA
        dw      0                       ; BB
        dw      0                       ; BC
        dw      0                       ; BD
        dw      0                       ; BE
        dw      0                       ; BF
        dw      0                       ; C0
        dw      0                       ; C1
        dw      0                       ; C2
        dw      0                       ; C3
        dw      0                       ; C4
        dw      0                       ; C5
        dw      0                       ; C6
        dw      0                       ; C7
        dw      0                       ; C8
        dw      0                       ; C9
        dw      0                       ; CA
        dw      0                       ; CB
        dw      0                       ; CC
        dw      0                       ; CD
        dw      0                       ; CE
        dw      0                       ; CF
        dw      0                       ; D0
        dw      0                       ; D1
        dw      0                       ; D2
        dw      0                       ; D3
        dw      0                       ; D4
        dw      0                       ; D5
        dw      0                       ; D6
        dw      0                       ; D7
        dw      0                       ; D8
        dw      0                       ; D9
        dw      0                       ; DA
        dw      0                       ; DB
        dw      0                       ; DC
        dw      0                       ; DD
        dw      0                       ; DE
        dw      0                       ; DF
        dw      0                       ; E0
        dw      0                       ; E1
        dw      0                       ; E2
        dw      0                       ; E3
        dw      0                       ; E4
        dw      0                       ; E5
        dw      0                       ; E6
        dw      0                       ; E7
        dw      0                       ; E8
        dw      int_e9                  ; E9 - Simple automatic repair
        dw      int_ea                  ; EA - Partial automatic repair
        dw      int_eb                  ; EB - Autoverv verification & repair
        dw      0                       ; EC
        dw      0                       ; ED
        dw      0                       ; EE
        dw      0                       ; EF
        dw      0                       ; F0
        dw      0                       ; F1
        dw      0                       ; F2
        dw      0                       ; F3
        dw      0                       ; F4
        dw      0                       ; F5
        dw      0                       ; F6
        dw      0                       ; F7
        dw      0                       ; F8
        dw      0                       ; F9
        dw      0                       ; FA
        dw      0                       ; FB
        dw      0                       ; FC
        dw      0                       ; FD
        dw      0                       ; FE
        dw      0                       ; FF


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

wTempSP         dw      ?
wTempFlags      dw      ?

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

int_21:

        db      63h                     ; ARPL
        db      090h, 0ffh              ; Special repair code...

        pushf
        pop     cs:wTempFlags

        mov     cs:wTempSP, sp
        add     sp, 6                   ; ready SP for push of flags
        push    cs:wTempFlags
        mov     sp, cs:wTempSP

        iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Info flags to be ORed with return value - only use lower 4-bits

REP_INFO_FLAG_COM        equ     0002h
REP_INFO_FLAG_EXE        equ     0001h

; Repair structure offsets

OFFSET_VSIZE            equ     02h
OFFSET_COM_OFFSET       equ     04h
OFFSET_EXE_OFFSET       equ     06h
OFFSET_BOOKMARK_OFFSET  equ     08h
OFFSET_BOOKMARK_COUNT   equ     0Ah
OFFSET_BOOKMARK_ARRAY   equ     0Bh


; The following offsets are relative to the start of the
;  COM repair information structure

OFFSET_COM_EP_CALLBACK  equ     00h     ; Offset of callback pointer
OFFSET_COM_TRUNC_CONST  equ     02h     ; Offset of truncation constant
OFFSET_COM_REP_INFO     equ     04h     ; Offset of COM repair info

; Copy contiguous structure offsets

OFFSET_CONT_SRC_OFFSET  equ     00h     ; Offset of contiguous src offset
OFFSET_CONT_DST_OFFSET  equ     02h     ; Offset of contiguous dst offset
OFFSET_CONT_COUNT       equ     03h     ; Offset of contiguous copy count

; Copy COM staggered structure offsets

OFFSET_COM_STAG_FLAGS   equ     00h     ; Offset of flags
OFFSET_COM_STAG_OFFSETS equ     02h     ; Offset of byte offset array

; The following offsets are relative to the start of the
;  EXE repair information structure

OFFSET_EXE_TRUNC_CONST  equ     00h     ;
OFFSET_EXE_REP_INFO     equ     02h     ; Offset of EXE repair info

; Copy EXE staggered structure offsets with respect to the start of the
; EXE staggered structure.

OFFSET_EXE_FLAGS                equ     00h
OFFSET_EXE_SS_OFFSET            equ     02h
OFFSET_EXE_SP_OFFSET            equ     04h
OFFSET_EXE_IP_OFFSET            equ     06h
OFFSET_EXE_CS_OFFSET            equ     08h
OFFSET_EXE_MINMEM_OFFSET        equ     0Ah
OFFSET_EXE_MAXMEM_OFFSET        equ     0Ch


wInfoFlags      dw      ?

wRepairFlags    dw      ?       ; Stores flags from repair structure

pRepStruct      dw      ?       ; Points to beginning of repair structure

pRepCOMOffset   dw      ?       ; Points to COM repair struct
pRepEXEOffset   dw      ?       ; Points to EXE repair struct

wRefOffsetLo    dw      ?       ; Linear offset in virtual memory
wRefOffsetHi    dw      ?       ;  where all offsets are relative

wFileTruncLo    dw      ?       ; Linear offset in file of relative
wFileTruncHi    dw      ?       ;  point for truncation

wVirusSize      dw      ?       ; Virus size from the repair struct

wFileSizeLo     dw      ?       ; File size taken from DX:AX
wFileSizeHi     dw      ?

wIPOffset       dw      ?       ; IP taken from ES:DI
wIPSegment      dw      ?

wEPOffset       dw      ?       ; EP taken from FS:SI
wEPSegment      dw      ?


wTemp           dw      ?       ; Used by functions - not guaranteed
                                ;  to hold value across a function call



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    LinOffToSegOff
;;
;; Description: Converts 32-bit linear value in DX:AX to
;;              segment:offset value in DX:AX
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LinOffToSegOff  proc

        push    di
        push    cx

        mov     di, ax
        and     ax, 0Fh         ; AX = 4 bit offset

        mov     cl, 4
        shr     di, cl          ; Segmentize low word

        mov     cl, 12
        shl     dx, cl

        or      dx, di          ; Procreate segment value

        pop     cx
        pop     di

        ret

LinOffToSegOff  endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    SegOffToLinOff
;;
;; Description: Converts segment:offset value in DX:AX to
;;              32-bit linear value in DX:AX
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SegOffToLinOff  proc

        push    bx
        push    cx

        mov     cx, 10h
        mov     bx, ax
        mov     ax, dx
        mul     cx
        add     ax, bx
        adc     dx, 0

        pop     cx
        pop     bx

        ret

SegOffToLinOff  endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    ComputeLinRefOff
;;
;; Description: Computes the linear reference offset in memory where
;;              all provided offsets in the repair structure should
;;              be relative to.
;;
;;              The linear reference offset is stored in:
;;
;;                      wRefOffsetHi::wRefOffsetLo
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = Error code
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ComputeLinRefOff proc

        push    dx
        push    cx
        push    bx

        test    wRepairFlags, REP_REL_IP
        jnz     lRepRefIP

        test    wRepairFlags, REP_REL_EP
        jnz     lRepRefEP

        test    wRepairFlags, REP_REL_VSIZE
        jnz     lRepRefVSize

        mov     ax, REP_ERR_NO_REL
        jmp     lRepRefExitFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Offsets relative to IP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepRefIP:

        mov     ax, wIPOffset
        mov     dx, wIPSegment

        jmp     lRepRefSOtoLin

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Offsets relative to EP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepRefEP:

        mov     ax, wEPOffset
        mov     dx, wEPSegment

lRepRefSOtoLin:

        ; Convert segment offset in dx:ax to linear in dx:ax

        call    SegOffToLinOff

        jmp     lRepRefStore

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Offsets relative to EOF minus virus size
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepRefVSize:

        ; If EXE then see if we should use the header image size

        test    wInfoFlags, REP_INFO_FLAG_EXE
        jz      lRepRefUseFS

        ; Check if we should use image size for file size

        test    wRepairFlags, REP_EXE_HDR_IMAGESIZE
        jz      lRepRefUseFS

        ; Compute file size from header image size field

        mov     bx, TOF_OFFSET
        mov     ax, [bx+4]              ; AX = # of pages
        mov     cx, 200h
        mul     cx
        mov     bx, [bx+2]              ; BX = partial page size
        add     ax, bx
        adc     dx, 0
        cmp     bx, 0
        je      lRepRefSubHS
        sub     ax, 200h
        sbb     dx, 0
        jmp     lRepRefSubHS

lRepRefUseFS:

        ; Put file size - virus size in dx:ax

        mov     ax, wFileSizeLo
        mov     dx, wFileSizeHi
        sub     ax, wVirusSize
        sbb     dx, 0

        ; If EXE then subtract header size

        test    wInfoFlags, REP_INFO_FLAG_EXE
        jz      lRepRefAdjust

lRepRefSubHS:

        mov     bx, TOF_OFFSET + 8           ; Get header size
        mov     bx, [bx]
        mov     ch, bh                  ; Save high byte
        mov     cl, 4                   ; Multiply by 0x10
        shl     bx, cl
        mov     cl, 12
        shr     cx, cl

        sub     ax, bx
        sbb     dx, cx

        ; What if it is negative?
        ; This does the check.

        jae     lRepRefAdjust

        ; Out of range

        mov     ax, REP_ERR_REL_VSIZE
        jmp     lRepRefExitFail

lRepRefAdjust:

        ; Adjust for start segment location

        add     ax, LOAD_OFFSET
        adc     dx, 0

lRepRefStore:

        ; Store linear offset

        mov     wRefOffsetLo, ax
        mov     wRefOffsetHi, dx

        jmp     lRepRefExitSuccess

lRepRefExitFail:

        stc
        jmp     lRepRefEnd

lRepRefExitSuccess:

        xor     ax, ax
        clc

lRepRefEnd:

        pop     bx
        pop     cx
        pop     dx

        ret

ComputeLinRefOff endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    ComputeTruncOff
;;
;; Description: Computes the linear offset in the file
;;              for truncation.
;;
;;              The linear offset is stored in:
;;
;;                      wFileTruncHi::wFileTruncLo
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = REP_ERR_TRUNC_CALC_FAIL
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ComputeTruncOff proc

        push    dx
        push    cx
        push    bx
        push    si
        push    di

        test    wRepairFlags, TRUNCATE_IP
        jnz     lRepTruncIP

        test    wRepairFlags, TRUNCATE_EP
        jnz     lRepTruncEP

        test    wRepairFlags, TRUNCATE_VSIZE
        jnz     lRepTruncVSize

        jmp     lRepTruncExitSuccess

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Truncate relative to IP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepTruncIP:

        mov     ax, wIPOffset
        mov     dx, wIPSegment

        jmp     lRepTruncSOtoLin

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Truncate relative to EP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepTruncEP:

        mov     ax, wEPOffset
        mov     dx, wEPSegment

lRepTruncSOtoLin:

        ; Convert segment offset in dx:ax to linear in dx:ax

        call    SegOffToLinOff

        sub     ax, LOAD_OFFSET
        sbb     dx, 0

        jc      lRepTruncOutOfRange

        ; Account for EXE header if applicable

        test    wInfoFlags, REP_INFO_FLAG_EXE
        jz      lRepTruncStore

        ; Put size of header in si:bx

        mov     bx, TOF_OFFSET
        mov     bx, [bx + 08h]
        mov     si, bx

        mov     cl, 12
        shr     si, cl

        mov     cl, 4
        shl     bx, cl

        add     ax, bx
        adc     dx, si

        jmp     lRepTruncStore

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Truncation relative to EOF minus virus size
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepTruncVSize:

        ; Put file size - virus size in dx:ax

        mov     ax, wFileSizeLo
        mov     dx, wFileSizeHi

        sub     ax, wVirusSize
        sbb     dx, 0

        ; Out of range?

        jnc     lRepTruncStore

lRepTruncOutOfRange:

        jmp     lRepTruncExitFail

lRepTruncStore:

        mov     si, ax
        mov     di, dx

        ; First add truncation constant

        test    wInfoFlags, REP_INFO_FLAG_EXE
        jz      lRepTruncCOM

        ; EXE truncation constant

        mov     bx, pRepEXEOffset
        mov     ax, [bx + OFFSET_EXE_TRUNC_CONST]

        jmp     lRepTruncAddConst

lRepTruncCOM:

        ; COM truncation constant

        mov     bx, pRepCOMOffset
        mov     ax, [bx + OFFSET_COM_TRUNC_CONST]

lRepTruncAddConst:

        cwd                             ; DX:AX = sign extended trunc const

        add     si, ax                  ; DI:SI = trunc offset in file
        adc     di, dx

        js      lRepTruncExitFail

        ; Store linear offset

        mov     wFileTruncLo, si
        mov     wFileTruncHi, di

        jmp     lRepTruncExitSuccess

lRepTruncExitFail:

        mov     ax, REP_ERR_TRUNC_CALC_FAIL
        stc
        jmp     lRepTruncEnd

lRepTruncExitSuccess:

        xor     ax, ax
        clc

lRepTruncEnd:

        pop     di
        pop     si
        pop     bx
        pop     cx
        pop     dx

        ret

ComputeTruncOff endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    VerifyBookmark
;;
;; Description: Verifies the bookmark information.
;;
;;              pRepSection should point to the bookmark section.
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = REP_ERR_BOOKMARK_FAIL
;;
;;      Also modifies pRepSection to skip past the bookmark section.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VerifyBookmark proc

        push    dx
        push    cx
        push    bx
        push    di
        push    si

        mov     bx, pRepStruct

        ; Compute linear offset of bookmark in virtual memory
        ;
        ; Contents of bookmark structure:
        ;
        ;       offset  contents
        ;       ---------------------------------------------------
        ;       0       relative location of bookmark value(s)
        ;       2       size of bookmark
        ;       3       bookmark verify byte(s)

        mov     ax, [bx+OFFSET_BOOKMARK_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        ; Store as segment:offset in es:di

        call    LinOffToSegOff

        mov     di, ax
        mov     es, dx

        ; CX = count

        xor     ch, ch
        mov     cl, [bx+OFFSET_BOOKMARK_COUNT]

        ; Do we need to verify?

        or      cl, cl
        jz      lRepVerBkmkSuccess

        ; Point si to Bookmark byte array

        lea     si, [bx + OFFSET_BOOKMARK_ARRAY]

        ; Verify the bookmark

        cld
        repe    cmpsb

        je      lRepVerBkmkSuccess

lRepVerBkmkFail:

        mov     ax, REP_ERR_BOOKMARK_FAIL
        stc
        jmp     lRepVerBkmkEnd

lRepVerBkmkSuccess:

        xor     ax, ax
        clc

lRepVerBkmkEnd:

        pop     si
        pop     di
        pop     bx
        pop     cx
        pop     dx

        ret

VerifyBookmark endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    RepairCopyCont
;;
;; Description: Copy a contiguous set of bytes from the virus
;;              to the beginning of the file.
;;
;;              BX should point to the copy contiguous section
;;              of the form:
;;                              WORD wSrcOffset
;;                              BYTE byDstOffset
;;                              BYTE byCount
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = REP_ERR_COPY_CONT_FAIL
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RepairCopyCont proc

        push    dx
        push    cx
        push    bx
        push    di
        push    si

        ; seek to top of host file...

        mov     ax, 4200h
        xor     cx, cx
        xor     dh, dh
        mov     dl, [bx+OFFSET_CONT_DST_OFFSET]
        int     21h

        jc      lRepCopyContFail

        ; Compute linear offset of source bytes in virtual memory

        mov     ax, [bx+OFFSET_CONT_SRC_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        ; Store as segment:offset in dx:ax

        call    LinOffToSegOff

        ; Get count

        xor     ch, ch
        mov     cl, [bx+OFFSET_CONT_COUNT]

        ; Store

        push    ds              ; save ds

        mov     ds, dx          ; ds:dx gets segment:offset from dx:ax
        mov     dx, ax

        mov     ax, 4000h
        int     21h

        pop     ds              ; restore ds

        jnc     lRepCopyContSuccess

lRepCopyContFail:

        mov     ax, REP_ERR_COPY_CONT_FAIL
        stc
        jmp     lRepCopyContEnd

lRepCopyContSuccess:

        xor     ax, ax
        clc

lRepCopyContEnd:

        pop     si
        pop     di
        pop     bx
        pop     cx
        pop     dx

        ret

RepairCopyCont endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    RepairCopyStag
;;
;; Description: Copy a staggered set of bytes from the virus
;;              to the beginning of the file.
;;
;;              BX should point to the copy staggered section
;;              of the form:
;;                              WORD wFlags
;;                              WORD wByteOffset[# of 1 bits in flags]
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = REP_ERR_COPY_STAG_FAIL
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RepairCopyStag proc

        push    dx
        push    cx
        push    bx
        push    di
        push    si

        mov     si, [bx+OFFSET_COM_STAG_FLAGS]
        mov     dx, 0                   ; DX contains the byte #

lRepCopyStagLoop:

        and     si, si                  ; see if we're done with 1 bits
        jz      lRepCopyStagSuccess

        ; Copy this byte?

        test    si, 1
        jz      lRepCopyStagNext

        add     bx, 2                   ; BX points to next offset

        ; Seek to location to copy byte

        mov     ax, 4200h
        xor     cx, cx
        int     21h

        jc      lRepCopyStagFail

        mov     dx, ax
        push    dx

        ; Compute linear offset of source byte in virtual memory

        mov     ax, [bx]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        ; Store as segment:offset in dx:ax

        call    LinOffToSegOff

        push    ds              ; save ds

        mov     ds, dx
        mov     dx, ax

        ; Write byte

        mov     ax, 4000h
        mov     cx, 1
        int     21h

        pop     ds              ; restore ds

        pop     dx

        jc      lRepCopyStagFail

lRepCopyStagNext:

        inc     dx
        shr     si, 1
        jmp     lRepCopyStagLoop

lRepCopyStagFail:

        mov     ax, REP_ERR_COPY_STAG_FAIL
        stc
        jmp     lRepCopyStagEnd

lRepCopyStagSuccess:

        xor     ax, ax
        clc

lRepCopyStagEnd:

        pop     si
        pop     di
        pop     bx
        pop     cx
        pop     dx

        ret

RepairCopyStag endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    RepWriteWTemp
;;
;; Description: Writes wTemp to the file at the current file location.
;;
;;              Modifies: CX, DX, AX, DS
;;
;; Returns:
;;      If success,     CLC
;;      If fail,        STC
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RepWriteWTemp   proc

        mov     cx, 2
        mov     dx, offset wTemp
        push    cs
        pop     ds
        mov     ax, 4000h
        int     21h

        ret

RepWriteWTemp   endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Function:    RepairCopyEXEStag
;;
;; Description: Copy a staggered set of bytes from the virus
;;              to the EXE header of the file.
;;
;;              BX should point to the EXE copy staggered section
;;              of the form:
;;
;;                      WORD wFlags             // COPY_SS
;;                                              // TWEAK_SS_SUB_10
;;                                              // COPY_SP
;;                                              // COPY_IP
;;                                              // COPY_CS
;;                                              // TWEAK_CS_SUB_10
;;                                              // MINMEM_ADD_CONST
;;                                              // MAXMEM_ADD_CONST
;;                      WORD wCopySSOffset
;;                      WORD wCopySPOffset
;;                      WORD wCopyIPOffset
;;                      WORD wCopyCSOffset
;;                      WORD wMinMemAddConst
;;                      WORD wMaxMemAddConst
;;
;; Returns:
;;      If success,     CLC     AX = 0
;;      If fail,        STC     AX = REP_ERR_STAG_EXE_FAIL
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RepairCopyEXEStag       proc
        push    dx
        push    cx
        push    bx
        push    si
        push    di

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy MinMem
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        test    word ptr [bx+OFFSET_EXE_FLAGS], MINMEM_ADD_CONST
        jz      lRepEXETryMaxMem

        ; Seek to MinMem field in EXE header

        xor     cx, cx
        mov     dx, 0Ah
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Get infected MinMem constant to add

        mov     ax, [bx + OFFSET_EXE_MINMEM_OFFSET]

        mov     si, TOF_OFFSET
        add     ax, [si + 0Ah]

        mov     wTemp, ax

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy MaxMem
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepEXETryMaxMem:

        test    word ptr [bx+OFFSET_EXE_FLAGS], MAXMEM_ADD_CONST
        jz      lRepEXETrySS

        ; Seek to MaxMem field in EXE header

        xor     cx, cx
        mov     dx, 0Ch
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Get infected MinMem constant to add

        mov     ax, [bx + OFFSET_EXE_MAXMEM_OFFSET]

        mov     si, TOF_OFFSET
        add     ax, [si + 0Ch]

        mov     wTemp, ax

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy SS
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepEXETrySS:

        test    word ptr [bx+OFFSET_EXE_FLAGS], COPY_SS
        jz      lRepEXETrySP

        ; Seek to SS field in EXE header

        xor     cx, cx
        mov     dx, 0Eh
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Compute linear offset of source word in virtual memory

        mov     ax, [bx+OFFSET_EXE_SS_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        call    LinOffToSegOff

        push    ds

        mov     ds, dx
        mov     si, ax

        mov     si, [si]

        pop     ds

        mov     wTemp, si

        test    word ptr [bx+OFFSET_EXE_FLAGS], TWEAK_SS_SUB_10
        jz      lStagWriteSSValue

        sub     wTemp, 10h              ; tweak by 10h

lStagWriteSSValue:

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy SP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepEXETrySP:

        test    word ptr [bx+OFFSET_EXE_FLAGS], COPY_SP
        jz      lRepEXETryIP

        ; Seek to SP field in EXE header

        xor     cx, cx
        mov     dx, 10h
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Compute linear offset of source word in virtual memory

        mov     ax, [bx+OFFSET_EXE_SP_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        call    LinOffToSegOff

        push    ds

        mov     ds, dx
        mov     si, ax

        mov     si, [si]

        pop     ds

        mov     wTemp, si

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy SP
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepEXETryIP:

        test    word ptr [bx+OFFSET_EXE_FLAGS], COPY_IP
        jz      lRepEXETryCS

        ; Seek to IP field in EXE header

        xor     cx, cx
        mov     dx, 14h
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Compute linear offset of source word in virtual memory

        mov     ax, [bx+OFFSET_EXE_IP_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        call    LinOffToSegOff

        push    ds

        mov     ds, dx
        mov     si, ax

        mov     si, [si]

        pop     ds

        mov     wTemp, si

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Copy CS
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepEXETryCS:

        test    word ptr [bx+OFFSET_EXE_FLAGS], COPY_CS
        jz      lRepStagEXESuccess

        ; Seek to CS field in EXE header

        xor     cx, cx
        mov     dx, 016h
        mov     ax, 4200h
        int     21h

        jc      lRepStagEXEFail

        ; Compute linear offset of source word in virtual memory

        mov     ax, [bx+OFFSET_EXE_CS_OFFSET]
        cwd

        add     ax, wRefOffsetLo
        adc     dx, wRefOffsetHi

        call    LinOffToSegOff

        push    ds

        mov     ds, dx
        mov     si, ax

        mov     si, [si]

        pop     ds

        mov     wTemp, si

        test    word ptr [bx+OFFSET_EXE_FLAGS], TWEAK_CS_SUB_10
        jz      lStagWriteCSValue

        sub     wTemp, 10h              ; tweak by 10h

lStagWriteCSValue:

        call    RepWriteWTemp
        jc      lRepStagEXEFail

        jmp     lRepStagEXESuccess

lRepStagEXEFail:

        mov     ax, REP_ERR_STAG_EXE_FAIL
        stc
        jmp     lRepStagEXEEnd

lRepStagEXESuccess:
        xor     ax, ax
        clc

lRepStagEXEEnd:

        pop     di
        pop     si
        pop     bx
        pop     cx
        pop     dx
        ret

RepairCopyEXEStag       endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;    INT EA - Repair
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

int_ea:

        pusha
        push    ds
        push    es

        push    cs
        pop     ds
        mov     pRepStruct, bx

        ; Save start of COM and EXE repair structs

        push    [bx+OFFSET_COM_OFFSET]
        pop     pRepCOMOffset

        push    [bx+OFFSET_EXE_OFFSET]
        pop     pRepEXEOffset

        ; Save flags

        push    [bx]
        pop     wRepairFlags

        ; Save virus size

        push    [bx + OFFSET_VSIZE]
        pop     wVirusSize

        ; Save useful info

        mov     wFileSizeLo, ax
        mov     wFileSizeHi, dx

        mov     wIPOffset, di
        mov     wIPSegment, es

        mov     wEPOffset, si
.486
        mov     wEPSegment, fs
.286
        ; EXE or COM?

        mov     bx, TOF_OFFSET
        cmp     word ptr [bx], 'MZ'             ; check if we are dealing
        je      lRepEXE                         ;  with an EXE file
        cmp     word ptr [bx], 'ZM'
        je      lRepEXE

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Repair COM file
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepCom:

        mov     wInfoFlags, REP_INFO_FLAG_COM

        ; COM repair info present?

        test    wRepairFlags, REP_COM_CONT or REP_COM_STAG
        jnz     lRepCOMEP

        mov     ax, REP_ERR_NO_REP
        jmp     lRepExitFail

lRepCOMEP:

        ; Update EP value

        test    wRepairFlags, REP_COM_EP_CALLBACK
        jz      lRepDetEP

        ; Call callback

        mov     bx, pRepCOMOffset
        add     bx, OFFSET_COM_EP_CALLBACK
        call    word ptr [bx]

        ; AX contains EP offset

        mov     wEPOffset, ax

        jmp     lContRepCOM

lRepDetEP:

        mov     bx, TOF_OFFSET
        cmp     byte ptr [bx], 0E9h
        je      lRepCalcEP

        ; Error out only if we were going to use EP

        test    wRepairFlags, REP_REL_EP or TRUNCATE_EP
        jz      lContRepCOM

        mov     ax, REP_ERR_INVALID_COM_EP
        jmp     lRepExitFail

lRepCalcEP:

        ; Calculate EP based on E9 near jump

        mov     ax, [bx+1]
        add     ax, 100h + 3
        mov     wEPOffset, ax

lContRepCOM:

        ; Compute linear reference offset for relative offsets

        call    ComputeLinRefOff
        jc      lRepExitFail

        ; Compute truncate offset in file

        call    ComputeTruncOff
        jc      lRepExitFail

        ; Verify bookmark

        call    VerifyBookmark
        jc      lRepExitFail

        ; BX gets offset of COM repair info

        mov     bx, pRepCOMOffset
        add     bx, OFFSET_COM_REP_INFO

        ; Repair contiguous or staggered

        test    wRepairFlags, REP_COM_CONT
        jz      lRepComCheckStag

        ; Repair contiguous

        ; RepairCopyCont expects BX to point to a structure of the form:
        ;       WORD wSrcOffset
        ;       BYTE byDstOffset
        ;       BYTE byCount

        call    RepairCopyCont
        jc      lRepExitFail

        jmp     lRepTruncate

lRepComCheckStag:

        ; Must be staggered

lRepComStag:

        call    RepairCopyStag
        jc      lRepExitFail

lRepTruncate:

        ; Do not attempt truncation if no TRUNCATE_ flag specified

        test    wRepairFlags, TRUNCATE_IP or TRUNCATE_EP or TRUNCATE_VSIZE
        jz      lRepExitSuccess

        ; Truncate file

        mov     dx, wFileTruncLo
        mov     cx, wFileTruncHi
        mov     ax, 4200h
        int     21h

        mov     ax, REP_ERR_TRUNCATE_FAIL
        jc      lRepExitFail

        mov     ax, 4000h
        xor     cx, cx
        int     21h

        mov     ax, REP_ERR_TRUNCATE_FAIL
        jc      lRepExitFail

        jmp     lRepExitSuccess

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Repair EXE file
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

lRepExe:

        mov     wInfoFlags, REP_INFO_FLAG_EXE

        ; EXE repair info present?

        test    wRepairFlags, REP_EXE_CONT or REP_EXE_STAG
        jnz     lContRepEXE

        mov     ax, REP_ERR_NO_REP
        jmp     lRepExitFail

lContRepEXE:

        ; Compute linear reference offset for relative offsets

        call    ComputeLinRefOff
        jc      lRepExitFail

        ; Compute truncate offset in file

        call    ComputeTruncOff
        jc      lRepExitFail

        ; Verify bookmark

        call    VerifyBookmark
        jc      lRepExitFail

        ; Repair contiguous or staggered

        ; BX gets offset of EXE repair info

        mov     bx, pRepEXEOffset
        add     bx, OFFSET_EXE_REP_INFO

        ; Repair contiguous or staggered

        test    wRepairFlags, REP_EXE_CONT
        jz      lRepEXECheckStag

        ; Repair contiguous

        ; RepairCopyCont expects BX to point to a structure of the form:
        ;       WORD wSrcOffset
        ;       BYTE byDstOffset
        ;       BYTE byCount

        call    RepairCopyCont
        jc      lRepExitFail

        jmp     lRepEXETruncate

lRepEXECheckStag:

        ; Must be staggered

lRepEXEStag:

        call    RepairCopyEXEStag
        jc      lRepExitFail

lRepEXETruncate:

        ; Do not attempt truncation if no TRUNCATE_ flag specified

        test    wRepairFlags, TRUNCATE_IP or TRUNCATE_EP or TRUNCATE_VSIZE
        jz      lRepExitSuccess

        ; Seek to partial page offset

        xor     cx, cx
        mov     dx, 02h
        mov     ax, 4200h
        int     21h

        mov     ax, REP_ERR_SEEK
        jc      lRepExitFail

        ; Compute image size

        ; Compute partial page field

        mov     ax, wFileTruncLo
        and     ax, 01FFh
        mov     wTemp, ax

        call    RepWriteWTemp

        mov     ax, REP_ERR_WRITE
        jc      lRepExitFail

        ; Compute # pages field

        mov     ax, wFileTruncLo
        mov     dx, wFileTruncHi
        add     ax, 511
        adc     dx, 0
        mov     cx, 512
        div     cx
        mov     wTemp, ax

        call    RepWriteWTemp
        mov     ax, REP_ERR_WRITE
        jc      lRepExitFail

        jmp     lRepTruncate

        ;;
        ;; Repair exit
        ;;

lRepExitFail:

        stc
        jmp     lRepExit

lRepExitSuccess:

        xor     ax, ax
        clc

lRepExit:

        pushf
        pop     wTempFlags

        ; Save return value

        mov     wTemp, ax

        ; Restore registers

        pop     es
        pop     ds
        popa

        ; Restore return value

        mov     ax, cs:wTemp

        ; Add insert flags

        or      ax, cs:wInfoFlags

        ; Return to caller



        mov     cs:wTempSP, sp
        add     sp, 6                   ; ready SP for push of flags
        push    cs:wTempFlags
        mov     sp, cs:wTempSP

        iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


int_e9:

        cmp     cx, 0FFFFh                      ; if we didn't find our
        je      lIntE9Error                     ; "entry sig", abort...

        int     0eah
        jc      lIntE9Error

        mov     ax, 0fd00h                      ; success!
        int     21h

lIntE9Error:

        mov     ax, 0fdffh
        int     21h

; Autoverv ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

int_eb:

        pusha
        push    ds
        push    es
	jmp	Autoverv

	INCLUDE AUTOVERV.INC

Autoverv_Exit:
        pop     es
        pop     ds
        popa
	iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CSEG    ENDS

END
