;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/ffall.asv   1.5   19 Apr 1996 10:18:56   MZAREMB  $
;;
;; Description: Defines FIL4_FIND_ALL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/ffall.asv  $
;  
;     Rev 1.5   19 Apr 1996 10:18:56   MZAREMB
;  Fixed subdirectory recursion problem in DX platform through IFDEFs.
;  
;     Rev 1.4   08 Mar 1996 14:56:50   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   27 Dec 1994 18:30:40   DALLEE
;  16 bit FIL4_FIND_ALL: preserve DS, SI, DI for C style function.
;  
;     Rev 1.2   09 Dec 1994 20:29:36   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.1   08 Dec 1994 20:24:32   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;     Rev 1.9   23 Jun 1994 07:42:12   RSTANEV
;  Fixed bugs in DOS platform.
;
;     Rev 1.8   22 Jun 1994 16:16:20   RSTANEV
;  Added support for a new callback.
;
;     Rev 1.7   26 May 1994 16:05:02   RSTANEV
;  Minor changes.
;
;     Rev 1.6   20 May 1994 08:30:46   RSTANEV
;  Now supporting LFN for INT-21 based platforms.
;
;     Rev 1.5   16 May 1994 10:58:12   RSTANEV
;  Small optimization.
;
;     Rev 1.4   13 May 1994 17:08:18   RSTANEV
;  Deleted distance override.
;
;     Rev 1.3   13 May 1994 17:04:30   RSTANEV
;  Keeping code consistent by using INVOKE for all WIN32 API calls.
;
;     Rev 1.2   13 May 1994 17:00:42   RSTANEV
;  Now handling ERROR_NO_MORE_FILES.
;
;     Rev 1.1   12 May 1994 15:47:58   RSTANEV
;  Now setting the attributes in DTA when notifying about "..".
;
;     Rev 1.0   09 May 1994 18:42:12   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_FIND_ALL
;
; Find all files and process an inputted callback.
; NOTE:  To process other than *.*, use the slower FIL4_FIND_WILD
;
;  **** IMPORTANT ****
;       When fixing bugs, make sure you also apply them to all platforms
;       and FIL4_FIND_WILD.
;
; BOOL WINAPI FIL4_FIND_ALL ( FARPROC Callback, LPARAM lParam,
;                             BOOL Flag, LPSTR Buffer );
;
; Input:
;       1)  CallBack is a FAR routine that will execute when:
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
;       2)  Flag = TRUE/FALSE
;               TRUE = All directories
;               FALSE = Only current directory
;       3)  Buffer = 1600 bytes to hold all DTAs
;           Buffer (WIN32) = MAX_PATH + max_dir_depth * sizeof (HANDLE) + 3
; Output:
;       1)  TRUE(1) Passed.
;           FALSE(0) Failed.
;----------------------------------------------------------------------------
IF bSYM_WIN32

  FIL4_FIND_ALL  PROC  CODEDISTANCE  USES  XBX XCX XDX XDI SES,
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

                                        ; Initiate scanning current directory
  ScanAll:

        INVOKE  FindFirstFile, ADDR cAllFiles, ADDR rFindData
        mov     XES[XDI],XAX
        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanAll            ; ZF will be set.

  ExamineFFD:

        cmp     rFindData.cFileName,'.' ; Skip directory entries starting
        je      ScanAllNext             ; with '.'

        ECHO Confirm that FindFirstFile does not return volume labels.

                                        ; Fill rFindItem
        INVOKE  STR4_COPY, ADDR rFindData.cFileName, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, ADDR rFindData.cAlternateFileName, ADDR rFindItem.szShortName

                                        ; If directory found, CD and notify
        test    rFindData.dwFileAttributes,FILE_ATTRIBUTE_DIRECTORY
        jnz     NewDirectory

        mov     XAX,rFindData.nFileSizeLow
        mov     rFindItem.dwFileSize,XAX
        mov     rFindItem.bIsDir,FALSE

                                        ; If file found, notify
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

  ScanAllNext:

        INVOKE  FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFD
                                        ; Error, see if because of no more
                                        ; files or real error.
  ErrorScanAll:
        INVOKE  GetLastError
        cmp     XAX,ERROR_NO_MORE_FILES
        jne     Error
                                        ; Close current search handle
        INVOKE  FindClose, XES[XDI]
                                        ; Check if this was the starting
                                        ; directory.
        cmp     XDI,pFirstHandle
        mov     XAX,TRUE                ; Shouldn't need to restore original
        je      FunctionEnd             ; directory, since all went OK.

        sub     XDI,SIZE HANDLE32
                                        ; Backup one directory.
        mov     DWORD PTR rFindItem.szLongName,'..'
        INVOKE  SetCurrentDirectory, ADDR rFindItem.szLongName
        or      XAX,XAX
        jz      Error
                                        ; Notify about dropping back.
        mov     DWORD PTR rFindItem.szShortName,'..'
        mov     rFindItem.bIsDir,TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jnz     ScanAllNext
        jmp     SHORT Error

  NewDirectory:

        cmp     Flag,FALSE
        je      ScanAllNext

        INVOKE  SetCurrentDirectory, ADDR rFindItem.szLongName
        or      XAX,XAX
        jz      ScanAllNext             ; Someone maybe just nuked it :-)

        mov     rFindItem.bIsDir, TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

        add     XDI,SIZE HANDLE32

        jmp     ScanAll

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

  FIL4_FIND_ALL  ENDP

