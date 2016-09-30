;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/INCLUDE/VCS/cts4.inv   1.97   09 Dec 1996 12:12:52   JALLEE  $
;
; Filename:
; Programmer:
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:
;
; Note:
;
; Copyright (C) 1992-1993 Symantec Corporation
;
; $Log:   S:/INCLUDE/VCS/cts4.inv  $
;  
;     Rev 1.97   09 Dec 1996 12:12:52   JALLEE
;  Ported from qak2
;  ;     Rev 1.81.1.17   09 Nov 1995 15:39:08   DCHI
;  ;  Added SCN4_GET_NAME and SCN4_GET_VIRUS prototypes.
;  ;  
;  ;     Rev 1.81.1.16   18 Sep 1995 17:23:04   DCHI
;  ;  Added FIL4_NEW_GET_START prototype.
;  
;     Rev 1.96   14 May 1996 20:36:10   JWILBER
;  Changed the offset of the non-resident name table in the NE header
;  structure definition to a DWORD instead of a WORD.
;
;     Rev 1.95   19 Mar 1996 14:51:52   DALLEE
;  Added LPBYTE typedef.
;
;     Rev 1.94   01 Jun 1995 13:23:30   DSACKING
;  Ported changes from 3.0.6
;
;     Rev 1.93   29 Nov 1994 14:23:40   DALLEE
;  Reget from QAK2 post NAV 305.
;
;     Rev 1.81.1.10   24 Oct 1994 10:45:10   DALLEE
;  Added defines for N30.wControl2 -- NO_GENERIC and DISPLAY_DIALOG.
;  Made rude change to N30NOTES -- Changed wControl to wControl2.
;     No code ever read this value.
;     Only VIRLIST wrote to this value, and wControl2 will work as well.
;     Can't change the size since this is passed betweeen .DLL's.
;     Scanner code now needs to reference the wControl flags.
;
;     Rev 1.81.1.9   22 Oct 1994 15:19:24   SZIADEH
;  added fixed id for one_half
;
;     Rev 1.81.1.8   13 Oct 1994 21:10:36   SZIADEH
;  prototyped GetInfectionData()
;
;     Rev 1.81.1.7   07 Oct 1994 19:04:24   SZIADEH
;  added BufferOffset component to STARTCODE struct.
;
;     Rev 1.81.1.6   15 Sep 1994 01:13:52   SZIADEH
;  changed SCN4_ALG to accept another paramerter for FirstLanding.
;
;     Rev 1.81.1.5   30 Mar 1994 10:52:54   PATRICKM
;  Checked in from latest QAK2 src
;
;     Rev 1.81.1.4   02 Mar 1994 18:17:00   DALLEE
;  Added excREG_CHANGE and incREG_CHANGE #defines.
;
;     Rev 1.81.1.3   16 Dec 1993 15:53:32   DALLEE
;  Added parameter to SCN4_BOOT to execute code chunks from VIRSCAN.DAT file.
;
;     Rev 1.81.1.2   01 Nov 1993 15:41:14   MFALLEN
;  GetAnsiFlag() and SetAnsiFlag()
;
;     Rev 1.81.1.1   22 Oct 1993 13:42:42   JMILLARD
;  add prototypes for RelinquishControl
;
;     Rev 1.81.1.0   19 Oct 1993 11:31:50   SCOTTP
;  Branch base for version QAK2
;
;     Rev 1.81   13 Oct 1993 17:36:02   BARRY
;  Merged QAK1 changes
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

;------------------------------------------------------------------------------
; For Repair functions prototypes include REPAIR.INC. snz
;------------------------------------------------------------------------------
; Preprocessor used to name each unique segment.
;------------------------------------------------------------------------------
; Allow for:
;   DOS         - 16 bit                /DDOS16
;   DOS         - 32 bit                /DDOS32
;   Windows     - 16 bit                /DWIN16
;   Windows     - 32 bit, segmented     /DWIN32
;   Windows NT  - 32 bit, flat          /DWINNT
;   OS/2        - 32 bit, flat          /DOS2
;   NLM         - 32 bit, flag          /DNLM
;   MAC         - 16 bit
;   MAC         - 32 bit
;   Unix        - 16 bit
;   Unix        - 32 bit
;------------------------------------------------------------------------------
;******************************************************************************
; Language and memory model section
;******************************************************************************
OPTION LANGUAGE:PASCAL

TRUE    EQU     1
FALSE   EQU     0
BOOL    TYPEDEF WORD

IFDEF   SDOS16
OS_16   EQU     TRUE
OS_32   EQU     FALSE
FARCALL EQU     FALSE
FARDATA EQU     TRUE
.286
.model  small
UniqueSegment   MACRO
.code
        ENDM
ENDIF

IFDEF   DOS16
OS_16   EQU     TRUE
OS_32   EQU     FALSE
FARCALL EQU     TRUE
FARDATA EQU     FALSE
.286
.model medium
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   DOS32
OS_16   EQU     FALSE
OS_32   EQU     TRUE
FARCALL EQU     TRUE
FARDATA EQU     FALSE
.386
.model medium
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   WIN16
OS_16   EQU     TRUE
OS_32   EQU     FALSE
FARCALL EQU     TRUE
FARDATA EQU     FALSE
OPTION PROC:EXPORT
.286
.model medium
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   WIN32
OS_16   EQU     FALSE
OS_32   EQU     TRUE
FARCALL EQU     TRUE
FARDATA EQU     FALSE
OPTION PROC:EXPORT
.386
.model medium
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   WINNT
OS_16   EQU     FALSE
OS_32   EQU     TRUE
FARCALL EQU     FALSE
FARDATA EQU     FALSE
.386
.model flat
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   OS2
OS_16   EQU     FALSE
OS_32   EQU     TRUE
FARCALL EQU     FALSE
FARDATA EQU     FALSE
OPTION PROC:EXPORT
.386
.model flat
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFDEF   NLM
OS_16   EQU     FALSE
OS_32   EQU     TRUE
FARCALL EQU     FALSE
FARDATA EQU     FALSE
.386
.model flat
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

IFNDEF  OS_16
OS_16   EQU     TRUE
OS_32   EQU     FALSE
FARCALL EQU     TRUE
FARDATA EQU     FALSE
.model medium
UniqueSegment   MACRO
.code @Filename
        ENDM
ENDIF

;------------------------------------------------------------------------------
; Define the REGISTER set we want to use.
;------------------------------------------------------------------------------
if OS_16

FAR16_NEAR32    EQU     FAR

XSP             EQU     sp
XBP             EQU     bp
XDI             EQU     di
XSI             EQU     si
XDX             EQU     dx
XCX             EQU     cx
XBX             EQU     bx
XAX             EQU     ax

XES             EQU     es:
XDS             EQU     ds:

SES             EQU     es
SDS             EQU     ds
SCS             EQU     cs
vSDS            EQU     ds::            ; Needed with INVOKE, DS::SI
vSES            EQU     es::            ; Needed with INVOKE, ES::DI

LPBYTE          TYPEDEF FAR PTR BYTE
LPSTR           TYPEDEF FAR PTR DWORD
FARNEAR         TYPEDEF FAR
WORD32          TYPEDEF WORD
UINT            TYPEDEF WORD
HGLOBAL         TYPEDEF WORD
HLOCAL          TYPEDEF WORD
LPCSTR          TYPEDEF FAR PTR DWORD
HINSTANCE       TYPEDEF WORD
HWND            TYPEDEF WORD
endif

if OS_32

FAR16_NEAR32    EQU     NEAR

XSP             EQU     esp
XBP             EQU     ebp
XDI             EQU     edi
XSI             EQU     esi
XDX             EQU     edx
XCX             EQU     ecx
XBX             EQU     ebx
XAX             EQU     eax

XES             EQU     <>
XDS             EQU     <>

SES             EQU
SDS             EQU
SCS             EQU
vSDS            EQU                     ; Needed with INVOKE, DS::SI
vSES            EQU                     ; Needed with INVOKE, ES::DI

LPBYTE          TYPEDEF NEAR PTR BYTE
LPSTR           TYPEDEF NEAR PTR DWORD
FARNEAR         TYPEDEF NEAR
WORD32          TYPEDEF DWORD
UINT            TYPEDEF DWORD
HGLOBAL         TYPEDEF WORD
HLOCAL          TYPEDEF WORD
LPCSTR          TYPEDEF FAR PTR DWORD
HINSTANCE       TYPEDEF WORD
HWND            TYPEDEF WORD
endif
;------------------------------------------------------------------------------
; MACRO Section
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; LDS_SI
;------------------------------------------------------------------------------
LDS_SI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        lds     si,arg1.arg2.arg3
  ELSE
        lds     si,arg1.arg2
  ENDIF
 ELSE
        lds     si,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     esi,arg1.arg2.arg3
  ELSE
        mov     esi,arg1.arg2
  ENDIF
 ELSE
        mov     esi,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LDS_DI
;------------------------------------------------------------------------------
LDS_DI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        lds     di,arg1.arg2.arg3
  ELSE
        lds     di,arg1.arg2
  ENDIF
 ELSE
        lds     di,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     edi,arg1.arg2.arg3
  ELSE
        mov     edi,arg1.arg2
  ENDIF
 ELSE
        mov     edi,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LDS_BX
;------------------------------------------------------------------------------
LDS_BX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        lds     bx,arg1.arg2.arg3
  ELSE
        lds     bx,arg1.arg2
  ENDIF
 ELSE
        lds     bx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     ebx,arg1.arg2.arg3
  ELSE
        mov     ebx,arg1.arg2
  ENDIF
 ELSE
        mov     ebx,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LDS_DX
;------------------------------------------------------------------------------
LDS_DX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        lds     dx,arg1.arg2.arg3
  ELSE
        lds     dx,arg1.arg2
  ENDIF
 ELSE
        lds     dx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     edx,arg1.arg2.arg3
  ELSE
        mov     edx,arg1.arg2
  ENDIF
 ELSE
        mov     edx,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LES_SI
;------------------------------------------------------------------------------
LES_SI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        les     si,arg1.arg2.arg3
  ELSE
        les     si,arg1.arg2
  ENDIF
 ELSE
        les     si,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     esi,arg1.arg2.arg3
  ELSE
        mov     esi,arg1.arg2
  ENDIF
 ELSE
        mov     esi,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LES_DI
;------------------------------------------------------------------------------
LES_DI  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        les     di,arg1.arg2.arg3
  ELSE
        les     di,arg1.arg2
  ENDIF
 ELSE
        les     di,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     edi,arg1.arg2.arg3
  ELSE
        mov     edi,arg1.arg2
  ENDIF
 ELSE
        mov     edi,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LES_BX
;------------------------------------------------------------------------------
LES_BX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        les     bx,arg1.arg2.arg3
  ELSE
        les     bx,arg1.arg2
  ENDIF
 ELSE
        les     bx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     ebx,arg1.arg2.arg3
  ELSE
        mov     ebx,arg1.arg2
  ENDIF
 ELSE
        mov     ebx,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; LES_DX
;------------------------------------------------------------------------------
LES_DX  MACRO arg1:REQ,arg2,arg3

IF OS_16
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        les     dx,arg1.arg2.arg3
  ELSE
        les     dx,arg1.arg2
  ENDIF
 ELSE
        les     dx,arg1
 ENDIF
ENDIF

IF OS_32
 IFNB <arg2>                             ; If blank skip
  IFNB <arg3>
        mov     edx,arg1.arg2.arg3
  ELSE
        mov     edx,arg1.arg2
  ENDIF
 ELSE
        mov     edx,arg1
 ENDIF
ENDIF
        ENDM

;------------------------------------------------------------------------------
; SMOV move segment to a segment, destroy! AX register
;------------------------------------------------------------------------------
SMOV MACRO seg1,seg2

if OS_16
        push    ax
        mov     ax,seg2
        mov     seg1,ax
        pop     ax
endif
if OS_32

endif
        ENDM

;------------------------------------------------------------------------------
; move to ds from a register
;------------------------------------------------------------------------------
MOV_DS  MACRO   arg1

if OS_16
        mov     ds,arg1
endif
if OS_32

endif
        ENDM

;------------------------------------------------------------------------------
; move to es from a register
;------------------------------------------------------------------------------
MOV_ES  MACRO   arg1

if OS_16
        mov     es,arg1
endif
if OS_32

endif
        ENDM

;------------------------------------------------------------------------------
; move to ss from a register
;------------------------------------------------------------------------------
MOV_SS  MACRO   arg1

if OS_16
        mov     ss,arg1
endif
if OS_32

endif
        ENDM

;------------------------------------------------------------------------------
; 32bit multiplication
;------------------------------------------------------------------------------
MUL32   MACRO   source

if OS_16
        push    bx
        mov     bx,dx
        mul     source
        push    ax
        push    dx
        mov     ax,bx
        mul     source
        pop     dx
        add     dx,ax
        pop     ax
        pop bx
endif
if OS_32
        mul     source
endif
        ENDM

;------------------------------------------------------------------------------
; Clear a string
;------------------------------------------------------------------------------
macClearString  MACRO   String,lenString
        LES_DI  String
        xor     XCX,XCX         ; 32 bit protection
        mov     cx,lenString
        xor     al,al
        rep     stosb
        ENDM

;------------------------------------------------------------------------------
; Copy a string
;------------------------------------------------------------------------------
macCopyString   MACRO   FromString,ToString
        LDS_SI  FromString
        LES_DI  ToString
mCS0:
        lodsb
        stosb
        or      al,al
        jnz     mCS0
        ENDM

;------------------------------------------------------------------------------
; Copy memory
;------------------------------------------------------------------------------
macMemCopy      MACRO   FromMem,ToMem,Count
        LDS_SI  FromMem
        LES_DI  ToMem
        xor     XCX,XCX         ; 32 bit protection
        mov     cx,Count
        rep     movsb
        ENDM


;******************************************************************************
; EQUATE Section
;******************************************************************************

fREAD           EQU 0                   ; Open types
fWRITE          EQU 1
fREAD_WRITE     EQU 2
fCREATE         EQU 4
fAPPEND         EQU 8

fSHARE_COMPAT       EQU 000h            ; Share bit flags
fSHARE_EXCLUSIVE    EQU 010h
fSHARE_DENY_WRITE   EQU 020h
fSHARE_DENY_READ    EQU 030h
fSHARE_DENY_NONE    EQU 040h

fSEEK_BEG       EQU 1                   ; Used for Seeking handle in file
fSEEK_FOR       EQU 2
fSEEK_BAK       EQU 4
fSEEK_END       EQU 8

DIR_ENTRY_LEN   EQU 256                 ; MAX Length of directory entry

GET_INFO        EQU 0                   ; Get or set info flag
SET_INFO        EQU 1

TRUE            EQU 1                   ; Typical T/F
FALSE           EQU 0

MAXPAD          EQU 64                  ; Used for Get Start padding count

