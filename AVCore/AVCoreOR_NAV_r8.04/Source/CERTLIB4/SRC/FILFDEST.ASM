INCLUDE CTS4.INC

UniqueSegment
;----------------------------------------------------------------------------
; FIL4_FILE_DESTINATION             7/16/93                         bAd
;
;   Returns full path of File found. Looks first for File1, then File2.
;
; UINT WINAPI FIL4_FILE_DESTINATION (LPSTR lpszFile1,LPSTR lpszFile2,
;                                     LPSTR lpszDest);
;
; Input:
;       1)  First file to search for.
;       2)  Next file to search for.
;       3)  String to put full pathname in.
;
; Output:
;       1)  UINT: (0) = found file
;                 (-1) = error
;                      = file not found
;       2)  lpszDest contains full drive, path, and filename of file found.
;
; LIBRARY: 
;
;-----------------------------------------------------------------------------
FIL4_FILE_DESTINATION PROC USES XBX XCX XSI XDI SDS SES,
        lpszFile1:LPSTR,
        lpszFile2:LPSTR,
        lpszDest:LPSTR

        LOCAL   lpszCurFile:LPSTR
        LOCAL   bDrive:BYTE
        LOCAL   szLocalSlash[2]:BYTE

        mov     word ptr szLocalSlash, 005ch

        mov     cx, 2                   ; set loop for two passes

;----------------------------
; point CurFile to File1
;----------------------------
        mov     ax, word ptr lpszFile1+2
        or      ax, ax                  ; skip if string is NULL
        jz      DoFile2
        mov     word ptr lpszCurFile+2, ax
        mov     ax, word ptr lpszFile1
        mov     word ptr lpszCurFile, ax

J_0:
        INVOKE  FIL4_ATTR,lpszCurFile,00h,GET_INFO ; get attrib for CurFile
        cmp     ax, -1                  ; does CurFile exist?
        jnz     ProcessCurFile          ; then, jmp to ProcessCurFile

;----------------------------
; else, point CurFile to File2
;----------------------------
DoFile2:
        mov     ax, word ptr lpszFile2+2
        or      ax, ax                  ; skip if string is NULL
        jz      Err
        mov     word ptr lpszCurFile+2, ax
        mov     ax, word ptr lpszFile2
        mov     word ptr lpszCurFile, ax

        loop    J_0                     ; go and look for File2

        jmp     NotFound                ; File2 not there either

ProcessCurFile:
        LDS_SI  lpszCurFile             ; DS:SI <- lpszCurFile
        LES_DI  lpszDest                ; ES:DI <- lpszDest

;----------------------------
; is drive letter and colon 
; there?        
;----------------------------
        cmp     byte ptr [XSI+1], ':'
        jz      IsDrive                ; skip it

; fill in the drive and colon
        INVOKE  FIL4_DRIVE,vSES XDI,GET_INFO

        mov     al, byte ptr XES[XDI]
        and     al, 11011111b            ; make sure it's uppercase
        sub     al, 'A'
        inc     al
        mov     bDrive, al

        inc     XDI
        inc     XDI
        jmp     LookPath

IsDrive:
        mov     al, byte ptr [XSI]
        and     al, 11011111b            ; make sure it's uppercase
        sub     al, 'A'
        inc     al
        mov     bDrive, al

        inc     XSI
        inc     XSI

; special case for drive:filename, with no path
        cmp     byte ptr [XSI], '\'
        jz      Copyfile
;-----------------------------------------------------------------------------
; INLINE #1, CAG, 10-4-93 (NIRVANA)
        cmp     byte ptr [XSI], '/'
        jz      CopyFile
;-----------------------------------------------------------------------------
        
        dec     XSI
        dec     XSI
        movsw
        INVOKE  FIL4_DIRECTORY,bDrive,vSES XDI,GET_INFO

        cmp     XES[XDI],word ptr '\'
        je      DontAddSlash

;-----------------------------------------------------------------------------
; INLINE #1, CAG, 10-4-93 (NIRVANA)
        cmp     XES[XDI],word ptr '/'
        je      DontAddSlash
;-----------------------------------------------------------------------------

        INVOKE  STR4_APPEND,vSES XDI,ADDR szLocalSlash
DontAddSlash:
        INVOKE  STR4_APPEND,vSES XDI,vSDS XSI
        jmp     FileFound

;----------------------------
; is path there?
;----------------------------
LookPath:
        cmp     byte ptr [XSI], '\'
        jz      CopyFile

;-----------------------------------------------------------------------------
; INLINE #1, CAG, 10-4-93 (NIRVANA)
        cmp     byte ptr [XSI], '/'
        jz      CopyFile
;-----------------------------------------------------------------------------

; path there
        INVOKE  FIL4_DIRECTORY,bDrive,vSES XDI,GET_INFO
; test for C:\0, so we don't get C:\\0
        INVOKE  STR4_LENGTH,vSES XDI
        mov     bx,di
        add     bx,ax
        cmp     XES[bx-1],byte ptr '\'
        je      CopyFile                ; We already have slash!

;-----------------------------------------------------------------------------
; INLINE #1, CAG, 10-4-93 (NIRVANA)
        cmp     XES[bx-1],byte ptr '/'
        je      CopyFile
;-----------------------------------------------------------------------------

        INVOKE  STR4_APPEND,vSES XDI,ADDR szLocalSlash

;----------------------------
; copy the CurFile to Dest
;----------------------------
CopyFile:
        INVOKE  STR4_APPEND,vSES XDI,lpszCurFile

FileFound:
        xor     ax, ax
        jmp     Done
NotFound:
Err:
        mov     ax, -1
Done:
        ret

FIL4_FILE_DESTINATION ENDP
;----------------------------------------------------------------------------
END