ELSEIF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ;--------------------------------------------------------------------------
  ;
  ; The FIL4_FIND_ALL for these platforms will first determine whether
  ; the extended DOS FindFirstFile/FindNextFile are supported and then
  ; will call the appropriate functions: FIL4_FIND_ALL_EX or
  ; FIL4_FIND_ALL_DTA.
  ;
  ;--------------------------------------------------------------------------

  ;--------------------------------------------------------------------------
  ;
  ; An old (DTA) FIL4_FIND_ALL.
  ;
  ;--------------------------------------------------------------------------
  FIL4_FIND_ALL_DTA  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

        LOCAL   cAllFiles[4]:BYTE       ; 4 bytes to place "*.*"
        LOCAL   lpDTA:LPSTR
        LOCAL   rFindItem:FINDITEM

        mov     bx,WORD PTR lpParam
        mov     cx,WORD PTR lpParam + 2
        mov     WORD PTR rFindItem.dwCookie,bx
        mov     WORD PTR rFindItem.dwCookie + 2,cx

  DTA_List  EQU  75                     ; Location of start DTA_LIST

        mov     WORD PTR cAllFiles,'.*'
        mov     WORD PTR cAllFiles + 2,'*'

                                        ; Save current directory

        INVOKE  FIL4_DIRECTORY,0,Buffer,GET_INFO

        INVOKE  FIL4_GET_DTA
        mov     WORD PTR lpDTA,ax
        mov     WORD PTR lpDTA + 2,dx

        LDS_DX  Buffer
        add     XDX,DTA_List
  J_3:
        mov     XBX,XDX
        INVOKE  FIL4_CHANGE_DTA,vSDS XDX; Change DTA to DS:DX

        INVOKE  FIL4_FIND_FIRST,ADDR cAllFiles,17h
  J_5:
        or      XAX,XAX                 ; If 0 then OK
        jnz     J_0

                                        ; Fill some of rFindItem
        add     XBX,DISK_TA.DTA_FILENAME
        INVOKE  STR4_COPY, vSDS XBX, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, vSDS XBX, ADDR rFindItem.szShortName
        sub     XBX,DISK_TA.DTA_FILENAME

                                        ; Check if this is a directory
        test    XDS[XBX].DISK_TA.DTA_Attr,10h
        jz      J_1                     ; Not a directory entry

        cmp     Flag,FALSE
        je      J_2                     ; No subdirectories

        cmp     rFindItem.szLongName,BYTE PTR '.'
        je      J_2                     ; Skip the . and .. directory entries

        mov     XDX,XBX
        add     XDX,DISK_TA.DTA_FILENAME

        INVOKE  FIL4_DIRECTORY,0,vSDS XDX,SET_INFO
        or      XAX,XAX
        jz      J_2

        push    XBX
        push    XDX
        mov     rFindItem.bIsDir,TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        pop     XDX
        pop     XBX

        or      XAX,XAX
        jz      J_ERRORDONE

        mov     XDX,XBX
        add     XDX,sDISK_TA            ; Next DTA pointer
        jmp     J_3

                                        ; If it is a file, then execute
                                        ; the callback with pointer to it.
  J_1:

        push    XBX
        push    XDX

        mov     ax,[XBX].DISK_TA.DTA_SizeL
        mov     dx,[XBX].DISK_TA.DTA_SizeH

        mov     WORD PTR rFindItem.dwFileSize,ax
        mov     WORD PTR rFindItem.dwFileSize + 2,dx

        mov     rFindItem.bIsDir,FALSE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem

        pop     XDX
        pop     XBX

        or      XAX,XAX
        jz      J_ERRORDONE

  J_2:
        INVOKE  FIL4_FIND_NEXT
        jmp     J_5

                                        ; If it is no more files, then go
                                        ; back one level in the DTA_List
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

        mov     WORD PTR rFindItem.szLongName,'..'
        mov     BYTE PTR rFindItem.szLongName + 2,0
        mov     WORD PTR rFindItem.szShortName,'..'
        mov     BYTE PTR rFindItem.szShortName + 2,0
        mov     rFindItem.bIsDir, TRUE

        INVOKE  FIL4_DIRECTORY,0,ADDR rFindItem.szLongName,SET_INFO

        push    XBX
        push    XDX

        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem

        pop     XDX
        pop     XBX

        or      XAX,XAX
        jz      J_ERRORDONE


        jmp     J_2

  J_Done:
        mov     XAX,TRUE
        jmp     J_Out

  J_ERRORDONE:
        xor     XAX,XAX
  J_Out:
        push    XAX
        INVOKE  FIL4_CHANGE_DTA,lpDTA
        INVOKE  FIL4_DIRECTORY,0,Buffer,SET_INFO
        pop     XAX

        ret

  FIL4_FIND_ALL_DTA ENDP

  ;--------------------------------------------------------------------------
  ;
  ; An extended (LFN) FIL4_FIND_ALL.
  ;
  ;--------------------------------------------------------------------------
  FIL4_FIND_ALL_EX  PROC  CODEDISTANCE  USES  XBX XCX XDX XDI SES,
        CallBack:LPBYTE,
        lpParam:DWORD,
        Flag:BOOL,
        Buffer:LPBYTE

        ECHO Warning: Don't know how to determine whether error or no more files
        ECHO -------- Look at the commented out lines at the end of FIL4_FIND_ALL_EX.

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

                                        ; Initiate scanning current directory
  ScanAll:

        INVOKE  ex_FindFirstFile, ADDR cAllFiles, ADDR rFindData, FILE_ATTRIBUTE_NORMAL OR FILE_ATTRIBUTE_READONLY OR FILE_ATTRIBUTE_HIDDEN OR FILE_ATTRIBUTE_SYSTEM OR FILE_ATTRIBUTE_DIRECTORY
        mov     XES[XDI],XAX
        inc     XAX                     ; If returned handle is invalid,
        jz      ErrorScanAll            ; ZF will be set.

  ExamineFFD:

        cmp     rFindData.cFileName,'.' ; Skip directory entries starting
        je      ScanAllNext             ; with '.'

