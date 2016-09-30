; Copyright 1993-1995 Symantec, Peter Norton Product Group
;*************************************************************************
;
; $Header:   S:\certlib4\vcs\bopen.asv   1.7   28 Mar 1995 11:31:16   DALLEE  $
;
; Description:
;
; Contains:
;
; See Also:
;*************************************************************************
; $Log:   S:\certlib4\vcs\bopen.asv  $
;  
;     Rev 1.7   28 Mar 1995 11:31:16   DALLEE
;  Added bogus int 10 call to BOX4_OPEN so TSR dialogs will pop up
;  over Windows 95 splash screen.
;*************************************************************************

include CTS4.INC

; Certus NOVI Standard
;BOX_TOP_RIGHT   EQU     184             ; Box characters
;BOX_TOP         EQU     205
;BOX_TOP_LEFT    EQU     213
;BOX_LEFT        EQU     179
;BOX_RIGHT       EQU     179
;BOX_BOT_RIGHT   EQU     190
;BOX_BOT         EQU     205
;BOX_BOT_LEFT    EQU     212

; Symantec NAV Standard
BOX_TOP_RIGHT   EQU     219             ; Box characters
BOX_TOP         EQU     219
BOX_TOP_LEFT    EQU     219
BOX_LEFT        EQU     219
BOX_RIGHT       EQU     219
BOX_BOT_RIGHT   EQU     219
BOX_BOT         EQU     219
BOX_BOT_LEFT    EQU     219

BOX_SPACE       EQU     32

SCR_WIDTH       EQU     80
SCR_HEIGTH      EQU     25

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_OPEN                            12/9/92                         CAG
;
;	Open a Window on the screen.
;
; UINT WINAPI BOX4_OPEN((DWORD) *WINDOW);
;
; Input:
;       1)  Pointer to a Window struct
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;----------------------------------------------------------------------------
BOX4_OPEN PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpWindow:LPSTR

        push    BP                      ; Make sure to clear Windows 95
        mov     ax, 1130h               ; splash screen.
        xor     bx, bx                  ; CX, DX, ES:BP set by this call.
        int     10h
        pop     BP

        LES_DI  lpWindow

        cmp     word ptr XES[XDI].WINDOW.wVideo+2,0
        jne     J_0

        INVOKE  SCR4_GET_VIDEOSEG
        or      ax,ax
        jz      J_ERROR
        mov     word ptr XES[XDI].WINDOW.wVideo+2,ax
J_0:

; Store the current video screen in buffer.
; ROW*COLUMN*SCREEN_WIDTH*2
        xor     XAX,XAX
        mov     cx,XES[XDI].WINDOW.wRow
        mov     al,SCR_WIDTH
        mul     cl
        add     ax,XES[XDI].WINDOW.wColumn
        shl     ax,1
        mov     XSI,XAX
        mov     word ptr XES[XDI].WINDOW.wVideo,ax

        LDS_SI  XES[XDI].WINDOW.wVideo
        mov     cx,XES[XDI].WINDOW.wHeigth
        mov     bx,XES[XDI].WINDOW.wWidth
        LES_DI  XES[XDI].WINDOW.wBuffer

J_1:
        push    XSI
        push    XCX
        mov     cx,bx
        rep     movsw                   ; Move from DS:SI to ES:DI, CX count
        pop     XCX
        pop     XSI
        add     XSI,160
        loop    J_1


; Display the "BOX" on the screen.
        LDS_SI  lpWindow
        mov     cx,[XSI].WINDOW.wHeigth
        mov     bx,[XSI].WINDOW.wWidth
        mov     ah,[XSI].WINDOW.wAttrib
        and     ah,11110111b            ; No high intensity
        LES_DI  [XSI].WINDOW.wVideo

; Top line
        push    XCX
        push    XDI
        mov     cx,bx                   ; Length of line
        mov     al,BOX_TOP_LEFT
        stosw
        dec     cx                      ; Less left and right corner
        dec     cx
        mov     al,BOX_TOP
        rep     stosw
        mov     al,BOX_TOP_RIGHT
        stosw
        pop     XDI
        pop     XCX
        add     XDI,160

        dec     cx                      ; Less Top and Bottom lines
        dec     cx

; Middle lines
J_2:
        push    XCX
        push    XDI
        mov     cx,bx
        mov     al,BOX_LEFT
        stosw
        dec     cx                      ; Less left and right sides
        dec     cx
        mov     al,BOX_SPACE
        rep     stosw
        mov     al,BOX_RIGHT
        stosw
        pop     XDI
        pop     XCX
        add     XDI,160
        loop    J_2
        
; Bottom line
        mov     cx,bx                   ; Length of line
        mov     al,BOX_BOT_LEFT
        stosw
        dec     cx                      ; Less left and right corner
        dec     cx
        mov     al,BOX_BOT
        rep     stosw
        mov     al,BOX_BOT_RIGHT
        stosw

        xor     ax,ax
        jmp     J_3

J_ERROR:
        mov     ax,1                    ; Error = 1
J_3:
        ret

BOX4_OPEN      ENDP
;-----------------------------------------------------------------------------
END
