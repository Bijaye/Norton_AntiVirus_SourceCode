;==============================================================================
;
; File name:     autoverv.inc
;
; Description:   binary virus verification & repair - foundation module.
;
; Statement:     Licensed Materials - Property of IBM
;                (c) Copyright IBM Corp. 1998-1999
;
; Author:        Andy Raybould
;
;                U.S. Government Users Restricted Rights - use,
;                duplication or disclosure restricted by GSA ADP
;                Schedule Contract with IBM Corp.
;
;
;==============================================================================

; Executes autoverv-generated binary-file verification and repair.
; Installed as an interrupt handler in the ERS foundation.
; Label 'Autoverv' is the entry point. On entry:
;    ax.dx is the length of the infected file;
;    bx is the address of the RepairParm structure, in which:
;        pStart is the address of the array of autoverv-generated instructions;
;        pEnd is the address one beyond the last item in that array.
;
; This routine executes each instruction in the array in turn.
;
; Each instruction indicates what type of object it applies to: COM, EXE or FILE,
; where FILE is either COM or EXE. The type may be further qualified as long or
; short, depending on whether the host is longer or shorter than the virus (i.e.
; whether the infected file is more than twice the length of the virus). If so,
; then there must be a CriticalLength macro specifying the threshold between long
; and short. The interpreter only performs instructions whose type describes the
; file: a long COM file matches types FILE, FILE-long, COM, or COM-long, but not
; FILE-short, EXE, EXE-short, EXE-long, or COM-short.
;
; Each location within the file is specified by an anchor point (beginning, end
; or entry point), and an offset from that anchor.
;
; The Verify macro specifies the beginning and end of a region to be verified,
; the encryption type and key location, and the CRC of that region.
;
; The Repair macro specifies the beginning and end of the source of a
; reconstruction, the encryption type and key location, and the destination
; location. It is assumed that the repair instructions are ordered by
; their destination position, beginning to end. Repair is done by building
; the reconstructed file on the end of the infected file, copying it to the
; beginning of the file, and truncating. This is crude, but it avoids the issues
; of adequate buffer allocation or of creating a new file that other approaches
; would have entailed (the documentation wasn't specific enough for me to be sure
; that these issues could be safely resolved).
;
; The SetLength macro instructs the interpreter to set the length field in the
; header of a reconstructed EXE file to the length of the reconstructed file.
;
; See autoverv.dcl for the physical format of the language.

;==============================================================================

BFRLEN  equ 1000h

;==============================================================================
OPTION LJMP

include autoverv.dcl                    ; shared declarations

DoubleWord struc
DDlo    dw      ?                       ;low word
DDhi    dw      ?                       ;high word
DoubleWord ends

;--- macros -------------------------------------------------------------------

FileIO  MACRO
    IFDEF STANDALONE
        mov bx, HANDLE
    ENDIF
        Int 21h
        ENDM

FileRead MACRO
        mov ah, 3Fh
        FileIO
        ENDM

FileWrite MACRO
        mov ah, 40h
        FileIO
        ENDM

SeekStart MACRO
        mov ax, 4200h
        FileIO
        ENDM

SeekCurr MACRO
        mov ax, 4201h
        FileIO
        ENDM

SeekEnd MACRO
        mov ax, 4202h
        FileIO
        ENDM



Failc   MACRO
        jc  Fail
        ENDM

Failne  MACRO
        jne  Fail
        ENDM

Failz   MACRO
        jz  Fail
        ENDM

Failnz  MACRO
        jnz  Fail
        ENDM

FailNow MACRO
        jmp  Fail
        ENDM



IFDEF DUMP
   include PAMdump.inc
ELSE

nFailc MACRO n
   local n
   Failc
   ENDM

nFailne MACRO n
   local n
   Failne
   ENDM

nFailz MACRO n
   local n
   Failz
   ENDM

nFailnz MACRO n
   local n
   Failnz
   ENDM

nFailNow MACRO n
   local n
   FailNow
   ENDM

ENDIF



HostPos MACRO locAnchor, locOffset
        LOCAL fromBegin, fromEntry, fromEof, Exit

        mov  dx, locOffset
        mov  cx, locOffset+2

        mov  al, locAnchor
        cmp  al, BEGIN
        je   fromBegin
        cmp  al, ENTRY
        je   fromEntry
        cmp  al, EOF
        je   fromEof
        nFailNow 1
fromEof:
        add  dx, Last
        adc  cx, Last+2
        jmp  Exit
fromEntry:
        add  dx, EP
        adc  cx, EP+2
