include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_GET_EXEC_DIR                    1/22/93                         CAG
;
;       Get the program execute directory.
;
; VOID WINAPI FIL4_GET_EXEC_DIR(LPSTR Storage);
;
; Input:
;       1)  Pointer to where to store directory
; Output:
;       1)  Directory buffer filled.
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
FIL4_GET_EXEC_DIR PROC USES XCX SDS SES XSI XDI,
        lpStorage:LPSTR


        INVOKE  FIL4_GET_EXEC_NAME
        or      dx,dx
        jz      NotHere
        mov     ds,dx
        mov     si,ax
        INVOKE  MEM4_SRCH_CHAR,vSDS XSI,0,256
        mov     XDI,XAX

MoreBack:
        dec     XDI
        cmp     [XDI],byte ptr 5Ch
        jne     MoreBack

        mov     XCX,XDI
        sub     XCX,XSI

        INVOKE  MEM4_COPY,vSDS XSI,lpStorage,XCX
NotHere:
        ret

FIL4_GET_EXEC_DIR ENDP
;-----------------------------------------------------------------------------
END

