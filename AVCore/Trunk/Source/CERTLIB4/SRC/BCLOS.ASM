include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_CLOSE                           12/9/92                         CAG
;
;	Close a previously opened Window from the screen.
;
; UINT WINAPI BOX4_CLOSE((DWORD) *WINDOW);
;
; Input:
;       1)  Pointer to window struct
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_CLOSE PROC USES XBX XCX SDS SES XSI XDI,
        lpWindow:LPSTR

        
        LDS_SI  lpWindow
        cmp     word ptr [XSI].WINDOW.wVideo+2,0
        je      J_ERROR

; Store the current video screen in buffer.
        LES_DI  [XSI].WINDOW.wVideo
        mov     cx,[XSI].WINDOW.wHeigth
        mov     bx,[XSI].WINDOW.wWidth
        LDS_SI  [XSI].WINDOW.wBuffer
J_1:
        push    XDI
        push    XCX
        mov     cx,bx
        rep     movsw                   ; Move from DS:SI to ES:DI, CX count
        pop     XCX
        pop     XDI
        add     XDI,160
        loop    J_1

        xor     ax,ax
        jmp     J_3

J_ERROR:
        mov     ax,1                    ; Error = 1
J_3:
        ret

BOX4_CLOSE      ENDP
;-----------------------------------------------------------------------------
END
