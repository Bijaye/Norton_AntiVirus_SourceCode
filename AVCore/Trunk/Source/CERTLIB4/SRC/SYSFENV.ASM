include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_FREE_ENVIROMENT                  10/30/91                        CAG
;
;      Free enviroment segment, return segment to PSP and
;      where enviroment segment was.
;
; WORD WINAPI SYS4_FREE_ENVIROMENT(VOID);
;
; Output:
;       1)  Segment of matching PSP
;       2)  Error (0)
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_FREE_ENVIROMENT PROC USES XBX SES

       mov     ah,51h
       int     21h                     ; Get PSP segment

       mov     es,bx
       mov     es,es:[44]              ; Segment of enviroment
       mov     ah,49h
       int     21h

       mov     ax,es
       ret

SYS4_FREE_ENVIROMENT ENDP
;-----------------------------------------------------------------------------
END
