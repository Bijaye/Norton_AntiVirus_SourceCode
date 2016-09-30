include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_PRINTF                          12/9/92                         CAG
;
;	Print text into a window that was opened, and format.
;
; UINT WINAPI BOX4_PRINTF((DWORD) *WINDOW,(DWORD) *Text, (DWORD)[args]);
;
; Input:
;       1)  Window struct
;       2)  Text to display
;       3)  Pointers to string to add to %s
;       4)  LAST ARG must be a NULL pointer
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_PRINTF PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpWindow:LPSTR,
        Message:LPSTR

        LOCAL   ScreenAddress:LPSTR
        LOCAL   wHeigth:WORD
        LOCAL   wWidth:WORD
        LOCAL   Count:WORD
        LOCAL   wAttrib:BYTE
        LOCAL   ptrStack:WORD32
        LOCAL   ptrLine:WORD32

        mov     bx,bp
        add     bx,14
        mov     ptrStack,XBX

; Caculate where to start storing
        LDS_SI  lpWindow
        LES_DI  [XSI].WINDOW.wVideo
        mov     cx,[XSI].WINDOW.wHeigth
        dec     cx
        dec     cx
        mov     bx,[XSI].WINDOW.wWidth
        dec     bx
        dec     bx
        mov     ah,[XSI].WINDOW.wAttrib
        mov     wHeigth,cx
        mov     wWidth,bx
        mov     Count,bx
        mov     wAttrib,ah

        LDS_SI  Message
        mov     ptrLine,XDI

J_6:
        mov     XDI,ptrLine
J_5:
        lodsb
        cmp     al,0
        jz      J_7
        cmp     al,'%'                  ; Test if %s string replacement
        je      J_2
        cmp     al,05Ch                 ; Test if CR command
        je      J_3

        cmp     Count,byte ptr 0        ; Can we fit more on this line?
        je      J_4
        dec     Count

        mov     ah,wAttrib
        stosw
        jmp     J_5
J_3:
        inc     XSI                     ; Skip over CR
        cmp     [XSI],byte ptr 0
        je      J_7
J_4:
        mov     bx,wWidth
        mov     Count,bx
        add     ptrLine,160
        jmp     J_6

J_2:

        inc     XSI                     ; Over %s
        cmp     [XSI],byte ptr 0
        je      J_7

        push    ds
        push    XSI
        mov     XBX,ptrStack
        LDS_SI  ss:[XBX]
        cmp     XSI,0
        jz      J_8
        add     ptrStack,4
J_10:
        lodsb
        cmp     al,0
        jz      J_8
        cmp     Count,byte ptr 0
        je      J_9
        dec     Count

        mov     ah,wAttrib
        stosw
        jmp     J_10
J_9:
        mov     bx,wWidth
        mov     Count,bx
        add     ptrLine,160
        mov     XDI,ptrLine
        jmp     J_10

J_8:
        pop     XSI
        pop     ds
        jmp     J_5
J_7:
        xor     ax,ax
        ret

BOX4_PRINTF    ENDP
;-----------------------------------------------------------------------------
END
