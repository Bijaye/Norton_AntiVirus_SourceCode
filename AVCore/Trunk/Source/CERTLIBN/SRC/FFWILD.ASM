;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/ffwild.asv   1.5   11 Sep 1997 19:48:34   KSACKIN  $
;;
;; Description: Defines FIL4_FIND_WILD function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/ffwild.asv  $
;  
;     Rev 1.5   11 Sep 1997 19:48:34   KSACKIN
;  Ported changes from QAKG.
;
;     Rev 1.4.2.1   22 Aug 1997 16:13:56   MKEATIN
;  For SYM_DOSX, FIL4_FIND_WILD should always call FIL4_FIND_WILD_DTA.
;
;
;     Rev 1.4   08 Mar 1996 14:56:52   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;
;     Rev 1.3   20 Jan 1995 15:30:12   DALLEE
;  FIL4_FIND_WILD:  C callable function must preserve DS, DI.
;
;     Rev 1.2   09 Dec 1994 20:29:38   SZIADEH
;  added UniqueSegmentEnd.
;
;     Rev 1.1   08 Dec 1994 20:24:34   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;
;     Rev 1.3   23 Jun 1994 07:42:34   RSTANEV
;  Added support for FINDITEM type callback.
;
;     Rev 1.2   27 May 1994 08:59:34   RSTANEV
;  Added support for INT 21 based platforms.
;
;     Rev 1.1   16 May 1994 10:58:42   RSTANEV
;  Fixed some bugs.
;
;     Rev 1.0   13 May 1994 17:08:40   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_FIND_WILD
;
; Find all files that match the filespec and pass to callback.
; NOTE:  Use FIL4_FIND_ALL if the filespec is "*.*" (it'll be faster)
;
;  **** IMPORTANT ****
;       When fixing bugs, make sure you also apply them to all platforms
;       and to FIL4_FIND_ALL.
;
; BOOL WINAPI FIL4_FIND_WILD ( LPSTR lpszFileSpec, FARPROC Callback,
;                              LPARAM lParam, BOOL Flag, LPSTR Buffer );
;
; Input:
;       1)  lpszFileSpec points to filespec to match.
;       2)  CallBack is a FAR routine that will execute when:
;
;           FILE is found:
;               BOOL WINAPI Callback ( LPFINDITEM lpFindItem );
;                Input:  1)  The following members of *lpFindItem are valid:
;                            szShortName
;                            szLongName
;                            dwCookie
;                            bIsDir == FALSE
;                            dwFileSize
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;           DIRECTORY is found:
;               BOOL WINAPI Callback ( LPFINDITEM lpFindItem );
;                Input:  1)  The following members of *lpFindItem are valid:
;                            szShortName
;                            szLongName
;                            dwCookie
;                            bIsDir == TRUE
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;           CHANGE BACK a directory:
;               BOOL WINAPI Callback ( LPFINDITEM lpFindItem );
;                Input:  1)  The following members of *lpFindItem are valid:
;                            szShortName == ".."
;                            szLongName == ".."
;                            dwCookie
;                            bIsDir == TRUE
;                Output: 1)  TRUE (1) Continue
;                        2)  FALSE (0) Stop
;
;       3)  A parameter to pass to the CallBack function
;       4)  Flag = TRUE/FALSE
;               TRUE = All directories
;               FALSE = Only current directory
;       5)  Buffer = 1600 bytes to hold all DTAs
;           Buffer (WIN32) = MAX_PATH + max_dir_depth * sizeof (HANDLE) + 3
; Output:
;       1)  TRUE(1) Passed.
;           FALSE(0) Failed.
;----------------------------------------------------------------------------
IF bSYM_WIN32

  FIL4_FIND_WILD  PROC  CODEDISTANCE  USES  XBX XCX XDX XDI SES,
        lpszFileSpec:LPSTR,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

        LOCAL   cAllFiles:WORD          ; That's actually char[2]
        LOCAL   rFindData:WIN32_FIND_DATA
        LOCAL   pFirstHandle:LPSTR
        LOCAL   rFindItem:FINDITEM

                                        ; Initialize local variables
        mov     cAllFiles,'*'
        mov     eax,lpParam
        mov     rFindItem.dwCookie,eax
                                        ; Save current directory
        LES_DI  Buffer
        INVOKE  GetCurrentDirectory, 260d, vSES XDI
        or      XAX,XAX
        jz      FunctionEnd
                                        ; Build FF handles after string
        lea     XDI,[XDI + XAX + 5]
        shr     XDI,2                   ; Round up strlen to DWORD
        shl     XDI,2
        mov     pFirstHandle,XDI

                                        ; Initiate search for files
  ScanForFiles:

        INVOKE  FindFirstFile, ADDR lpszFileSpec, ADDR rFindData

        mov     XES[XDI],XAX

        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanForFiles       ; ZF will be set.

  ExamineFFDScanForFiles:
                                        ; Fill rFindItem
        INVOKE  STR4_COPY, ADDR rFindData.cFileName, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, ADDR rFindData.cAlternateFileName, ADDR rFindItem.szShortName

                                        ; Skip directories
        test    rFindData.dwFileAttributes,FILE_ATTRIBUTE_DIRECTORY
        jnz     ScanForMoreFiles

        mov     XAX,rFindData.nFileSizeLow
        mov     rFindItem.dwFileSize,XAX
        mov     rFindItem.bIsDir,FALSE

                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

  ScanForMoreFiles:
                                        ; Search for more files.
        INVOKE  FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFDScanForFiles

  ErrorScanForFiles:
                                        ; End of file search.  Determine
                                        ; whether successful or error.
        INVOKE  GetLastError
        cmp     XAX,ERROR_NO_MORE_FILES
        jne     Error
                                        ; Close file search handle
        INVOKE  FindClose, XES[XDI]
                                        ; Quit if search current dir only
        cmp     Flag,FALSE
        mov     XAX,TRUE
        je      FunctionEnd

                                        ; Initiate search for directories

        INVOKE  FindFirstFile, ADDR cAllFiles, ADDR rFindData

        mov     XES[XDI],XAX

        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanForDirs        ; ZF will be set.

  ExamineFFDScanForDirs:
                                        ; Skip files
        test    rFindData.dwFileAttributes,FILE_ATTRIBUTE_DIRECTORY
        jz      ScanForMoreDirs
                                        ; Skip directories starting with '.'
        cmp     rFindData.cFileName,'.'
        je      ScanForMoreDirs
                                        ; Change to this directory
        INVOKE  SetCurrentDirectory, ADDR rFindData.cFileName

        mov     rFindItem.bIsDir,TRUE
                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error
                                        ; Set new pointer for next handlers
        add     XDI,SIZE HANDLE32
                                        ; Now that we are in the new
                                        ; directory, start all over again!
        jmp     ScanForFiles

  ScanForMoreDirs:
                                        ; Search for more directories.
        INVOKE  FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFDScanForDirs

  ErrorScanForDirs:
                                        ; End of directory search.  Determine
                                        ; whether successful or error.
        INVOKE  GetLastError
        cmp     XAX,ERROR_NO_MORE_FILES
        jne     Error
                                        ; Close directory search handle
        mov     XAX,-1
        xchg    XAX,XES[XDI]
        INVOKE  FindClose,XAX

                                        ; Directory drop back

        cmp     XDI,pFirstHandle
        mov     XAX,TRUE
        je      FunctionEnd

        sub     XDI,SIZE HANDLE32

        mov     DWORD PTR rFindItem.szLongName,'..'
        mov     DWORD PTR rFindItem.szShortName,'..'
        mov     rFindItem.bIsDir,TRUE
        INVOKE  SetCurrentDirectory, ADDR rFindItem.szLongName

                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jnz     ScanForMoreDirs

  Error:
                                        ; Save error information
        INVOKE  GetLastError
        push    eax
  CloseNext:                            ; Close all valid FF handles
        mov     XBX,XES[XDI]
        inc     XBX
        jz      DoNotClose
        dec     XBX
        INVOKE  FindClose, XBX
  DoNotClose:
        cmp     XDI,pFirstHandle
        lahf
        sub     XDI,SIZE HANDLE32
        sahf
        jne     CloseNext
                                        ; Restore original directory
        INVOKE  SetCurrentDirectory,Buffer
                                        ; Restore error information
        pop     eax
        INVOKE  SetLastError,eax

        xor     XAX,XAX

  FunctionEnd:

        ret

  FIL4_FIND_WILD  ENDP

