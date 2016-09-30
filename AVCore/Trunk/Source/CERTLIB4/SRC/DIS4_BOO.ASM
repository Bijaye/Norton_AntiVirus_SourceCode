include CTS4.INC
include REPAIR.INC

UniqueSegment
;---------------------------------------------------------------------------
; DIS4_MOV_BOOT                    1/5/93                              JOE
;
;       Copies original Boot from where virus stored it to
;       logical sector using WIN_INT25 and WIN_INT26.
;
; UINT DIS4_MOV_LOG(WORD wLogicalSector, WORD wLogicalDriveNumber, 
;                    WORD wVerifyFlag, LPSTR lpBuffer);
;
; Input:
;        1) Logical sector of stored original boot
;        2) Logical Drive Number
;        3) Verify flag (Non-Zero checks for to verify a boot sector)
;        4) ptr to 512 byte buffer
;
; Output:
;        1) FAILURE (0)
;        2) SUCCESS (1)
;--------------------------------------------------------------------------
DIS4_MOV_BOOT   PROC    USES XBX XCX XDX XES,
                wLogicalSector:WORD,   
                wLogicalDriveNumber:WORD,    
                wVerifyFlag:WORD,
                lpBuffer:LPSTR

                LOCAL uDrive:BYTE
                LOCAL dwSector:DWORD
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        xor     XAX,XAX
        xor     XBX,XBX
        xor     XCX,XCX
        xor     XDX,XDX

        mov     ax,wLogicalDriveNumber
        mov     byte ptr uDrive,al
        mov     ax,wLogicalSector
        mov     word ptr dwSector+2,ax
        mov     word ptr dwSector,0
        mov     cx,1

        INVOKE  DIS4_READ_LOGICAL,lpBuffer,uDrive,dwSector,cx
        or      ax,ax
        jz      ERR
        
        cmp     VerifyFlag,0
        je      @F

        MOV_ES  ax
        cmp     byte ptr XEX[XBX],0EBh
        je      @F
        cmp     byte ptr XEX[XBX],0E9h
        jne     ERR
@@:
        mov     ax,wLogicalDriveNumber
        mov     byte ptr uDrive,al
        xor     ax,ax
        mov     word ptr dwSector+2,ax
        mov     cx,1

        INVOKE  DIS4_WRITE_LOGICAL,lpBuffer,uDrive,dwSector,cx
        or      ax,ax
        jz      ERR
        mov     ax,1
        jmp     OK
ERR:
        xor     ax,ax                   ; Return 0 for error
OK:
        ret

DIS4_MOV_BOOT    ENDP
;-----------------------------------------------------------------------------
END

