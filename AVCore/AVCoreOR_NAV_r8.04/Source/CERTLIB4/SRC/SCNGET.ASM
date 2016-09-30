include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scnget.asv   1.8   10 Jan 1996 06:56:22   JWORDEN  $
;
; SCN4_GET_ENTRY                       1/15/93                         CAG
;
;       Retrieve an entry from the virus database.
;
; BOOL WINAPI SCN4_GET_ENTRY(UINT Handle,WORD Entry,LPSTR Buffer);
;
; Input:
;       1)  Handle to opened database
;       2)  Entry number to lookup
;       3)  Buffer to store entry at (SIZE OF N30 structure)
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
SCN4_GET_ENTRY PROC USES XCX XDX,
        hHandle:WORD,
        uEntry:WORD,
        lpBuffer:LPSTR

IFDEF VIRUS_SUBSET
        mov     ax,uSizeVirscanHeader   ; Skip over the
        xor     dx,dx                   ; virscan header
         
        INVOKE  FIL4_SEEK,hHandle,dx::ax,fSEEK_BEG

        mov     cx, uEntry              ; Numer to search for
        LES_DI  lpBuffer                ; Point to our buffer!

J_WalkSkipLoop:
        INVOKE	FIL4_READ,hHandle,lpBuffer,sN30

        cmp     ax,-1                   ; Did we fail?
        je      J_Error

        cmp     ax,sN30                 ; Or read the wrong number of bytes?
        jne     J_Error

        mov     ax, XES[XDI].N30.wControl2  ; IN_TSR bit is in 2nd control bit
        test    ax, NOT_IN_TSR              ; If not in the TSR, don't count it!
        jnz     J_WalkSkipLoop

        loop    J_WalkSkipLoop          ; If we fall out of this loop,
                                        ; we are at the one we want.
        mov     ax,1
        jmp     J_Done

ELSE    ; VIRUS_SUBSET

        mov     ax,uEntry
        dec     ax
        xor     dx,dx
        mov     cx,sN30
        mul     cx
        add     ax,uSizeVirscanHeader                   ; Added to compensate
        adc     dx,0                                    ; for virscan header
         
        INVOKE  FIL4_SEEK,hHandle,dx::ax,fSEEK_BEG
        INVOKE	FIL4_READ,hHandle,lpBuffer,sN30

        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     J_Error

        mov     ax,1
        jmp     J_Done
ENDIF   ; VIRUS_SUBSET

J_Error:
        xor     ax,ax
J_Done:
        ret

SCN4_GET_ENTRY ENDP
;-----------------------------------------------------------------------------
END
