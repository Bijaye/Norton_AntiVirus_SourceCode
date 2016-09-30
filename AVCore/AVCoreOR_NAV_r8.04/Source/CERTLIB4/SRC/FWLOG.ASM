include cts4.inc

LOGCODELENGTH   EQU     2

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_WRITELOG                 3/3/93                                  CAG
;
;       Write an Symantec log entry to a log file.
;
; BOOL WINAPI FIL4_WRITELOG(UINT Handle,void FAR *LogArray);
;
; Input:
;       1)  Handle of file opened for WRITE access
;       2)  Array of fields to write to the log file.
; Output:
;       1)  TRUE, successful write
;       2)  FALSE, error in writing log
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       The LogArray is in the format:
;               FAR *Code,FAR *Text
;               FAR *Code,FAR *Text
;               NULL,NULL
;       Where the code is what the element is,
;       and the text is the element.
;
;       CODE MUST be a two digit code.
;       TEXT MUST be NULL terminated.
;       
;-----------------------------------------------------------------------------
FIL4_WRITELOG PROC USES SDS XSI,
        hFile:WORD,
        lpArray:LPSTR

        LOCAL   szCRLF[2]:BYTE
        LOCAL   szSPACE[1]:BYTE

        mov     szCRLF[0],CR
        mov     szCRLF[1],LF
        mov     szSPACE[0],20h

        LDS_SI  lpArray                 ; Array of fields to write

MoreArray:
        cmp     [XSI],WORD32 ptr 0
        je      EndOfArray

; Write ID of code
        LES_DI  [XSI]                   ; Get pointer to write CODE
        INVOKE  FIL4_WRITE,hFile,vSES XDI,LOGCODELENGTH
        or      ax,ax
        jz      IsError
        cmp     ax,LOGCODELENGTH
        jne     IsError

; Write a SPACE after code
        INVOKE  FIL4_WRITE,hFile,ADDR szSPACE,1

; Write the TEXT for this element
        LES_DI  [XSI+4]                 ; Get pointer to write TEXT
        INVOKE  STR4_LENGTH,vSES XDI
        or      XAX,XAX
        jz      NothingToWrite

        mov     XCX,XAX
        INVOKE  FIL4_WRITE,hFile,vSES XDI,cx
        or      XAX,XAX
        jz      IsError
        cmp     XAX,XCX
        jne     IsError

NothingToWrite:
        INVOKE  FIL4_WRITE,hFile,ADDR szCRLF,2


        add     XSI,8                   ; Next array pointer
        jmp     MoreArray

EndOfArray:
        INVOKE  FIL4_WRITE,hFile,ADDR szCRLF,2
        mov     XAX,TRUE                ; Assure that we have a CRLF terminator
        jmp     Done

IsError:
        mov     XAX,FALSE

Done:

        ret

FIL4_WRITELOG ENDP
;-----------------------------------------------------------------------------
END
