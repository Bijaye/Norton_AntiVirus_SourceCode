include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_FIND_WILD                       08/16/93                        BJG
;
;       Find all files that match the filespec and pass to callback.
;       NOTE:  Use FIL4_FIND_ALL if the filespec is "*.*" (it'll be faster)
;
;  **** IMPORTANT ****
;       When fixing bugs, make sure you also apply them to FIL4_FIND_ALL
;
; BOOL WINAPI FIL4_FIND_WILD(FARPROC Callback,LPARAM lParam,
;                           BOOL Flag,LPSTR Buffer);
;
; Input:
;       1)  lpszFileSpec points to filespec to match.
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
;                Input:  1)  Directory name.  EX: THISDIR
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
; History:
;       08/16/93 BG Derived from and replaces FIL4_FIND_ALL().
;       08/17/93 BG Fixed bug searching sub-dirs when non-*.* is passed
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_FIND_WILD PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpszFileSpec:LPSTR,
        CallBack:LPSTR,
        lpParam:DWORD,
        Flag:WORD,
        Buffer:LPSTR

        LOCAL   cAllDirs[4]:BYTE        ; 4 bytes to place *.*,NULL
        LOCAL   cBackOne[3]:BYTE        ; 3 bytes to place ..
        LOCAL   lpDTA:DWORD

DTA_List        EQU     75              ; Location of start DTA_LIST

        SMOV    SES,ss                   ; Setup LOCAL variables
        lea     XDI,cBackOne
        mov     ax,'..'
        stosw                           ; Store ..
        xor     al,al
        stosb                           ; Store NULL

        lea     XDI,cAllDirs
        mov     ax,'.*'
        stosw                           ; Store *.
        mov     ax,002Ah
        stosw                           ; Store * NULL

        INVOKE  FIL4_DIRECTORY,0,Buffer,GET_INFO

        INVOKE  FIL4_GET_DTA            ; Save original DTA
        mov     word ptr lpDTA,ax
        mov     word ptr lpDTA+2,dx

        LDS_DX  Buffer
        add     XDX,DTA_List

;--------------------------------------
; Find files (not dirs yet) that match the filespec

LblSetNewDTA:
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XDX; Setup new DTA at DS:DX

        INVOKE  FIL4_FIND_FIRST,lpszFileSpec,07h

LblCheckFileFindStat:
        or      ax,ax                   ; If !0 then no more files
        jnz     LblStartDirSearch

;--------------------------------------
; If it is a file, then execute the callback with pointer to it.

        cmp     word ptr Callback+2,0
        je      LblFileNoCallback

        push    XBX
        push    XDX

        mov     XDX,XBX
        add     XDX,DISK_TA.DTA_FILENAME

        INVOKE  FF_PTR PTR CallBack,vSDS XDX,vSDS XBX,lpParam

        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE             ; Callback is asking us to quit

LblFileNoCallback:
        INVOKE  FIL4_FIND_NEXT          ; Look for next...
        jmp     LblCheckFileFindStat

;--------------------------------------
; If no more files, search *.* for directories

LblStartDirSearch:
        cmp     ax,12h                  ; No more files found
        jne     J_Done

        cmp     Flag, FALSE             ; No subdirectories
        je      J_Done

        INVOKE  FIL4_FIND_FIRST,ADDR cAllDirs,17h
        jmp     LblCheckDirFindStat

LblFindNextDir:
        INVOKE  FIL4_FIND_NEXT          ; Look for next dir...

LblCheckDirFindStat:
        or      ax,ax                   ; If !0 then no more files
        jnz     LblDirSearchBackup

;--------------------------------------
; If it is a directory, then change to 
; this directory, increase the DTA_List

        test    XDS[XBX].DISK_TA.DTA_Attr,10h
        jz      LblFindNextDir          ; Not a directory entry

        cmp     XDS[XBX].DISK_TA.DTA_FILENAME,byte ptr '.'
        je      LblFindNextDir          ; Skip the . and .. directory entries
                             
        ; It's a directory.  Traverse downward...
        mov     XDX,XBX
        add     XDX,DISK_TA.DTA_FILENAME

        INVOKE  FIL4_CHANGE_DIR,vSDS XDX
        or      ax,ax
        jz      LblFindNextDir

        cmp     word ptr Callback+2,0
        je      LblDirNoCallback

        push    XBX
        push    XDX
        INVOKE  FF_PTR PTR CallBack,vSDS XDX,vSDS XBX,lpParam
        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE

LblDirNoCallback:
        mov     XDX,XBX
        add     XDX,sDISK_TA            ; Next DTA pointer
        jmp     LblSetNewDTA

;--------------------------------------
; If no more directories at this level,
; back up one level and continue looking.

LblDirSearchBackup:
        ;;;MOREWORK:  Check this conditional against the original code
        cmp     ax,12h                  ; No more files found
        jne     J_Done

        mov     XDX,XBX
        cmp     XDX,DTA_List
        jb      J_Done                  ; Done with all files and dirs

        sub     XDX,sDISK_TA
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XBX

        mov     XDX,XBX

        INVOKE  FIL4_CHANGE_DIR,ADDR cBackOne

                                        ; Fake a directory for callback's sake
        mov     XDS[XBX].DISK_TA.DTA_Attr,10h

        cmp     word ptr Callback+2,0
        je      LblDirNoCallback2

        push    XBX
        push    XDX

        INVOKE  FF_PTR PTR CallBack,ADDR cBackOne,vSDS XBX,lpParam
        pop     XDX
        pop     XBX

        or      ax,ax
        jz      J_ERRORDONE

LblDirNoCallback2:
        jmp     LblFindNextDir

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

FIL4_FIND_WILD ENDP
;-----------------------------------------------------------------------------
END

