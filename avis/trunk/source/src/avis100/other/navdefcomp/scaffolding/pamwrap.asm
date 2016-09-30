include autoverv.dcl

WC MACRO CHAR
	push ax
	push dx
	mov ah,02h
	mov dl, CHAR
	int 21h
	pop dx
	pop ax
	ENDM



_TEXT	segment para public 'CODE'
_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
_TEXT	segment para public 'CODE'
   ;
   ;	int PAM(int HANDLE) {
   ;
	assume	cs:_TEXT

_PAM	proc	near
	push	bp
	mov	bp,sp

        push    di                      ;save caller's di
        push    si                      ;save caller's si
        push    ds                      ;save caller's ds
        push    es                      ;save caller's es

;- PAM/ERS simulation --------------------------------------------------
OPTION LJMP

   ;ds = cs
	mov ax, cs
	mov ds, ax

   ;save the handle locally (not done in PAM)
	mov bx,[bp+4]   ;+4 if near, +6 if far
	mov HANDLE,bx

   ;load the header   ?? will be done by PAM; Hdr EQ 0FF00h
	xor  cx, cx
	xor  dx, dx
	mov  bx, HANDLE
	mov  ax, 4200h
	int  21h	;seek to start
	jc   Fail

	mov dx, OFFSET Hdr
	mov cx, HDRLEN
	mov bx, HANDLE
	mov ah, 3Fh
	int 21h
	jc  Fail

   ;get the infected file length   ?? in PAM, value arrives in dx.ax
	xor  cx, cx
	xor  dx, dx
	mov  bx, HANDLE
	mov  ax, 4202h
	int  21h
	jc   Fail

	push dx
	push ax
	xor  cx, cx
	xor  dx, dx
	mov  bx, HANDLE
	mov  ax, 4200h
	int  21h
	jc   Fail
	pop  ax
	pop  dx

;-----------------------------------------------------------------------

   ; set up the parameters
	mov	bx, offset Instructions
	mov	cx, offset InstrEnd

   ; call the autoverv routine
	jmp	Autoverv

Autoverv_Exit:
	jnc  Success

	IFNDEF NODUMP
	   mov ax, DumpCode
	ELSE
	   mov ax, 0FFFFh
	ENDIF
	jmp Exit

Success:
	xor ax, ax

Exit:
	mov bx, ax
	xor cx, cx
	xor dx, dx
;-----------------------------------------------------------------------
        pop     es                      ;restore caller's es
        pop     ds                      ;restore caller's ds
        pop     si                      ;restore caller's si
        pop     di                      ;restore caller's di

	mov	sp,bp
	pop	bp
	ret

;=======================================================================
Instructions:
	INCLUDE autoverv.ers
InstrEnd:

HANDLE	DW 0

INCLUDE autoverv.inc
;=======================================================================

Hdr	DB HDRLEN dup(?)

_PAM	endp
_TEXT	ends
_DATA	segment word public 'DATA'
s@	label	byte
_DATA	ends
_TEXT	segment para public 'CODE'
_TEXT	ends
	public	_PAM
_s@	equ	s@
	end