IF FILE_ATTRIBUTE_VOLUME GT 0FFFFh
        .ERR    <The next check will not be correct>
ENDIF
        test    WORD PTR rFindData.dwFileAttributes, FILE_ATTRIBUTE_VOLUME
        jnz     ScanAllNext

                                        ; Fill rFindItem
        INVOKE  STR4_COPY, ADDR rFindData.cFileName, ADDR rFindItem.szLongName
        INVOKE  STR4_COPY, ADDR rFindData.cAlternateFileName, ADDR rFindItem.szShortName

                                        ; If directory found, CD and notify
IF FILE_ATTRIBUTE_DIRECTORY GT 0FFFFh
        .ERR    <The next check will not be correct>
ENDIF
        test    WORD PTR rFindData.dwFileAttributes,FILE_ATTRIBUTE_DIRECTORY
        jnz     NewDirectory

                                        ; If file found, notify
        mov     ax,WORD PTR rFindData.nFileSizeLow
        mov     bx,WORD PTR rFindData.nFileSizeLow + 2

        mov     WORD PTR rFindItem.dwFileSize,ax
        mov     WORD PTR rFindItem.dwFileSize + 2,bx

        mov     rFindItem.bIsDir,FALSE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

  ScanAllNext:

        INVOKE  ex_FindNextFile, XES[XDI], ADDR rFindData
        or      XAX,XAX
        jnz     ExamineFFD
                                        ; Error, see if because of no more
                                        ; files or real error.
  ErrorScanAll:
