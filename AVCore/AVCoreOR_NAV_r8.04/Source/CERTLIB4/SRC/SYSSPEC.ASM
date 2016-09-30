include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_COMSPEC                     2/8/93                          CAG
;
;       Retrieve the COMMAND interperter used for this computer.
;
; BOOL WINAPI SYS4_GET_COMSPEC(LPSTR lpBuffer);
;
; Input:
;       1)  Buffer of 128 bytes to store name in
; Output:
;       1)  TRUE (1)  Found.
;       2)  FALSE (0) Not found.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
SYS4_GET_COMSPEC PROC,
        lpBuffer:LPSTR

        LOCAL   lpCOMSPEC[8]:BYTE

        mov     lpCOMSPEC+0,'C'
        mov     lpCOMSPEC+1,'O'
        mov     lpCOMSPEC+2,'M'
        mov     lpCOMSPEC+3,'S'
        mov     lpCOMSPEC+4,'P'
        mov     lpCOMSPEC+5,'E'
        mov     lpCOMSPEC+6,'C'
        mov     lpCOMSPEC+7,0

        INVOKE  SYS4_GET_ENVIROMENT
        or      dx,dx
        jz      J_Error

        INVOKE  MEM4_SRCH_STRING,dx::ax,ADDR lpCOMSPEC,08000h
        or      dx,dx
        jz      J_Error

        INVOKE  MEM4_SRCH_CHAR,dx::ax,'=',128
        or      dx,dx
        jz      J_Error

        inc     ax
        INVOKE  STR4_COPY,dx::ax,lpBuffer
        mov     XAX,TRUE
J_Error:
        ret

SYS4_GET_COMSPEC ENDP
;-----------------------------------------------------------------------------
END
