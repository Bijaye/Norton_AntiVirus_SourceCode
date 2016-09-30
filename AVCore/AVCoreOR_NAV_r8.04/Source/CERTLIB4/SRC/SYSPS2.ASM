include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_IS_PS2                          1/19/93                         CAG
;
;       Determine if this is a PS2 system or not.
;
; BOOL WINAPI SYS4_IS_PS2(void);
;
; Output:
;       1) FALSE (0) Not a PS2
;       2) TRUE (1) Is a PS2
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_IS_PS2 PROC USES XBX SES

        mov     ax,0C000h
        int     15h
        sti
        jc      NotPS2

        or      ah,ah
        jnz     NotPS2

        test    byte ptr es:[bx+5],00010000y
        jz      NotPS2

        mov     ax,1
        jmp     Done
NotPS2:
        xor     ax,ax
Done:

        ret

SYS4_IS_PS2 ENDP
;-----------------------------------------------------------------------------
END