;        INVOKE  ex_GetLastError
;        cmp     XAX,ERROR_NO_MORE_FILES
;        jne     Error
                                        ; Close current search handle
        INVOKE  ex_FindClose, XES[XDI]
                                        ; Check if this was the starting
                                        ; directory.
        cmp     XDI,pFirstHandle
        mov     XAX,TRUE                ; Shouldn't need to restore original
        je      FunctionEndEx           ; directory, since all went OK.

        sub     XDI,SIZE HANDLE32
                                        ; Backup one directory.
        mov     WORD PTR rFindItem.szLongName,'..'
        mov     BYTE PTR rFindItem.szLongName + 2,0
        INVOKE  FIL4_DIRECTORY,0,ADDR rFindItem.szLongName,SET_INFO
        or      XAX,XAX
        jz      Error
                                        ; Notify about dropping back.
        mov     WORD PTR rFindItem.szShortName,'..'
        mov     BYTE PTR rFindItem.szShortName + 2,0
        mov     rFindItem.bIsDir,TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jnz     ScanAllNext
        jmp     SHORT Error

  NewDirectory:

        cmp     Flag,FALSE
        je      ScanAllNext

        INVOKE  FIL4_DIRECTORY,0,ADDR rFindData.cFileName,SET_INFO
        or      XAX,XAX
        jz      ScanAllNext             ; Someone maybe just nuked it :-)

        mov     rFindItem.bIsDir,TRUE
        INVOKE  FF_PTR PTR CallBack, ADDR rFindItem
        or      XAX,XAX
        jz      Error

        add     XDI,SIZE HANDLE32

        jmp     ScanAll

  Error:
                                        ; Save error information
;        INVOKE  ex_GetLastError
;        push    XAX
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
;        pop     XAX
;        INVOKE  ex_SetLastError,XAX

        xor     XAX,XAX

  FunctionEndEx:

        ret

  FIL4_FIND_ALL_EX  ENDP

  ;--------------------------------------------------------------------------
  ;
  ; And here is FIL4_FIND_ALL itself.
  ;
  ;--------------------------------------------------------------------------

  FIL4_FIND_ALL  PROC  CODEDISTANCE USES XSI XDI SDS,
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

        INVOKE  FIL4_FIND_ALL_EX, CallBack, lpParam, Flag, Buffer

        jmp     SHORT FunctionEnd

  NoLFN:
ENDIF
        INVOKE  FIL4_FIND_ALL_DTA, CallBack, lpParam, Flag, Buffer

  FunctionEnd:

        ret

  FIL4_FIND_ALL  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_FIND_ALL.
ENDIF

UniqueSegmentEnd
END
