include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_HALT                            1/20/93                         CAG
;
;       Halt CPU processor.
;
; VOID WINAPI SYS4_HALT(VOID);
;
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\syswhalt.asv  $
;  
;     Rev 1.6   29 Nov 1994 13:21:22   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.1   17 Oct 1994 11:03:58   DALLEE
;  IFDEF DEBUG around live int 3.
;-----------------------------------------------------------------------------

SYS4_HALT PROC

IFDEF MIRRORS
; &? Calls to this function need to be examined individually for OS/2
        int     3
        ret
ELSE

        mov     ah,0Dh                  ; Flush buffers before we halt
        int     21h

        mov     bl,0
ExceptMore:
        mov     ax,0203h
        mov     cx,cs
        lea     XDX,HaltMe
        int     31h
        inc     bl
        cmp     bl,1Fh
        jne     ExceptMore

IFDEF DEBUG
        int     3h
        nop
ENDIF
        ret

HaltMe:
        cli
HaltMe2:
        jmp     HaltMe2

ENDIF

SYS4_HALT ENDP
;-----------------------------------------------------------------------------
END