fromBegin:
Exit:
        ENDM



HostGo  MACRO anchor, offset
        HostPos anchor, offset
        SeekStart
        nFailc 2
        ENDM



HostGet MACRO srcAnchor, srcOffset, dest, length
        HostGo srcAnchor, srcOffset
        mov  dx, offset dest
        mov  cx, length
        FileRead
        nFailc 3
        ENDM



HostAppend MACRO src, length

   ;go to end of file
        xor  cx,cx
        xor  dx,dx
        SeekEnd
        nFailc 4

   ;write the new data
        mov  dx,offset src
        mov  cx,length
        FileWrite
        nFailc 5

        add  RepairLength,  ax
        adc  RepairLength+2,0

        ENDM


CopyBack MACRO                  ;not used in PAM??
        LOCAL loop, done

        mov dx, InfectedLen
        mov cx, InfectedLen+2

loop:
        mov RepairSrc,  dx
        mov RepairSrc+2,cx

   ;go to read posn cx.dx
        SeekStart
        nFailc 6
        ;cmp ax,RepairSrc       even if it's one past the end??
        ;Failne
        ;cmp dx,RepairSrc+2
        ;Failne

   ;read
        mov cx, BFRLEN
        mov dx,offset Bfr
        FileRead
        nFailc 7
        mov FieldLen,ax

   ;go to write posn
        mov dx,RepairDest
        mov cx,RepairDest+2
        SeekStart
        nFailc 8
        cmp ax,RepairDest
        nFailne 9
        cmp dx,RepairDest+2
        nFailne 10

   ;write
        mov dx, offset Bfr
        mov cx, FieldLen
        FileWrite
        nFailc 11
        cmp cx,ax
        nFailne 12

   ;finished?
        cmp ax,0
        jz  done

   ;adjust pointers & go again
        add RepairDest,ax
        adc RepairDest+2,0

        mov dx,RepairSrc
        mov cx,RepairSrc+2
        add dx,ax
        adc cx,0
        jmp loop

done:
        ENDM

;==============================================================================

Autoverv:
        IFDEF DUMPIMG
           INCLUDE DUMPIMG.INC   ;!!! write out the loaded image & exit
        ENDIF

   ;get the instruction list offsets
        mov  pInstructions, bx
        mov  pInstrEnd, cx

   ;is there a repair?
        pushf
        cmp    cx, bx
        jnz    HaveRepInstr
        popf
        jmp    Fail

HaveRepInstr:
        popf

   ;save the host length
        mov  InfectedLen,   ax
        mov  InfectedLen+2, dx

        dec  ax
        sbb  dx, 0
        mov  Last,  ax
        mov  Last+2,dx

   ; writethru off
        IFNDEF STANDALONE
             mov  ah, 0FFh
             int  21h
             nFailc 13
        ENDIF

FindEP:
   ;find entry point
        IFDEF STANDALONE
             mov bx,cs
             mov es,bx
             mov  bx, offset Hdr
        ELSE
             mov  bx, 0f000h
             mov  es, bx
             mov  bx, 0ff00h            ;??Hdr equ 0ff00h?
        ENDIF
        cmp  byte ptr es:[bx], 0E9h
        jz   is_E9_com
        cmp  byte ptr es:[bx], 0E8h
        jz   is_E8_com
        cmp  word ptr es:[bx], 'MZ'
        jz   is_exe
        cmp  word ptr es:[bx], 'ZM'
        jz   is_exe

is_P_com:
        xor  ax, ax             ;does not begin with a jump
        jmp  is_com

is_E8_com:
is_E9_com:
        mov  ax, es:[bx+1]      ;initial jump target
        add  ax, 3
        nFailc 15     ;??

is_com:
        mov  EP, ax
        xor  dx, dx
        mov  EP+2, dx
        mov  FileType, COM
        jmp  EP_found

is_exe:
        mov  ax, es:[bx+8h]    ;load module start paragraph
        add  ax, es:[bx+16h]   ;offset (in paras.) of code seg. from LM start

        xor  dx, dx            ;convert to a byte offset in dword dx.ax
        mov  cx, 4             ;16 = 2**4
EPX_p2b:
        shl  ax, 1
        rcl  dx, 1
        loop EPX_p2b

        add  ax, es:[bx+14h]   ;add IP
        adc  dx, 0

        cmp  dx, 10h           ;subtract 100000h if > 100000h
        jb   EPX_sav
        ja   EPX_sub
        cmp  ax, 0
        jbe  EPX_sav
