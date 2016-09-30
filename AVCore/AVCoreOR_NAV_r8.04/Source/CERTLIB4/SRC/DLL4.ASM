include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_GETPROCADDRESS_ARRAY              1/9/93                          CAG
;
;       Return the address of a procedure loaded from an array of DLLs.
;
; DWORD WINAPI DLL4_GETPROCADDRESS_ARRAY(WORD FAR *LibHandles,
;			LPSTR Name,WORD MemHandle);
;
; Input:
;       1)  An array of Library handles to load from,NULL Terminated.
;       2)  Name of function to load (or ordial value)
;       3)  Memory handle to load to
; Output:
;       1)  FAR Pointer to code (AX:DX)
;       2)  ERROR <32
;               1 = Not found RESIDENT_Name table
;               2 = Not a movable segment, must be fixed
;               3 = Unable to load the segment
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
DLL4_GETPROCADDRESS_ARRAY PROC USES XSI SDS,
	lpLibHandles:LPSTR,
	lpName:LPSTR,
	uMemHandle:WORD
	
	LDS_SI	lpLibHandles
TryMore:
	INVOKE	DLL4_GETPROCADDRESS,[XSI],lpName,uMemHandle
	cmp	ax,32
	jg	FoundIt
	inc	XSI
	inc	XSI
	cmp	[XSI],word ptr 0
	jne	TryMore

FoundIt:
	
	ret

DLL4_GETPROCADDRESS_ARRAY ENDP
;-----------------------------------------------------------------------------
END


