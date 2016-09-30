include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_SEEK                            12/23/92                        CAG
;
;       Seek to a new place in file.
;
; DWORD WINAPI FIL4_SEEK(UINT Handle,DWORD Number,WORD Flag)
;
; Input:
;       1)  Handle
;       2)  Bytes to seek
;       3)  Seek flag
;               fSEEK_BEG (1)  Seek from beginning of file
;               fSEEK_FOR (2)  Seek forward from current position
;               fSEEK_BAK (4)  Seek backward from current position
;               fSEEK_END (8)  Seek backward from end of file
; Output:
;       1)  Newlocation of handle.
;       2)  -1 if error
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_SEEK PROC USES XBX XCX,
        Handle:WORD,
        Number:DWORD,
        Flags:WORD

        mov     cx,word ptr Number+2
        mov     dx,word ptr Number

        mov     al,2h                   ; Seek from end
        test    Flags,fSEEK_END
        jnz     J_SeekFromEnd

        mov     al,0h                   ; Seek from start
        test    Flags,fSEEK_BEG
        jnz     J_SeekFromStart

        mov     al,1h                   ; Seek from current
        test    Flags,fSEEK_FOR
        jnz     J_SeekForward

J_SeekFromEnd:
        cmp     cx,0
        jnz     J_SeekBackward
        cmp     dx,0
        jz      DoSeek
J_SeekBackward:
        test    dx,8000h
        jnz     J_1
        and     dx,0111111111111111b
        inc     cx
J_1:
        neg     dx
        neg     cx
        or      cx,cx
        jnz     J_2
        dec     cx
J_2:
        jmp     DoSeek

J_SeekForward:
J_SeekFromStart:
;        test    dx,8000h
;        jz      DoSeek
;        and     dx,0111111111111111b
;        inc     cx
;        jmp     DoSeek

DoSeek:
        mov     XBX,Handle
        mov     ah,42h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jnc     J_Done
        mov     ax,-1
        mov     dx,-1
J_Done:
        ret

FIL4_SEEK ENDP
;-----------------------------------------------------------------------------
END
