include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_PRINT                           12/9/92                         CAG
;
;	Print text into a window that was opened.
;
; UINT WINAPI BOX4_PRINT((DWORD) *WINDOW,(DWORD) *Text);
;
; Input:
;       1)  Pointer to Window struct
;       2)  Text to print
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_PRINT PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpWindow:LPSTR,
        Message:LPSTR
        
        LOCAL   wHeigth:WORD
        LOCAL   wWidth:WORD
        LOCAL   Count:WORD
        LOCAL   wAttrib:BYTE
        LOCAL   ptrLine:WORD32
        LOCAL   ptrMessage:WORD32

        LDS_SI  lpWindow
        LES_DI  [XSI].WINDOW.wVideo
        add     XDI,160                 ; Down one line
        add     XDI,2                   ; Over two spaces

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
        mov     ptrMessage,XSI

J_NextLine:
        mov     XDI,ptrLine             ; Where to place text ES:DI
        mov     XSI,ptrMessage
        mov     XBX,ptrMessage
J_NextChar:
        lodsb
        cmp     al,0Ah                  ; LF, just skip.
        je      J_NextChar

        or      al,al
        jz      J_Done

        cmp     al,0Dh                  ; CR
        je      J_PrintLine

        cmp     al,' '
        jne     J_Continue
        mov     XBX,XSI                 ; Place book mark in BX
J_Continue:
        dec     Count
        cmp     Count,0
        jne     J_NextChar

J_PrintLine:
        mov     XSI,ptrMessage
        mov     XCX,XBX
        sub     XCX,XSI
        mov     ah,wAttrib

        or      XCX,XCX
        jnz     PrintChar2

        mov     cx,wWidth
        add     ptrMessage,XCX
        jmp     PrintChar

PrintChar2:
        mov     ptrMessage,XBX

        lodsb
        cmp     al,' '
        jne     IsNotFirstSpace
        dec     cx

PrintChar:
        lodsb
IsNotFirstSpace:
        stosw
        loop    PrintChar

        mov     bx,wWidth
        mov     Count,bx
        add     ptrLine,160
        jmp     J_NextLine
J_Done:
        mov     XSI,ptrMessage
        mov     ah,wAttrib
J_DoChar:
        lodsb 
        or      al,al
        jz      J_ReallyDone
        stosw
        jmp     J_DoChar



J_ReallyDone:
        xor     ax,ax
        ret

BOX4_PRINT    ENDP
;-----------------------------------------------------------------------------
END

