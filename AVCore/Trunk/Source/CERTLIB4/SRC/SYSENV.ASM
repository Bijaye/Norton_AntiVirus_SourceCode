include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_ENVIROMENT                  2/8/93                          CAG
;
;       Retrieve the pointer to the currents PSPs enviroment.
;
; LPSTR WINAPI SYS4_GET_ENVIROMENT(VOID);
;
; Output:
;       1)  Pointer to current PSP enviroment string.
;       2)  Not found (0)
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB	
; LIBRARY:WIN16.LIB
; LIBRARY:WIN32.LIB	
;
;-----------------------------------------------------------------------------
SYS4_GET_ENVIROMENT PROC USES XBX SES

        mov     ax,6200h
        int     21h                     ; Get current PSP
        mov     es,bx
        mov     dx,XES[2Ch]             ; Pointer to enviroment
        xor     ax,ax
        ret

SYS4_GET_ENVIROMENT ENDP
;-----------------------------------------------------------------------------
END
