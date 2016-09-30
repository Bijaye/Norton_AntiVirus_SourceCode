include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_ADDPATH                  4/13/93                                 CAG
;
;       Add a path or drive and path to a filename.
;
; BOOL FIL4_ADDPATH(LPSTR Filename,LPSTR Buffer);
;
; Input:
;       1)  Filename to add path and drive to.
;       2)  Temp work buffer, 128 bytes long.
; Output:
;       1)  Directory added to filename.
;       2)  TRUE - Success.
;       3)  FALSE - Fail.
;-----------------------------------------------------------------------------
FIL4_ADDPATH PROC USES SDS XSI,
        lpFilename:LPSTR,
        lpTempBuff:LPSTR

        LOCAL   szLocalSlash[2]:BYTE

        mov     word ptr szLocalSlash,005Ch

        LDS_SI  lpFilename

        cmp     [XSI+1],byte ptr ':'    ; Do we have drive?
        je      HaveItAll

        INVOKE  FIL4_DRIVE,lpTempBuff,GET_INFO

        LDS_SI  lpTempBuff
        inc     XSI
        inc     XSI
        INVOKE  FIL4_DIRECTORY,0,vSDS XSI,GET_INFO

        INVOKE  STR4_APPEND,lpTempBuff,ADDR szLocalSlash
        INVOKE  STR4_APPEND,lpTempBuff,lpFilename

        INVOKE  STR4_COPY,lpTempBuff,lpFilename

HaveItAll:
        mov     ax,TRUE

        ret

FIL4_ADDPATH    ENDP
;-----------------------------------------------------------------------------
END
