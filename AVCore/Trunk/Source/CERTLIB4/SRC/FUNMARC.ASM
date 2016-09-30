include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_UNMARC                            1/19/93                        MRH
;
;       Unarchive a file.
;
; BOOL WINAPI FIL4_UNMARC(LPSTR SearchFile,LPSTR Archive,LPSTR lpMarc,
;                         LPSTR WorkBuff,UINT uWorkBuffer,WORD Flags);
; 
; Input:
;       1)  Search for this file to unmarc, no directory, just name
;       2)  Name of archive to place file in
;       3)  Buffer of MARC structure to use
;       4)  Work buffer to use
;       5)  Size of work buffer
;       6)  Flags - MARC_ORIG (0) Place in original directory.
;                   MARC_NEW  (1) Place in new directory.
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_UNMARC PROC USES SDS XSI,
        lpInput:LPSTR,
        lpArchive:LPSTR,
        lpMarc:LPSTR,
        lpWork:LPSTR,
        uWork:UINT,
        uFlag:WORD

	LOCAL	ArcHandle:UINT
	LOCAL	InputHandle:UINT

        mov     ArcHandle,0
        mov     InputHandle,0

	INVOKE	FIL4_SEARCH_ARC,lpInput,lpArchive,lpMARC
        or      XAX,XAX
        jz      J_Error
	mov	ArcHandle,XAX

        cmp     uFlag,MARC_NEW
        je      NoDirectory
        LDS_SI  lpMarc
        add     XSI,MARC.ArcDir
        INVOKE  STR4_COPY,vSDS XSI,lpWork
        INVOKE  FIL4_CREATE_DIR,lpWork
        LDS_SI  lpMarc
        add     XSI,MARC.ArcName
        INVOKE  STR4_APPEND,lpWork,vSDS XSI
        jmp     DoIt
NoDirectory:
        LDS_SI  lpMarc
        add     XSI,MARC.ArcName
        INVOKE  STR4_COPY,vSDS XSI,lpWork
DoIt:
	INVOKE	FIL4_OPEN,lpWork,fCREATE
        cmp     ax, -1
        jz      J_Error
	mov	InputHandle,XAX

        LDS_SI  lpMarc
	INVOKE	FIL4_COPY_HANDLE,ArcHandle,InputHandle,lpWork,uWork,[XSI].MARC.ArcSize
        or      ax,ax
        jz      J_Error

; Reset the Date,Time and attribute of file from archive.
        LDS_SI  lpMARC
	INVOKE	FIL4_DATETIME,InputHandle,[XSI].MARC.ArcDate,[XSI].MARC.ArcTime,SET_INFO
	INVOKE	FIL4_CLOSE,InputHandle
	INVOKE	FIL4_CLOSE,ArcHandle
	INVOKE	FIL4_ATTR,lpInput,[XSI].MARC.ArcAttr,SET_INFO
	mov	XAX,TRUE
        jmp     J_Out

J_Error:
        cmp     ArcHandle,0
        jz      NoArcToClose
        INVOKE  FIL4_CLOSE,ArcHandle
NoArcToClose:
        cmp     InputHandle,0
        jz      NoInputToClose
        INVOKE  FIL4_CLOSE,InputHandle
NoInputToClose:
        mov     XAX,FALSE
J_Out:
        ret

FIL4_UNMARC ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; FIL4_SEARCH_ARC			2-1-93				MRH
;
;	Find a file in the archive, and advance the file pointer.
;
; UINT FIL4_SEARCH_ARC(LPSTR lpFileName, LPSTR lpArchive, LPSTR lpMARC);
;
; Input:
;	1)  File name to search the ARC for, no directory, just name.
;	2)  Archive file name
;       3)  Buffer of MARC structure to use
;
; Output:
;	1)  Arc File handle, advanced to the header of the requested file.
;	2)  0 if error
;
; LIBRARY:{INTEL}.LIB
;		
;-----------------------------------------------------------------------------
FIL4_SEARCH_ARC	PROC USES SDS XSI,
        lpFileName:LPSTR,
        lpArchive:LPSTR,
        lpMarc:LPSTR

	LOCAL   ArcHandle:UINT
	LOCAL   InputHandle:UINT

        INVOKE  FIL4_OPEN,lpArchive,fREAD
        cmp     ax, -1
        jz      ErrorOut
	mov     ArcHandle,XAX

FIL4_SEARCH_LOOP:                       
	INVOKE	FIL4_READ,ArcHandle,lpMarc,sMARC
	cmp	XAX,0                   ; Read header
	je      ErrorOut
        cmp     XAX,sMARC
        jne     ErrorOut

        LDS_SI  lpMarc
        add     XSI,MARC.ArcName
	INVOKE  STR4_ICOMPARE,lpFileName,vSDS XSI
	or      ax,ax                   ; Test file name
	jnz     FoundFile		; File name found

        LDS_SI  lpMarc
	INVOKE  FIL4_SEEK,ArcHandle,[XSI].MARC.ArcSize,fSEEK_FOR
        jmp     FIL4_SEARCH_LOOP

ErrorOut:
        mov     XAX,FALSE
        jmp     J_Done

FoundFile:
	mov     XAX,ArcHandle
J_Done:
	ret

FIL4_SEARCH_ARC	ENDP
;-----------------------------------------------------------------------------
END