ELSEIF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ;--------------------------------------------------------------------------
  ;
  ; The FIL4_FIND_WILD for these platforms will first determine whether
  ; the extended DOS FindFirstFile/FindNextFile are supported and then
  ; will call the appropriate functions: FIL4_FIND_WILD_EX or
  ; FIL4_FIND_WILD_DTA.
  ;
  ;--------------------------------------------------------------------------

  ;--------------------------------------------------------------------------
  ;
  ; An old (DTA) FIL4_FIND_WILD.
  ;
  ;--------------------------------------------------------------------------
  FIL4_FIND_WILD_DTA  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpszFileSpec:LPSTR,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

        LOCAL   cAllDirs[4]:BYTE
        LOCAL   lpDTA:LPSTR
        LOCAL   rFindItem:FINDITEM

                                        ; Initialize local variables
        mov     bx,WORD PTR lpParam
        mov     cx,WORD PTR lpParam + 2
        mov     WORD PTR rFindItem.dwCookie,bx
        mov     WORD PTR rFindItem.dwCookie + 2,cx

        mov     WORD PTR cAllDirs,'.*'
        mov     WORD PTR cAllDirs + 2,'*'

                                        ; Save current directory
        INVOKE  FIL4_DIRECTORY,0,Buffer,GET_INFO

        INVOKE  FIL4_GET_DTA            ; Save original DTA
        mov     WORD PTR lpDTA,ax
        mov     WORD PTR lpDTA + 2,dx
                                        ; Calculate start of DTA buffers
        LDS_DX  Buffer
        add     XDX,260d
                                        ; Scan for files
  ScanForFilesDTA:
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XDX ; Setup new DTA at DS:DX

        INVOKE  FIL4_FIND_FIRST,lpszFileSpec,07h
                                        ; Check if no more files
  ExamineFindFileDTA:
        or      XAX,XAX
        jnz     ScanForDirectoriesDTA
                                        ; It's a file, notify.
        push    XBX
        push    XDX
        lea     XDX,[XBX + DISK_TA.DTA_FILENAME]

                                        ; Setup rFindItem
        INVOKE  STR4_COPY, vSDS XDX, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, vSDS XDX, ADDR rFindItem.szShortName
        mov     ax,[XBX].DISK_TA.DTA_SizeL
        mov     dx,[XBX].DISK_TA.DTA_SizeH
        mov     WORD PTR rFindItem.dwFileSize,ax
        mov     WORD PTR rFindItem.dwFileSize + 2,dx
        mov     rFindItem.bIsDir,FALSE

        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        pop     XDX
        pop     XBX
                                        ; Check if callback wants us to quit
        or      XAX,XAX
        jz      J_ERRORDONE
                                        ; Scan for more files
        INVOKE  FIL4_FIND_NEXT
        jmp     SHORT ExamineFindFileDTA
                                        ; Scan for directories
  ScanForDirectoriesDTA:
        cmp     ax,12h                  ; No more files found
        jne     J_Done
                                        ; No directory scan
        cmp     Flag,FALSE
        je      J_Done
                                        ; Start scanning for directories
        INVOKE  FIL4_FIND_FIRST,ADDR cAllDirs,17h
        jmp     SHORT ScanForDirectoriesExamineDTA

  ScanForMoreDirectoriesDTA:
                                        ; Scan for next directory
        INVOKE  FIL4_FIND_NEXT

  ScanForDirectoriesExamineDTA:
                                        ; Check if no more directories
        or      XAX,XAX
        jnz     NoMoreDirectoriesDTA
                                        ; If it is a directory, then change
                                        ; to this directory, increase DTA ptr.
        test    XDS[XBX].DISK_TA.DTA_Attr,10h
        jz      ScanForMoreDirectoriesDTA
                                        ; Skip the . and .. directory entries
        cmp     XDS[XBX].DISK_TA.DTA_FILENAME,BYTE PTR '.'
        je      ScanForMoreDirectoriesDTA
                                        ; Change to this directory
        lea     XDX,[XBX + DISK_TA.DTA_FILENAME]
        INVOKE  FIL4_DIRECTORY,0,vSDS XDX,SET_INFO
        or      XAX,XAX
        jz      ScanForMoreDirectoriesDTA
                                        ; Notify
        push    XBX
        push    XDX
        INVOKE  STR4_COPY, vSDS XDX, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, vSDS XDX, ADDR rFindItem.szShortName
        mov     rFindItem.bIsDir,TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        pop     XDX
        pop     XBX
                                        ; Check if callback wants us to quit
        or      XAX,XAX
        jz      J_ERRORDONE

        mov     XDX,XBX
        add     XDX,sDISK_TA            ; Next DTA pointer
        jmp     ScanForFilesDTA

  NoMoreDirectoriesDTA:
                                        ; Check if error or no more dirs
        cmp     ax,12h
        jne     J_Done
                                        ; Check if no more scan.
        mov     XDX,XBX
        cmp     XDX,260d
        jb      J_Done
                                        ; Back up one DTA & directory
        sub     XDX,sDISK_TA
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XBX

        mov     XDX,XBX
        mov     WORD PTR rFindItem.szLongName,'..'
        mov     BYTE PTR rFindItem.szLongName + 2,0
        mov     WORD PTR rFindItem.szShortName,'..'
        mov     BYTE PTR rFindItem.szShortName + 2,0
        mov     rFindItem.bIsDir,TRUE
        INVOKE  FIL4_DIRECTORY,0,ADDR rFindItem.szLongName,SET_INFO

        push    XBX
        push    XDX
        INVOKE  FF_PTR PTR CallBack,ADDR rFindItem
        pop     XDX
        pop     XBX
        or      XAX,XAX
        jz      J_ERRORDONE

        jmp     ScanForMoreDirectoriesDTA

  J_Done:
        mov     XAX,TRUE
        jmp     SHORT J_Out

  J_ERRORDONE:
        xor     XAX,XAX
  J_Out:
                                        ; Restore original status.
        push    XAX
        INVOKE  FIL4_CHANGE_DTA,lpDTA
        INVOKE  FIL4_DIRECTORY,0,Buffer,SET_INFO
        pop     XAX

        ret

  FIL4_FIND_WILD_DTA  ENDP

  ;--------------------------------------------------------------------------
  ;
  ; An extended (LFN) FIL4_FIND_WILD.
  ;
  ;--------------------------------------------------------------------------
  FIL4_FIND_WILD_EX  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpszFileSpec:LPSTR,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

        ECHO Warning: Don't know how to determine whether error or no more files
        ECHO -------- Look at the commented out lines at the end of FIL4_FIND_WILD_EX.

        LOCAL   cAllFiles[2]:BYTE
        LOCAL   rFindData:WIN32_FIND_DATA
        LOCAL   pFirstHandle:HANDLE32
        LOCAL   rFindItem:FINDITEM

                                        ; Initialize local variables
        mov     bx,WORD PTR lpParam
        mov     cx,WORD PTR lpParam + 2
        mov     WORD PTR rFindItem.dwCookie,bx
        mov     WORD PTR rFindItem.dwCookie + 2,cx

        mov     WORD PTR cAllFiles,'*'
                                        ; Save current directory
        LES_DI  Buffer
        INVOKE  FIL4_DIRECTORY,0,vSES XDI,GET_INFO
        or      XAX,XAX
        jz      FunctionEndEx
                                        ; Build FF handles after string
        add     XDI,XAX
        add     XDI,3
        shr     XDI,1                   ; Round up strlen to HANDLE32
        shl     XDI,1
        mov     pFirstHandle,XDI

                                        ; Initiate search for files
  ScanForFilesEx:

        INVOKE  ex_FindFirstFile, ADDR lpszFileSpec, ADDR rFindData, FILE_ATTRIBUTE_NORMAL OR FILE_ATTRIBUTE_READONLY OR FILE_ATTRIBUTE_HIDDEN OR FILE_ATTRIBUTE_SYSTEM

        mov     XES[XDI],XAX

        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanForFilesEx     ; ZF will be set.

  ExamineFFDScanForFilesEx:

        INVOKE  STR4_COPY, ADDR rFindData.cFileName, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, ADDR rFindData.cAlternateFileName, ADDR rFindItem.szShortName

        mov     ax,WORD PTR rFindData.nFileSizeLow
        mov     bx,WORD PTR rFindData.nFileSizeLow + 2

        mov     WORD PTR rFindItem.dwFileSize,ax
        mov     WORD PTR rFindItem.dwFileSize + 2,bx

        mov     rFindItem.bIsDir,FALSE
                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

  ScanForMoreFilesEx:
                                        ; Search for more files.
        INVOKE  ex_FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFDScanForFilesEx

  ErrorScanForFilesEx:
                                        ; End of file search.  Determine
                                        ; whether successful or error.
