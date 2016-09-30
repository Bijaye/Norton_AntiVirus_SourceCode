include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_GET_VIDEOSEG                     8/5/91                          CAG
;
;       Return video segment for this monitor, also return video mode.
;       Will only do TEXT 80x25 modes.
;
; UINT WINAPI SCR4_GET_VIDEOSEG(void);
;
; Output:
;       1)  Segment of video 
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_GET_VIDEOSEG PROC USES XBX XDX XSI XDI

        mov     ah,0Fh
        int     10h
        mov     bl,al

        cmp     al,2
        je      seg_b800
        cmp     al,3
        je      seg_b800

        cmp     al,7
        je      seg_b000

        stc
        jmp     no_good                 ; Not a currently supported mode.
        
seg_b800:
        mov     ax,0B800h               ; Adjust ES for B800
        jmp     good

seg_b000:
        mov     ax,0B000h               ; Set ES for B000
        jmp     good
no_good:
        xor     ax,ax
good:
        ret

SCR4_GET_VIDEOSEG ENDP
;-----------------------------------------------------------------------------
END
