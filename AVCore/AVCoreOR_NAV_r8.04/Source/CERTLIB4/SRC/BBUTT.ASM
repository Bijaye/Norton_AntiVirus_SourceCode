include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
GetChar PROC NEAR

        lodsb
        dec     cx
        ret

GetChar ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; BOX4_BUTTON                          12/9/92                         CAG
;
;	Display the buttons in windows,and get response.
;
; UINT WINAPI BOX4_BUTTON(WINDOW FAR *,LPSTR Text,WORD Seconds, WORD Def,
;                         BYTE Attribute);
;
; Input:
;       1)  Pointer to Window structure
;       2)  Pointer to button text
;               RETURNVALUE,STRING  '0<CAN&CEL>1<&OK>',0
;       3)  Time delay, -1 unlimited
;       4)  Default key if timed out, or ENTER pressed
;       5)  Attribute of ENTER button, FOREGROUND ONLY!
; Output:
;       1)  Return value from button
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_BUTTON  PROC USES XBX XCX SDS SES XSI XDI,
        lpWindow:LPSTR,
        Message:LPSTR,
        uSeconds:WORD,
        uDefault:WORD,
        bAttrib:BYTE

        LOCAL   LastLine:WORD
        LOCAL   CheckW:WORD
        LOCAL   Highlight:BYTE

        mov     Highlight,FALSE
        mov     CheckW,0

        LDS_SI  Message
LookMoreForIt:
        cmp     [XSI],byte ptr 0        ; Get actual length
        je      IsEndOfIt
        cmp     [XSI],byte ptr 1
        je      AddMeTo
        cmp     [XSI],byte ptr '['
        jne     NotABracket
AddMeTo:
        add     CheckW,word ptr 1
NotABracket:
        inc     XSI
        jmp     LookMoreForIt
IsEndOfIt:

        LDS_SI  lpWindow
        mov     ax,[XSI].WINDOW.wWidth
        dec     ax
        dec     ax                      ; Minus borders
        add     CheckW,ax

; Display buttons on bottom line
; Caculate where to start storing
        LES_DI  [XSI].WINDOW.wVideo
        mov     ax,[XSI].WINDOW.wHeigth
        dec     ax                      ; Put on last line
        dec     ax                      ; Put on line above border
        mov     cl,160
        mul     cl
        inc     ax
        inc     ax
        add     di,ax
        add     di,2                    ; Over two spaces

        INVOKE  STR4_LENGTH,Message     ; Get length of buttons
        mov     cx,ax
        cmp     cx,CheckW
        jl      WereOk

        mov     LastLine,di

        sub     di,160                  ; Less one line

DoItAgain:
        INVOKE  STR4_LENGTH_TO_LAST,Message,''
        cmp     ax,CheckW
        jl      FinallyOk
        LDS_SI  Message
        add     si,ax
        mov     [XSI],byte ptr ''      ; Change it to a space command
        LDS_SI  lpWindow
        jmp     DoItAgain

FinallyOk:
        mov     cx,ax                   ; Length of first line!
WereOk:
        mov     ah,[XSI].WINDOW.wAttrib
        mov     bh,ah                   ; Store Attibute in BH


        LDS_SI  Message
NextButton:
        call    GetChar                 ; Get NUMBER of button
        or      al,al
        jz      EndOfMessage

        xor     ah,ah
        sub     al,30h                  ; Convert to a "number"
        cmp     ax,uDefault
        jne     NotHighlightIt
        mov     Highlight,TRUE
        jmp     KeepItGoing
NotHighlightIt:
        mov     Highlight,FALSE


KeepItGoing:
        lodsb                           ; Print out button [xxxxx]
        or      al,al
        jz      EndOfMessage

        mov     ah,bh                   ; Restore screen attribute

        cmp     al,'['
        je      OpenBracket
        cmp     al,']'
        je      CloseBracket
        cmp     al,'&'
        je      ValuedChar

;        or      al,20h                  ; Make it LowerCase
        jmp     StoreOnScreen

ValuedChar:
        call    GetChar
        dec     cx
;        and     al,5Fh                  ; Make it UpperCase
        and     ah,11110000b            ; Strip foreground
        or      ah,bAttrib              ; Add new foreground
        
OpenBracket:
        cmp     Highlight,FALSE
        je      StoreOnScreen
        and     ah,11110000b            ; Strip foreground
        or      ah,bAttrib              ; Add new foreground

StoreOnScreen:
        stosw
        loop    KeepItGoing
        jmp     NextLine

CloseBracket:
        dec     cx
        cmp     Highlight,FALSE
        je      AfterText
        and     ah,11110000b            ; Strip foreground
        or      ah,bAttrib              ; Add new foreground
AfterText:
        stosw                           ; Print out text after button til  or 
        lodsb
        mov     ah,bh                   ; Restore screen attribute
        or      al,al
        jz      EndOfMessage
        cmp     al,1                    ; End of this button
        je      NextButton
        cmp     al,2
        je      NextButton
        loop    AfterText

NextLine:
        cmp     [si],byte ptr 0
        je      EndOfMessage
        cmp     [si],byte ptr 1
        je      DoNextLine
        cmp     [si],byte ptr 2
        je      DoNextLine
        inc     si
        jmp     NextLine
DoNextLine:
        inc     si
        mov     di,LastLine             ; Place buttons on last line
        jmp     NextButton

EndOfMessage:

EndOfLine:

FlushKeys:
        mov     ah,1h                   ; Get keyboard status
        int     16h
        jz      NoKeyYet
        mov     ah,0h
        int     16h                     ; Clear the keyboard buffer
        jmp     FlushKeys
NoKeyYet:

        INVOKE  KEY4_GET_DELAY,uSeconds
        or      ax,ax
        jz      TimedOut

        cmp     al,0Dh                  ; Enter key pressed
        je      TimedOut                ; Do default key

        LDS_SI  Message

;        and     al,5Fh                  ; Make key uppercase
; BYTE WINAPI CHR4_TO_ALT_CASE(BYTE bChar,BYTE fCase);

        INVOKE  CHR4_TO_ALT_CASE,al,CHR_UPPER

LookMore:
        cmp     [XSI],byte ptr 0
        je      EndOfLine

        cmp     [XSI],byte ptr '['      ; Start of BUTTON, prev was return
        jne     NotReturn
        mov     bl,[XSI-1]
        sub     bl,30h                  ; Place return value in BL
NotReturn:
        cmp     [XSI],byte ptr '&'      ; Next char is key symbol
        jne     LookNext
        inc     XSI
        mov     ah,[XSI]
        or      ah,ah
        jz      EndOfLine
        and     ah,5Fh
        cmp     al,ah
        je      GotIt
LookNext:
        inc     XSI
        jmp     LookMore
GotIt:
        xor     bh,bh
        mov     uDefault,bx
TimedOut:
        mov     ax,uDefault
        ret

BOX4_BUTTON    ENDP
;-----------------------------------------------------------------------------
END
