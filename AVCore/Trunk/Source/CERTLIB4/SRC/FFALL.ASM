include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_FIND_ALL                        12/29/92                        CAG
;
;       Find all files and process an inputted callback.
;       NOTE:  To process other than *.*, use the slower FIL4_FIND_WILD
;
;  **** IMPORTANT ****
;       When fixing bugs, make sure you also apply them to FIL4_FIND_WILD
;
; BOOL WINAPI FIL4_FIND_ALL(FARPROC Callback,LPARAM lParam,
;                           BOOL Flag,LPSTR Buffer);
;
; Input:
;       1)  CallBack is a FAR routine that will execute when:
;
;           FILE is found:
;               UINT WINAPI Callback(LPSTR Filename,FAR *DTA,LPARAM);
;                Input:  1)  File name.  EX: THISFILE.TXT
;                        2)  FAR * to Current DTA
;                        3)  LPARAM
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;           DIRECTORY is found:
;               UINT WINAPI Callback(LPSTR Filename,FAR *DTA,LPARAM);
;                Input:  1)  Directory name.  EX: THISFILE.TXT
;                        2)  FAR * to Current DTA
;                        3)  LPARAM
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;           CHANGE BACK a directory:
;               UINT WINAPI Callback(LPSTR Filename,FAR *DTA,LPARAM);
;                Input:  1)  Pointer to '..',0
;                        2)  FAR * to Current DTA
;                        3)  LPARAM
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;       2)  Flag = TRUE/FALSE
;               TRUE = All directories
;               FALSE = Only current directory
;       3)  Buffer = 1600 bytes to hold all DTA's
; Output:
;       1)  TRUE(1) Passed.
;           FALSE(0) Failed.
;
; Note:
;       FAR * to DTA is uses to caculate any information of found
;       file or directory.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_FIND_ALL PROC USES XBX XCX XDX SDS SES XSI XDI,
        CallBack:LPSTR,
        lpParam:DWORD,
        Flag:WORD,
        Buffer:LPSTR

        LOCAL   cBackOne[3]:BYTE        ; 3 bytes to place ..
        LOCAL   cAllFiles[4]:BYTE       ; 4 bytes to place *.*
        LOCAL   lpDTA:DWORD

DTA_List        EQU     75              ; Location of start DTA_LIST

        SMOV    SES,ss                   ; Setup LOCAL variables
        lea     XDI,cBackOne
        mov     ax,'..'
        stosw                           ; Store ..
        xor     al,al
        stosb                           ; Store NULL

        lea     XDI,cAllFiles
        mov     ax,'.*'
        stosw                           ; Store *.
        mov     ax,002Ah
        stosw                           ; Store * NULL

        INVOKE  FIL4_DIRECTORY,0,Buffer,GET_INFO

        INVOKE  FIL4_GET_DTA
        mov     word ptr lpDTA,ax
        mov     word ptr lpDTA+2,dx

        LDS_DX  Buffer
        add     XDX,DTA_List
J_3:
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XDX; Change DTA to DS:DX

        INVOKE  FIL4_FIND_FIRST,ADDR cAllFiles,17h
J_5:
        or      ax,ax                   ; If 0 then OK
        jnz     J_0

;--------------------------------------
; If it is a directory, then change to 
; this directory, increase the DTA_List

        test    XDS[XBX].DISK_TA.DTA_Attr,10h
        jz      J_1                     ; Not a directory entry

        cmp     Flag,FALSE
        je      J_2                     ; No subdirectories

        cmp     XDS[XBX].DISK_TA.DTA_FILENAME,byte ptr '.'
        je      J_2                     ; Skip the . and .. directory entries

        mov     XDX,XBX
        add     XDX,DISK_TA.DTA_FILENAME

        INVOKE  FIL4_CHANGE_DIR,vSDS XDX
        or      ax,ax
        jz      J_2

        push    XBX
        push    XDX
        cmp     word ptr Callback+2,0
        je      J_4a
        INVOKE  FF_PTR PTR CallBack,vSDS XDX,vSDS XBX,lpParam
J_4a:
        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE

        mov     XDX,XBX
        add     XDX,sDISK_TA            ; Next DTA pointer
        jmp     J_3

;--------------------------------------
; If it is a file, then execute the callback with pointer to it.
J_1:

        push    XBX
        push    XDX

        mov     XDX,XBX
        add     XDX,DISK_TA.DTA_FILENAME

        cmp     word ptr Callback+2,0
        je      J_4
        INVOKE  FF_PTR PTR CallBack,vSDS XDX,vSDS XBX,lpParam
J_4:
        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE

J_2:
        INVOKE  FIL4_FIND_NEXT
        jmp     J_5

; If it is no more files, then go back one level in the DTA_List
J_0:
        cmp     ax,12h                  ; No more files found
        jne     J_Done

        mov     XDX,XBX
        cmp     XDX,DTA_List
        jb      J_Done                  ; Done with all files

        sub     XDX,sDISK_TA
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XBX

        mov     XDX,XBX

        INVOKE  FIL4_CHANGE_DIR,ADDR cBackOne

        mov     XDS[XBX].DISK_TA.DTA_Attr,10h

        push    XBX
        push    XDX

        cmp     word ptr Callback+2,0
        je      J_4b
        INVOKE  FF_PTR PTR CallBack,ADDR cBackOne,vSDS XBX,lpParam
J_4b:
        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE


        jmp     J_2

J_Done:
        mov     ax,TRUE
        jmp     J_Out

J_ERRORDONE:
        mov     ax,FALSE
J_Out:
        push    ax
        INVOKE  FIL4_CHANGE_DTA,lpDTA   ; Change DTA back
        INVOKE  FIL4_CHANGE_DIR,Buffer  ; Change to original directory
        pop     ax

        ret

FIL4_FIND_ALL ENDP
;-----------------------------------------------------------------------------
END

