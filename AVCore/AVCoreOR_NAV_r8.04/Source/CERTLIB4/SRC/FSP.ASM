include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_SPLIT_PATHNAME                   2/5/93                          CAG
;
;       Split the path and filename from a filename passed in.
;
; BOOL WINAPI FIL4_SPLIT_PATHNAME(LPSTR FullName,LPSTR Dir,LPSTR File);
;
; Input:
;       1)  Full name of the file, with directory.
;       2)  Where to store the directory part.
;       3)  Where to store the file part.
; Output:
;       1)  TRUE (1)  Directory not in name, got default dir.
;       2)  FALSE (0) Directory in name.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_SPLIT_PATHNAME PROC USES SDS SES XSI XDI,
        lpFullname:LPSTR,
        lpDir:LPSTR,
        lpFile:LPSTR

        LOCAL   uStatus:BOOL
        mov     uStatus,FALSE

        INVOKE  STR4_LENGTH_TO_LAST,lpFullname,5Ch
        or      XAX,XAX
        jz      NoSlash

        LDS_SI  lpFullname              ; Point DS:SI to filename
        add     XSI,XAX

        INVOKE  MEM4_COPY,lpFullname,lpDir,ax
        jmp     HadSlash
NoSlash:
        INVOKE  FIL4_DRIVE,lpDir,GET_INFO
        LES_DI  lpDir
        add     XDI,2
        INVOKE  FIL4_DIRECTORY,0,vSES XDI,GET_INFO

        LES_DI  lpDir
        INVOKE  STR4_SRCH_END,lpDir
        mov     XDI,XAX
        mov     [XDI],byte ptr 5Ch
        mov     [XDI+1],byte ptr 0
        LDS_SI  lpFullname

HadSlash:
        INVOKE  STR4_COPY,vSDS XSI,lpFile

        mov     ax,uStatus

        ret

FIL4_SPLIT_PATHNAME ENDP
;-----------------------------------------------------------------------------
END