; MAX_PATH commented out since it does not appear to be used in any .ASM's and
; it has been removed from CTS4.H (it is #define'd  to 144 in UNIVERSL.H).
;MAX_PATH        EQU 78                  ; Max with of a DOS path

MAX_PATH_SIZE   EQU 144
MAX_NET_PATH_SIZE EQU 256

PROTECT_ADD     EQU 0                   ; Used for File protection
PROTECT_TEST    EQU 1
PROTECT_DELETE  EQU 2
PROTECT_REPAIR  EQU 4
PROTECT_CREATE  EQU 8
PROTECT_QUICK_INOC  EQU 16

HIDDEN          EQU 2                   ; Used to search for files
SYSTEM          EQU 4
VOLLABEL        EQU 8
DIR             EQU 16

IS_COM          EQU 0                   ; Used for Get Start
IS_EXE          EQU 1
IS_SYS          EQU 2
IS_BCOM         EQU 3                   ; Bulgarian COM
IS_VCOM         EQU 4                   ; Verified COM
IS_ZIP          EQU 5
IS_LZEXE        EQU 6
IS_PKEXE        EQU 7
IS_WHALE        EQU 8
IS_INVALID      EQU 9
STARTSIZE       EQU 256

IS_FILE         EQU 0                   ; Used by Find All
IS_DIRECTORY    EQU 1
DIR_DEPTH       EQU 33                  ; How many levels of directories

MEM_NORMAL      EQU 0h                  ; Memory flags
MEM_LOCKED      EQU 1h
MEM_FIXED       EQU 2h
MEM_READONLY    EQU 4h
MEM_CODE        EQU 8h
MEM_DATA        EQU MEM_NORMAL

MEM_TYPE_UMB    EQU 1h
MEM_TYPE_CONV   EQU 2h
MEM_TYPE_EMS    EQU 3h
MEM_TYPE_XMS    EQU 4h
MEM_TYPE_SWAP   EQU 5h

MEM_REQUEST_UMB EQU 10h
MEM_RELEASE_UMB EQU 11h
MEM_ALLOCATE_XMS EQU 09h
MEM_FREE_XMS    EQU 0Ah
MEM_LOCK_XMS    EQU 0Ch
MEM_UNLOCK_XMS  EQU 0Dh

MEM_QUERY_A20           EQU 07h
MEM_LOCALENABLE_A20     EQU 05h
MEM_LOCALDISABLE_A20    EQU 06h
MEM_GLOBALENABLE_A20    EQU 03h
MEM_GLOBALDISABLE_A20   EQU 04h
MEM_REQUEST_HMA         EQU 01h

HMA_STATUS      EQU 1
HMA_ON          EQU 2
HMA_OFF         EQU 3

CR              EQU 0Dh
LF              EQU 0Ah

;------------------------------------------------------------------------------
; Command line flags

CL_FLAG         EQU     1               ; /FLAG
CL_STRING       EQU     2               ; "This string" or String
CL_FILENAME     EQU     2               ; FILENAME
CL_NUMERIC      EQU     3               ; /NUM 1234
CL_OTHER        EQU     4               ; /STR "This string" or String
CL_HEX          EQU     5               ; /HEX AB

;------------------------------------------------------------------------------
; ERROR EQUates
;------------------------------------------------------------------------------
FIL4_NOT_FOUND  EQU 1
REPAIR_FAIL     EQU 4
FILE_CHANGED    EQU 5
INVALID_INPUT   EQU 6

SELF_TEST_BUFFER_SIZE   EQU 512
                                        ; When computing, if the self test
                                        ; CRC == 0, fudge it to this value,
SELF_TEST_ZERO_CRC      EQU 1           ; since 0 means SELF_TEST_NO_CRC.

SELF_TEST_SUCCESS       EQU 0
SELF_TEST_NO_CRC        EQU 1
SELF_TEST_FAIL          EQU 2
SELF_TEST_FILE_ERROR    EQU 3


;------------------------------------------------------------------------------
; Virus detection equates
;------------------------------------------------------------------------------
CRCtype         EQU 0
ALGtype         EQU 1
BOOtype         EQU 2
MACtype         EQU 3                   ; Don't blame me, its Dennis' fault

SIZE_VIRUSNAME  EQU (21 + 1)            ; This includes the EOS character
SIZE_VINFO      EQU 32
SIZE_MEM        EQU 15
SIZE_SIG        EQU 17
SIZE_FIXED      EQU 64
SIZE_BOOTSIG    EQU 14
SIZE_COMMENT    EQU 301                                                                 ; 5 lines of 60 chars per line + EOS
SIZE_ALIAS      EQU 22
SIZE_CRC        EQU 2
SIZE_CHECK_DATA EQU 2

SIZE_TSR        EQU     35056           ; Default size for DOS_TSR

uSizeVirscanHeader EQU  256             ; "VIRSCAN.DAT" header size

bitINFO_COMFILE    EQU 0001h
bitINFO_EXEFILE    EQU 0002h
bitINFO_SYSFILE    EQU 0004h
bitINFO_COMCOM     EQU 0008h
bitINFO_FLOPBOOT   EQU 0010h
bitINFO_HDISKBOOT  EQU 0020h
bitINFO_MBOOTREC   EQU 0040h
bitINFO_DIR2       EQU 0080h
bitINFO_RESIDENT   EQU 0100h
bitINFO_SIZESTLTH  EQU 0200h
bitINFO_FULLSTLTH  EQU 0400h
bitINFO_ENCRYPT    EQU 0800h
bitINFO_POLYMORPH  EQU 1000h
bitINFO_TRIGGERED  EQU 2000h
bitINFO_INTHEWILD  EQU 4000h
bitINFO_MULTIPARTY EQU 8000h
bitCTRL_NOTCLEAN   EQU 8000h

; bit Settings for uLFLAGS
bitDSSI         EQU     0001h           ; Filename located in DS:SI not DS:DX
bitINT13        EQU     0002h           ; If SET WIN/DOS selector/segment usage
bitTSR          EQU     8000h           ; If SET TSR doing work.
bitWINDOWS      EQU     4000h           ; If SET Windows version doing work.

; bit Setting for Buttons
bitREPAIR       EQU     0001h
bitDELETE       EQU     0002h
bitINOCULATE    EQU     0004h
bitREINOCULATE  EQU     0008h
bitCONTINUE     EQU     0010h
bitSTOP         EQU     0020h
bitEXCLUDE      EQU     0040h

bitAUTOREPAIR   EQU     0100h
bitAUTODELETE   EQU     0200h
bitAUTOINOCUL   EQU     0400h

; Exception database flags.
excVIOLATING    EQU     0002h
excVIRUS_FOUND  EQU     0004h
excINOC_CHANGE  EQU     0008h
excFILE_WATCH   EQU     0010h
excLOW_FORMAT   EQU     0020h OR excVIOLATING
excWR_HBOOT     EQU     0040h OR excVIOLATING
excWR_FBOOT     EQU     0080h OR excVIOLATING
excWR_ATLIST    EQU     0100h OR excVIOLATING
excRO_CHANGE    EQU     0200h OR excVIOLATING
excREG_CHANGE   EQU     0400h

; Inclusion database flags.  (For BANKERS file registration)
incREG_CHANGE   EQU     0001h


cmbHalt         EQU     3
cmbREPAIR       EQU     14
cmbDELETE       EQU     15
cmbINOCULATE    EQU     16
cmbREINOCULATE  EQU     17
cmbCONTINUE     EQU     18
cmbSTOP         EQU     19
cmbEXCLUDE      EQU     20

ObjectAtList    EQU     1
ObjectLogName   EQU     2
ObjectNetUsers  EQU     3
ObjectExcept    EQU     4
ObjectLogCMB    EQU     5
ObjectPath      EQU     6
ObjectMessCMB   EQU     7
ObjectN30       EQU     8
ObjectNetMess   EQU     9
ObjectUserName  EQU     10
ObjectNetINOC   EQU     11
ObjectScanAll   EQU     12
ObjectChkBothFl EQU     13
ObjectRemoveAft EQU     14
ObjectSeconds   EQU     15
ObjectBeepTsr   EQU     16
ObjectAlertUsr  EQU     17
ObjectAlertCon  EQU     18
ObjectAlrtKnown EQU     19
ObjectAlrtUnkno EQU     20
ObjectAlrtInocu EQU     21
ObjectAlrtVLike EQU     22
ObjectActKnown  EQU     23
ObjectActUnknow EQU     24
ObjectActInocu  EQU     25
ObjectActVLike  EQU     26
ObjectBackupRep EQU     27
ObjectBackupExt EQU     28
ObjectAlertMsg  EQU     29
ObjectInocName  EQU     30
ObjectUnLevel   EQU     31              ; Level of flags for file watch


CRITICAL_MESS   EQU     0001h
CRITICAL_QUES   EQU     0002h

;------------------------------------------------------------------------------
; File length equates
;------------------------------------------------------------------------------
sDOSFILE        EQU     78
sNETFILE        EQU     128
sLONGFILE       EQU     256
sVERYLONGFILE   EQU     260

;------------------------------------------------------------------------------
; MARC equates
;------------------------------------------------------------------------------
MARC_ORIG       EQU     0           ; Place file in original directory
MARC_NEW        EQU     1           ; Place file in new directory

;---------------------------------------------------------------------------
; FIXED ID equates
;---------------------------------------------------------------------------
FID_MTE         EQU     2      ;  1   Value is offset in word array
FID_POGUE       EQU     4      ;  2   converted to zerobase.(-1)*2
FID_WHALE       EQU     6      ;  3   Thus ID 1 = 0, 2 = 2, 3 = 4, 4 = 6, 5 = 8
FID_TPE         EQU     8      ;  4   Fixed ID
FID_V2PX        EQU     10     ;  5
FID_CASCADE     EQU     12     ;  6   For Repair Decrypt
FID_TEQUILA     EQU     14     ;  7   For Repair Decrypt
FID_MALTESE     EQU     16     ;  8
FID_PX2000      EQU     18     ;  9
FID_NOINT       EQU     20     ; 10   For Mem detect
FID_FLIP2343    EQU     22     ; 11   For Repair Decrypt
FID_FLIP2153    EQU     24     ; 12   For Repair Decrypt
FID_DIR2        EQU     26     ; 13   For custom repair
FID_TREMOR      EQU     28     ; 14   Detection
FID_TODOR       EQU     30     ; 15   Detection
FID_GROOVE      EQU     32     ; 16   Detection
FID_MONKEY      EQU     34     ; 17   Repair Monky class of viruses
FID_ONEHALF     EQU     35     ; 18   ONE_HALF repair
;-----------------------------------------------------------------------------
; BOOTLOCK equates
;-----------------------------------------------------------------------------
BPB_OFFSET      equ     0bh    ; offset into the boot sector
PARTITION_OFFSET equ    1beh
global_offset   equ     7C00h  ; Global offset for boot code
int13address    equ     4Ch
ISR13seg        equ     09FE0h
ISR13offset     equ     0000h

;-------------------------------------------------------------------------
; Equates for Control Word
;-------------------------------------------------------------------------
GENERIC_COM     EQU 1b
GENERIC_EXE     EQU 10b
PREPENDING      EQU 100b
REBUILD_EXE     EQU 1000b
IMPROVED        EQU 10000b
EXE_CLEAN       EQU 1010b
COM_CLEAN       EQU 10101b
SECONDBOOK      EQU 1000000000b
SLIDER          EQU 10000000000b
DECRYPT         EQU 100000000000b
NOCLEAN         EQU 8000h
;-------------------------------------------------------------------------
; Equates for Second Control Word
;-------------------------------------------------------------------------
CUT_BEGIN       EQU 1b
TWEEK_CS_10     EQU 10b
TWEEK_SS_10     EQU 100b
BOOK_FROM_BEGIN EQU 1000b
COM_DISTINCT    EQU 10000b
EXEC_CODE       EQU 100000b
DECRYPT_XOR     EQU 1000000b
DECRYPT_ADD     EQU 10000000b
; high byte
DECRYPT_WORD    EQU 100000000b
NO_GENERIC      EQU 1000000000b
DISPLAY_DIALOG  EQU 10000000000b
NOT_IN_TSR      EQU 100000000000b

;-----------------------------------------------------------------------------
; Equates for TSR
;-----------------------------------------------------------------------------
TSR_ID          EQU     0E5E5h          ; My TSR ID
TSR_SIZE        EQU     45 * 1024       ; Size of TSR code/data
TSR_AREYOUHERE  EQU     1               ; Test if we're in memory
TSR_GETDATA     EQU     2               ; Get pointer to internal flags
TSR_NETWORK     EQU     3               ; NR Toggle
TSR_GETXMS      EQU     4               ; Get address of XMS
TSR_PUTXMS      EQU     5               ; Put XMS mem back
TSR_GETEMS      EQU     6               ; Get address of EMS
TSR_PUTEMS      EQU     7               ; Put EMS mem back

;------------------------------------------------------------------------------
; Equates for Network messageing
;------------------------------------------------------------------------------
NETMAXSERVERNAME        EQU     64
NETMAXUSERNAME          EQU     64
NOV_MAXMESSLEN          EQU     55
NAV_NLM                 EQU     0531h

;------------------------------------------------------------------------------
; Beep level defines
;------------------------------------------------------------------------------
BEEP_UNDEFINED          EQU     20
BEEP_KNOWNBOX           EQU     5
BEEP_FBOOTBOX           EQU     5
BEEP_INOCBOX            EQU     15
BEEP_REINOCBOX          EQU     7
BEEP_AUDITERROR         EQU     12
BEEP_INOCERROR          EQU     10
BEEP_BOOTPROTECT        EQU     10
BEEP_PTABPROTECT        EQU     10
BEEP_SYSFILEPROTECT     EQU     10
BEEP_WBOOTSCAN          EQU     10
BEEP_DURATION           EQU     1
BEEP_MONBOX             EQU     1

;------------------------------------------------------------------------------
; Equates for Character Conversions
;------------------------------------------------------------------------------
CHR_LOWER               EQU     0
CHR_UPPER               EQU     1

;******************************************************************************
; STRUCT Section
;******************************************************************************
;------------------------------------------------------------------------------
; File protection used with Inoculation/FileWatch/SysCheck
;------------------------------------------------------------------------------
File_Protect    STRUCT
if OS_16
        Filename        db      MAX_PATH_SIZE+1 dup(0)
else
        Filename        db      MAX_NET_PATH_SIZE+30 dup(0)
endif
        Fileattr        db      0
        Filetime        dw      0
        Filedate        dw      0
        Filesize        dd      0
        Header          db      35 dup(0)
        Version         db      0
        wReserved1      dw      0
        wReserved2      dw      0
        StartCRC        dw      0
        CRCAllBuf       dw      0
        CRCLastK        dw      0
File_Protect    ENDS
sFile_Protect   EQU     SIZE File_Protect

;------------------------------------------------------------------------------
; Format of a standard Disk Transfer Area
;------------------------------------------------------------------------------
DISK_TA STRUCT
        DTA_Search      db      21 dup(0)
        DTA_Attr        db      0
        DTA_Time        dw      0
        DTA_Date        dw      0
        DTA_SizeL       dw      0
        DTA_SizeH       dw      0
        DTA_FILENAME    db      13 dup(0)
DISK_TA ENDS
sDISK_TA EQU SIZE DISK_TA

;------------------------------------------------------------------------------
; Header of a Hashed Database structure
;------------------------------------------------------------------------------
DBC4_HEADER     struct
        Free_Pointer    dd      0
        Elem_Size       dw      0
        Total_Elem_Size dw      0
        IndexSize       dd      0
        DataStart       dd      0
        IndexStart      dd      0
        QuickStart      dd      0
        Comp_Func       dd      0
        Hash_Func       dd      0
        Quick_Func      dd      0
        FileHandle      dw      0
DBC4_HEADER     ends
sDBC4_HEADER    EQU     SIZE DBC4_HEADER

;------------------------------------------------------------------------------
; Type of chaining used in hash table from DBC4_HEADER
;------------------------------------------------------------------------------
CHAIN_TYPE  struct
        Next            dd      0
        Active          db      0
CHAIN_TYPE  ends
sCHAIN_TYPE     EQU     SIZE CHAIN_TYPE

;------------------------------------------------------------------------------
; Structure for Get Start code routine
;------------------------------------------------------------------------------
STARTCODE       struct
        Filetype        db      0
        FirstLanding    dd      0
    BufferOffset    dd      0
        Padding         db      0
STARTCODE       ends
sSTARTCODE      EQU     SIZE STARTCODE

;------------------------------------------------------------------------------
; Initialization for GET_ME_MEMORY
;------------------------------------------------------------------------------
INIT_MEMORY struct
        memTotal        dw      0       ; Total bytes wanted in paragraphs
        memUMB          dw      0       ; Total bytes wanted in UMB paragraphs
        memEMS          dw      0       ; Total bytes wanted in EMS paragraphs
        memHMA          dw      0       ; Total bytes wanted in HMA paragraphs
        memCONV         dw      0       ; Total bytes wanted in CONV paragraphs
        memlpCONV       dw      0       ; Pointer to CONV to use
        memDATA         dw      0       ; Total bytes wanted in DATA paragraphs
        memlpDATA       dw      0       ; Pointer to DATA to use
        memXMS          dw      0       ; Total bytes wanted in XMS paragraphs
        memlpFilename   dd      0       ; Pointer to temp file to create
INIT_MEMORY ends
sINIT_MEMORY    EQU     SIZE INIT_MEMORY

;------------------------------------------------------------------------------
; GET_ME_MEMORY structure
;------------------------------------------------------------------------------
GMEMORY struct
        lpUMB           dw      0       ; Segment of UMB memory
        lpCONV          dw      0       ; Segment of CONV memory
        lpDATA          dw      0       ; Segment of DATA memory
        lpHMA           dd      0       ; Pointer to HMA memory
        hEMS            dw      0       ; Handle to EMS memory
        hXMS            dw      0       ; Handle to XMS memory
        hSwapFile       dw      0       ; Handle to swap file
        uLastHandle     dw      0       ; Last handle allocated
        uFreeHandle     dw      0       ; Known freed handle
GMEMORY ends
sGMEMORY        EQU     SIZE GMEMORY

;------------------------------------------------------------------------------
; Local memory struct for allocation of global memory
;------------------------------------------------------------------------------
LMEMORY struct
        hHandle         dw      0       ; Handle to memory
        uSize           dw      0       ; Size of memory in paragraphs
        lpNEXT          dw      0       ; Segment of next memory
        lpPREV          dw      0       ; Segment of previous memory
        uFlags          dw      0       ; Flags of this memory seg
LMEMORY ends
sLMEMORY        EQU     SIZE LMEMORY

;-----------------------------------------------------------------------------
; Local memory struct for allocation of LOCAL memory
;-----------------------------------------------------------------------------
MEMLOCAL   STRUCT
        lLocked db      0
        lUsed   db      0
        lSize   dw      0
        lNext   dw      0
        lPrev   dw      0
        lHandle dw      0
MEMLOCAL   ENDS
sMEMLOCAL  EQU     SIZE MEMLOCAL

;-----------------------------------------------------------------------------
; Real Mode structure for calling bios interrupts
;-----------------------------------------------------------------------------
REALMODE STRUCT
       RM_DI   dw      0
               dw      0
       RM_SI   dw      0
               dw      0
       RM_BP   dw      0
               dw      0
               dd      0
       RM_BX   dw      0
               dw      0
       RM_DX   dw      0
               dw      0
       RM_CX   dw      0
               dw      0
       RM_AX   dw      0
               dw      0
       RM_FL   dw      0
       RM_ES   dw      0
       RM_DS   dw      0
       RM_FS   dw      0
       RM_GS   dw      0
       RM_IP   dw      0
       RM_CS   dw      0
       RM_SP   dw      0
       RM_SS   dw      0
REALMODE ENDS
sREALMODE      EQU     SIZE REALMODE


;------------------------------------------------------------------------------
; Structure for EXE header
;------------------------------------------------------------------------------
EXEHEADER       STRUCT
        exSignature     dw      5A4Dh   ; Sig of .EXE
        exExtraBytes    dw      ?       ; Number of extra bytes in last page
        exPages         dw      ?       ; Number of pages
        exRelocItems    dw      ?       ; Number of pointers in relocation table
        exHeaderSize    dw      ?       ; Size of header in paragraphs
        exMinAlloc      dw      ?       ; Minimun allocation
        exMaxAlloc      dw      ?       ; Max allocation
        exInitSS        dw      ?       ; Initial SS value
        exInitSP        dw      ?       ; Initial SP value
        exCheckSum      dw      ?       ; Complemented checksum
        exInitIP        dw      ?       ; Initial IP value
        exInitCS        dw      ?       ; Initial CS value
        exRelocTable    dw      ?       ; bytes offset to relocation table
        exOverlay       dw      ?       ; Overlay number
        exReserved      dw      16 dup(0)
        exExtendOffset  dw      0       ; Location of EH
EXEHEADER       ENDS
sEXEHEADER      EQU     SIZE EXEHEADER - 66
sEXELARGE       EQU     SIZE EXEHEADER

;------------------------------------------------------------------------------
; NE Header structure
;------------------------------------------------------------------------------
NEHEADER        STRUCT
        neSignature     dw      454Eh   ; Sig of the WINDOWS.EXE
        neLinkVersion   db      ?       ; Version of LINK used
        neLinkRevision  db      ?       ; Revision of LINK used
        neEntryOffset   dw      ?       ; Offset to entry table from EH Header
        neEntryLength   dw      ?       ; Length of entry table
        neNextDLL       dw      0       ; Reserved for windows
        neReserved1     dw      ?       ; Reserved
        neExeFlags      dw      ?       ; Flags describing EXE information
        neAutoDataNumb  dw      ?       ; Number of auto data segment
        neHeapSize      dw      ?       ; Initial size of heap
        neStackSize     dw      ?       ; Initial size of stack
        neCSIP          dd      ?       ; Segment:Offset of CS:IP
        neSSSP          dd      ?       ; Segment:Offset of SS:SP
        neSegTabNumb    dw      ?       ; Number of entries in the seg table
        neModRefNumb    dw      ?       ; Number of entries in the Mod ref table
        neNonresNumb    dw      ?       ; Number of entries in the NonResName table
        neSegTabOff     dw      ?       ; Relative offset to the segment table
        neResourceOff   dw      ?       ; Relative offset to the resource table
        neResidentOff   dw      ?       ; Relative offset to the ResidentName table
        neModRefOff     dw      ?       ; Relative offset to the ModuleRef table
        neImportOff     dw      ?       ; Relative offset to the ImportedName table
        neNonresOff     dd      ?       ; Relative (beg of file) to Nonres name table
        neMovable       dw      ?       ; Number of movable entry points
        neShift         dw      ?       ; Shift count to align the logical sectors
        neResourceNumb  dw      ?       ; Number of resource segments
        neOSFlags       db      ?       ; Flags describing the OS System
        neExeFlags2     db      ?       ; Flags describing EXE information
        neFastSector    dd      ?       ; Offset of fast load area in sectors
        neFastLength    dw      ?       ; Length of fast load area in sectors
        neVersion       dw      ?       ; Windows version number
NEHEADER        ENDS
sNEHEADER       EQU     NEHEADER

;------------------------------------------------------------------------------
; NE Support structures
;------------------------------------------------------------------------------
NESEGMENT_TABLE STRUCT
        neSegDataSec    dw      ?       ; Offset, in sectors, to the segment data (relative to begin of file)
        neSegLength     dw      ?       ; Segment length in file
        neSTFlags       dw      ?       ; Segment table flags
        neSegMin        dw      ?       ; Minimum size of segment
NESEGMENT_TABLE ENDS
sNESEGMENT_TABLE        EQU     SIZE NESEGMENT_TABLE

NEMODULE_REFERENCE STRUCT
        neModuleOffset  dw      ?       ; Offset to names in imported-name table
NEMODULE_REFERENCE ENDS
sNEMODULE_REFERENCE     EQU     SIZE NEMODULE_REFERENCE

NEENTRY_TABLE   STRUCT
        neBundleCount   db      ?       ; Number of entries in bundle, 0 = END
        neBundleFlag    db      ?       ; Type of bundle (FF=Fixed, FE=Movable)
NEENTRY_TABLE   ENDS
sNEENTRY_TABLE          EQU     SIZE NEENTRY_TABLE

NEENTRY_MOVABLE STRUCT
        neMovableFlag   db      ?       ; Flags for movable code
        neMovable3Fh    dw      ?       ; The INTERRUPT 3F
        neMovableSeg    db      ?       ; Specifies the segment number
        neMovableOff    dw      ?       ; Specifies the segment offset
NEENTRY_MOVABLE ENDS
sNEENTRY_MOVABLE        EQU     SIZE NEENTRY_MOVABLE

NEENTRY_FIXED STRUCT
        neFixedFlag     db      ?       ; Flags for fixed code
        neFixedOffset   dw      ?       ; Specifies an offset
NEENTRY_FIXED ENDS
sNEENTRY_FIXED          EQU     SIZE NEENTRY_FIXED

NERELOCATION_HEADER STRUCT
        neCount         dw      ?       ; Number of relocations
NERELOCATION_HEADER ENDS
sNERELOCATION_HEADER    EQU     SIZE NERELOCATION_HEADER

NEREL_IMP_NAME STRUCT
        neAddressType   db      ?       ; Type of address to fix
        neRelType       db      ?       ; Type of relocation to fix
        neRelFixMe      dw      ?       ; Offset needing to be fixed
        neIndexModule   dw      ?       ; Index to module reference table
        neOrdalNumber   dw      ?       ; Ordial number
NEREL_IMP_NAME ENDS
sNEREL_IMP_NAME         EQU     SIZE NEREL_IMP_NAME

NEREL_IMP_ORDIAL STRUCT
        neAddressType   db      ?       ; Type of address to fix
        neRelType       db      ?       ; Type of relocation to fix
        neRelFixMe      dw      ?       ; Offset needing to be fixed
        neIndexModule   dw      ?       ; Index to module reference table
        neNameOffset    dw      ?       ; Offset in name table
NEREL_IMP_ORDIAL ENDS
sNEREL_IMP_ORDIAL       EQU     SIZE NEREL_IMP_ORDIAL

NEREL_INT_FIXED STRUCT
        neAddressType   db      ?       ; Type of address to fix
        neRelType       db      ?       ; Type of relocation to fix
        neRelFixMe      dw      ?       ; Offset needing to be fixed
        neSegmentNumber db      ?       ; Segment number
                        db      0       ; Always a 0
        neOffset        dw      ?       ; Offset in segment
NEREL_INT_FIXED ENDS
sNEREL_INT_FIXED        EQU     SIZE NEREL_INT_FIXED

NEREL_INT_MOVE STRUCT
        neAddressType   db      ?       ; Type of address to fix
        neRelType       db      ?       ; Type of relocation to fix
        neRelFixMe      dw      ?       ; Offset needing to be fixed
                        db      0FFh    ; Always a 0FFh
                        db      0       ; Always a 0
        neOrdial        dw      ?       ; Ordial to segment entry table
NEREL_INT_MOVE ENDS
sNEREL_INT_MOVE         EQU     SIZE NEREL_INT_MOVE

;------------------------------------------------------------------------------
; Dynamic loading of a DLL for DOS structure
;------------------------------------------------------------------------------
DYNA_HEADER STRUCT
        lpSegment       dd      0       ; Pointer to segment table
        lpResource      dd      0       ; Pointer to resource table
        lpResident      dd      0       ; Pointer to resident name table
        lpModuleRef     dd      0       ; Pointer to module reference table
        lpImport        dd      0       ; Pointer to import name table
        lpEntry         dd      0       ; Pointer to entry table
        lpNonres        dd      0       ; Pointer to non resident table
        lpNextDLL       dd      0       ; Pointer to next loaded DLL
        hFilehandle     dw      0       ; Handle to DLL to load from
DYNA_HEADER ENDS
sDYNA_HEADER            EQU     SIZE DYNA_HEADER

;-----------------------------------------------------------------------------
; THE TSR Structure
;-----------------------------------------------------------------------------
TSR_Struct     STRUCT
; Global   - Data used by all EV, thru pointers
   lpRegs      dd      0       ; Register structure, not used in here
   lpBuffer    dd      0       ; Work buffer
   lpExtra     dd      0       ; Extra text buffer pointer
   uSizeBuffer dw      0       ; Size of work buffer
   uSizeExtra  dw      0       ; Size of extra buffer

; Internal "Message" handler
   uMessage1   dw      0       ; Message to place for user
   uQuestion1  dw      0       ; Question 1 to ask
   uMessage2   dw      0       ; Message to place for user if first failed.
   uQuestion2  dw      0       ; Question 2 to ask
   uLogBase    dw      0       ; Base to add to log

; Local    - Data only used by a single EV
   lpData      dd      0       ; Pointer to data for function
   uLFlags     dw      0       ; Flags for messages (FLAGS1)
   uCurrPSP    dw      0       ; Current Program Segment Prefix

; Extension to TSR functionallity
   lpGlobal    dd      0       ; Extension of GLOBAL Data
   lpCallBack  dd      0       ; Extension of CALLBACK Data
TSR_Struct     ENDS
sTSR_Struct    EQU     SIZE TSR_Struct

;-----------------------------------------------------------------------------
HOOK    STRUCT
   Interrupt   WORD    0       ; Interrupt to intercept
   Registers   WORD    0       ; Kernel function to hook
   Multi       db      0       ; Set to true if uses previous TSR_STRUCT
   EVDLL       db      20 dup(0); EV Function DLL
   EVFunc      db      40 dup(0); EV Function
   DataDLL     db      20 dup(0); Data dll
   DataFunc    db      40 dup(0); Data build routine
HOOK    ENDS
sHOOK   EQU     SIZE HOOK

;-----------------------------------------------------------------------------
BINHEADER   STRUCT
    Header              db      128 dup(0)
    EndOvl              db      0
    Signature           db      4 dup(0)
    uNumStructs         UINT    0
    uNumHooks           UINT    0
    uNumCallbacks       UINT    0
    bUSEUMB             db      0
    bUSEEMS             db      0
    bUSEXMS             db      0
    uTsrID              UINT    0       ; Id number for interrupt 2F
    uTsrSize            UINT    0
    uWorkBuffer         UINT    0
    uExtraBuffer        UINT    0
    GlobalDLL           db      20 dup(0)
    GlobalFunc          db      40 dup(0)
    EndOvl2             db      0
BINHEADER   ENDS
sBINHEADER  EQU SIZE BINHEADER

;-----------------------------------------------------------------------------
; Event Verify Structure
;-----------------------------------------------------------------------------
EV  STRUCT
    evUseMe         db  0   ; Use this or not
    evINTERRUPT     dw  0       ; Interrupt
    evFUNCTION      dw  0       ; Function, wild cards = ff
    evRoutine       dw  0   ; Pointer to offset of far routine
EV  ENDS
sEV     EQU     SIZE EV

;------------------------------------------------------------------------------
; Structure for callbacks in OVL file
;------------------------------------------------------------------------------
EVCALLBACK STRUCT
    CallbackDLL     db  20 dup(0)       ; DLL Name
    CallbackFunc    db  40 dup(0)       ; Function name
EVCALLBACK ENDS
sEVCALLBACK EQU SIZE EVCALLBACK

;------------------------------------------------------------------------------
; Structure for CMB file
;------------------------------------------------------------------------------
MESSAGE_HEADER  STRUCT
        POINTER             dd      0   ; Pointer to where in file message is
        MESSAGE_SIZE    UINT    0       ; Size of this message
MESSAGE_HEADER  ENDS
sMESSAGE_HEADER EQU     SIZE MESSAGE_HEADER

;------------------------------------------------------------------------------
; Register structure for TSR work
;------------------------------------------------------------------------------
TSR_REGS    STRUCT
        r_ES            WORD    0
        r_DS            WORD    0
        r_DI            UINT    0
        r_SI            UINT    0
        r_BP            UINT    0
        r_SP            UINT    0
        r_BX            UINT    0
        r_DX            UINT    0
        r_CX            UINT    0
        r_AX            UINT    0
TSR_REGS     ENDS
sTSR_REGS      EQU     SIZE TSR_REGS

;-----------------------------------------------------------------------------
; Window Structure
;-----------------------------------------------------------------------------
WINDOW  STRUCT
        wRow     dw     0
        wColumn  dw     0
        wWidth   dw     0
        wHeigth  dw     0
        wAttrib  db     0
        wBuffer  dd     0
        wVideo   dd     0
WINDOW  ENDS
sWINDOW EQU     SIZE WINDOW

;-----------------------------------------------------------------------------
; NOVELL Request/Reply structures
;-----------------------------------------------------------------------------
Novell_SendConsole  struct
        Request dw      0
        id      db      0
        MessLen db      0
        Message db      55 dup(0)
        Reply   dw      0
Novell_SendConsole  ends
sSendConsole_Reply      EQU 2
sSendConsole_Request    EQU SIZE Novell_SendConsole - sSendConsole_Reply

Novell_SendMessage  struct
        Request dw      0
        id      db      0
        Count   db      0
        UserID  db      0
        MessLen db      0
        Message db      55 dup(0)
        Reply   dw      0
        Cnt     db      0
        Result  db      2 dup(0)
Novell_SendMessage  ends
sSendMessage_Reply      EQU 5
sSendMessage_Request    EQU SIZE Novell_SendMessage - sSendMessage_Reply

sNovell_SendMessageConList EQU 100
Novell_SendMessageNew  struct
    Request dw      0
    id      db      0
    Count   dw      0
    ConList db      sNovell_SendMessageConList dup(0)
    MessLen db      0
    Message db      55 dup(0)
Novell_SendMessageNew  ends
sSendMessageNew_Reply      EQU 5
sSendMessageNew_Request    EQU SIZE Novell_SendMessageNew - sSendMessageNew_Reply

Novell_GetUserNumb  struct
        Request dw      0
        id      db      15h
                db      0
        ObjType db      0
        ObjLen  db      0
        ObjName db      47 dup(0)
        Reply   dw      0
        NumCon  db      0
        ConList db      100 dup(0)
Novell_GetUserNumb  ends
sGetUserNumb_Reply      EQU 103
sGetUserNumb_Request    EQU SIZE Novell_GetUserNumb - sGetUserNumb_Reply

sObjectNameLen          EQU 46
Novell_GetUserNumbNew  struct
    Request dw      0
    id      db      1Bh
        ConNumb dw      0
           dw      0
    ObjType dw      0
    ObjLen  db      0
    ObjName db      sObjectNameLen+1 dup(0)
    Reply   dw      0
    NumCon  db      0
    ConList db      100 dup(0)
Novell_GetUserNumbNew  ends
sGetUserNumbNew            EQU SIZE Novell_GetUserNumbNew
sGetUserNumbNew_Reply      EQU 103
sGetUserNumbNew_Request    EQU SIZE Novell_GetUserNumbNew - sGetUserNumbNew_Reply

Novell_GetServerInformation  struct
    Reply        dw      0
        ServerName   db      48 dup(0)
        VerMajor     db      0
        VerMinor     db      0
        MaxCons      dw      0
        ConsInUse    dw      0
        NumMntVols   dw      0
        Revision     db      0
        SFTLevel     db      0
        TTSLevel     db      0
        MaxCnsUsed   dw      0
        AccntVer     db      0
        VAPVer       db      0
        QueueVer     db      0
        PrintVer     db      0
        VirtConslVer db      0
        RstrctLvl    db      0
        IntrntBrdg   db      0
        Reserved     db      60 dup(0)
    Request      dw      0
    id           db      11h
Novell_GetServerInformation  ends
sGetServerInformation_Request  EQU 3
sGetServerInformation_Reply    EQU SIZE Novell_GetServerInformation - sGetServerInformation_Request

Novell_GetUserName   struct
        Request dw      0
        id      db      16h
        ConNumb db      0
        Reply   dw      0
        ObjID   dd      0
                db      0
        ObjType db      0
        ObjName db      48 dup(0)
        LogTime db      7 dup(0)
Novell_GetUserName  ends
sGetUserName_Reply      EQU 63
sGetUserName_Request    EQU SIZE Novell_GetUserName - sGetUserName_Reply

Novell_ScanBindery  struct
        Request dw      0
        id      db      37h
        LastObj dd      0
                db      0
        ObjType db      0
        ObjLen  db      1
        ObjName db      '*'
        Reply   dw      0
        ObjID   dd      0
                db      0
        rType   db      0
        rText   db      48 dup(0)
        OFlag   db      0
        OSec    db      0
        OProp   db      0
Novell_ScanBindery  ends
sScanBindery_Reply      EQU 59
sScanBindery_Request    EQU SIZE Novell_ScanBindery - sScanBindery_Reply

tagNETUSERITEM STRUC
 szServer                BYTE    NETMAXSERVERNAME+1 dup(0)
 szObject                BYTE    NETMAXUSERNAME+1 dup(0)
 uFlag                   WORD    0
tagNETUSERITEM ENDS
stagNETUSERITEM equ     SIZE tagNETUSERITEM

NOVELLREQ_E33D  STRUC
 wBuffLen                WORD    0       ; Buffer length-2
 bCmd                    BYTE    3Dh     ; Command I.D.
 bObjectType             BYTE    0       ; Object Type WORD (hi-lo)
                         BYTE    0
 bObjectLength           BYTE    0       ; Object Length
 bObjectName             BYTE    47 dup(0) ; Object Name
 bSegmentNumber          BYTE    0       ; Number of the data segment
 bPropertyNameLength     BYTE    0       ; Property Name Length (1-15)
 bPropertyName           BYTE    15 dup(0) ; Property Name (max 15 char.)
NOVELLREQ_E33D  ENDS
sNOVELLREQ_E33D equ     SIZE NOVELLREQ_E33D

NOVELLREP_E33D  STRUC
 wBuffLen                WORD    0       ; Buffer length-2
 bPropertyValue          BYTE    128 dup(0) ; Property Value;
 bMoreSegments           BYTE    0       ; More Segment Flags (0=no, 255=more)
 bPropertyFlags          BYTE    0       ; Property Flags (dynamic, static, ...)
NOVELLREP_E33D  ENDS
sNOVELLREP_E33D equ     SIZE NOVELLREP_E33D

;--------------------------------------------------------------
ScanBinderyRequest   STRUC
        wScanBinderyRequestLength    WORD    0    ; lo-hi
        bReserved               BYTE    37h
        dwLastObjectID          DWORD   0               ; hi-lo
        wObjectType             WORD    0               ; hi-lo
        bObjectNameLength       BYTE    0
        sObjectName             BYTE    47 dup(0)
ScanBinderyRequest   ENDS
sScanBinderyRequest  EQU     SIZE ScanBinderyRequest

;--------------------------------------------------------------
ScanBinderyReply   STRUC
        wScanBinderyReplyLength      WORD    0    ; lo-hi
        dwObjectID              DWORD   0               ; hi-lo
        wObjectType             WORD    0               ; hi-lo
        sObjectName             BYTE    48 dup(0)
        bObjectFlag             BYTE    0
        bOjectSecurity          BYTE    0
        bOjectHasProperties     BYTE    0
ScanBinderyReply   ENDS
sScanBinderyReply  EQU     SIZE ScanBinderyReply

;--------------------------------------------------------------
tagECB          STRUC
        sLinkAddress            BYTE    4 dup(0)
        sESRAddress             BYTE    4 dup(0)
        fInUseFlag              BYTE    0
        bCompletionCode         BYTE    0
        wSocketNumber           WORD    0               ; hi - lo byte order
        sIPXWordspace           BYTE    4 dup(0)
        sDriverWorkspace        BYTE    12 dup(0)
        sImmediateAddress       BYTE    6 dup(0)
        wFragmentCount          WORD    0               ; hi - lo byte order
        sFragmentAddress1       BYTE    4 dup(0)
        wFragmentSize1          WORD    0
        sFragmentAddress2       BYTE    4 dup(0)
        wFragmentSize2          WORD    0
tagECB          ENDS
stagECB         EQU     SIZE tagECB

;--------------------------------------------------------------
ReadPropertyValueRequest   STRUC
        wReadPropertyValueRequestLength    WORD    0    ; lo-hi
        bReserved               BYTE    3dh
        wObjectType             WORD    0               ; hi-lo
        bObjectNameLength       BYTE    0
        sObjectName             BYTE    47 dup(0)
                                BYTE    17 dup(0)
ReadPropertyValueRequest   ENDS
sReadPropertyValueRequest  EQU     SIZE ReadPropertyValueRequest

;--------------------------------------------------------------
ReadPropertyValueReply   STRUC
        wReadPropertyValueReplyLength      WORD    0    ; lo-hi
        sPropertyValue          BYTE    128 dup(0)
        bMoreSegments           BYTE    0
        bPropertyFlags          BYTE    0
ReadPropertyValueReply   ENDS
sReadPropertyValueReply  EQU     SIZE ReadPropertyValueReply

;------------------------------------------------------------------------------
tagIPXHeader    STRUC
        wChecksum               WORD    0       ; hi-lo
        wLength                 WORD    0       ; hi-lo
        bTransportLayer         BYTE    0
        bPacketType             BYTE    0
        sDestinationNetwork     BYTE    4 dup(0) ; hi-lo
        sDestinationNode        BYTE    6 dup(0) ; hi-lo
        sDestinationSocket      WORD    0        ; hi-lo
        sSourceNetwork          BYTE    4 dup(0) ; hi-lo
        sSourceNode             BYTE    6 dup(0) ; hi-lo
        sSourceSocket           WORD    0        ; hi-lo
tagIPXHeader    ENDS
stagIPXHeader   EQU     SIZE tagIPXHeader

;------------------------------------------------------------------------------
; Command line stuff
;------------------------------------------------------------------------------
COMMANDLINE STRUCT
        CL_Type         db      0       ; Type of command line option
        CL_MASK         dd      0       ; Look for this
        CL_STORE        dd      0       ; Where to store value
COMMANDLINE ENDS
sCOMMANDLINE EQU        SIZE COMMANDLINE

;------------------------------------------------------------------------------
; Virus database structes
;------------------------------------------------------------------------------
N30 STRUCT
    wVirID          WORD 0              ; Virus ID Number
    wType           WORD 0              ; Type
    wCrc            WORD 0              ; CRC
    wControl        WORD 0              ; Control word for Cleaning
    wInfo           WORD 0              ; Information Word
    wFixedID        WORD 0              ; Fixed algo virus ID number

    lpVName         BYTE SIZE_VIRUSNAME dup(0); Virus Name
    lpMem           BYTE SIZE_MEM dup(0); Memory Signature
    lpSig           BYTE SIZE_SIG dup(0); Regular Signature

    wInfSize        WORD 0
    wTagSize        WORD 0
    wHostJmp        WORD 0
    wJmpLen         WORD 0
    wSS_reg         WORD 0
    wSP_reg         WORD 0
    wCS_reg         WORD 0
    wIP_reg         WORD 0
    wHeader         WORD 0
    wHeadLen        WORD 0
    wBookLocation   WORD 0
    wBookMark       WORD 0
    dwComOff        DWORD 0             ; Offset into N30.NFO
    wControl2       WORD 0
N30 ENDS
sN30 EQU    SIZE N30

HEADER STRUCT
        uNumALGs        WORD 0              ; # of Algorythmic virus signatures
        uNumBOOs        WORD 0              ; # of Boot virus signatures
        uNumCRCs        WORD 0              ; # of CRC virus signatures
    uNumREMs        WORD 0              ; # of signatures which are turned off
    wFixedIDs       WORD SIZE_FIXED dup(0); N30 Record Numbers
HEADER ENDS
sHEADER EQU SIZE HEADER

N30NOTES STRUCT
    lpVirName        BYTE SIZE_VIRUSNAME dup(0)
    wVirID           WORD 0
    wType            WORD 0
    wControl2        WORD 0
    wInfo            WORD 0
    wSize            WORD 0
    hNext            WORD 0
N30NOTES ENDS
sN30NOTES EQU SIZE N30NOTES

VIRSCANDATHEADER STRUCT
   szSignature          BYTE    16 dup(0)   ; Signature "VIRSCAN.DAT"
   dwVersion            DWORD   0           ; Numeric version number
   byDay                BYTE    0           ; Day of this version
   byMonth              BYTE    0           ; Month of this version
   wYear                WORD    0           ; Year of this version
   wNumSigs             WORD    0           ; Number of sigs total
   wNumALGs             WORD    0           ; Number of ALG sigs
   wNumBOOs             WORD    0           ; Number of BOO sigs
   wNumCRCs             WORD    0           ; Number of CRC sigs
   wNumREMs             WORD    0           ; Number of REMed out sigs
   wHeaderCheckSum      WORD    0           ; check sum of header(see note below)
   wTotalVir            WORD    0           ; total # of viruses we "detect"
   byFiller             BYTE    218 dup(0)  ; Filler to add extra 'stuff'
VIRSCANDATHEADER ENDS
sVIRSCANDATHEADER EQU SIZE VIRSCANDATHEADER

;------------------------------------------------------------------------------
; SDA Infomation block
;------------------------------------------------------------------------------
SDA     STRUCT
        lpFrom          dd      0
        lpTo            dd      0
        lpINDOS         dd      0
        uSmall          dw      0
        uLarge          dw      0
SDA     ENDS
sSDA    EQU     SIZE SDA

;------------------------------------------------------------------------------
; External message load information
;------------------------------------------------------------------------------
EXTERN_MESSAGE STRUCT
        MessageID       db      0       ; Message number
        MessageText     dd      0       ; Where to store it
        MessageSize     dw      0       ; Max size of message
EXTERN_MESSAGE ENDS
sEXTERN_MESSAGE EQU     SIZE EXTERN_MESSAGE

;------------------------------------------------------------------------------
; Simple archive method
;------------------------------------------------------------------------------
MARC    STRUCT
        ArcDir  db      64 dup(0)       ; Original directory
        ArcName db      15 dup(0)       ; Name of file in arc
        ArcAttr db      0               ; Attr of file in arc
        ArcTime dw      0               ; Time of file in arc
        ArcDate dw      0               ; Date of file in arc
        ArcSize dd      0               ; Size of file in arc
        ArcRes  db      7 dup(0)        ; Reserved for future
MARC    ENDS
sMARC   EQU     SIZE MARC

;------------------------------------------------------------------------------
; For DLL load of array of DLL's
;------------------------------------------------------------------------------
LIB_ARRAY       STRUCT
        LibraryName     db      78 dup(0)
        LibraryArray    HINSTANCE 101 dup(0)
LIB_ARRAY       ENDS
sLIB_ARRAY EQU SIZE LIB_ARRAY


;------------------------------------------------------------------------------
; STARTUP_INFO, instance data for Windows
;------------------------------------------------------------------------------
STARTUP_INFO    STRUCT
    VersionHigh db      0       ; Major version
    VersionLow  db      0       ; Minor version
    NextStartup dd      0       ; Pointer to next startup
    ASCIZ_Name  dd      0       ; Pointer to name
    Ref_Data    dd      0       ; Reference data for ASCIZ_Name
    InstRecords dd      0       ; Pointer to instance records (Struct)
STARTUP_INFO    ENDS
sSTARTUP_INFO   EQU SIZE STARTUP_INFO

;------------------------------------------------------------------------------
; INSTANCE_RECORD
;------------------------------------------------------------------------------
INSTANCE_RECORD STRUCT
    InstanceAddr    dd  0       ; Address of instance record
    InstanceSize    dw  0       ; Size of instance record
INSTANCE_RECORD ENDS
sINSTANCE_RECORD EQU SIZE INSTANCE_RECORD

;-----------------------------------------------------------------------------
; Structure for Bios Parameter Block
;-----------------------------------------------------------------------------
BPB_struc       STRUC
  BytesPerSec   dw      ?               ; bytes per sector
  SecPerClust   db      ?               ; sectors per cluster
  ResSectors    dw      ?               ; number of reserved sectors
  FATs          db      ?               ; number of file allocation tables
  RootDirEnts   dw      ?               ; number of root-directory entries */
  Sectors       dw      ?               ; total number of sectors (if 0 refer to HugeSectors
  Media         db      ?               ; media descriptor
  FATsecs       dw      ?               ; number of sectors per FAT
  SecPerTrack   dw      ?               ; sectors per track
  Heads         dw      ?               ; number of heads
  HiddenSecs    dd      ?               ; number of hidden sectors
  HugeSectors   dd      ?               ; number of sectors if Sectors == 0
BPB_struc       ENDS
sBPB_struc      EQU     SIZE BPB_struc

;-----------------------------------------------------------------------------
; Structure for Partition Table Entries
;-----------------------------------------------------------------------------
PARTENTRY       STRUC
  peBootable    db      ?               ; 80h - bootable, 00h = nonbootable
  peBeginHead   db      ?               ; beginning head
  peBeginSector db      ?               ; beginning sector
  peBeginCylinder db    ?               ; beginning cylinder
  peFileSystem  db      ?               ; name of file system
  peEndHead     db      ?               ; ending head
  peEndSector   db      ?               ; ending sector
  peEndCylinder db      ?               ; ending cylinder
  peStartSector dd      ?               ; starting sector (relative to beg. of disk)
  peSectors     dd      ?               ; number of sectors in partition
PARTENTRY       ENDS
sPARTENTRY      EQU     SIZE PARTENTRY

;---------------------------------------------------------------------------
; Drive Parameter Block (DPB)
;---------------------------------------------------------------------------
DPB     STRUC
  dpbDrive              db      ?       ;drive number (0=A,1=B,etc.)
  dpbUnit               db      ?       ;unit number for drive
  dpbSectorSize         dw      ?       ;sector size, in bytes
  dpbClusterMask        db      ?       ;sectors per cluster - 1
  dpbClusterShift       db      ?       ;sectors per cluster, as power of 2
  dpbFirstFAT           dw      ?       ;first sector containing FAT
  dpbFATCount           db      ?       ;number of FATs
  dpbRootEntries        dw      ?       ;number of root-directory entries
  dpbFirstSector        dw      ?       ;first sector of first cluster
  dpbMaxCluster         dw      ?       ;number of clusters on drive + 1
  dpbFATSize            dw      ?       ;number of sectors occupied by FAT
  dpbDirSector          dw      ?       ;first sector containing director
  dpbDriverAddr         dd      ?       ;address of device driver
  dpbMedia              db      ?       ;media descriptor
  dpbFirstAccess        db      ?       ;indicates access to the drive
  dpbNextDPB            dd      ?       ;address of next drive paramter block
  dpbNextFree           dw      ?       ;last allocated cluster
  dpbFreeCnt            dw      ?       ;number of free cluster
DPB     ENDS
sDPB    equ     SIZE DPB

;---------------------------------------------------------------------------
; Exclude structure used for the exclude binary file
;---------------------------------------------------------------------------
EXCLUDE STRUCT
        szText          byte    MAX_PATH_SIZE+1 dup(0)
        bSubDirectory   byte    0
        wBits           word    0
EXCLUDE ENDS
sEXCLUDE EQU SIZE EXCLUDE


;******************************************************************************
; PROTO Type section
;******************************************************************************
if FARCALL

DB_ROUTINES         PROTO FAR,:WORD,:LPSTR
DB_PROTO            TYPEDEF PROTO FAR,:WORD,:LPSTR
DB_PTR              TYPEDEF PTR DB_PROTO

DBC4_PROTO_HASH     TYPEDEF PROTO FAR,:LPSTR
DBC4_HASH           TYPEDEF PTR DBC4_PROTO_HASH

DBC4_PROTO_COMP     TYPEDEF PROTO FAR,:LPSTR,:LPSTR
DBC4_COMP           TYPEDEF PTR DBC4_PROTO_COMP

DBC4_PROTO_QUICK    TYPEDEF PROTO FAR,:LPSTR
DBC4_QUICK          TYPEDEF PTR DBC4_PROTO_QUICK

FF_CALLBACK         PROTO FAR,:LPSTR,:LPSTR,:DWORD
FF_PROTO            TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:DWORD
FF_PTR              TYPEDEF PTR FF_PROTO

EV_PROTO            TYPEDEF PROTO FAR,:LPSTR
EV_PTR              TYPEDEF PTR EV_PROTO

SCNPTR_PROTO        TYPEDEF PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTR              TYPEDEF PTR SCNPTR_PROTO

SCNPTRTSR_PROTO     TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTRTSR           TYPEDEF PTR SCNPTRTSR_PROTO


REPAIR_PROTO        TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
REPAIR_PTR          TYPEDEF PTR REPAIR_PROTO

REP_BVIR_PROTO      TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR
REP_BVIR_PTR        TYPEDEF PTR REP_BVIR_PROTO

KILL_PROTO          TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD
KILL_PTR            TYPEDEF PTR KILL_PROTO

NET_PROTO           TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NET_PTR             TYPEDEF PTR NET_PROTO

BAN4_TEST           PROTO FAR

BOX4_BUTTON         PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD,:BYTE
BOX4_CLOSE          PROTO FAR,:LPSTR
BOX4_OPEN           PROTO FAR,:LPSTR
BOX4_PRINTF         PROTO FAR,:LPSTR,:LPSTR
BOX4_PRINT          PROTO FAR,:LPSTR,:LPSTR
BOX4_PRINT_LINE     PROTO FAR,:LPSTR,:LPSTR,:BYTE
BOX4_TITLE          PROTO FAR,:LPSTR,:LPSTR,:BYTE

CHR4_UPPER          PROTO FAR,:BYTE
CHR4_LOWER          PROTO FAR,:BYTE
CHR4_PRINT          PROTO FAR,:BYTE
CHR4_TO_ALT_CASE    PROTO FAR,:BYTE,:BYTE

CMB4_READ           PROTO FAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_CLOSE          PROTO FAR,:UINT
CMB4_DELETE         PROTO FAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_OPEN           PROTO FAR,:LPSTR
CMB4_WRITE          PROTO FAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_CREATE         PROTO FAR,:LPSTR,:WORD
CMB4_LOAD_EXTERN    PROTO FAR,:LPSTR,:LPSTR

DAT4_ASCII_TO_ENCODE  PROTO FAR,:LPSTR,:LPSTR
DAT4_ENCODE_TO_ASCII  PROTO FAR,:WORD,:LPSTR
DAT4_ENCODE         PROTO
DAT4_SPLIT_ENCODE   PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR

DIS4_RESET          PROTO FAR
DIS4_INT13          PROTO FAR,:LPSTR
DIS4_SET_IO         PROTO FAR,:LPSTR,:DWORD,:WORD
DIS4_INT26          PROTO FAR,:LPSTR,:DWORD
DIS4_INT25          PROTO FAR,:LPSTR,:DWORD
DIS4_WRITEL_PHYSICAL PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_WRITE_PHYSICAL PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READL_PHYSICAL PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READ_PHYSICAL  PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READ_LOGICAL   PROTO FAR,:LPSTR,:BYTE,:DWORD,:WORD
DIS4_WRITE_LOGICAL  PROTO FAR,:LPSTR,:BYTE,:DWORD,:WORD
DIS4_ABSOLUTE_TO_LOGICAL PROTO FAR,:WORD,:BYTE,:BYTE,:BYTE
DIS4_LOGICAL_TO_ABSOLUTE PROTO FAR,:DWORD,:BYTE,:WORD,:WORD
DIS4_CLUSTER_TO_LOGICAL  PROTO FAR,:WORD,:BYTE
DIS4_LOGICAL_TO_CLUSTER  PROTO FAR,:DWORD,:BYTE
DIS4_MOV_BOOT       PROTO FAR,:WORD,:WORD,:WORD,:LPSTR
DIS4_MOV_MBR        PROTO FAR,:BYTE,:BYTE,:BYTE,:BYTE,:BYTE,:LPSTR
DIS4_BOOTLOCKED_STATE   PROTO FAR,:BYTE,:LPSTR
DIS4_CLUSTER_TO_ABSOLUTE        PROTO FAR,:BYTE,:DWORD,:LPSTR
DIS4_CREATE_RECOVER     PROTO FAR,:LPSTR,:LPSTR
DIS4_GET_BPB            PROTO FAR,:BYTE,:LPSTR,:LPSTR
DIS4_IS_BOOTLOCKED      PROTO FAR,:BYTE,:LPSTR
DIS4_LOC_BOOTSECTOR     PROTO FAR,:BYTE,:LPSTR
DIS4_LOC_BOOTSECTOR_NUM PROTO FAR,:BYTE,:WORD,:LPSTR
DIS4_MODIFY_BOOTCODE    PROTO FAR,:BYTE,:WORD,:BYTE,:BYTE,:DWORD,:WORD,:LPSTR
DIS4_MOVE_MBR           PROTO FAR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE,:LPSTR
DIS4_READ_BOOTSECTOR    PROTO FAR,:BYTE,:LPSTR
DIS4_READ_MBR           PROTO FAR,:BYTE,:LPSTR
DIS4_READ_ROOT          PROTO FAR,:BYTE,:LPSTR
DIS4_RECOVER            PROTO FAR,:LPSTR,:WORD,:LPSTR
DIS4_UNLOCK_BOOT        PROTO FAR,:LPSTR
DIS4_LOC_HIDDEN_MBR     PROTO FAR,:BYTE,:LPSTR
DIS4_GET_BOOTABLE_PARTITION PROTO FAR,:BYTE,:LPSTR,:LPSTR
DIS4_GET_DPB            PROTO FAR,:BYTE,:LPSTR
DIS4_LOCK_LASTFAT       PROTO FAR,:BYTE,:DWORD,:DWORD,:LPSTR
DIS4_LOG_TO_ABS         PROTO FAR,:DWORD,:BYTE,:LPSTR
DIS4_CLUS_TO_LOG        PROTO FAR,:WORD,:BYTE
DIS4_UNMARK_FAT         PROTO FAR,:BYTE,:DWORD,:WORD,:LPSTR
DIS4_GET_MARKED_FAT     PROTO FAR,:BYTE,:DWORD,:DWORD,:LPSTR

DPMI21h                 PROTO FAR

DBC4_CLOSE          PROTO FAR,:LPSTR
DBC4_CREATE         PROTO FAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR
DBC4_OPEN           PROTO FAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD
DBC4_DELETE         PROTO FAR,:LPSTR,:LPSTR,:LPSTR
DBC4_INSERT         PROTO FAR,:LPSTR,:LPSTR,:LPSTR
DBC4_SEARCH         PROTO FAR,:LPSTR,:LPSTR,:LPSTR
DBC4_OPEN_CREATE    PROTO FAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR
DBC4_GET_NUM_ENTRIES    PROTO FAR,:LPSTR
DBC4_GET_ENTRY      PROTO FAR,:LPSTR,:DWORD,:LPSTR

DLL4_LOADLIBRARY    PROTO FAR,:LPSTR
DLL4_FREELIBRARY    PROTO FAR,:WORD
DLL4_GETORDIAL      PROTO FAR,:LPSTR,:WORD
DLL4_GETMOVABLEENTRY PROTO FAR,:WORD,:WORD
DLL4_GETSEGMENTENTRY PROTO FAR,:BYTE,:WORD
DLL4_LOADSEGMENTENTRY PROTO FAR,:LPSTR,:WORD,:WORD
DLL4_LOADSEGMENT    PROTO FAR,:BYTE,:WORD,:WORD
DLL4_GETPROCADDRESS PROTO FAR,:WORD,:LPSTR,:WORD
DLL4_GETPROCADDRESS_ARRAY PROTO FAR,:LPSTR,:LPSTR,:WORD
DLL4_FREELIBRARY_ARRAY    PROTO FAR,:LPSTR
DLL4_LOADLIBRARY_ARRAY    PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:BYTE

ExcludeCheck        PROTO FAR,:LPSTR,:LPSTR,:WORD

FIL4_IS_RO          PROTO FAR,:LPSTR
FIL4_IS_AT          PROTO FAR,:LPSTR,:LPSTR
FIL4_PROTECT        PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR,:LPSTR
FIL4_STAT           PROTO FAR,:LPSTR,:LPSTR,:LPSTR
FIL4_GET_DTA        PROTO FAR
FIL4_CHANGE_DTA     PROTO FAR,:LPSTR
FIL4_FIND_FIRST     PROTO FAR,:LPSTR,:WORD
FIL4_FIND_NEXT      PROTO FAR
FIL4_OPEN           PROTO FAR,:LPSTR,:WORD
FIL4_READ           PROTO FAR,:WORD,:LPSTR,:WORD
FIL4_WRITE          PROTO FAR,:WORD,:LPSTR,:WORD
FIL4_CLOSE          PROTO FAR,:WORD
FIL4_SEEK           PROTO FAR,:WORD,:DWORD,:WORD
FIL4_GET_START      PROTO FAR,:WORD,:LPSTR,:LPSTR
FIL4_NEW_GET_START  PROTO FAR,:WORD,:LPSTR,:LPSTR
FIL4_CRC            PROTO FAR,:WORD,:WORD,:DWORD,:LPSTR,:WORD
FIL4_ATTR           PROTO FAR,:LPSTR,:WORD,:WORD
FIL4_DATETIME       PROTO FAR,:WORD,:WORD,:WORD,:WORD
FIL4_COPY           PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:WORD
FIL4_COPY_HANDLE    PROTO FAR,:WORD,:WORD,:LPSTR,:WORD,:DWORD
FIL4_DELETE         PROTO FAR,:LPSTR
FIL4_KILL           PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD
FIL4_DUP_HANDLE     PROTO FAR,:WORD
FIL4_FLUSH          PROTO FAR,:WORD
FIL4_HANDLE_TO_NAME PROTO FAR,:LPSTR,:WORD,:WORD
FIL4_FIND_ALL       PROTO FAR,:LPSTR,:DWORD,:WORD,:LPSTR
FIL4_FIND_WILD      PROTO FAR,:LPSTR,:LPSTR,:DWORD,:WORD,:LPSTR
FIL4_SIZE           PROTO FAR,:WORD
FIL4_DIRECTORY      PROTO FAR,:BYTE,:LPSTR,:WORD
FIL4_CHANGE_DIR     PROTO FAR,:LPSTR
FIL4_RENAME         PROTO FAR,:LPSTR,:LPSTR
FIL4_DRIVE          PROTO FAR,:LPSTR,:WORD
FIL4_OPEN_GLOBAL    PROTO FAR,:BYTE,:WORD,:LPSTR,:LPSTR
FIL4_GET_EXEC_NAME  PROTO FAR
FIL4_GET_EXEC_DIR   PROTO FAR,:LPSTR
FIL4_CRCFILE        PROTO FAR,:LPSTR,:LPSTR
FIL4_SELF_TEST      PROTO FAR,:LPSTR,:LPSTR
FIL4_SEARCH_ARC     PROTO FAR,:LPSTR,:LPSTR,:LPSTR
FIL4_MARC           PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD
FIL4_UNMARC         PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD,:WORD
FIL4_SPLIT_PATHNAME PROTO FAR,:LPSTR,:LPSTR,:LPSTR
FIL4_CREATE_DIR     PROTO FAR,:LPSTR
FIL4_EXISTS         PROTO FAR,:LPSTR
FIL4_OPEN_CACHE     PROTO FAR,:LPSTR,:WORD,:LPSTR,:WORD
FIL4_READLINE_CACHE PROTO FAR,:LPSTR,:WORD,:LPSTR,:WORD
FIL4_CLOSE_CACHE    PROTO FAR,:LPSTR,:WORD
FIL4_INSTALLCACHE   PROTO FAR,:LPSTR,:LPSTR,:WORD
FIL4_DESTROYCACHE   PROTO FAR,:LPSTR
FIL4_WRITELOG       PROTO FAR,:WORD,:LPSTR
FIL4_READLINE       PROTO FAR,:WORD,:LPSTR,:WORD
FIL4_WRITELINE      PROTO FAR,:WORD,:LPCSTR
FIL4_MEDIA          PROTO FAR,:LPSTR
FIL4_GETDRIVEFROMNAME PROTO FAR,:LPSTR
FIL4_ADDPATH        PROTO FAR,:LPSTR,:LPSTR
FIL4_FILE_DESTINATION PROTO FAR,:LPSTR,:LPSTR,:LPSTR

GetPrivateProfileString PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD,:LPSTR
GetPrivateProfileInt    PROTO FAR,:LPSTR,:LPSTR,:WORD,:LPSTR
GlobalAlloc         PROTO FAR,:UINT,:DWORD
GlobalLock          PROTO FAR,:HGLOBAL

INOC_REP_CMP35          PROTO FAR,:LPSTR,:LPSTR
INOC_REP_MAIN           PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
INOC_REP_COPY_BACK      PROTO FAR,:WORD,:LPSTR,:DWORD
INOC_REP_TYPE           PROTO FAR,:WORD,:LPSTR,:LPSTR,:DWORD
INOC_REP_COM_APPEND     PROTO FAR,:WORD,:LPSTR,:LPSTR,:DWORD
INOC_REP_COM_PREPEND    PROTO FAR,:WORD,:LPSTR,:DWORD,:DWORD
INOC_REP_FIND35         PROTO FAR,:WORD,:DWORD,:LPSTR,:LPSTR

INSCANB             PROTO FAR,:LPSTR
INSCANM             PROTO FAR,:LPSTR
INSCANF             PROTO FAR,:LPSTR

KEY4_SHIFTSTATE     PROTO FAR,:WORD
KEY4_GET_CHAR       PROTO FAR
KEY4_GET_DELAY      PROTO FAR,:WORD
KEY4_INTERCEPT      PROTO FAR

MEM4_CRC            PROTO FAR,:WORD,:LPSTR,:WORD
MEM4_SETOBJECT      PROTO FAR,:BYTE,:LPSTR,:LPSTR,:WORD
MEM4_GETOBJECT      PROTO FAR,:BYTE,:LPSTR
MEM4_GALLOCATE      PROTO FAR,:WORD,:WORD,:WORD
MEM4_GREALLOCATE    PROTO FAR,:WORD,:WORD,:WORD
MEM4_GDESTROY       PROTO FAR,:WORD,:LPSTR
MEM4_GFREE          PROTO FAR,:WORD,:WORD
MEM4_GINIT          PROTO FAR,:LPSTR
MEM4_GLOCK          PROTO FAR,:WORD,:WORD
MEM4_GUNLOCK        PROTO FAR,:WORD,:WORD
MEM4_UMB_ALLOCATE   PROTO FAR,:WORD
MEM4_UMB_FREE       PROTO FAR,:WORD
MEM4_CONV_ALLOCATE  PROTO FAR,:WORD
MEM4_CONV_FREE      PROTO FAR,:WORD
MEM4_XMS_ALLOCATE   PROTO FAR,:WORD
MEM4_XMS_FREE       PROTO FAR,:WORD
MEM4_EMS_ALLOCATE   PROTO FAR,:WORD
MEM4_EMS_FREE       PROTO FAR,:WORD
MEM4_EMS_LOCK       PROTO FAR,:WORD
MEM4_EMS_UNLOCK     PROTO FAR,:WORD
MEM4_XMS_LOCK       PROTO FAR,:WORD
MEM4_XMS_UNLOCK     PROTO FAR,:WORD
MEM4_EMS_GETPAGE    PROTO FAR
MEM4_REPLACE        PROTO FAR,:LPSTR,:BYTE,:BYTE,:UINT
MEM4_SRCH_ALPHA     PROTO FAR,:LPSTR,:UINT
MEM4_SRCH_CHAR      PROTO FAR,:LPSTR,:BYTE,:UINT
MEM4_SET            PROTO FAR,:LPSTR,:BYTE,:UINT
MEM4_SRCH_STRING    PROTO FAR,:LPSTR,:LPSTR,:UINT
MEM4_SEGTOSEL       PROTO FAR,:WORD
MEM4_COPY_TO_CHAR   PROTO FAR,:LPSTR,:LPSTR,:UINT,:BYTE
MEM4_COPY           PROTO FAR,:LPSTR,:LPSTR,:UINT
MEM4_GCONV          PROTO FAR,:WORD
MEM4_HMA_ALLOCATE   PROTO FAR
MEM4_HMA            PROTO FAR,:WORD
MEM4_LOCALALLOCATE  PROTO FAR,:WORD,:LPSTR,:WORD

LOCALHMA            PROTO FAR,:WORD,:LPSTR
FREEHMA             PROTO FAR,:WORD,:LPSTR
UNLOCALHMA          PROTO FAR,:WORD,:LPSTR

XMS_CALL            PROTO FAR
EMS_DETECT          PROTO FAR
REALLOCATEMEMORY    PROTO FAR,:WORD,:WORD,:WORD
ALLOCATEMEMORY      PROTO FAR,:WORD,:WORD,:WORD,:WORD
FREEMEMORY          PROTO FAR,:WORD,:WORD,:WORD
LOCKMEMORY          PROTO FAR,:WORD,:WORD
UNLOCKMEMORY        PROTO FAR,:WORD,:WORD

MUS4_BEEP           PROTO FAR,:BYTE

NOV4_GETALLUSERS    PROTO FAR,:LPSTR,:LPSTR
NOV4_GETLOGGEDIN    PROTO FAR,:LPSTR,:LPSTR
NOV4_GETUSERCONNECT PROTO FAR,:LPSTR,:LPSTR
NOV4_SENDMESSAGE    PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NOV4_SEND_LIST      PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NOV4_TEST           PROTO FAR
NOV4_USER           PROTO FAR,:LPSTR
NOV4_USERNAME           PROTO FAR,:LPSTR
NOV4_GETFILESERVERINFO  PROTO FAR,:LPSTR
NOV4_SENDCONNMESSAGE    PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NOV4_GETCONNID          PROTO FAR
NOV4_DEFAULTSERVER      PROTO FAR
NOV4_SETSERVER          PROTO FAR,:BYTE
NOV4_GETSERVER          PROTO FAR,:LPSTR
NOV4_ALERTBROADCAST     PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_BUILDMESSAGE       PROTO FAR,:LPSTR,:BYTE,:LPSTR,:LPSTR
NOV4_GETCONNECTIONS     PROTO FAR,:LPSTR,:LPSTR
NOV4_GETGROUPUSERS      PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:BYTE
NOV4_CREATEUSERENTRY    PROTO FAR,:UINT,:LPSTR,:LPSTR,:BYTE,:LPSTR
NOV4_IDTONAME           PROTO FAR,:DWORD,:LPSTR
NOV4_CONSOLEMESSAGE     PROTO FAR,:LPSTR,:LPSTR
NOV4_SCAN_BINDERY           PROTO FAR,:DWORD,:WORD,:LPSTR,:LPSTR
NOV4_READ_PROPERTY_VALUE    PROTO FAR,:WORD,:LPSTR,:BYTE,:LPSTR,:LPSTR
NOV4_IPX_GET_LOCAL_TARGET   PROTO FAR,:DWORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:DWORD
NOV4_IPX_SEND_PACKET        PROTO FAR,:DWORD,:LPSTR
NOV4_INIT_IPX               PROTO FAR
NOV4_BUILD_IPX_HEADER   PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_BUILD_ECB          PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_NOTIFY_NLM         PROTO FAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_ALERT_NLM          PROTO FAR,:LPSTR,:LPSTR,:LPSTR

NUM4_MODULUS        PROTO FAR,:DWORD,:DWORD

OTH4_COMMAND_LINE   PROTO FAR,:LPSTR


SCN4_VERIFY_VIRUS   PROTO FAR,:WORD,:WORD,:LPSTR,:LPSTR
SCN4_GET_VIRUS      PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_VERIFY_NAME    PROTO FAR,:WORD,:WORD,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_GET_NAME       PROTO FAR,:WORD,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_ME             PROTO FAR,:WORD,:LPSTR,:LPSTR,:WORD,:LPSTR,:LPSTR,:LPSTR,:WORD
SCN4_CRC            PROTO FAR,:LPSTR,:WORD,:LPSTR,:WORD
SCN4_ALG            PROTO FAR,:WORD,:LPSTR,:WORD,:LPSTR, :DWORD
GetInfectionData    PROTO FAR, :LPSTR, :LPSTR, :DWORD
SCN4_MALTA          PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_V2P            PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_MTE            PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_BOOT           PROTO FAR,:LPSTR,:LPSTR,:BOOL
SCN4_MEMORY         PROTO FAR,:LPSTR,:BYTE
SCN4_LOAD_CRC       PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_CRC      PROTO FAR,:WORD,:LPSTR
SCN4_LOAD_ALG       PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_COUNT_ALG      PROTO FAR,:WORD,:LPSTR
SCN4_LOAD_BOOT      PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_BOOT     PROTO FAR,:WORD,:LPSTR
SCN4_LOAD_MEMORY    PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_MEMORY   PROTO FAR,:WORD,:LPSTR
SCN4_COUNT_REM      PROTO FAR,:WORD,:LPSTR
SCN4_GET_ENTRY      PROTO FAR,:WORD,:WORD,:LPSTR
SCN4_LOAD_CRC_CHECK_DATA PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_LOAD_NAMES     PROTO FAR,:WORD,:LPSTR,:LPSTR
SCN4_DELETE         PROTO FAR,:WORD,:LPSTR,:LPSTR

SCR4_BLANK          PROTO FAR
SCR4_GET_ATTR       PROTO FAR
SCR4_GET_VIDEOSEG   PROTO FAR
SCR4_GETPAGE        PROTO FAR
SCR4_GETATTR        PROTO FAR
SCR4_GETCURR        PROTO FAR
SCR4_STRING         PROTO FAR :LPSTR

STR4_ASCII_TO_INT   PROTO FAR,:LPSTR
STR4_ASCII_TO_WORD  PROTO FAR,:LPSTR
STR4_INT_TO_ASCII   PROTO FAR,:WORD,:LPSTR
STR4_WORD_TO_ASCII  PROTO FAR,:WORD,:LPSTR
STR4_UPPER          PROTO FAR,:LPSTR
STR4_LOWER          PROTO FAR,:LPSTR
STR4_ASCIIHEX_TO_HEX PROTO FAR,:LPSTR,:LPSTR
STR4_COPY           PROTO FAR,:LPSTR,:LPSTR
STR4_COPY_TO_CHAR   PROTO FAR,:LPSTR,:LPSTR,:BYTE
STR4_LENGTH         PROTO FAR,:LPSTR
STR4_LENGTH_TO_CHAR PROTO FAR,:LPSTR,:BYTE
STR4_PRINT          PROTO FAR,:LPSTR
STR4_FORMAT         PROTO FAR,:LPSTR,:LPSTR
STR4_COMPARE        PROTO FAR,:LPSTR,:LPSTR
STR4_ICOMPARE       PROTO FAR,:LPSTR,:LPSTR
STR4_SRCH_END       PROTO FAR,:LPSTR
STR4_LENGTH_TO_LAST PROTO FAR,:LPSTR,:BYTE
STR4_APPEND         PROTO FAR,:LPSTR,:LPSTR
STR4_SRCH_CHAR      PROTO FAR,:LPSTR,:BYTE
STR4_COUNT_CHAR     PROTO FAR,:LPSTR,:BYTE
STR4_COUNT_STRINGS  PROTO FAR,:LPSTR
STR4_ADD_LIST       PROTO FAR,:LPSTR,:LPSTR
STR4_WORD_TO_ASCIIHEX  PROTO FAR,:WORD,:LPSTR
STR4_ASCIIHEX_TO_WORD  PROTO FAR,:LPSTR
STR4_DWORD_CONV     PROTO FAR,:DWORD,:WORD,:LPSTR,:WORD
STR4_ASCII_CONV     PROTO FAR,:LPSTR,:WORD,:LPSTR,:WORD
STR4_MODIFY_LONG_FILENAME PROTO FAR,:LPSTR,:LPSTR

ADD_FILE        PROTO FAR,fStruct:LPSTR,fDest:LPSTR
ADD_DIR         PROTO FAR,fStruct:LPSTR,fDest:LPSTR
ADD_DRIVE_LETTER PROTO FAR,fStruct:LPSTR,fDest:LPSTR
ADD_EXTRA       PROTO FAR,fStruct:LPSTR,fDest:LPSTR
ADD_BUFFER      PROTO FAR,fStruct:LPSTR,fDest:LPSTR
ADD_DATE        PROTO FAR,fDest:LPSTR
ADD_TIME        PROTO FAR,fDest:LPSTR
ADD_USER        PROTO FAR,fStruct:LPSTR,fDest:LPSTR
INSERT_GUILTY_PROGRAM PROTO FAR,fStruct:LPSTR,fDest:LPSTR

SYS4_IS_PS2         PROTO FAR
SYS4_GET_SDA        PROTO FAR,:LPSTR
SYS4_HALT           PROTO FAR
SYS4_VECTOR         PROTO FAR,:BYTE,:LPSTR,:LPSTR
SYS4_TSR            PROTO FAR,:WORD
SYS4_UNVECTOR       PROTO FAR,:BYTE,:LPSTR,:LPSTR
SYS4_GET_MCB_CHAIN  PROTO FAR,:WORD
SYS4_GET_MYTSR      PROTO FAR,:WORD
SYS4_GET_ORIG13     PROTO FAR,:WORD,:WORD
SYS4_GET_VECTOR     PROTO FAR,:BYTE
SYS4_GET2_ORIG13    PROTO FAR
SYS4_GET_COMSPEC    PROTO FAR,:LPSTR
SYS4_GET_ENVIROMENT PROTO FAR
SYS4_HOOK_21h       PROTO FAR
SYS4_HOOK_13h       PROTO FAR
SYS4_PRE_09h        PROTO FAR
SYS4_POST_09h       PROTO FAR
SYS4_PRE_13h        PROTO FAR
SYS4_POST_13h       PROTO FAR
SYS4_PRE_15h        PROTO FAR
SYS4_POST_15h       PROTO FAR
SYS4_PRE_21h        PROTO FAR
SYS4_POST_21h       PROTO FAR
SYS4_PRE_DOS        PROTO FAR
SYS4_POST_DOS       PROTO FAR
SYS4_PRE_NET        PROTO FAR
SYS4_POST_NET       PROTO FAR
SYS4_PRE_2Fh        PROTO FAR
SYS4_POST_2Fh       PROTO FAR
SYS4_FREE_ENVIROMENT PROTO FAR
SYS4_PARSE_CL       PROTO FAR,:LPSTR,:LPSTR
SYS4_GET_NAMES      PROTO FAR,:LPSTR
SYS4_GET_CPU_TYPE   PROTO FAR
SYS4_FIND_QEMM      PROTO FAR,:LPSTR
SYS4_QEMM_INSTALLED PROTO FAR
SYS4_QEMM_VERSION   PROTO FAR
SYS4_QEMM_STEALTH_MODE  PROTO FAR
SYS4_QEMM_COPY_STEALTH_ROM      PROTO FAR,:LPSTR,:LPSTR,:DWORD
SYS4_GET_SYSTEM_ID  PROTO FAR,:LPSTR

SetAnsiFlag         PROTO FAR :WORD
GetAnsiFlag         PROTO FAR

ThreadSwitch        PROTO FAR SYSCALL           ; NetWare Os Call
RelinquishControl   PROTO FAR SYSCALL           ; NetWare Os Call

TIM4_ASCII_TO_ENCODE  PROTO FAR,:LPSTR,:LPSTR
TIM4_ENCODE_TO_ASCII  PROTO FAR,:WORD,:LPSTR
TIM4_ENCODE         PROTO
TIM4_SPLIT_ENCODE   PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR

WRITEPRIVATEPROFILESTRING PROTO FAR,:LPCSTR,:LPCSTR,:LPCSTR,:LPCSTR
ANSITOOEM           PROTO FAR,:LPSTR,:LPSTR
OEMTOANSI           PROTO FAR,:LPSTR,:LPSTR

Seek_to_index_entry PROTO FAR,:LPSTR,:DWORD
Seek_to_data_record PROTO FAR,:LPSTR,:DWORD
Seek_to_data_chain  PROTO FAR,:LPSTR,:DWORD
Seek_to_quick_entry PROTO FAR,:LPSTR,:DWORD
Find_free_data_entry PROTO FAR,:LPSTR

UnEncrypt           PROTO FAR,:LPSTR
Encrypt             PROTO FAR,:LPSTR
SetEncryptINI       PROTO FAR,:BOOL
EncryptHeader       PROTO FAR,:UINT,:UINT

;------------------------------------------------------------------------------
; PROTO Type section - 32 bit (NEAR)
;------------------------------------------------------------------------------
else

if FARDATA
DB_ROUTINES         PROTO FAR,:WORD,:LPSTR
DB_PROTO            TYPEDEF PROTO FAR,:WORD,:LPSTR
DB_PTR              TYPEDEF PTR DB_PROTO

DBC4_PROTO_HASH     TYPEDEF PROTO FAR,:LPSTR
DBC4_HASH           TYPEDEF PTR DBC4_PROTO_HASH

DBC4_PROTO_COMP     TYPEDEF PROTO FAR,:LPSTR,:LPSTR
DBC4_COMP           TYPEDEF PTR DBC4_PROTO_COMP

DBC4_PROTO_QUICK    TYPEDEF PROTO FAR,:LPSTR
DBC4_QUICK          TYPEDEF PTR DBC4_PROTO_QUICK

FF_CALLBACK         PROTO FAR,:LPSTR,:LPSTR,:DWORD
FF_PROTO            TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:DWORD
FF_PTR              TYPEDEF PTR FF_PROTO

EV_PROTO            TYPEDEF PROTO FAR,:LPSTR
EV_PTR              TYPEDEF PTR EV_PROTO

SCNPTR_PROTO        TYPEDEF PROTO FAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTR              TYPEDEF PTR SCNPTR_PROTO

SCNPTRTSR_PROTO     TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTRTSR           TYPEDEF PTR SCNPTRTSR_PROTO

REPAIR_PROTO        TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
REPAIR_PTR          TYPEDEF PTR REPAIR_PROTO

REP_BVIR_PROTO      TYPEDEF PROTO FAR,:WORD,:WORD,:LPSTR
REP_BVIR_PTR        TYPEDEF PTR REP_BVIR_PROTO

KILL_PROTO          TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:WORD,:WORD
KILL_PTR            TYPEDEF PTR KILL_PROTO

NET_PROTO           TYPEDEF PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NET_PTR             TYPEDEF PTR NET_PROTO
else
DB_ROUTINES         PROTO NEAR,:WORD,:LPSTR
DB_PROTO            TYPEDEF PROTO NEAR,:WORD,:LPSTR
DB_PTR              TYPEDEF PTR DB_PROTO

DBC4_PROTO_HASH     TYPEDEF PROTO NEAR,:LPSTR
DBC4_HASH           TYPEDEF PTR DBC4_PROTO_HASH

DBC4_PROTO_COMP     TYPEDEF PROTO NEAR,:LPSTR,:LPSTR
DBC4_COMP           TYPEDEF PTR DBC4_PROTO_COMP

DBC4_PROTO_QUICK    TYPEDEF PROTO NEAR,:LPSTR
DBC4_QUICK          TYPEDEF PTR DBC4_PROTO_QUICK

FF_CALLBACK         PROTO NEAR,:LPSTR,:LPSTR,:DWORD
FF_PROTO            TYPEDEF PROTO NEAR,:LPSTR,:LPSTR,:DWORD
FF_PTR              TYPEDEF PTR FF_PROTO

EV_PROTO            TYPEDEF PROTO NEAR,:LPSTR
EV_PTR              TYPEDEF PTR EV_PROTO

SCNPTR_PROTO        TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTR              TYPEDEF PTR SCNPTR_PROTO

SCNPTRTSR_PROTO     TYPEDEF PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCNPTRTSR           TYPEDEF PTR SCNPTRTSR_PROTO

REPAIR_PROTO        TYPEDEF PROTO NEAR,:WORD,:WORD,:LPSTR,:WORD,:LPSTR
REPAIR_PTR          TYPEDEF PTR REPAIR_PROTO

REP_BVIR_PROTO      TYPEDEF PROTO NEAR,:WORD,:WORD,:LPSTR
REP_BVIR_PTR        TYPEDEF PTR REP_BVIR_PROTO

KILL_PROTO          TYPEDEF PROTO NEAR,:LPSTR,:LPSTR,:WORD,:WORD
KILL_PTR            TYPEDEF PTR KILL_PROTO

NET_PROTO           TYPEDEF PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
NET_PTR             TYPEDEF PTR NET_PROTO
endif

BAN4_TEST           PROTO NEAR

BOX4_BUTTON         PROTO NEAR,:LPSTR,:LPSTR,:WORD,:WORD,:BYTE
BOX4_CLOSE          PROTO NEAR,:LPSTR
BOX4_OPEN           PROTO NEAR,:LPSTR
BOX4_PRINTF         PROTO NEAR,:LPSTR,:LPSTR
BOX4_PRINT          PROTO NEAR,:LPSTR,:LPSTR
BOX4_PRINT_LINE     PROTO NEAR,:LPSTR,:LPSTR,:BYTE
BOX4_TITLE          PROTO NEAR,:LPSTR,:LPSTR,:BYTE

CHR4_UPPER          PROTO NEAR,:BYTE
CHR4_LOWER          PROTO NEAR,:BYTE
CHR4_PRINT          PROTO NEAR,:BYTE
CHR4_TO_ALT_CASE    PROTO NEAR,:BYTE,:BYTE

CMB4_READ           PROTO NEAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_CLOSE          PROTO NEAR,:UINT
CMB4_DELETE         PROTO NEAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_OPEN           PROTO NEAR,:LPSTR
CMB4_WRITE          PROTO NEAR,:UINT,:UINT,:LPSTR,:UINT
CMB4_CREATE         PROTO NEAR,:LPSTR,:WORD
CMB4_LOAD_EXTERN    PROTO NEAR,:LPSTR,:LPSTR

DAT4_ASCII_TO_ENCODE  PROTO NEAR,:LPSTR,:LPSTR
DAT4_ENCODE_TO_ASCII  PROTO NEAR,:WORD,:LPSTR
DAT4_ENCODE         PROTO
DAT4_SPLIT_ENCODE   PROTO NEAR,:WORD,:LPSTR,:LPSTR,:LPSTR

DIS4_RESET          PROTO NEAR
DIS4_INT13          PROTO NEAR,:LPSTR
DIS4_SET_IO         PROTO NEAR,:LPSTR,:DWORD,:WORD
DIS4_INT26          PROTO NEAR,:LPSTR,:DWORD
DIS4_INT25          PROTO NEAR,:LPSTR,:DWORD
DIS4_WRITEL_PHYSICAL PROTO NEAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_WRITE_PHYSICAL PROTO NEAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READL_PHYSICAL PROTO NEAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READ_PHYSICAL  PROTO NEAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
DIS4_READ_LOGICAL   PROTO NEAR,:LPSTR,:BYTE,:DWORD,:WORD
DIS4_WRITE_LOGICAL  PROTO NEAR,:LPSTR,:BYTE,:DWORD,:WORD
DIS4_ABSOLUTE_TO_LOGICAL PROTO NEAR,:WORD,:BYTE,:BYTE,:BYTE
DIS4_LOGICAL_TO_ABSOLUTE PROTO NEAR,:DWORD,:BYTE,:WORD,:WORD
DIS4_CLUSTER_TO_LOGICAL  PROTO NEAR,:WORD,:BYTE
DIS4_LOGICAL_TO_CLUSTER  PROTO NEAR,:DWORD,:BYTE
DIS4_MOV_BOOT       PROTO NEAR,:WORD,:WORD,:WORD,:LPSTR
DIS4_MOV_MBR        PROTO NEAR,:BYTE,:BYTE,:BYTE,:BYTE,:BYTE,:LPSTR
DIS4_BOOTLOCKED_STATE   PROTO NEAR,:BYTE,:LPSTR
DIS4_CLUSTER_TO_ABSOLUTE        PROTO NEAR,:BYTE,:DWORD,:LPSTR
DIS4_CREATE_RECOVER     PROTO NEAR,:LPSTR,:LPSTR
DIS4_GET_BPB            PROTO NEAR,:BYTE,:LPSTR,:LPSTR
DIS4_IS_BOOTLOCKED      PROTO NEAR,:BYTE,:LPSTR
DIS4_LOC_BOOTSECTOR     PROTO NEAR,:BYTE,:LPSTR
DIS4_LOC_BOOTSECTOR_NUM PROTO NEAR,:BYTE,:WORD,:LPSTR
DIS4_MODIFY_BOOTCODE    PROTO NEAR,:BYTE,:WORD,:BYTE,:BYTE,:DWORD,:WORD,:LPSTR
DIS4_MOVE_MBR           PROTO NEAR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE,:LPSTR
DIS4_READ_BOOTSECTOR    PROTO NEAR,:BYTE,:LPSTR
DIS4_READ_MBR           PROTO NEAR,:BYTE,:LPSTR
DIS4_READ_ROOT          PROTO NEAR,:BYTE,:LPSTR
DIS4_RECOVER            PROTO NEAR,:LPSTR,:WORD,:LPSTR
DIS4_UNLOCK_BOOT        PROTO NEAR,:LPSTR
DIS4_LOC_HIDDEN_MBR     PROTO NEAR,:BYTE,:LPSTR
DIS4_GET_BOOTABLE_PARTITION PROTO NEAR,:BYTE,:LPSTR,:LPSTR
DIS4_GET_DPB            PROTO NEAR,:BYTE,:LPSTR
DIS4_LOCK_LASTFAT       PROTO NEAR,:BYTE,:DWORD,:DWORD,:LPSTR
DIS4_LOG_TO_ABS         PROTO NEAR,:DWORD,:BYTE,:LPSTR
DIS4_CLUS_TO_LOG        PROTO NEAR,:WORD,:BYTE
DIS4_UNMARK_FAT         PROTO NEAR,:BYTE,:DWORD,:WORD,:LPSTR
DIS4_GET_MARKED_FAT     PROTO NEAR,:BYTE,:DWORD,:DWORD,:LPSTR

DPMI21h                 PROTO NEAR

DBC4_CLOSE          PROTO NEAR,:LPSTR
DBC4_CREATE         PROTO NEAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR
DBC4_OPEN           PROTO NEAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD
DBC4_DELETE         PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
DBC4_INSERT         PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
DBC4_SEARCH         PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
DBC4_OPEN_CREATE    PROTO NEAR,:LPSTR,:DWORD,:WORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR
DBC4_GET_NUM_ENTRIES    PROTO NEAR,:LPSTR
DBC4_GET_ENTRY      PROTO NEAR,:LPSTR,:DWORD,:LPSTR

DLL4_LOADLIBRARY    PROTO NEAR,:LPSTR
DLL4_FREELIBRARY    PROTO NEAR,:WORD
DLL4_GETORDIAL      PROTO NEAR,:LPSTR,:WORD
DLL4_GETMOVABLEENTRY PROTO NEAR,:WORD,:WORD
DLL4_GETSEGMENTENTRY PROTO NEAR,:BYTE,:WORD
DLL4_LOADSEGMENTENTRY PROTO NEAR,:LPSTR,:WORD,:WORD
DLL4_LOADSEGMENT    PROTO NEAR,:BYTE,:WORD,:WORD
DLL4_GETPROCADDRESS PROTO NEAR,:WORD,:LPSTR,:WORD
DLL4_GETPROCADDRESS_ARRAY PROTO NEAR,:LPSTR,:LPSTR,:WORD
DLL4_FREELIBRARY_ARRAY    PROTO NEAR,:LPSTR
DLL4_LOADLIBRARY_ARRAY    PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:BYTE

ExcludeCheck        PROTO NEAR,:LPSTR,:LPSTR,:WORD

FIL4_IS_RO          PROTO NEAR,:LPSTR
FIL4_IS_AT          PROTO NEAR,:LPSTR,:LPSTR
FIL4_PROTECT        PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR,:LPSTR
FIL4_STAT           PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
FIL4_GET_DTA        PROTO NEAR
FIL4_CHANGE_DTA     PROTO NEAR,:LPSTR
FIL4_FIND_FIRST     PROTO NEAR,:LPSTR,:WORD
FIL4_FIND_NEXT      PROTO NEAR
FIL4_OPEN           PROTO NEAR,:LPSTR,:WORD
FIL4_READ           PROTO NEAR,:WORD,:LPSTR,:WORD
FIL4_WRITE          PROTO NEAR,:WORD,:LPSTR,:WORD
FIL4_CLOSE          PROTO NEAR,:WORD
FIL4_SEEK           PROTO NEAR,:WORD,:DWORD,:WORD
FIL4_GET_START      PROTO NEAR,:WORD,:LPSTR,:LPSTR
FIL4_NEW_GET_START  PROTO NEAR,:WORD,:LPSTR,:LPSTR
FIL4_CRC            PROTO NEAR,:WORD,:WORD,:DWORD,:LPSTR,:WORD
FIL4_ATTR           PROTO NEAR,:LPSTR,:WORD,:WORD
FIL4_DATETIME       PROTO NEAR,:WORD,:WORD,:WORD,:WORD
FIL4_COPY           PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:WORD
FIL4_COPY_HANDLE    PROTO NEAR,:WORD,:WORD,:LPSTR,:WORD,:DWORD
FIL4_DELETE         PROTO NEAR,:LPSTR
FIL4_KILL           PROTO NEAR,:LPSTR,:LPSTR,:WORD,:WORD
FIL4_DUP_HANDLE     PROTO NEAR,:WORD
FIL4_FLUSH          PROTO NEAR,:WORD
FIL4_HANDLE_TO_NAME PROTO NEAR,:LPSTR,:WORD,:WORD
FIL4_FIND_ALL       PROTO NEAR,:LPSTR,:DWORD,:WORD,:LPSTR
FIL4_FIND_WILD      PROTO NEAR,:LPSTR,:LPSTR,:DWORD,:WORD,:LPSTR
FIL4_SIZE           PROTO NEAR,:WORD
FIL4_DIRECTORY      PROTO NEAR,:BYTE,:LPSTR,:WORD
FIL4_CHANGE_DIR     PROTO NEAR,:LPSTR
FIL4_RENAME         PROTO NEAR,:LPSTR,:LPSTR
FIL4_DRIVE          PROTO NEAR,:LPSTR,:WORD
FIL4_OPEN_GLOBAL    PROTO NEAR,:BYTE,:WORD,:LPSTR,:LPSTR
FIL4_GET_EXEC_NAME  PROTO NEAR
FIL4_GET_EXEC_DIR   PROTO NEAR,:LPSTR
FIL4_CRCFILE        PROTO NEAR,:LPSTR,:LPSTR
FIL4_SELF_TEST      PROTO NEAR,:LPSTR,:LPSTR
FIL4_SEARCH_ARC     PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
FIL4_MARC           PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD
FIL4_UNMARC         PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD,:WORD
FIL4_SPLIT_PATHNAME PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
FIL4_CREATE_DIR     PROTO NEAR,:LPSTR
FIL4_EXISTS         PROTO NEAR,:LPSTR
FIL4_OPEN_CACHE     PROTO NEAR,:LPSTR,:WORD,:LPSTR,:WORD
FIL4_READLINE_CACHE PROTO NEAR,:LPSTR,:WORD,:LPSTR,:WORD
FIL4_CLOSE_CACHE    PROTO NEAR,:LPSTR,:WORD
FIL4_INSTALLCACHE   PROTO NEAR,:LPSTR,:LPSTR,:WORD
FIL4_DESTROYCACHE   PROTO NEAR,:LPSTR
FIL4_WRITELOG       PROTO NEAR,:WORD,:LPSTR
FIL4_READLINE       PROTO NEAR,:WORD,:LPSTR,:WORD
FIL4_WRITELINE      PROTO NEAR,:WORD,:LPCSTR
FIL4_MEDIA          PROTO NEAR,:LPSTR
FIL4_GETDRIVEFROMNAME PROTO NEAR,:LPSTR
FIL4_ADDPATH        PROTO NEAR,:LPSTR,:LPSTR
FIL4_FILE_DESTINATION PROTO NEAR,:LPSTR,:LPSTR,:LPSTR

GetPrivateProfileString PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:WORD,:LPSTR
GetPrivateProfileInt    PROTO NEAR,:LPSTR,:LPSTR,:WORD,:LPSTR
GlobalAlloc         PROTO NEAR,:UINT,:DWORD
GlobalLock          PROTO NEAR,:HGLOBAL

INOC_REP_CMP35          PROTO NEAR,:LPSTR,:LPSTR
INOC_REP_MAIN           PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
INOC_REP_COPY_BACK      PROTO NEAR,:WORD,:LPSTR,:DWORD
INOC_REP_TYPE           PROTO NEAR,:WORD,:LPSTR,:LPSTR,:DWORD
INOC_REP_COM_APPEND     PROTO NEAR,:WORD,:LPSTR,:LPSTR,:DWORD
INOC_REP_COM_PREPEND    PROTO NEAR,:WORD,:LPSTR,:DWORD,:DWORD
INOC_REP_FIND35         PROTO NEAR,:WORD,:DWORD,:LPSTR,:LPSTR

INSCANB             PROTO NEAR,:LPSTR
INSCANM             PROTO NEAR,:LPSTR
INSCANF             PROTO NEAR,:LPSTR

KEY4_SHIFTSTATE     PROTO NEAR,:WORD
KEY4_GET_CHAR       PROTO NEAR
KEY4_GET_DELAY      PROTO NEAR,:WORD
KEY4_INTERCEPT      PROTO NEAR

MEM4_CRC            PROTO NEAR,:WORD,:LPSTR,:WORD
MEM4_SETOBJECT      PROTO NEAR,:BYTE,:LPSTR,:LPSTR,:WORD
MEM4_GETOBJECT      PROTO NEAR,:BYTE,:LPSTR
MEM4_GALLOCATE      PROTO NEAR,:WORD,:WORD,:WORD
MEM4_GREALLOCATE    PROTO NEAR,:WORD,:WORD,:WORD
MEM4_GDESTROY       PROTO NEAR,:WORD,:LPSTR
MEM4_GFREE          PROTO NEAR,:WORD,:WORD
MEM4_GINIT          PROTO NEAR,:LPSTR
MEM4_GLOCK          PROTO NEAR,:WORD,:WORD
MEM4_GUNLOCK        PROTO NEAR,:WORD,:WORD
MEM4_UMB_ALLOCATE   PROTO NEAR,:WORD
MEM4_UMB_FREE       PROTO NEAR,:WORD
MEM4_CONV_ALLOCATE  PROTO NEAR,:WORD
MEM4_CONV_FREE      PROTO NEAR,:WORD
MEM4_XMS_ALLOCATE   PROTO NEAR,:WORD
MEM4_XMS_FREE       PROTO NEAR,:WORD
MEM4_EMS_ALLOCATE   PROTO NEAR,:WORD
MEM4_EMS_FREE       PROTO NEAR,:WORD
MEM4_EMS_LOCK       PROTO NEAR,:WORD
MEM4_EMS_UNLOCK     PROTO NEAR,:WORD
MEM4_XMS_LOCK       PROTO NEAR,:WORD
MEM4_XMS_UNLOCK     PROTO NEAR,:WORD
MEM4_EMS_GETPAGE    PROTO NEAR
MEM4_REPLACE        PROTO NEAR,:LPSTR,:BYTE,:BYTE,:UINT
MEM4_SRCH_ALPHA     PROTO NEAR,:LPSTR,:UINT
MEM4_SRCH_CHAR      PROTO NEAR,:LPSTR,:BYTE,:UINT
MEM4_SET            PROTO NEAR,:LPSTR,:BYTE,:UINT
MEM4_SRCH_STRING    PROTO NEAR,:LPSTR,:LPSTR,:UINT
MEM4_SEGTOSEL       PROTO NEAR,:WORD
MEM4_COPY_TO_CHAR   PROTO NEAR,:LPSTR,:LPSTR,:UINT,:BYTE
MEM4_COPY           PROTO NEAR,:LPSTR,:LPSTR,:UINT
MEM4_GCONV          PROTO NEAR,:WORD
MEM4_HMA_ALLOCATE   PROTO NEAR
MEM4_HMA            PROTO NEAR,:WORD
MEM4_LOCALALLOCATE  PROTO NEAR,:WORD,:LPSTR,:WORD

LOCALHMA            PROTO NEAR,:WORD,:LPSTR
FREEHMA             PROTO NEAR,:WORD,:LPSTR
UNLOCALHMA          PROTO NEAR,:WORD,:LPSTR

XMS_CALL            PROTO NEAR
EMS_DETECT          PROTO NEAR
REALLOCATEMEMORY    PROTO NEAR,:WORD,:WORD,:WORD
ALLOCATEMEMORY      PROTO NEAR,:WORD,:WORD,:WORD,:WORD
FREEMEMORY          PROTO NEAR,:WORD,:WORD,:WORD
LOCKMEMORY          PROTO NEAR,:WORD,:WORD
UNLOCKMEMORY        PROTO NEAR,:WORD,:WORD

MUS4_BEEP           PROTO NEAR,:BYTE

NOV4_GETALLUSERS    PROTO NEAR,:LPSTR,:LPSTR
NOV4_GETLOGGEDIN    PROTO NEAR,:LPSTR,:LPSTR
NOV4_GETUSERCONNECT PROTO NEAR,:LPSTR,:LPSTR
NOV4_SENDMESSAGE    PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
NOV4_SEND_LIST      PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
NOV4_TEST           PROTO NEAR
NOV4_USER           PROTO NEAR,:LPSTR
NOV4_USERNAME           PROTO NEAR,:LPSTR
NOV4_GETFILESERVERINFO  PROTO NEAR,:LPSTR
NOV4_SENDCONNMESSAGE    PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
NOV4_GETCONNID          PROTO NEAR
NOV4_DEFAULTSERVER      PROTO NEAR
NOV4_SETSERVER          PROTO NEAR,:BYTE
NOV4_GETSERVER          PROTO NEAR,:LPSTR
NOV4_ALERTBROADCAST     PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_BUILDMESSAGE       PROTO NEAR,:LPSTR,:BYTE,:LPSTR,:LPSTR
NOV4_GETCONNECTIONS     PROTO NEAR,:LPSTR,:LPSTR
NOV4_GETGROUPUSERS      PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:BYTE
NOV4_CREATEUSERENTRY    PROTO NEAR,:UINT,:LPSTR,:LPSTR,:BYTE,:LPSTR
NOV4_IDTONAME           PROTO NEAR,:DWORD,:LPSTR
NOV4_CONSOLEMESSAGE     PROTO NEAR,:LPSTR,:LPSTR
NOV4_SCAN_BINDERY           PROTO NEAR,:DWORD,:WORD,:LPSTR,:LPSTR
NOV4_READ_PROPERTY_VALUE    PROTO NEAR,:WORD,:LPSTR,:BYTE,:LPSTR,:LPSTR
NOV4_IPX_GET_LOCAL_TARGET   PROTO NEAR,:DWORD,:LPSTR,:LPSTR,:LPSTR,:LPSTR,:DWORD
NOV4_IPX_SEND_PACKET        PROTO NEAR,:DWORD,:LPSTR
NOV4_INIT_IPX               PROTO NEAR
NOV4_BUILD_IPX_HEADER   PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_BUILD_ECB          PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_NOTIFY_NLM         PROTO NEAR,:LPSTR,:LPSTR,:LPSTR,:LPSTR
NOV4_ALERT_NLM          PROTO NEAR,:LPSTR,:LPSTR,:LPSTR


NUM4_MODULUS        PROTO NEAR,:DWORD,:DWORD

OTH4_COMMAND_LINE   PROTO NEAR,:LPSTR


SCN4_VERIFY_VIRUS   PROTO NEAR,:WORD,:WORD,:LPSTR,:LPSTR
SCN4_GET_VIRUS      PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_VERIFY_NAME    PROTO NEAR,:WORD,:WORD,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_GET_NAME       PROTO NEAR,:WORD,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_ME             PROTO NEAR,:WORD,:LPSTR,:LPSTR,:WORD,:LPSTR,:LPSTR,:LPSTR,:WORD
SCN4_CRC            PROTO NEAR,:LPSTR,:WORD,:LPSTR,:WORD
SCN4_ALG            PROTO NEAR,:WORD,:LPSTR,:WORD,:LPSTR, :DWORD
GetInfectionData    PROTO NEAR,:LPSTR, :LPSTR, :DWORD
SCN4_MALTA          PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_V2P            PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_MTE            PROTO NEAR,:LPSTR,:WORD,:LPSTR,:LPSTR
SCN4_BOOT           PROTO NEAR,:LPSTR,:LPSTR,:BOOL
SCN4_MEMORY         PROTO NEAR,:LPSTR,:BYTE
SCN4_LOAD_CRC       PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_CRC      PROTO NEAR,:WORD,:LPSTR
SCN4_LOAD_ALG       PROTO NEAR,:WORD,:LPSTR,:LPSTR,:LPSTR
SCN4_COUNT_ALG      PROTO NEAR,:WORD,:LPSTR
SCN4_LOAD_BOOT      PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_BOOT     PROTO NEAR,:WORD,:LPSTR
SCN4_LOAD_MEMORY    PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_COUNT_MEMORY   PROTO NEAR,:WORD,:LPSTR
SCN4_COUNT_REM      PROTO NEAR,:WORD,:LPSTR
SCN4_GET_ENTRY      PROTO NEAR,:WORD,:WORD,:LPSTR
SCN4_LOAD_CRC_CHECK_DATA PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_LOAD_NAMES     PROTO NEAR,:WORD,:LPSTR,:LPSTR
SCN4_DELETE         PROTO NEAR,:WORD,:LPSTR,:LPSTR

SCR4_BLANK          PROTO NEAR
SCR4_GET_ATTR       PROTO NEAR
SCR4_GET_VIDEOSEG   PROTO NEAR
SCR4_GETPAGE        PROTO NEAR
SCR4_GETATTR        PROTO NEAR
SCR4_GETCURR        PROTO NEAR
SCR4_STRING         PROTO NEAR :LPSTR

STR4_ASCII_TO_INT   PROTO NEAR,:LPSTR
STR4_ASCII_TO_WORD  PROTO NEAR,:LPSTR
STR4_INT_TO_ASCII   PROTO NEAR,:WORD,:LPSTR
STR4_WORD_TO_ASCII  PROTO NEAR,:WORD,:LPSTR
STR4_UPPER          PROTO NEAR,:LPSTR
STR4_LOWER          PROTO NEAR,:LPSTR
STR4_ASCIIHEX_TO_HEX PROTO NEAR,:LPSTR,:LPSTR
STR4_COPY           PROTO NEAR,:LPSTR,:LPSTR
STR4_COPY_TO_CHAR   PROTO NEAR,:LPSTR,:LPSTR,:BYTE
STR4_LENGTH         PROTO NEAR,:LPSTR
STR4_LENGTH_TO_CHAR PROTO NEAR,:LPSTR,:BYTE
STR4_PRINT          PROTO NEAR,:LPSTR
STR4_FORMAT         PROTO NEAR,:LPSTR,:LPSTR
STR4_COMPARE        PROTO NEAR,:LPSTR,:LPSTR
STR4_ICOMPARE       PROTO NEAR,:LPSTR,:LPSTR
STR4_SRCH_END       PROTO NEAR,:LPSTR
STR4_LENGTH_TO_LAST PROTO NEAR,:LPSTR,:BYTE
STR4_APPEND         PROTO NEAR,:LPSTR,:LPSTR
STR4_SRCH_CHAR      PROTO NEAR,:LPSTR,:BYTE
STR4_COUNT_CHAR     PROTO NEAR,:LPSTR,:BYTE
STR4_COUNT_STRINGS  PROTO NEAR,:LPSTR
STR4_ADD_LIST       PROTO NEAR,:LPSTR,:LPSTR
STR4_WORD_TO_ASCIIHEX  PROTO NEAR,:WORD,:LPSTR
STR4_ASCIIHEX_TO_WORD  PROTO NEAR,:LPSTR
STR4_DWORD_CONV     PROTO NEAR,:DWORD,:WORD,:LPSTR,:WORD
STR4_ASCII_CONV     PROTO NEAR,:LPSTR,:WORD,:LPSTR,:WORD
STR4_MODIFY_LONG_FILENAME PROTO NEAR,:LPSTR,:LPSTR

ADD_FILE        PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
ADD_DIR         PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
ADD_DRIVE_LETTER PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
ADD_EXTRA       PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
ADD_BUFFER      PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
ADD_DATE        PROTO NEAR,fDest:LPSTR
ADD_TIME        PROTO NEAR,fDest:LPSTR
ADD_USER        PROTO NEAR,fStruct:LPSTR,fDest:LPSTR
INSERT_GUILTY_PROGRAM PROTO NEAR,fStruct:LPSTR,fDest:LPSTR


SYS4_IS_PS2         PROTO NEAR
SYS4_GET_SDA        PROTO NEAR,:LPSTR
SYS4_HALT           PROTO NEAR
SYS4_VECTOR         PROTO NEAR,:BYTE,:LPSTR,:LPSTR
SYS4_TSR            PROTO NEAR,:WORD
SYS4_UNVECTOR       PROTO NEAR,:BYTE,:LPSTR,:LPSTR
SYS4_GET_MCB_CHAIN  PROTO NEAR,:WORD
SYS4_GET_MYTSR      PROTO NEAR,:WORD
SYS4_GET_ORIG13     PROTO NEAR,:WORD,:WORD
SYS4_GET_VECTOR     PROTO NEAR,:BYTE
SYS4_GET2_ORIG13    PROTO NEAR
SYS4_GET_COMSPEC    PROTO NEAR,:LPSTR
SYS4_GET_ENVIROMENT PROTO NEAR
SYS4_HOOK_21h       PROTO NEAR
SYS4_HOOK_13h       PROTO NEAR
SYS4_PRE_09h        PROTO NEAR
SYS4_POST_09h       PROTO NEAR
SYS4_PRE_13h        PROTO NEAR
SYS4_POST_13h       PROTO NEAR
SYS4_PRE_15h        PROTO NEAR
SYS4_POST_15h       PROTO NEAR
SYS4_PRE_21h        PROTO NEAR
SYS4_POST_21h       PROTO NEAR
SYS4_PRE_DOS        PROTO NEAR
SYS4_POST_DOS       PROTO NEAR
SYS4_PRE_NET        PROTO NEAR
SYS4_POST_NET       PROTO NEAR
SYS4_PRE_2Fh        PROTO NEAR
SYS4_POST_2Fh       PROTO NEAR
SYS4_FREE_ENVIROMENT PROTO NEAR
SYS4_PARSE_CL       PROTO NEAR,:LPSTR,:LPSTR
SYS4_GET_NAMES      PROTO NEAR,:LPSTR
SYS4_GET_CPU_TYPE   PROTO NEAR
SYS4_FIND_QEMM      PROTO NEAR,:LPSTR
SYS4_QEMM_INSTALLED PROTO NEAR
SYS4_QEMM_VERSION   PROTO NEAR
SYS4_QEMM_STEALTH_MODE  PROTO NEAR
SYS4_QEMM_COPY_STEALTH_ROM      PROTO NEAR,:LPSTR,:LPSTR,:DWORD
SYS4_GET_SYSTEM_ID  PROTO NEAR,:LPSTR

SetAnsiFlag         PROTO NEAR :WORD
GetAnsiFlag         PROTO NEAR

ThreadSwitch        PROTO NEAR SYSCALL          ; NetWare Os Call
RelinquishControl   PROTO NEAR SYSCALL          ; NetWare Os Call

TIM4_ASCII_TO_ENCODE  PROTO NEAR,:LPSTR,:LPSTR
TIM4_ENCODE_TO_ASCII  PROTO NEAR,:WORD,:LPSTR
TIM4_ENCODE         PROTO
TIM4_SPLIT_ENCODE   PROTO NEAR,:WORD,:LPSTR,:LPSTR,:LPSTR

WRITEPRIVATEPROFILESTRING PROTO NEAR,:LPCSTR,:LPCSTR,:LPCSTR,:LPCSTR
ANSITOOEM           PROTO NEAR,:LPSTR,:LPSTR
OEMTOANSI           PROTO NEAR,:LPSTR,:LPSTR

Seek_to_index_entry PROTO NEAR,:LPSTR,:DWORD
Seek_to_data_record PROTO NEAR,:LPSTR,:DWORD
Seek_to_data_chain  PROTO NEAR,:LPSTR,:DWORD
Seek_to_quick_entry PROTO NEAR,:LPSTR,:DWORD
Find_free_data_entry PROTO NEAR,:LPSTR

UnEncrypt           PROTO NEAR,:LPSTR
Encrypt             PROTO NEAR,:LPSTR
SetEncryptINI       PROTO NEAR,:BOOL
EncryptHeader       PROTO NEAR,:UINT,:UINT

endif
