include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_MODIFY_BOOTCODE                  3/5/93                          bAd
;
;      Returns our version of the master boot record.
;
; DWORD WINAPI DIS4_MODIFY_BOOTCODE(BYTE Drive,WORD Cyl,BYTE Side,Byte Sector,
;                                      DWORD LogFATSector,WORD OffsetFAT,LPSTR mbrBuffer);
;
; Input:
;       1)  Bootable drive (80h or 81h)
;       2)  Cyl of real MBR
;       3)  Side of real MBR
;       4)  Sector of real MBR
;       5)  Logical Sector Number in FAT where cluster marked bad, or
;               0 if FAT unaltered        
;       6)  Offset (in bytes) in the FAT sector
;       7)  Buffer to contain modified bootcode     
; Output:
;       1)  Address of mbrBuffer
;       2)  Buffer containing modified bootcode     
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_MODIFY_BOOTCODE PROC USES XBX XCX XSI XDI SDS SES,
        Drive:BYTE,
        Cyl:WORD,
        Side:BYTE,
        Sector:BYTE,
        LogFATSector:DWORD,
        OffsetFAT:WORD,
        mbrBuffer:LPSTR
        
;----------------------------
; move our CODE int 13h handler to
; the buffer so that we may
; work on it there
;
        mov     ax,seg BootLock         ; DS:SI -> _bootcode
        mov     SDS,ax
        xor     XSI,XSI
        

        mov     ax, word ptr mbrBuffer+2 ; ES:DI -> buffer to hold
        mov     SES, ax                  ;  our version of MBR
        mov     XDI, word ptr mbrBuffer 

        cld
        mov     cx,512
        rep     movsb                   ; perform the copy

        mov     dx, word ptr mbrBuffer+2 ; DX:AX -> pointer to our
        mov     SDS, dx                  ; modified bootcode
        mov     XSI, word ptr mbrBuffer
        mov     ax, XSI
     
; make the modifications
        add     XSI,offset PartTable - OffCylSec  ; point to first data item

        mov     cl,Side
        mov     byte ptr [XSI+3],cl
        mov     cx,Cyl
        xchg    ch,cl
        shl     cl,1
        shl     cl,1
        shl     cl,1
        shl     cl,1
        shl     cl,1
        shl     cl,1
        or      cl,Sector
        mov     word ptr [XSI+0],cx

; if the FAT was altered, write the sector and offset info to the bootcode
        cmp     word ptr LogFATSector+2, 0
        jnz     J_1
        cmp     word ptr LogFATSector, 0
        jnz     J_1
        jmp     Done
J_1:
        mov     XSI, word ptr mbrBuffer
        add     XSI, offset PartTable - OffLSNFat
        mov     bx, word ptr LogFATSector+2
        mov     word ptr [XSI+2], bx
        mov     bx, word ptr LogFATSector
        mov     word ptr [XSI], bx
        mov     bx, OffsetFAT
        mov     word ptr [XSI+4], bx
Done:
        ret
DIS4_MODIFY_BOOTCODE ENDP
;-----------------------------------------------------------------------------
BootLock        Segment para    private 'CODE'
_bootcode	proc	far
        org     0
        jmp	around_mess

cright 	db	'Copyright (C) Symantec 1993.'

around_mess:
        push	ds		        ;Save original regs
	push	es
	push	XSI
	push	XDI
	push	dx
	push	cx

	cli
	xor	ax,ax                   ; point DS and ES to segment 0
	mov	ds,ax
	mov	es,ax
	sti

	dec	word ptr ds:413h        ; subtract 1k from avail memory

	mov	XSI,04Ch                ; int 13h offset in IVT
	mov	XDI,offset cs:PrevISR13 ; storage for real 13h
	add	XDI,global_offset
        cld
	movsw                           ; save real 13h
	movsw                           

        mov     XDI,04Ch                ; point new 13h to bootlock handler
        mov     [XDI],WORD PTR ISR13offset
        mov     [XDI +2],WORD PTR ISR13seg

	mov	ax,ISR13seg             ; copy the handler to last 1k of RAM
	mov	es,ax
	mov	XDI,00h
	mov	XSI,offset cs:ISR13
	add	XSI,global_offset
	mov	cx,sISR13
	rep	movsb

	xor	ax,ax                   ; move the rest of this code to
	mov	es,ax                   ; 0:600 in memory so we can further
	mov	XDI,600h                ; load the real MBR and boot sector
	mov	XSI,offset cs:scode
	add	XSI,global_offset
	mov	cx,sscode
	rep	movsb
                                        
	mov	bx,0600h                ; jmp to the rest of our code
	jmp	bx                      ; that we just moved

scode:
	mov	cx,0001h                ; read the real MBR
        mov     dx,0080h                ; 
	mov	ax,0201h                ; 
	mov	bx,7C00h                ; to address 0:7c00
	int	13h

	pop	cx		        ; restore original regs
	pop	dx
	pop	XDI
	pop	XSI
	pop	es
	pop	ds

	mov	bx,07C00h               ; jmp to real MBR and
	jmp	bx                      ; boot sector

sscode  EQU     $ - scode
;---------------------------------------------
ISR13:
        cmp     ah,03h
        ja      ISR13_out
        cmp     al,01h
        jne     ISR13_out
        cmp     cx,0001h
        jne     ISR13_out
        cmp     dx,0080h
        jne     ISR13_out

        db      0B9h
CylSec  dw      1234H                   ; mov cx, 1234h

        db      0B6h
Side    db      99h                     ; mov dh, 99h
        
ISR13_out:
        db      0eah
PrevISR13 dd    12345678h               ; jmp far 1234:5678h

LSNFat    dd    00000000h               ; FAT sector with cluster marked BAD
ByteOffset  dw  00000h                  ; offset into that cluster
    
;----------------------------
;  data
;
                org     PARTITION_OFFSET
OffCylSec       equ     $ - CylSec
OffLSNFat       equ     $ - LSNFat
; partition itself
PartTable       PARTENTRY        <0,1,1,1,0,1,1,1,1,65>
                PARTENTRY        <80h,1,1,1,06h,1,1,1,1,1>
                PARTENTRY        <0,0,0,0,0,0,0,0,0,0>                
                PARTENTRY        <0,0,0,0,0,0,0,0,0,0>                
; signatature word
                org     01feh
		db	055h,0AAh

sISR13          EQU     $ - ISR13

_bootcode	endp

BootLock        ends
END
