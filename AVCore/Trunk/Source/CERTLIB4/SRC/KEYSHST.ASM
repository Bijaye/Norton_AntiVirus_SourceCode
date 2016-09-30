include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; KEY4_SHIFTSTATE                      12/17/92                        CAG
;
;       Test the state of the SHIFT keys. Both SHIFT/ALT/CTRL.
;
; BOOL WINAPI KEY4_SHIFTSTATE(WORD Flag);
;
; Input:
;       1)  Flag = 1 - Test if both SHIFT keys are down.
;                  2 - Test if both ALT keys are down.
;                  3 - Test if both CTRL keys are down.
;
; Output:
;       1)  FALSE (0) Not down.
;       2)  TRUE (1)  Are down.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;       
;-----------------------------------------------------------------------------
KEY4_SHIFTSTATE PROC USES SES XDI,
        KeyCheck:WORD

        mov     ax, 40h
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax       ; Memory segment of BIOS
        or      ax, ax
        jz      J_NO
endif
        mov     es,ax

        test    XES[96h],byte ptr 10h   ; Test if 101/102 keyboard
        jnz     IS_101

TestShift:
        mov     ah,02h                  ; Get shift state flags
        int     16h

        cmp     KeyCheck,word ptr 1
        jne     NotShift_1

        and     al,03h
        cmp     al,03h                  ; Test SHIFT state
        je      J_YES
        jmp     J_NO

NotShift_1:
        cmp     KeyCheck,word ptr 2
        jne     NotAlt_1

        and     al,08h
        cmp     al,08h                  ; Test ALT key
        je      J_YES
        jmp     J_NO

NotAlt_1:
        and     al,04h
        cmp     al,04h                  ; Test CTRL key
        je      J_YES
        jmp     J_NO

IS_101:
        mov     ah,12h                  ; Get shift state flags
        int     16h

        cmp     KeyCheck,word ptr 1
        jne     NotShift_2

        and     al,03h
        cmp     al,03h                  ; Test SHIFT state
        je      J_YES
        jmp     J_NO

NotShift_2:
        cmp     KeyCheck,word ptr 2
        jne     NotAlt_2

        and     ah,00001010b
        cmp     ah,00001010b            ; Test BOTH Alt keys
        je      J_YES
        jmp     J_NO

NotAlt_2:
        and     ah,00000101b
        cmp     ah,00000101b            ; Test BOTH Ctrl Keys
        je      J_YES
        jmp     J_NO

J_YES:
        mov     ax,1
        jmp     J_OK
J_NO:
        xor     ax,ax

J_OK:
        ret

KEY4_SHIFTSTATE	ENDP
;-----------------------------------------------------------------------------
END
