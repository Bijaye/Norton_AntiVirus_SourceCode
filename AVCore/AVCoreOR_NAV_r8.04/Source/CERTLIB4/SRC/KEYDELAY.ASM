include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; KEY4_GET_DELAY                       2/28/93                         CAG
;
;       Get a key, return if delay time was meet.
;
; WORD WINAPI KEY4_GET_DELAY(WORD DelayTime);
;
; Input:
;       1)  Delay time (in 1/2 seconds) before return, -1 unlimited
; Output:
;       1)  Keystroke
;       2)  0 If timed out
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
KEY4_GET_DELAY PROC,
        uSeconds:WORD

        mov     ax,uSeconds
        cmp     ax,-1
        je      GotKey

        shl     ax,1
        shl     ax,1                    ; Make it .4 of a second
        mov     bx,ax

NotTimedOut:
        mov     ah,1
        int     16h
        jnz     GotKey

        mov     cx,1
        mov     dx,34464                ; 0.1 Second wait
        mov     ah,86h
        int     15h

        dec     bx
        cmp     bx,0
        jne     NotTimedOut
        xor     ax,ax
        jmp     Timedout

GotKey:
        INVOKE  KEY4_GET_CHAR

TimedOut:

        ret

KEY4_GET_DELAY ENDP
;-----------------------------------------------------------------------------
END
