include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_LOAD_EXTERN                     1/27/93                         CAG
;
;       Load an entire array of external messages from a CMB file.
;
; WORD WINAPI CMB4_LOAD_EXTERN(LPSTR Filename, FAR *EXTERN_MESSAGE);
;
; Input:
;       1)  Filename to open
;       2)  Array of EXTERN_MESSAGE struct.
; Output:
;       1)  Number of messages loaded
;       2)  ERROR (-1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_LOAD_EXTERN PROC USES XBX XCX SDS XSI,
        lpFilename:LPSTR,
        lpExternMess:LPSTR

        INVOKE  CMB4_OPEN,lpFilename
        or      ax,ax
        jz      J_NoCMB
        mov     bx,ax
        LDS_SI  lpExternMess
MoreToDo:
        cmp     [XSI],byte ptr 0
        jz      NoGo1
        inc     XCX
        INVOKE  CMB4_READ,byte ptr [XSI].EXTERN_MESSAGE.MessageID,bx,
                [XSI].EXTERN_MESSAGE.MessageTEXT,
                [XSI].EXTERN_MESSAGE.MessageSIZE
        cmp     ax,-1
        je      J_ErrorOut
        add     XSI,sEXTERN_MESSAGE
        jmp     MoreToDO
J_ErrorOut:
        INVOKE  CMB4_CLOSE,bx
        jmp     J_NoCMB

NoGo1:
        INVOKE  CMB4_CLOSE,bx

        mov     XAX,XCX
        jmp     J_Done

J_NoCMB:
        mov     ax,-1
J_Done:
        ret

CMB4_LOAD_EXTERN ENDP
;-----------------------------------------------------------------------------
END
