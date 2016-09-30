include CTS4.INC
include REPAIR.INC
UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_MOV_MBR                    1/5/93                              JOE
;
;       Copies MBR from where the virus stored it to Absolute sector
;       one using WIN_READSECTOR and WIN_WRITESECTOR
;
; UINT DIS4_MOV_ABS(BYTE uCylinder, BYTE uSector,BYTE uHead, BYTE uDrive,
;                   WORD Verify,LPSTR lpBuffer)
; Input:
;        1) Cylinder of stored original MBR
;        2) Sector of stored original MBR
;        3) Head of stored original MBR 
;        4) Drive of stored original MBR
;        5) Verify flag (Non-zero checks for 55 AA in original sector)
;        6) ptr to 512 byte buffer
;
; Output:
;        1) FAILURE (0)
;        2) SUCCESS (1)
;
;-----------------------------------------------------------------------------
DIS4_MOV_MBR    PROC USES XBX XCX XDX XES,
        uCylinder:BYTE,
        uSector:BYTE, 
        uHead:BYTE,
        uDrive:BYTE,    
        Verify:BYTE,   
        lpBuffer:LPSTR
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        xor     XBX,XBX
        xor     XCX,XCX
        xor     XDX,XDX

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uCylinder,uSector,uHead,uDrive 
        jc      ERR

        cmp     Verify,0
        je      @F

        MOV_ES  ax
        cmp     word ptr XES[XBX+510],0AA55h
        jne     ERR
@@:
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,uDrive 
        jc      ERR                                   

        mov     ax,1
        clc
        jmp     short OK
ERR:
        xor     XAX,XAX                       ; Return 0 for error
        stc
OK:
        ret

DIS4_MOV_MBR  ENDP
;-----------------------------------------------------------------------------
END