;        INVOKE  GetLastError
;        cmp     XAX,ERROR_NO_MORE_FILES
;        jne     Error
                                        ; Close file search handle
        INVOKE  ex_FindClose, XES[XDI]
                                        ; Quit if search current dir only
        cmp     Flag,FALSE
        mov     XAX,TRUE
        je      FunctionEndEx

                                        ; Initiate search for directories

        INVOKE  ex_FindFirstFile, ADDR cAllFiles, ADDR rFindData, FILE_ATTRIBUTE_DIRECTORY

        mov     XES[XDI],XAX

        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanForDirsEx      ; ZF will be set.

  ExamineFFDScanForDirsEx:
                                        ; Skip directories starting with '.'
        cmp     rFindData.cFileName,'.'
        je      ScanForMoreDirsEx
                                        ; Change to this directory
        INVOKE  FIL4_DIRECTORY,0,ADDR rFindData.cFileName,SET_INFO
                                        ; Create rFindItem
        INVOKE  STR4_COPY, ADDR rFindData.cFileName, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, ADDR rFindData.cAlternateFileName, ADDR rFindItem.szShortName

        mov     rFindItem.bIsDir,TRUE
                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error
                                        ; Set new pointer for next handlers
        add     XDI,SIZE HANDLE32
                                        ; Now that we are in the new
                                        ; directory, start all over again!
        jmp     ScanForFilesEx

  ScanForMoreDirsEx:
                                        ; Search for more directories.
        INVOKE  ex_FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFDScanForDirsEx

  ErrorScanForDirsEx:
                                        ; End of directory search.  Determine
                                        ; whether successful or error.
