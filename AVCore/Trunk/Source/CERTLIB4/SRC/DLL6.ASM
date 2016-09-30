include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_FREELIBRARY_ARRAY               1/8/93                          CAG
;
;       Free an array of librarys previously allocated by LoadLibrary
;
; UINT WINAPI DLL4_FREELIBRARY_ARRAY(LPSTR ArrayOfHandles);
;
; Input:
;       1)  Handles of librarys to free
; Output:
;       1)  ERROR (?)
;       2)  PASSED (0)
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
DLL4_FREELIBRARY_ARRAY PROC USES SDS XSI,
	lpArray:LPSTR

; Gotta free what we loaded.
	LDS_SI	lpArray
MoreFree:
	cmp	[XSI],word ptr 0
	je	ReallyDone

	INVOKE	DLL4_FREELIBRARY,[XSI]
        mov     [XSI],word ptr 0
	inc	XSI
	inc	XSI
	jmp	MoreFree

ReallyDone:
	ret

DLL4_FREELIBRARY_ARRAY ENDP
;-----------------------------------------------------------------------------
END
