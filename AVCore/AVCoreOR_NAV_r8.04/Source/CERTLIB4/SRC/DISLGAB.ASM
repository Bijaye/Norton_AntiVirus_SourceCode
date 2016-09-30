include CTS4.INC

Int13Record STRUC
    generated		db	?	; Was Int 13h generated?
    driveNum		db	?	; Drive number
    startHead		db	?	; Starting head
    startTrack		dw	?	; Starting track
    startSector		db	?	; Starting sector
    trigger		db	?	; Time to take special int 13h action?
Int13Record ENDS

BIOSINSTALLINT13HANDLER         PROTO FAR PASCAL,:LPSTR
BIOSUNINSTALLINT13HANDLER       PROTO FAR PASCAL

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOG_TO_ABS             6/10/93                          bAd
;
;       Convert a LOGICAL SECTOR NUMBER to a ABSOLUTE SECTOR NUMBER
;
; DWORD WINAPI DIS4_LOG_TO_ABS(DWORD Sector,BYTE Drive,LPSTR lpBuffer);
;
; Input:
;       1)  Long sector number 
;       2)  Drive to caculate on (80h or 81h for fixed, 00,01...for floppy)
;       3)  Buffer of at least 512 bytes.
; Output:
;       1)  ERROR (0)
;       2)  HIWORD (DX)         = Cylinder 
;           LOWORD(HIBYTE) (AH) = Head
;           LOWORD(LOBYTE) (AL) = Sector
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; This procedure hooks INT 13h and does an INT 25h call.
;
;-----------------------------------------------------------------------------
ifdef WIN16
else
Hooked13 LABEL  WORD

        cli                             ; disable hardware interrupts
        push    ax
        push    cx

; get side
        mov     bSide, dh

; get sector
        mov     ax, cx
        and     al, 00111111b
        mov     bSector, al

; get low byte of cylinder
        mov     byte ptr uCylinder, ch

; get high byte of cylinder
        mov     ax, cx
        mov     cl, 6
        shr     al, cl
        mov     byte ptr uCylinder+1, al

        pop     cx
        pop     ax
        sti                             ; enable hardware interrupts
        iret
;---------------------------------------------------------------------------

uCylinder       WORD    0
bSector         BYTE    0
bSide           BYTE    0

endif
;---------------------------------------------------------------------------
DIS4_LOG_TO_ABS PROC USES XBX XCX XSI XDI SDS SES,
        Sector:DWORD,
        Drive:BYTE,
        lpBuffer:LPSTR

        LOCAL   bAltDrive:BYTE
ifdef WIN16
        LOCAL   str13Rec:Int13Record
else
        LOCAL   dwOld13:DWORD
        LOCAL   dwMy13:DWORD
endif

        mov     al, Drive
        test    al, 80h                 ; HDD or FDD?
        jz      J_0
        and     al, 0111111b
        inc     al
        inc     al
        mov     bAltDrive, al
J_0:
; hook int 13h
ifdef WIN16
IFDEF DEBUG
        int 3h
ENDIF
;        INVOKE  BIOSInstallINT13Handler,ADDR str13Rec
else
        INVOKE  SYS4_VECTOR,13h,ADDR Hooked13,ADDR dwOld13
; call int 25h and get cyl, sec, side
endif

        INVOKE  DIS4_READ_LOGICAL,lpBuffer,bAltDrive,Sector,1

; unhook int 13
ifdef WIN16
;        INVOKE  BIOSUninstallINT13Handler

; fill AX and DX with cyl,sec,side
        mov     dx, str13Rec.startTrack
        mov     ah, str13Rec.startHead
        mov     al, str13Rec.startSector
else
        INVOKE  SYS4_VECTOR,13h,dwOld13,ADDR dwMy13

; fill AX and DX with cyl,sec,side
        mov     dx, uCylinder
        mov     ah, bSide
        mov     al, bSector
endif
        jmp     Done

Err:
        mov     ax, -1
        mov     dx, ax
Done:
        ret

DIS4_LOG_TO_ABS ENDP
;-----------------------------------------------------------------------------
END
