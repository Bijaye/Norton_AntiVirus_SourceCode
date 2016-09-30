; writes the loaded file image to the output file.

PAMDUMP:
   ;ds = cs
	mov bx, cs
	mov ds, bx

   ;save the host length
	mov  cs:FileLength,   ax
	mov  cs:FileLength+2, dx

   ;go to the start of the file
	xor cx, cx
	xor dx, dx
	mov ax, 4200h
	int 21h
	jc  Fail

   ;destroy the signature, so it appears to have been repaired
        mov dx, offset MZcover
	mov cx, 2h
	mov ah, 40h
	int 21h
	jc  Fail

   ;get the header length in cx.dx
	xor cx, cx
	mov dx, 8
	mov ax, 4200h
	int 21h
	jc  Fail

        mov dx, offset HdrPara
	mov cx, 2h
	mov ah, 3Fh
	int 21h
	jc  Fail

        mov dx, cs:HdrPara
  	mov ax, cs:HdrPara+2
	mov cx, 4
HLx16:
        shl dx, 1
        rcl ax, 1
	loop HLx16 
        mov cx, ax

   ;go to end of header
	mov ax, 4200h
	int 21h
	jc  Fail

   ;point to the loaded image
	mov dx, 0800h

writeloop:		;dx contains the paragraph offset to write from
	mov ds, dx
	xor dx, dx
	mov cx, 1000h
  ;MOV AX,DS
  ;MOV CS:TMP_DS,AX
  ;MOV AX,CS
  ;MOV DS,AX
  ;MOV DX, OFFSET DUMMY_STR
	mov ah, 40h
	int 21h
	jc  Fail
	cmp ax,cx
	jne Fail
	mov dx, ds
  ;MOV DX,cs:TMP_DS
	add dx, 100h
	cmp dx, 0A000h
	jne writeloop

   ;truncate
	mov dx, cs:FileLength
	mov cx, cs:FileLength+2
	mov ax, 4200h
	int 21h
	jc  Fail
	mov cx,0
	mov ah, 40h
        int 21h
        jc  Fail
	jmp OK

FileLength	dw ?

MZcover		dw 0
HdrPara		dw 0

TMP_DS		dw ?
DUMMY_STR	db 33h
		db 0FFFh dup(77h)
