include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_GET_EXEC_NAME                   1/22/93                         CAG
;
;       Get the program execute name and directory.
;
; LPSTR WINAPI FIL4_GET_EXEC_NAME(VOID);
;
; Output:
;       1)  Pointer to the name of the file that was executed.
;       2)  Error (0) Name not found
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
FIL4_GET_EXEC_NAME PROC USES XBX SES XDI

        mov     ah,62h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h                     ; PSP in BX
ENDIF

        MOV_ES  bx
        mov     bx,XES[2Ch]

        xor     ax,ax
        xor     dx,dx
        or      bx,bx
        jz      NoGo

        MOV_ES  bx
        xor     XDI,XDI

        xor     ax,ax
        mov     cx,-1
MoreToDo:
        cmp     XES[XDI],word ptr 0
        je      Found
        inc     XDI
        loop    MoreToDo
Found:
        add     XDI, 4
        mov     dx,es
        mov     ax,di

NoGo:
        ret

FIL4_GET_EXEC_NAME ENDP
;-----------------------------------------------------------------------------
END
