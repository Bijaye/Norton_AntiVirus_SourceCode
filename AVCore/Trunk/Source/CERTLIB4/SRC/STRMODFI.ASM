include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_MODIFY_LONG_FILENAME             9/7/92                        bAd
;
;       Modifies "C:\LONG\DIRECTORY\ENTRY\FILENAME", to 
;                "C:\LONG\...\ENTRY\FILENAME".
;
; VOID WINAPI STR4_MODIFY_LONG_FILENAME(LPSTR lpszShortName,LPSTR
;                                        lpszOriginalName);
;
; Input:
;       1)  The new, shortened version of the filename.
;       2)  The complete, original version.
;
; Output:
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; NOTE:
;       Keeps first directory name and last directory name, but replaces
;       everything in between.
;
;-----------------------------------------------------------------------------
STR4_MODIFY_LONG_FILENAME PROC USES XBX XCX XSI XDI SDS SES,
        lpszShortName:LPSTR,
        lpszOriginalName:LPSTR

        LOCAL   pCurrent:WORD

        ; keep everything up to and include second '\'
        LDS_SI  lpszOriginalName
        LES_DI  lpszShortName
        cld
        xor     bx, bx                  ; count the slashes

TwoSlashes:
        lodsb
        stosb
        cmp     al, 00h                 ; string ended
        jz      NotLongEnough
        cmp     al, '\'
        jnz     TwoSlashes
        inc     bx
        cmp     bx, 2
        jnz     TwoSlashes

Middle:
        ; find out how many characters to skip
        xor     cx, cx                  ; count how many chars to skip
        mov     pCurrent, XSI
        ; find NULL terminator
Len:
        lodsb                           
        inc     cx
        or      al, al
        jnz     Len

        ; now back up two slashes
        xor     bx, bx
        dec     XSI                     ; point to NULL
        std
BackSlash:
        lodsb
        dec     cx
        jcxz    NotLongEnough
        cmp     al, '\'
        jnz     BackSlash
        inc     bx
        cmp     bx, 2
        jnz     BackSlash
        cld

        ; is filename long enough
        cmp     XSI, pCurrent
        jl      NotLongEnough

        ; append "..."
        mov     al, '.'
        stosb
        stosb
        stosb

        mov     XSI, pCurrent
        add     XSI, cx

        ; copy until end of string
ToEnd:
        lodsb
        stosb
        or      al, al
        jz      Done
        jmp     ToEnd


NotLongEnough:
        ; just copy it as is
        INVOKE  STR4_COPY,lpszOriginalName,lpszShortName
Done:
        ret

STR4_MODIFY_LONG_FILENAME ENDP
;-----------------------------------------------------------------------------
END

