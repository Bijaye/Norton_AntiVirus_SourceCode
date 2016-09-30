;===================================================================
;
; File name:     autoverv.dcl
;
; Description:   binary virus verification & repair - declarations.
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
;===================================================================

OPTION LJMP

;------------------------------------------------------------------------------
; Verification & repair manifest constants
;------------------------------------------------------------------------------

FILE       equ 'F'
COMs       equ 'c'
EXEs       equ 'x'
COMl       equ 'C'
EXEl       equ 'X'
COM        equ 'C'      ; ANY is equivalent to LONG, because for length-independent code, the critical length is zero.
EXE        equ 'X'

DO_VERIFY  equ 'V'
DO_REPAIR  equ 'R'
DO_SETLEN  equ 'L'
DO_CRITLEN equ 'C'

BEGIN      equ 'B'
ENTRY      equ 'T'
EOF        equ 'E'

PLAIN      equ 'P'
ADD1       equ 'a'
XOR1       equ 'x'
ADD2       equ 'A'
XOR2       equ 'X'

;------------------------------------------------------------------------------
; verify / repair instruction structure
;------------------------------------------------------------------------------

Instrctn struc
   op           db ?
   objectType   db ?
   encr         db ?
   key_anchor   db ?
   key_offset   dw ?
                dw ?
   start_anchor db ?
   start_offset dw ?
                dw ?
   end_anchor   db ?
   end_offset   dw ?
                dw ?
   dest_anchor  db ?            ;?? not needed if always appending
   dest_offset  dw ?            ;?? ditto
                dw ?
   fieldCRC     equ dest_offset
   critLen      equ key_offset
Instrctn ends

;------------------------------------------------------------------------------
; instruction macros
;------------------------------------------------------------------------------

Verify MACRO ObjType, EncrType,
             KeyAnchor, KeyOffset,
             StartAnchor, StartOffset,
             EndAnchor, EndOffset,
             CRCvalue
        DB DO_VERIFY, ObjType, EncrType
        DB KeyAnchor
        DD KeyOffset
        DB StartAnchor
        DD StartOffset
        DB EndAnchor
        DD EndOffset
        DB ?
        DD CRCvalue
        ENDM

Repair MACRO ObjType, EncrType,
             KeyAnchor, KeyOffset,
             StartAnchor, StartOffset,
             EndAnchor, EndOffset,
             DestAnchor, DestOffset
        DB DO_REPAIR, ObjType, EncrType
        DB KeyAnchor
        DD KeyOffset
        DB StartAnchor
        DD StartOffset
        DB EndAnchor
        DD EndOffset
        DB DestAnchor
        DD DestOffset
        ENDM

SetLength MACRO ObjType
        DB DO_SETLEN, ObjType, 0
        DB 0
        DD 0
        DB 0
        DD 0
        DB 0
        DD 0
        DB 0
        DD 0
        ENDM

CriticalLength MACRO len
        DB DO_CRITLEN, FILE, 0
        DB 0
        DD len
        DB 0
        DD 0
        DB 0
        DD 0
        DB 0
        DD 0
        ENDM

;------------------------------------------------------------------------------
; parameters to the autoverv verification & repair code
;------------------------------------------------------------------------------

;!!RepairParm struc
;!!     pStart  dw ?   ; points to an array of autoverv-generated instructions
;!!     pEnd    dw ?   ; the address after the last item in that array
;!!RepairParm ends

;==============================================================================
