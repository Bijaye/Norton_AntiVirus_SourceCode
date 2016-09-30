include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_WRITELINE                       3/16/93                         CAG
;
;       Writes a line to a file, adding a CR/LF to the end of it.
;
; WORD WINAPI FIL4_WRITELINE(UINT Handle,LPCSTR lpszString);
;
; Input:
;       1)  Handle of file opened for Write.
;       2)  Pointer to a string to write, NULL terminated
; Output:
;       1)  Number of bytes written to the file, including CR/LF
;       2)  -1 ERROR
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_WRITELINE PROC USES XCX,
        uHandle:UINT,
        lpszString:LPCSTR

        LOCAL   szCRLF[2]:BYTE

        mov     szCRLF[0],CR
        mov     szCRLF[1],LF

        INVOKE  STR4_LENGTH,lpszString
        mov     cx,ax
        INVOKE  FIL4_WRITE,uHandle,lpszString,cx
        cmp     ax,-1
        je      IsError
        INVOKE  FIL4_WRITE,uHandle,ADDR szCRLF,2

        inc     cx
        inc     cx
        mov     ax,cx
IsError:
        ret

FIL4_WRITELINE ENDP
;-----------------------------------------------------------------------------
END