;        INVOKE  GetLastError
;        cmp     XAX,ERROR_NO_MORE_FILES
;        jne     Error
                                        ; Close directory search handle
        mov     XAX,-1
        xchg    XAX,XES[XDI]
        INVOKE  ex_FindClose,XAX

                                        ; Directory drop back

        cmp     XDI,pFirstHandle
        mov     XAX,TRUE
        je      FunctionEndEx

        sub     XDI,SIZE HANDLE32

        mov     WORD PTR rFindItem.szLongName,'..'
        mov     BYTE PTR rFindItem.szLongName + 2,0
        mov     WORD PTR rFindItem.szShortName,'..'
        mov     BYTE PTR rFindItem.szShortName + 2,0

        mov     rFindItem.bIsDir,TRUE
        INVOKE  FIL4_DIRECTORY,0,ADDR rFindItem.szLongName,SET_INFO
                                        ; Notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jnz     ScanForMoreDirsEx

  Error:
                                        ; Save error information
;        INVOKE  GetLastError
;        push    eax
  CloseNext:                            ; Close all valid FF handles
        mov     XBX,XES[XDI]
        inc     XBX
        jz      DoNotClose
        dec     XBX
        INVOKE  ex_FindClose, XBX
  DoNotClose:
        cmp     XDI,pFirstHandle
        lahf
        sub     XDI,SIZE HANDLE32
        sahf
        jne     CloseNext
                                        ; Restore original directory
        INVOKE  FIL4_DIRECTORY,0,Buffer,SET_INFO
                                        ; Restore error information
