include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_SYSTEM_ID                    9/6/93                        bAd
;
;       Returns 16 bytes from address FD00:0000
;
; BYTE WINAPI SYS4_GET_SYSTEM_ID(LPSTR lpSysID);
;
; Input:
;       1)   Buffer to place 16 bytes of system ID.
;
; Output:
;       1)   0 on success
;       2)   Buffer containing 16 bytes.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
SYS4_GET_SYSTEM_ID PROC USES XCX XSI XDI SDS SES,
        lpSysID:LPSTR


ifndef WIN16

; QEMM in installed...
; QEMM must be version 6.0 or greater
        INVOKE  SYS4_QEMM_VERSION
        cmp     ax,-1
        je      GetIDBytes
        cmp     ax, 0600h
        jb      GetIDBytes

; QEMM is version 6.0 or greater...
; Stealth mode cannot be 0
        INVOKE  SYS4_QEMM_STEALTH_MODE
        cmp     ax,-1
        je      GetIDBytes

; handle the qemm stuff here
; copy 16 bytes from the stealth rom area
        mov     ax, 0fd00h
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL,ax
endif
        mov     SDS, ax
        xor     XSI, XSI
        INVOKE  SYS4_QEMM_COPY_STEALTH_ROM,lpSysID,vSDS XDI,16

        cmp     ax,-1
        je      GetIdBytes

        xor     ax, ax
        jmp     Done

GetIDBytes:

endif

; just copy the bytes from FD00:0000 to lpSysID
        LES_DI  lpSysID
        mov     ax, 0fd00h
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL,ax
endif
        mov     SDS, ax
        xor     XSI, XSI
        LES_DI  lpSysID
        mov     cx, 16
        cld
        rep     movsb

        xor     ax, ax
Done:
        ret

SYS4_GET_SYSTEM_ID ENDP
;----------------------------------------------------------------------------
END