EPX_sub:
        sub  dx, 10h;
EPX_sav:
        mov  EP,   ax
        mov  EP+2, dx
        mov  FileType, EXE

EP_found:


Interpret:   ; do each instruction in turn

   ;point si at the first verification/repair instruction
        mov  si, pInstructions

IntLoop:     ; begin the next instruction

   ;finish if no more intructions
        cmp  si, pInstrEnd
        jae  IntEnd

   ;skip the instruction unless it applies to the current file type
        mov  al, cs:Instrctn.objectType[si]

        cmp  al, FILE
        je   IntTypeOK

        cmp  al, FileType
        je   IntTypeOK    ; exact match

        cmp  al, COM
        je   IntTypeCOM
        cmp  al, EXE
        je   IntTypeEXE
        jmp  IntNext

IntTypeCOM:
        cmp FileType, COMs
        je  IntTypeOK
        cmp FileType, COMl
        je  IntTypeOK
        jmp  IntNext

IntTypeEXE:
        cmp FileType, EXEs
        je  IntTypeOK
        cmp FileType, EXEl
        je  IntTypeOK
        jmp  IntNext

IntTypeOK:

   ; is this a SETLEN instruction?
        mov  al, cs:Instrctn.op[si]
        cmp  al, DO_SETLEN
        je   IntSetMZlen

   ; is this a CRITLEN instruction?
        cmp  al, DO_CRITLEN
        je   IntShortFileTest

   ;load the key, if encrypted
        mov  al, cs:Instrctn.encr[si]
        cmp  al, PLAIN
        je   IntGotKey
        HostGet cs:Instrctn.key_anchor[si], cs:Instrctn.key_offset[si], offset Key, 2

IntGotKey:

   ;find position of field in host
        HostGo cs:Instrctn.start_anchor[si], cs:Instrctn.start_offset[si]
        mov  FieldPos,  ax
        mov  FieldPos+2,dx

   ;calculate length of field
        HostPos cs:Instrctn.end_anchor[si], cs:Instrctn.end_offset[si]
        sub  dx, FieldPos
        sbb  cx, FieldPos+2
        inc  dx
        adc  cx,0
        ;opt - will be saved below   mov  FieldLen,  dx
        ;opt - ditto                 mov  FieldLen+2,cx

   ;initialize CRC
        xor  ax,ax
        mov  CRC,  ax
        mov  CRC+2,ax

IntFieldLoop: ; if the field to be processed is longer than the buffer, do it in parts

   ;process the current field until its remaining length is 0
        cmp  cx, 0
        jne  IntFieldGo
        cmp  dx, 0
        jne  IntFieldGo
        jmp  IntFieldEnd

IntFieldGo:

   ;read min (field length, buffer size)
        cmp  cx, 0
        jnz  IntPartialRead
        cmp  dx, BFRLEN
        ja   IntPartialRead

IntFullRead:                  ;get the whole (remaining) field into the buffer
        mov  FieldLen,  cx    ;set remaining field length (we know cx is 0 here)
        mov  FieldLen+2,cx
        mov  cx, dx           ;set length to read
        jmp  IntRead

IntPartialRead:               ;only part of the field will fit in the buffer
        sub  dx, BFRLEN
        sbb  cx, 0
        mov  FieldLen,  dx    ;set remaining field length
        mov  FieldLen+2,cx
        mov  cx, BFRLEN       ;set length to read

IntRead:
        mov  dx, offset Bfr
        FileRead
        nFailc 17

        cmp  ax, cx           ;didn't read what we expected
        nFailc 18

        mov  ReadLen,    ax
        add  FieldPos,   ax   ;set remaining field position ?? can skip if verifying
        adc  FieldPos+2, 0

IntDecrypt:
        mov  al, cs:Instrctn.encr[si]
        cmp  al, PLAIN
        je   IntDecrypted
        cmp  al, ADD1
        je   DecryptADD1
        cmp  al, XOR1
        je   DecryptXOR1
        cmp  al, ADD2
        je   DecryptADD2
        cmp  al, XOR2
        je   DecryptXOR2

IntDecrypted:

   ;verify or repair
        mov  al, cs:Instrctn.op[si]
        cmp  al, DO_REPAIR
        je   IntRepair

IntVerify:
        jmp  CRCcalc          ;?? use a foundation routine?

IntCRCdone:
        jmp  IntFieldNext

IntRepair:
   ;append the modified buffer to the end of the file
        HostAppend Bfr, ReadLen
        cmp  cx,ax
        nFailne 19        ;didn't write what was expected

   ;go back to where we last read
        mov  dx,FieldPos
        mov  cx,FieldPos+2
        SeekStart
        nFailc 20

