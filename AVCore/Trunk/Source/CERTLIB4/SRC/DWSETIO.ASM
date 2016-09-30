include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_SET_IO                          1/7/93                          CAG
;
;       According to the version of DOS, setup the call structure
;       for interrupt 25h or 26h.
;
; UINT WINAPI DIS4_SET_IO(LPSTR Buffer,DWORD Sector, WORD Count);
;
; Input:
;       1)  DOS LPSTR to store information at.
;       2)  Sector, can be for large media.
;       3)  Number of sectors to read
; Output:
;       1)  0 No need to do extended read
;       2)  1 Must do extended read, buffer setup.
;       3)  -1 Critical error
;
; LIBRARY: WIN16.LIB
;          WIN32.LIB
;
;-----------------------------------------------------------------------------
DIS4_SET_IO PROC USES XBX XCX XDX SDS XSI,
        lpBuffer:LPSTR,
        uSector:DWORD,
        uCount:WORD


        mov     ax,3000h                ; Get DOS version
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        
        xchg    al,ah
        cmp     ax,031Eh                ; Version 3.30?
        jb      J_1
        ja      J_0

        cmp     bh,05h                  ; Zenith OEM Version 3.30
        jne     J_1
     
;--------------------------------------
J_0:                                    ; Extended format
        mov     ax, word ptr lpBuffer+2
IFDEF WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jnz     GotSel
        mov     ax, -1
        jmp     J_Done

GotSel:
ENDIF
        mov     ds,ax
        mov     XSI,word ptr lpBuffer

        mov     ax,word ptr uSector+2
        mov     [XSI+2],ax
        mov     ax,word ptr uSector
        mov     [XSI],ax                ; Logical sector to read

        mov     ax,uCount
        mov     [XSI+4],ax              ; Number to read

        mov     [XSI+6],XSI
        mov     ax,word ptr lpBuffer+2
        mov     [XSI+8],ax              ; Buffer is at DS:SI

        mov     ax,1
        jmp     J_Done

;--------------------------------------
J_1:
        xor     ax,ax

J_Done:
        ret


DIS4_SET_IO ENDP
;-----------------------------------------------------------------------------
END
