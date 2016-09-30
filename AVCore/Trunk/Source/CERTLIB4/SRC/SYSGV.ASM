include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_VECTOR                      1/12/93                         CAG
;
;       Return the value of a current vector.
;
; DWORD WINAPI SYS4_GET_VECTOR(BYTE Vector);
;
; Input:
;       1)  Vector number to change
; Output:
;	1)  SEG:OFF of vector in vector table.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_GET_VECTOR PROC USES XBX SES, 
        byVector:BYTE

        mov     ah,35h
        mov     al,byVector
        int     21h                     ; Get the current vector

	mov	dx,es
	mov	ax,bx
        ret

SYS4_GET_VECTOR ENDP
;-----------------------------------------------------------------------------
END