IntFieldNext:
        mov  dx, FieldLen
        mov  cx, FieldLen+2
        jmp  IntFieldLoop   ; do the next part of the field

IntFieldEnd:

   ;verify or repair?
        mov  al, cs:Instrctn.op[si]
        cmp  al, DO_REPAIR
        je   IntNext

FieldVerify:   ; is the CRC correct?
        mov  ax, cs:Instrctn.fieldCRC[si];
        cmp  ax, CRC
        jne  FailVerify
        mov  ax, cs:Instrctn.fieldCRC+2[si];
        cmp  ax, CRC+2
        jne  FailVerify

IntNext:     ; do next instruction

   ;point si at the next verification/repair instruction...
        add si, SIZE Instrctn
        nFailc 21
   ;...and go again
        jmp IntLoop

IntEnd:

   ;copy the new file (appended to the original host) into position
        CopyBack
        jmp OK

;------------------------------------------------------------------------------

FailVerify:
        IFDEF DUMP
           nFailNow 1000
        ENDIF
Fail:
        stc
        jmp  Autoverv_Exit
OK:
        clc
        jmp  Autoverv_Exit

;=== CRC routine (may be replaced by foundation subroutine?)===================

;------------------------------------------------------------------------------
; Register usage:
;   DX:AX = running residue
;   DI    = offset to input data
;   CX    = number of bytes of input data
;   SI    = offset to lookup table
;   BX    = index into lookup table
;------------------------------------------------------------------------------

CRCcalc:
        mov     SI_REG,si               ;??
        mov     si,offset CRCpoly       ;get offset of lookup table
        mov     di,offset Bfr           ;get offset of input data
        mov     ax,CRC                  ;get current residue, low word
        mov     dx,CRC+2                ;get current residue, high word
        mov     cx,ReadLen              ;get number bytes of input data
;
CRCloop:
        sub     bx,bx                   ;clear index into lookup table
        mov     bl,cs:[di]              ;get next byte of input data
        inc     di                      ;increment data pointer
        xor     bl,dh                   ;xor input with residue hi byte
        shl     bx,1                    ;multiply by 4 = lookup index
        shl     bx,1                    ;(do in two shifts for 8088)
        mov     dh,dl                   ;shift residue left 1 byte
        mov     dl,ah
        mov     ah,al
        sub     al,al
        xor     ax,cs:DoubleWord.DDlo[si][bx]      ;xor residue with lookup (low)
        xor     dx,cs:DoubleWord.DDhi[si][bx]      ;xor residue with lookup (high)
        loop    CRCloop                 ;do next input byte
;
        mov     CRC,  ax                ;save residue
        mov     CRC+2,dx
        mov     si,SI_REG               ;??
        jmp     IntCRCdone

SI_REG  dw ?

;=== decryption routines (foundation?) ========================================

DecryptADD1:
        mov  di, offset Bfr
        mov  ax, Key
        mov  cx, ReadLen
ADD1loop:
        sub  [di],al
        inc  di
        dec  cx
        jnz  ADD1loop
;
        jmp IntDecrypted



DecryptXOR1:
        mov  di, offset Bfr
        mov  ax, Key
        mov  cx, ReadLen
XOR1loop:
        xor  [di],al
        inc  di
        dec  cx
        jnz  XOR1loop
;
        jmp IntDecrypted



DecryptADD2:
        mov  di, offset Bfr
        mov  ax, Key
        mov  cx, ReadLen
ADD2loop:
        dec  cx
        jz   ADD2oddLength
        sub  [di],ax
        inc  di
        inc  di
        dec  cx
        jnz  ADD2loop

        jmp  IntDecrypted

ADD2oddLength:
        sub  [di],al
        jmp  IntDecrypted



DecryptXOR2:
        mov  di, offset Bfr
        mov  ax, Key
        mov  cx, ReadLen
XOR2loop:
        dec  cx
        jz   XOR2oddLength
        xor  [di],ax
        inc  di
        inc  di
        dec  cx
        jnz  XOR2loop

        jmp  IntDecrypted

XOR2oddLength:
        xor  [di],al
        jmp  IntDecrypted


;=== set the repaired file's length and MZ flag in its header (EXE only) ======

