include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_SIZE                            12/23/92                        CAG
;
;       Return the size of a file.
;
; DWORD WINAPI FIL4_SIZE(UINT Handle);
;
; Input:
;       1)  Handle
; Output:
;       1)  File size
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_SIZE PROC USES XBX XCX,
        Handle:UINT

        LOCAL   Current:DWORD

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_FOR
        mov     word ptr Current+2,dx
        mov     word ptr Current,ax

        INVOKE  FIL4_SEEK,Handle,0,fSEEK_END
        mov     cx,dx
        mov     bx,ax

        INVOKE  FIL4_SEEK,Handle,Current,fSEEK_BEG
        mov     dx,cx
        mov     ax,bx

        ret

FIL4_SIZE ENDP
;-----------------------------------------------------------------------------
END