;        pop     eax
;        INVOKE  SetLastError,eax

        xor     XAX,XAX

  FunctionEndEx:

        ret

  FIL4_FIND_WILD_EX  ENDP

  ;--------------------------------------------------------------------------
  ;
  ; And here is FIL4_FIND_WILD itself.
  ;
  ;--------------------------------------------------------------------------

  FIL4_FIND_WILD  PROC  CODEDISTANCE USES SDS XDI,
        lpszFileSpec:LPSTR,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

IFNDEF  SYM_DOSX
        mov     XAX,71A0h
        LES_DI  Buffer
        mov     XCX,2
        LDS_DX  Buffer
        inc     XDX
        inc     XDX
        stc
        CALLDOS
        jc      NoLFN

        test    bx,FS_LFN_APIS
        jz      NoLFN

        INVOKE  FIL4_FIND_WILD_EX, lpszFileSpec, CallBack, lpParam, Flag, Buffer

        jmp     SHORT FunctionEnd

  NoLFN:
ENDIF
        INVOKE  FIL4_FIND_WILD_DTA, lpszFileSpec, CallBack, lpParam, Flag, Buffer

  FunctionEnd:

        ret

  FIL4_FIND_WILD  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_FIND_WILD.
ENDIF

UniqueSegmentEnd
END