IntSetMZlen:
   ;go to beginning of the reconstruction
        mov  dx, InfectedLen
        mov  cx, InfectedLen+2
        SeekStart
        nFailc 25

   ;read the reconstructed header
        mov  dx,  offset Bfr
        mov  cx,  HDRLEN
        FileRead
        nFailc 26
        cmp  ax,  HDRLEN
        nFailne 27

   ;set the header's EXE signature
        mov  exeM,  'M'
        mov  exeZ,  'Z'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; it seems that in real files, the 2nd word is FILE (not LM image) length mod 512
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;get the header length in dx.cx
;;;     mov  ax,  hdrParaCount  ;get header length in paragraphs
;;;     xor  dx,  dx
;;;     mov  cx,  4             ;convert to bytes by multiplying by 16 (2**4)
;hdrParaCount_mult16:
;;;     shl  ax,  1
;;;     rcl  dx,  1
;;;     loop hdrParaCount_mult16
;;;     mov  cx, ax
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   ;get the file length in bx.ax
        mov  ax,  RepairLength  ;get the restored file length
        mov  bx,  RepairLength+2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;get the image length (= file length - header length)
;;;     sub  ax,  cx            ;subtract the header length
;;;     sbb  bx,  dx
;;;     nFailc 28               ;header longer than file!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   ;set the header's image-length-mod-512 field
        and  ax,  1FFh          ;image length mod 512 (200h)
        mov  imgLenMod512,ax

   ;set the header's file-length-in-pages field
        mov  ax, RepairLength
        mov  bx, RepairLength+2
        xor  dx, dx
        mov  cx, 9              ;512 = 2**9
setlen_div512:                  ;divide by 512
        shr  bx, 1
        rcr  ax, 1
        rcr  dx, 1              ;collect remainder
        loop setlen_div512
        and dx,dx       ;don't need to adjust remainder (shr dx,16-9); just need to know if there is one
        jz   HaveFilePageCount
        add  ax, 1              ;extra page for the remainder
        nFailc 29               ;overflow!

HaveFilePageCount:
        and  bx,bx
        nFailnz 30              ;overflow!
        mov  exePageCount, ax

   ;go to beginning of the reconstruction
        mov  dx, InfectedLen
        mov  cx, InfectedLen+2
        SeekStart
        nFailc 31

   ;write the header's 1st 6 bytes
        mov  dx, offset Bfr
        mov  cx, 6
        FileWrite
        nFailc 32
        cmp  ax, 6
        nFailne 33

        jmp  IntNext


;=== is the file short or long (with respect to the critical length) ======

IntShortFileTestDone   equ IntNext
IntLongFileTestDone    equ IntNext

IntShortFileTest:
        mov  ax, cs:Instrctn.critLen[si]
        mov  dx, cs:Instrctn.critLen[si]+2
        sub  ax, InfectedLen
        sbb  dx, InfectedLen+2
        js   is_long

is_short:
        cmp FileType, COM
        jz  is_short_COM

is_short_EXE:
        mov FileType, EXEs
        jmp IntShortFileTestDone

is_short_COM:
        mov FileType, COMs
        jmp IntShortFileTestDone

is_long:
        cmp FileType, COM
        jz  is_long_COM

is_long_EXE:
        mov FileType, EXEl
        jmp IntLongFileTestDone

is_long_COM:
        mov FileType, COMl
        jmp IntLongFileTestDone

;=== initialized variables ====================================================
ALIGN 16

CRCpoly label   dword
        INCLUDE CRCtable.inc

RepairLength    dw 0
                dw 0
RepairSrc       dw 0
                dw 0
RepairDest      dw 0
                dw 0

;=== uninitialized variables ==================================================
ALIGN 16

pInstructions   dw ?
pInstrEnd       dw ?

InfectedLen     dw ?
                dw ?
Last            dw ?
                dw ?
EP              dw ?
                dw ?
FieldPos        dw ?
                dw ?
FieldLen        dw ?
                dw ?
CRC             dw ?
                dw ?

Key             dw ?
ReadLen         dw ?

FileType        db ?

;--- parts of the file --------------------------------------------------------
ALIGN 16

Bfr     label   byte
exeM            db ?
exeZ            db ?
imgLenMod512    dw ?
exePageCount    dw ?
RelocItemCount  dw ?
hdrParaCount    dw ?
minParaCount    dw ?
maxParaCount    dw ?
SSparagraph     dw ?
initialSP       dw ?
checksum        dw ?
entryPoint      dw ?
CSparagraph     dw ?
pRelocTable     dw ?
nOverlay        dw ?

HDRLEN          equ offset nOverlay - offset exeM + 1

                db BFRLEN-HDRLEN dup(?)

;==============================================================================
