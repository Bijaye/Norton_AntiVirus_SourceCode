include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_LOADLIBRARY_ARRAY                1/8/93                          CAG
;
;       Load an array of DLL librarys for DOS to use.
;
; WORD WINAPI DLL4_LOADLIBRARY_ARRAY
;               (WORD FAR *Array, LPSTR Libname, LPSTR Buffer,BYTE Type);
;
; Input:
;	1)  Storage of 102 words for libraries to be loaded.
;       2)  Name of library to load
;	3)  128 byte buffer to work with.
;       4)  Type of a library to load (First char in extension)
; Output:
;       1)  Array of Handles to this library to work with, filled in, 33.
;       2)  ERROR <32
;               1 = File not found
;               2 = Not enough memory to load
;               3 = Invalid format of a DLL
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
; Note:  For DOS this function will search for files with the same
;	 first name, but with extension of Y?? where ?? is a number
;	 from 00 to 99.
;
;-----------------------------------------------------------------------------
DLL4_LOADLIBRARY_ARRAY PROC USES SDS XSI,
	lpArray:LPSTR,
	lpLibName:LPSTR,
	lpBuffer:LPSTR,
        byType:BYTE

	LOCAL 	lpNumber:DWORD
	LOCAL 	uLastLook:WORD
	LOCAL	lpNewArray:DWORD
	LOCAL	uStatus:WORD

	mov	uLastLook,3939h		; Put '99' in uLastLook
	mov	ax,word ptr lpArray
	mov	word ptr lpNewArray,ax
	mov	ax,word ptr lpArray+2
	mov	word ptr lpNewArray+2,ax

; We need to load the number libraries first so later when we getprocaddress
; we'll get the newest routine.
	INVOKE	STR4_COPY,lpLibName,lpBuffer
	INVOKE	MEM4_SRCH_CHAR,lpBuffer,'.',128
	inc	ax			; Point past .
	mov	word ptr lpNumber,ax
	mov	ax,word ptr lpBuffer+2
	mov	word ptr lpNumber+2,ax

LoadMore:
	LDS_SI	lpNumber
        mov     al,byType
	mov	[XSI],al
	mov	ax,uLastLook
	mov	[XSI+1],ax

; Use FIL4_ATTR,GET to determine if name is here.
	INVOKE	FIL4_ATTR,lpBuffer,0,GET_INFO
	cmp	ax,-1
	je	NotThisOne

	INVOKE	DLL4_LOADLIBRARY,lpBuffer
	cmp	ax,32
	jle	IsError

	LDS_SI	lpNewArray
	mov	[XSI],ax
	mov	[XSI+2],word ptr 0
        add     word ptr lpNewArray,2

NotThisOne:
	mov	ax,uLastLook
	dec	al
	cmp	al,2Fh			; Below '0'?
	jne	GotNext
	dec	ah
	cmp	ah,2Fh
	je	J_Done
	mov	al,39h			; Put '9' back in AL
GotNext:
	mov	uLastLook,ax
	jmp	LoadMore
J_Done:
        INVOKE  DLL4_LOADLIBRARY,lpLibName
        cmp     ax,32
        jle     IsError
        LDS_SI  lpNewArray
        mov     [XSI],ax
        mov     [XSI+2],word ptr 0
	mov	uStatus,33h
	jmp	ReallyDone
IsError:
	mov	uStatus,ax
; Gotta free what we loaded.
	LDS_SI	lpArray
MoreFree:
	cmp	[XSI],word ptr 0
	je	ReallyDone

	INVOKE	DLL4_FREELIBRARY,ax
	inc	XSI
	inc	XSI
	jmp	MoreFree

ReallyDone:
	mov	ax,uStatus


	ret

DLL4_LOADLIBRARY_ARRAY ENDP
;-----------------------------------------------------------------------------
END

