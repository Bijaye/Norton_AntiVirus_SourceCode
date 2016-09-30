; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\ir_detmn.asv   1.5   29 Nov 1994 13:04:22   DALLEE  $
;
; Filename:     IR_DETMN.ASM
; Programmer:   Carey Nachenberg
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_TYPE, INOC_REP_COM_APPEND, INOC_REP_COM_PREPEND
;
; Note:         Not written for 32 bit environment
;
; $Log:   S:\certlib4\vcs\ir_detmn.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:04:22   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:20:22   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:31:00   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.0   03 Sep 1993 19:29:16   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   20 May 1993 08:14:46   DDREW
;  No change.
;  
;     Rev 1.1   24 Apr 1993 14:09:18   CJKUO
;  Removed redundant code (looking for the 35 byte header a second time).
;  
;     Rev 1.0   21 Apr 1993 21:19:02   CJKUO
;  Initial revision.
include CTS4.INC

; the following DEFINES should be placed in the CTS4.INC file ASAP :)

COM_PREPEND_REPAIR      equ     1
COM_JUMP_REPAIR         equ     2

;-----------------------------------------------------------------------------
; INOC_REP_TYPE                       17APR93
;
;       Determines the type of infection.
;
; DWORD WINAPI INOC_REP_TYPE(WORD Handle, LPSTR DataBuf,
;                            LPSTR WorkBuf, DWORD LocalFilesize)
;
; Input:
;       1)  Handle : File handle
;       2)  DataBuf : pointer to the 35 byte inoculation entry
;       3)  WorkBuf : An area of scratch memory at least 1024 bytes big
;       4)  LocalFilesize : Size of file
; Output: AX,CX,DX
;       AX = repair type
;       DX::CX = offset if COM_PREPEND_REPAIR
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------

UniqueSegment
INOC_REP_TYPE PROC USES XSI XDI SDS SES,
        Handle : WORD,
        DataBuf : LPSTR,
        WorkBuf : LPSTR,
        LocalFileSize : DWORD

        cld

        INVOKE  FIL4_SEEK, Handle, 0, fSEEK_BEG
        INVOKE  FIL4_READ, Handle, WorkBuf, 35

        lds     si, DataBuf
        lodsw
        cmp     ax, 04d5ah                      ; EXE file?
        jz      GENERIC_FILE
        cmp     ax, 05a4dh                      ; EXE file?
        jz      GENERIC_FILE

COM_FILE:                                       ; COM file!

        dec     si                              ; point SI back 1 word
        dec     si
        les     di, WorkBuf                     ; what we're comparing to
        mov     cx, 34                          ; point to buffer(s) + 34
        add     si, cx                          ; go to end of both buffers
        add     di, cx
        inc     cx                              ; compare 35 bytes (0 to 34)
        std                                     ; slide backwards
        repz    cmpsb                           ; compare!
        cld                                     ; reset direction flag

        cmp     cx, 10                          ; if at least 25/35 last bytes
                                                ; are ok, then appending com
        jle     GENERIC_FILE                    ; generic EXE/COM append repair

        INVOKE  INOC_REP_FIND35, Handle, 0, DataBuf, WorkBuf

        mov     cx, ax
        or      ax, dx                          ; do we have a com prepend
        jnz     COM_PREPEND                     ; yes!

; redundant because first INOC_REP_FIND35 goes all the way to the end.
;       INVOKE  INOC_REP_FIND35, Handle, LocalFileSize, DataBuf, WorkBuf

;       mov     cx, ax
;       or      ax, dx                          ; do we have a com overwrite
;       jnz     COM_PREPEND                     ; yes!

        ; if we can't figure out what to do, just do a generic repair...

GENERIC_FILE:
        mov     ax, COM_JUMP_REPAIR
        ret

COM_PREPEND:
        mov     ax, COM_PREPEND_REPAIR
        ret

INOC_REP_TYPE ENDP


; this function returns AX = 0 on success, AX = ffff on error
; it will attempt to fix a standard exe infection, com jump infection,
; or unknown infection


INOC_REP_COM_APPEND PROC USES XSI XDI,
        Handle : WORD,
        DataBuf : LPSTR,                ; assumed it's filled already
        WorkBuf : LPSTR,
        LocalFileSize : DWORD

        INVOKE  FIL4_SEEK, Handle, 0, fSEEK_BEG
        INVOKE  FIL4_WRITE, Handle, DataBuf, 35

        ; now seek to original EOF and truncate file.
        ; writing 0 bytes out truncates the file

        INVOKE  FIL4_SEEK, Handle, LocalFileSize, fSEEK_BEG
        INVOKE  FIL4_WRITE, Handle, DataBuf, 0

        ret

INOC_REP_COM_APPEND ENDP


; this function returns AX = 0 on success, AX = ffff on error
; it will attempt to fix prepending and modified overwriting com infections


INOC_REP_COM_PREPEND PROC USES XSI XDI,
        Handle : WORD,
        WorkBuf : LPSTR,
        LocalFileSize : DWORD,
        CopyStart : DWORD

        INVOKE  INOC_REP_COPY_BACK, Handle, WorkBuf, CopyStart

        ; now seek to original EOF and truncate file.
        ; writing 0 bytes out truncates the file

        INVOKE  FIL4_SEEK, Handle, LocalFileSize, fSEEK_BEG
        INVOKE  FIL4_WRITE, Handle, WorkBuf, 0

        ret

INOC_REP_COM_PREPEND ENDP

END
