include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_PRINT_LINE                      5/12/92                         CAG
;
;	Print text into a window that was opened.
;
; UINT WINAPI BOX4_PRINT_LINE(WINDOW FAR *lpWindow,LPSTR Text,BYTE LineNumber);
;
; Input:
;       1)  Pointer to Window struct
;       2)  Text to print
;       3)  Line to print it on (0 = First line in box, not border)
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_PRINT_LINE PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpWindow:LPSTR,
        Message:LPSTR,
        byLine:BYTE
        
        LOCAL   wHeigth:WORD
        LOCAL   wWidth:WORD
        LOCAL   Count:WORD
        LOCAL   wAttrib:BYTE
        LOCAL   ptrLine:WORD32
        LOCAL   ptrMessage:WORD32

        LDS_SI  lpWindow
        LES_DI  [XSI].WINDOW.wVideo
        add     XDI,160                 ; Down one line

        add     XDI,2                   ; Over one space
        add     XDI,2                   ; Over one space

        mov     cl,byLine
        xor     ch,ch
        jcxz    NoLine
NextLine:
        add     XDI,160                 ; Add each line
        loop    NextLine
NoLine:

        mov     cx,[XSI].WINDOW.wHeigth
        dec     cx
        dec     cx
        mov     bx,[XSI].WINDOW.wWidth
        dec     bx
        dec     bx
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
        jmp     OverLine

J_PrintLine:
        mov     XBX,XSI

OverLine:

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

        cmp     al,0Dh                  ; CR/LF
        je      DoNextLine

        cmp     al,1                    ; Special space character 
        jne     DoIt1
        mov     al,' '                  ; Make it a space
DoIt1:
        stosw
        loop    PrintChar

DoNextLine:
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

        cmp     al,1                    ; Special space character 
        jne     DoIt2
        mov     al,' '                  ; Make it a space
DoIt2:

        stosw
        jmp     J_DoChar



J_ReallyDone:
        xor     ax,ax
        ret

BOX4_PRINT_LINE ENDP
;-----------------------------------------------------------------------------
END

