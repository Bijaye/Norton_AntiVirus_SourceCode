include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_KILL                           12/29/92                        CAG
;
;       Kill and overwrite a file from the drive.
;
; BOOL WINAPI FIL4_KILL(LPSTR Filename,LPSTR Buffer,WORD SizeBuffer,WORD Flag);
;
; Input:
;       1)  Filename to kill
;       2)  Buffer to use if flag >0
;       3)  Size of buffer
;       4)  Flag (Number of times to overwrite)
; Output:
;       1)  TRUE (1)  Passed.
;       2)  FALSE (0) Failed.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_KILL PROC USES XDX SDS,
        Filename:LPSTR,
        Buffer:LPSTR,
        sBuffer:WORD,
        Flag:WORD

        LOCAL   Handle:WORD
        LOCAL   FileSize:DWORD
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, Filename, Filename
@@:
ENDIF

        ; first we must make sure we CAN write to/delete this file!

        INVOKE  FIL4_ATTR, Filename, 0, SET_INFO

        cmp     Flag,0
        je      JustDelete

;--------------------------------------
; Open file for WRITE
        INVOKE  FIL4_OPEN,Filename,fWRITE
        cmp     ax, -1
        jz      JustDelete
        mov     Handle,ax

;--------------------------------------
; Get Size and write entire file of garbage
J_OverAgain:
        INVOKE  FIL4_SEEK,Handle,0,fSEEK_END
        mov     word ptr FileSize+2,dx
        mov     word ptr FileSize,ax
        INVOKE  FIL4_SEEK,Handle,0,fSEEK_BEG

J_Again:
        INVOKE  FIL4_WRITE,Handle,Buffer,sBuffer
        cmp     ax,-1
        je      J_Error

        cmp     word ptr FileSize+2,0
        jnz     J_0
        cmp     ax,word ptr FileSize
        ja      J_1
J_0:
        sub     word ptr FileSize,ax
        sbb     word ptr FileSize+2,0
        jmp     J_Again

J_1:
        dec     Flag
        cmp     Flag,0
        jne     J_OverAgain

;--------------------------------------
; Write 0 bytes to TRUNCATE file
        INVOKE  FIL4_SEEK,Handle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,Handle,Buffer,0


;--------------------------------------
J_Error:
        INVOKE  FIL4_CLOSE,Handle

;--------------------------------------
JustDelete:
        INVOKE  FIL4_DELETE,Filename

                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, Filename, Filename
@@:
        pop     ax
ENDIF
        ret

FIL4_KILL ENDP
;-----------------------------------------------------------------------------
END

