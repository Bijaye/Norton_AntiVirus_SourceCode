; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/asfcache/VCS/cachemov.asv   1.0   06 Feb 1997 20:48:24   RFULLER  $
;
; Filename:     CACHEMOV.ASM
; Assembler:    MASM 6.0+
;
; Contents:     A set of buffer management routines for NAV NLM file cache
;
;-----------------------------------------------------------------------------
; $Log:   S:/asfcache/VCS/cachemov.asv  $
;  
;     Rev 1.0   06 Feb 1997 20:48:24   RFULLER
;  Initial revision
;  
;     Rev 1.1   13 May 1996 18:33:04   TSmith
;
;     Rev 1.0   02 May 1996 16:19:38   TSmith
;  Initial revision.
;
;     Rev 1.12   15 Dec 1994 17:42:22   KEITH
;  Use newer CTSN.INC rather than CTS4.INC
;
;     Rev 1.11   15 Dec 1994 16:40:16   KEITH
;  Change forwarded from Vishnu build 97
;-----------------------------------------------------------------------------
;Include PLATFORM.INC
Include CTSN.INC

OPTION SCOPED

; The following structs have parallel declarations in the
; file 'ASFCACHE.CPP' Any changes to the structs here should
; also be made in that file.

LIMIT  STRUCT
        nUpper         QWORD   0        ; highest file id in block
        nLower         QWORD   0        ; lowest file id in block
        nTimeBase      QWORD   0        ; last access time base
        dwReserved     DWORD   0
        wFlag          WORD    0        ; flag bits
        wMaxEntries    WORD    0        ; max number of entries
        wCount         WORD    0        ; number of entries
        wOffset        WORD    0        ; offset of first entry
LIMIT   ENDS

FILEPTR STRUCT
        nFileID        QWORD   0        ; unique 64-bit file identifier
        nFileTime      QWORD   0        ; time added
FILEPTR ENDS

UniqueSegment
;-----------------------------------------------------------------------------
; CACHEMOV                           6/23/93                        DDREW
;
;
;
;WORD WINAPI MoveDownAndInsert( PFILEPTR pLowPFILE,
;                               DWORD dwMatchAddr,
;                               WORD wSize );
;
;
;WORD WINAPI MoveUpAndInsert(   PFILEPTR pHighPFILE,
;                               DWORD dwMatchAddr,
;                               WORD wSize );
;
;
;WORD WINAPI MoveDownAndClose(  PFILEPTR pHighPFILE,
;                               PFILEPTR pFilePtr,
;                               WORD wSize );
;
;
;WORD WINAPI MoveUpAndClose(    PFILEPTR pLowPFILE,
;                               PFILEPTR pFilePtr,
;                               WORD wSize );
;
;
; Input:
;
; Output:
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------
MOVEDOWNANDINSERT PROC USES XCX XSI XDI,
        pLowPFILE:DWORD,
        dwMatchAddr:DWORD,
        wSize:WORD

        mov     esi, pLowPFILE          ; lowest struct in cell
        mov     ecx, dwMatchAddr        ; target insert address

        movzx   eax, wSize              ; size of struct to move

        cmp     esi, ecx                ; target and dest not same
        ja      NoMove

        sub     ecx, esi                ; calculate range
        add     ecx, eax                ; one structure size
        shr     ecx, 2                  ; divide by 4

        mov     edi, esi                ; prepare dest pointer
        sub     edi, eax                ;

        cld                             ; go up
        rep     movsd                   ; slide data down
NoMove:
        mov     edi, dwMatchAddr        ; target insert address
        mov     ecx, eax                ; structure size
        shr     ecx, 2
        xor     eax, eax                ; clear al
        rep     stosd                   ; null oldstruct

        ret

MOVEDOWNANDINSERT ENDP
;-----------------------------------------------------------------------------

MOVEUPANDINSERT PROC USES XCX SDS SES XSI XDI,
        pHighPFILE:DWORD,
        dwMatchAddr:DWORD,
        wSize:WORD

        movzx   eax, wSize              ; number of bytes to move

        mov     esi, pHighPFILE         ; highest struct in cell
        mov     edi, dwMatchAddr        ; target insert address
        cmp     edi, esi                ;
        ja      NoMove                  ; if target is above pHighPFILE

        add     esi, eax                ;
        mov     ecx, esi                ; calculate range
        sub     ecx, edi                ;

        shr     ecx, 2                  ; divide by 4

        sub     esi, 4
        mov     edi, esi                ; prepare dest pointer
        add     edi, eax                ;

        std                             ; go down
        rep     movsd                   ; slide data down
NoMove:
        mov     edi, dwMatchAddr        ; target insert address
        mov     ecx, eax                ; structure size
        shr     ecx, 2                  ; divide by 4
        xor     eax, eax                ; clear al
        cld                             ; preserve standard dir flag
        rep     stosd                   ; null oldstruct

        ret

MOVEUPANDINSERT ENDP
;-----------------------------------------------------------------------------

MOVEDOWNANDCLOSE PROC USES XCX SDS SES XSI XDI,
        pHighPFILE:DWORD,
        pFilePtr:DWORD,
        wSize:WORD


        mov     ecx, pHighPFILE         ; hiest structure address
        mov     esi, pFilePtr           ; prepare src pointer

        movzx   eax, wSize              ; structure size

        sub     ecx, esi                ; calculate range
        shr     ecx, 2                  ; divide by 4

        mov     edi, esi                ; prepare source pointer
        add     esi, eax                ;  by adding structure size

        cld                             ; go down
        rep     movsd                   ; slide data

        mov     edi, pHighPFILE         ; target insert address
        mov     ecx, eax                ; structure size
        shr     ecx, 2                  ; div by 4
        xor     eax, eax                ; clear al
        rep     stosd                   ; null oldstruct

        ret

MOVEDOWNANDCLOSE ENDP
;-----------------------------------------------------------------------------

MOVEUPANDCLOSE PROC USES XCX SDS SES XSI XDI,
        pLowPFILE:DWORD,
        pFilePtr:DWORD,
        wSize:WORD

        mov     ecx, pFilePtr           ; target insert address
        mov     esi, ecx                ; prepare src pointer

        movzx   eax, wSize              ; structure size

        sub     ecx, pLowPFILE          ; calculate range
        jecxz   NoMove
        shr     ecx, 2                  ; divide by 4

        sub     esi, 4
        mov     edi, esi                ; prepare dest pointer
        add     edi, eax                ;  by adding structure size

        std                             ; copy down
        rep     movsd                   ; slide data
NoMove:
        mov     edi, pLowPFILE          ; target insert address
        mov     ecx, eax                ; structure size
        shr     ecx, 2
        xor     eax, eax                ; clear eax
        cld                             ; go up
        rep     stosd                   ; null oldstruct

        ret

MOVEUPANDCLOSE ENDP
;-----------------------------------------------------------------------------

BackCopy PROC USES XCX SDS SES XSI XDI,
        pDest:DWORD,
        pSrc:DWORD,
        wCount:WORD

        movzx   ecx, wCount             ; bytes to copy

        mov     esi, pSrc               ; prepare src pointer
        mov     edi, pDest              ; prepare dest pointer

        add     esi, ecx
        add     edi, ecx

        sub     esi, 4
        sub     edi, 4

        shr     ecx, 2                  ; divide by 4

        std                             ; go down
        rep     movsd                   ; slide data
        cld                             ; restore direction

        ret

BackCopy ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END