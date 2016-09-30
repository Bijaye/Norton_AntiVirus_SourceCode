include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_HANDLE_TO_NAME                  12/29/92                        CAG
;
;       Get the name of a file from the given handle.
;
; BOOL WINAPI FIL4_HANDLE_TO_NAME(LPSTR Filename,UINT Handle,WORD PSP);
;
; Input:
;       1)  Filename buffer of 128 bytes.
;       2)  Handle to search
;       3)  PSP of program to get handle from
; Output:
;       1)  TRUE(1) Passed, Filename filled in.
;           FALSE(0) Failed.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
FIL4_HANDLE_TO_NAME PROC USES XBX XCX XDX SDS SES XSI XDI,
        Filename:LPSTR,
        Handle:UINT,
        PSP:WORD

        LOCAL   lpHANDLE:DWORD          ; Pointer to HANDLE TABLE in PSP
        LOCAL   uJFT:UINT               ; Size of JFT
        LOCAL   lpSFT:DWORD             ; Pointer to System File Table

        mov     ax,PSP
        MOV_ES  ax
        mov     XDI,34h
        LDS_SI  XES[XDI]                ; Get HANDLE TABLE pointer

if OS_16
        mov     word ptr lpHANDLE+2,SDS
        mov     word ptr lpHANDLE,XSI
endif
if OS_32
        mov     lpHANDLE,XSI
endif

        mov     ah,30h
        int     21h                     ; Get version of DOS
        mov     uJFT,35h                ; Size of JFT for DOS 3.x
        cmp     al,3
        je      J_10
        mov     uJFT,3Bh                ; Size of JFT for DOS >= 4.x
J_10:
        mov     ah,52h
        int     21h                     ; Get list of lists
        LDS_SI  XES[XBX+4]

if OS_16
        mov     word ptr lpSFT+2,SDS
        mov     word ptr lpSFT,XSI
endif
if OS_32
        mov     lpSFT,XSI
endif

        LES_DI  lpHandle
        add     XDI,Handle
        xor     ax,ax
        mov     al,XES[XDI]             ; AX = Index into SFT

        LES_DI  lpSFT
J_4:
        cmp     XES[XDI],word ptr -1
        je      J_ERROR

        cmp     XES[XDI+4],ax
        jb      J_1

        add     XDI,6                   ; ES:DI=Entry
        inc     ax
J_3:
        dec     ax
        jz      J_2
        add     XDI,uJFT
        jmp     J_3
J_2:
        SMOV    ds,es
        mov     XSI,XDI                 ; DS:SI Name of file
        add     XSI,20h

        LES_DI  Filename
        mov     cx,8
J_6:
        lodsb                           ; Get from DS:SI
        cmp     al,' '
        je      J_5
        stosb
        loop    J_6
J_5:
        mov     al,'.'
        stosb

        mov     cx,2
J_9:
        lodsb
        cmp     al,' '
        je      J_9
        stosb
J_8:
        lodsb
        cmp     al,' '
        je      J_7
        stosb
        loop    J_8
J_7:
        xor     al,al
        stosb
        jmp     J_0

J_1:
        sub     ax,XES[XDI+4]
        LES_DI  XES[XDI]
        jmp     J_4
       
J_ERROR:
        mov     ax,FALSE
        jmp     J_Done
J_0:
        mov     ax,TRUE
J_Done:

        ret

FIL4_HANDLE_TO_NAME ENDP
;-----------------------------------------------------------------------------
END

