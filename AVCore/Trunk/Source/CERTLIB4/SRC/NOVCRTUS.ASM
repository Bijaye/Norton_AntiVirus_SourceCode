include         CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_CREATEUSERENTRY               5/13/93                        bAd
;
;  Writes an entry to the file.
;
; UINT WINAPI NOV_CREATEUSERENTRY(UINT bHandle,LPSTR lpszServerName,LPSTR
;				  lpszObjectName,BYTE bFlag,LPSTR lpszBuffer);
;
; Input:
;       1)  File to write entry to.
;	2)  Server (can by NULL)
;	3)  Name of user or group.
;	4)  Flag: 0 = USER, 1 = GROUP
;       5)  work buffer of at least 200 bytes.
; Output:
;       1) 
;
; LIBRARY: 
;
;-----------------------------------------------------------------------------
NOV4_CREATEUSERENTRY PROC USES XCX XSI XDI SDS SES,
        uHandle:WORD,
	lpszServerName:LPSTR,
	lpszObjectName:LPSTR,
	bFlag:BYTE,
        lpszBuffer:LPSTR

        LDS_SI  lpszBuffer
        LES_DI  lpszBuffer

        INVOKE  MEM4_SET,lpszBuffer,00h,stagNETUSERITEM

        mov     ax, word ptr lpszServerName+2
        or      ax, ax
        jnz     ServerName
        stosw   
        jmp     DoObject

ServerName:
        lea     XSI, XES[XDI].tagNETUSERITEM.szServer
	INVOKE	STR4_COPY,lpszServerName,vSDS XSI

DoObject:
        lea     XSI, XDS[XSI].tagNETUSERITEM.szObject
	INVOKE	STR4_COPY,lpszObjectName,vSDS XSI

        mov     al, bFlag
        xor     ah,ah
        mov     word ptr XES[XDI].tagNETUSERITEM.uFlag, ax

        INVOKE  FIL4_WRITE,uHandle,lpszBuffer,stagNETUSERITEM
Done:
        ret

NOV4_CREATEUSERENTRY endp
;-----------------------------------------------------------------------------
END
