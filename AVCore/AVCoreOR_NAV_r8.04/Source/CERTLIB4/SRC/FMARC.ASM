include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_MARC                            1/19/93                         MRH
;
;       Archive a file into a master file.
;
; BOOL WINAPI FIL4_MARC(LPSTR InputFile,LPSTR Archive,LPSTR lpMarc,
;                       LPSTR WorkBuffer, UINT BufferSize);
; 
; Input:
;       1)  Name of file to archive
;       2)  Name of archive to place file in
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;
;-----------------------------------------------------------------------------
FIL4_MARC PROC USES SDS XSI,
        lpInput:LPSTR,
        lpArchive:LPSTR,
        lpMarc:LPSTR,
        lpBuffer:LPSTR,
        uBuffer:UINT

	LOCAL	ArcHandle:UINT
	LOCAL	InputHandle:UINT

        mov     ArcHandle,0
        mov     InputHandle,0

        INVOKE  FIL4_OPEN,lpArchive,fAPPEND OR fWRITE
        cmp     ax, -1
        jz      ErrorOut        
        mov     ArcHandle,XAX

        LDS_SI  lpMarc
        add     XSI,MARC.ArcAttr
        INVOKE  FIL4_STAT,lpInput,vSDS XSI,lpBuffer
        
        LDS_SI  lpMarc
        LES_DI  lpMarc
        add     XSI,MARC.ArcDir
        add     XDI,MARC.ArcName
        INVOKE  FIL4_SPLIT_PATHNAME,lpInput,vSDS XSI,vSES XDI

	INVOKE	FIL4_OPEN,lpInput,fREAD
        cmp     ax, -1
        jz      ErrorOut
	mov	InputHandle,XAX

	INVOKE	FIL4_WRITE,ArcHandle,lpMarc,sMARC
        or      XAX,XAX
        jz      ErrorOut
        cmp     XAX,sMARC
        jne     ErrorOut

	INVOKE	FIL4_COPY_HANDLE,InputHandle,ArcHandle,lpBuffer,uBuffer,-1
        or      XAX,XAX
        jz      ErrorOut

	INVOKE	FIL4_CLOSE,InputHandle
	INVOKE	FIL4_CLOSE,ArcHandle
        mov     XAX,TRUE
        jmp     J_Out
ErrorOut:

        cmp     ArcHandle,0
        jz      NoClose1
        INVOKE  FIL4_CLOSE,ArcHandle
NoClose1:
        cmp     InputHandle,0
        jz      NoClose2
        INVOKE  FIL4_CLOSE,InputHandle
NoClose2:
        mov     XAX,FALSE
J_Out:
	ret

FIL4_MARC ENDP
;-----------------------------------------------------------------------------
END
