include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; BOX4_TITLE                         5/12/92                         CAG
;
;	Print title onto the box.
;
; UINT WINAPI BOX4_TITLE(WINDOW FAR *lpWindow,LPSTR Text,BYTE Attribute);
;
; Input:
;       1)  Pointer to Window struct
;       2)  Text to print
;       3)  Attribute of Title
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
BOX4_TITLE PROC USES XCX SDS SES XSI XDI,
        lpWindow:LPSTR,
        lpMessage:LPSTR,
        byAttr:BYTE
        
        LDS_SI  lpWindow
        LES_DI  [XSI].WINDOW.wVideo

; ES:DI = Start of where BOX is, need to center title, then print.
        INVOKE  STR4_LENGTH,lpMessage
        mov     cx,[XSI].WINDOW.wWidth
        sub     cx,ax

        add     di,cx                   ; New location of title, centered
        shr     di,1
        shl     di,1                    ; Convert to even number
        mov     ah,byAttr

        LDS_SI  lpMessage
PrintChar:
        lodsb
        or      al,al
        jz      J_Done
        stosw
        jmp     PrintChar
J_Done:
        ret

BOX4_TITLE ENDP
;-----------------------------------------------------------------------------
END


