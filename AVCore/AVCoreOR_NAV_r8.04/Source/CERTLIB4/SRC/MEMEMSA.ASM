include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_EMS_ALLOCATE                    1/5/93                          CAG
;
;       Allocate memory in the EMS area.
;
; DWORD WINAPI MEM4_EMS_ALLOCATE(WORD Paragraphs);
;
; Input:
;       1)  Number of paragraphs to allocate
; Output:
;       1)  LOWORD ERROR (0) HIWORD Paragraphs available
;       2)  LOWORD Handle of EMS
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_EMS_ALLOCATE PROC USES XBX,
        lParagraphs:WORD


        INVOKE  EMS_DETECT
        or      ax,ax
        jz      J_ERROR

        mov     bx,lParagraphs
        mov     ah,43h
        mov     cl,0Ah
        shr     bx,cl                   ; Divide by 1024
        inc     bx                      ; But allow for overflow
        int     67h
        or      ah,ah
        jnz     J_Error
        mov     ax,dx
        jmp     J_Done
J_Error:
        mov     ah,42h
        int     67h
        or      ah,ah
        jz      J_Major
        mov     cl,0Ah
        shl     dx,cl                   ; Multiply by 1024
        jmp     J_DoOut

J_Major:
        xor     dx,dx
J_DoOut:
        xor     ax,ax
J_Done:
        ret

MEM4_EMS_ALLOCATE ENDP
;-----------------------------------------------------------------------------
EMS_DETECT PROC USES SDS XSI
        
        xor     bx,bx
        SMOV    ds,cs
        mov     ax,3D00h
        mov     dx,offset EMMName
        int     21h
        jc      J_DE

        mov     bx,ax
        mov     ax,4400h
        int     21h
        test    dl,10000000b            ; Test if file or memory.
        jz      J_DE

        mov     ax,4407h
        int     21h
        or      al,al
        jz      J_DE                    ; Device not ready.

        mov     ah,3Eh                  ; Close handle
        int     21h

; Get number of pages available.
        mov     ah,42h
        int     67h                     ; Get number of pages free.
        or      bx,bx
        jnz     J_DE0

J_DE:
        xor     ax,ax
        jmp     J_Done
J_DE0:
        mov     ax,1
J_Done:
        ret

EMMName         db      'EMMXXXX0',0

EMS_DETECT ENDP
;-----------------------------------------------------------------------------
END
