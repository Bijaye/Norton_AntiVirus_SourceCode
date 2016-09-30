; Copyright 1997 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:/SYMKRNL/VCS/vxdkrnl.asv   1.7   18 Feb 1998 10:02:26   mdunn  $
;
; Description:
;       This file contains asm-specific code for SYMKrnl's VxD platform.
;
; Contains:
;       SYMKrnl_Control
;       SYMKRNL_SYS_DYNAMIC_DEVICE_INIT (local function)
;       FixUpDOSResult                  (local function)
;       FileCreate()                    (Core API)
;       FileOpen()                      (Core API)
;       FileClose()                     (Core API)
;       FileRead()                      (Core API)
;       FileHugeRead()                  (Core API)
;       FileWrite()                     (Core API)
;       FileHugeWrite()                 (Core API)
;       FileLockRegion()                (Core API)
;       FileUnlockRegion()              (Core API)
;       FileSeek()                      (Core API)
;       FileLength()                    (Core API)
;       FileDelete()                    (Core API)
;       FileCommit()                    (Core API)
;       FileDup()                       (Core API)
;       FileRename()                    (Core API)
;       FileGetAttr()                   (Core API)
;       FileSetAttr()                   (Core API)
;       FileGetTime()                   (Core API)
;       FileSetTime()                   (Core API)
;       DOSGetDate()                    (Core API)
;       DOSGetTime()                    (Core API)
;       FileTimeToDosTime()             (Core API)
;       DosTimeToFileTime()             (Core API)
;       DiskGet()                       (Core API)
;       __DiskGetAllocInfo()            (Core API)
;       __DiskIsNetwork()               (Core API)
;       __DiskIsRemovable()             (Core API)
;       TrueNameGet()                   (Core API)
;       DirMake()                       (Core API)
;       DirGet()                        (Core API)
;       DTAGet()                        (Core API)
;       DTASet()                        (Core API)
;       __FileFindFirstOld()            (Core API)
;       __FileFindNextOld()             (Core API)
;       __FileFindFirstNew()            (Core API)
;       __FileFindNextNew()             (Core API)
;       __FileFindCloseNew()            (Core API)
;       VxDNameReturnShortName()        (Core API)
;       VxDNameReturnLongName()         (Core API)
;       VolumeInformationGet()          (Core API)
;       DiskLockPhysicalVolume()        (Core API)
;       DiskUnlockPhysicalVolume()      (Core API)
;       DiskGetCurrentLockState()       (Core API)
;       UniToBCS()                      (IFSMGR wrap)
;       BCSToUni()                      (IFSMGR wrap)
;       UniToUpper()                    (IFSMGR wrap)
;       IOS_Find_Int13_Drive()          (IOS wrap)
;
; Note:
;       All Core APIs may destroy eax, ecx, edx and flag registers, unless
;       otherwise stated in the function header.  The only exception is the
;       direction flag, which is either cleared or not touched at all.
;
;****************************************************************************
; $Log:   S:/SYMKRNL/VCS/vxdkrnl.asv  $
;  
;     Rev 1.7   18 Feb 1998 10:02:26   mdunn
;  Ported ROSWELL and ROSNEC changes from the K branch.
;  
;     Rev 1.6   03 Jun 1997 14:25:02   RStanev
;  Enhanced FileCreate() to support uAttr flag 0x80000000.
;
;     Rev 1.5   17 Mar 1997 19:27:18   RStanev
;  When generating control procedure with the Begin_Control_Dispatch and
;  End_Control_Dispatch macros we should use only the Control_Dispatch macro
;  to service messages.
;
;     Rev 1.4   13 Mar 1997 12:58:48   RStanev
;  Made bVxDIsDynaloaded public.
;
;     Rev 1.3   05 Mar 1997 16:20:34   RSTANEV
;  Updated file header.
;
;     Rev 1.2   05 Mar 1997 16:18:26   RSTANEV
;  Added bVxDIsDynaloaded.
;
;     Rev 1.1   06 Jan 1997 18:22:08   RSTANEV
;  Changed the MakeCPROC to set the current language to SYSCALL.
;
;     Rev 1.0   06 Jan 1997 16:00:12   RSTANEV
;  Initial revision.
;
;****************************************************************************

        .XLIST
        INCLUDE platform.inc
        INCLUDE ios.inc
        INCLUDE blockdev.inc
        INCLUDE ifsmgr.inc
        .LIST

;----------------------------------------------------------------------------
;
; Define general VxD stuff...
;
; Keep the DDB, even though SYMKrnl is a static library.  This will make
; linking of the control functions much easier.
;
;----------------------------------------------------------------------------
VXD_LOCKED_CODE_SEG

Declare_Virtual_Device  SYMKrnl, 0, 0,                                  \
                        SYMKrnl_Control, UNDEFINED_DEVICE_ID,           \
                        UNDEFINED_INIT_ORDER, 0, 0

Begin_Control_Dispatch  SYMKrnl
        Control_Dispatch  SYS_DYNAMIC_DEVICE_INIT, SYMKRNL_SYS_DYNAMIC_DEVICE_INIT
End_Control_Dispatch  SYMKrnl

VXD_LOCKED_CODE_ENDS

;----------------------------------------------------------------------------
;
; Define bVxDIsDynaloaded.
;
;----------------------------------------------------------------------------
VXD_PAGEABLE_DATA_SEG

        PUBLIC  _bVxDIsDynaloaded, _bFileSharing

_bVxDIsDynaloaded  DD  0
_bFileSharing      DD  TRUE

VXD_PAGEABLE_DATA_ENDS

;----------------------------------------------------------------------------
;
; SYMKRNL_SYS_DYNAMIC_DEVICE_INIT services the SYS_DYNAMIC_DEVICE_INIT
; message.
;
;----------------------------------------------------------------------------
VXD_PAGEABLE_CODE_SEG

BeginProc  SYMKRNL_SYS_DYNAMIC_DEVICE_INIT
        inc     _bVxDIsDynaloaded
        clc
        ret
EndProc  SYMKRNL_SYS_DYNAMIC_DEVICE_INIT

;----------------------------------------------------------------------------
;
; Define macros for creating VxD service functions.
;
;----------------------------------------------------------------------------

StartProc  MACRO  procname, procargs, langtype

  set_&langtype&_calling_convention

  PUBLIC  procname

  procname  PROC , procargs

ENDM

StopProc  MACRO  procname

  procname  ENDP

ENDM

set_ccall_calling_convention  MACRO
  OPTION LANGUAGE:C
ENDM

set_scall_calling_convention  MACRO
  OPTION LANGUAGE:STDCALL
ENDM

set__calling_convention  MACRO
  set_scall_calling_convention
ENDM

;****************************************************************************
;
; External function prototypes.
;
;****************************************************************************
        EXTRN   _GetLastError@0: PROC
        EXTRN   _SetLastError@4: PROC

;****************************************************************************
;
; Start function definitions.
;
;****************************************************************************

;----------------------------------------------------------------------------
;
; FixUpDOSResult  PROC  NEAR
;
; This procedure should be called immediately after a return from Int 21h.
; FixUpDOSResult will examine cf (0 - ok, 1 - err) and will set
; appropriate return values and error variables.
;
; Input:
;       back from a standard DOS function (cf set / clear, ax result)
;
; Output:
;       df cleared;
;       if cf cleared on entry:
;               eax = (DWORD)ax
;               Instanced last error unchanged
;       if cf set on entry:
;               eax = -1
;               Instanced last error set to error code
;
; Note: This function preserves all registers, except the ones specified
;       in the output section.
;
;----------------------------------------------------------------------------
FixUpDOSResult  PROC  NEAR

        cld
        movzx   eax,ax
        jnc     fixupdosresultnodoserror
        pushf
        pushad
        push    eax
        call    _SetLastError@4
        popad
        popf
        mov     eax,-1
  fixupdosresultnodoserror:
        ret

FixUpDOSResult  ENDP

;----------------------------------------------------------------------------
;
; HFILE FileCreate ( PCSTR pszFileName, UINT uAttr );
;
; Creates the file pszFileName, and gives it attributes uAttr.
; Returns file handle, -1 if error.
;
; If the highest bit of uAttr is set, then the file creation will fail if
; the file exists.  If the 716Ch API is not supported, then this function
; will ignore the flag.
;
;----------------------------------------------------------------------------
StartProc  FileCreate, < pszFileName:DWORD, uAttr:UINT >

        push    ebx
        push    esi
        mov     esi,pszFileName
        mov     ebx,2
        movzx   ecx,WORD PTR uAttr
        mov     edx,0012h
        shl     uAttr,1
        jnc     filecreatenospecialflag
        mov     edx,0010h
  filecreatenospecialflag:
        shr     uAttr,1
        mov     eax,716Ch
        stc
        VxDInt  21h
        jnc     donefilecreategetresult
        cmp     ax,7100h
        stc
        jne     donefilecreategetresult
        mov     edx,pszFileName
        mov     ecx,uAttr
        mov     ah,3Ch
        VxDInt  21h
  donefilecreategetresult:
        call    FixUpDOSResult
        pop     esi
        pop     ebx
        ret

StopProc  FileCreate

;----------------------------------------------------------------------------
;
; HFILE FileOpen ( PCSTR pszFileName, UINT uMode );
;
; Opens the file pszFileName, open mode uMode.
; Returns file handle, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileOpen, < pszFileName:DWORD, uMode:UINT >

        push    ebx
        push    esi
        mov     esi,pszFileName
        mov     ebx,uMode
        cmp     _bFileSharing, TRUE
        je      FileSharingEnabled
        and     ebx, 7h
FileSharingEnabled:

        xor     ecx,ecx                 ; Will fail, if not 0 for OPEN
        mov     edx,1
        mov     eax,716Ch
        stc
        VxDInt  21h
        jnc     donefileopen
        cmp     ax,7100h
        stc
        jne     donefileopen
        mov     edx,pszFileName
        mov     ah,3Dh
        mov     al,BYTE PTR uMode
        VxDInt  21h
  donefileopen:
        call    FixUpDOSResult
        pop     esi
        pop     ebx
        ret

StopProc  FileOpen

;----------------------------------------------------------------------------
;
; HFILE FileClose ( HFILE hFile );
;
; Closes file handle hFile.
; Returns 0 for success, -1 for error.
;
;----------------------------------------------------------------------------
StartProc  FileClose, <_hFile:DWORD>

        push    ebx
        mov     ebx,_hFile
        mov     ah,3Eh
        VxDInt  21h
        jc      donefileclose
        xor     eax,eax
  donefileclose:
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  FileClose

;----------------------------------------------------------------------------
;
; UINT FileRead ( HFILE hFile, PVOID pBuffer, UINT uBytes );
;
; Reads uBytes from hFile and places them in pBuffer.
; Returns bytes read, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileRead, < _hFile:DWORD, pBuffer:DWORD, uBytes:UINT >

        push    ebx
        mov     eax,-1
        mov     ecx,uBytes
        cmp     ecx,0FFFFh
        ja      donefileread            ; no need to set LastError
        mov     ebx,_hFile
        mov     edx,pBuffer
        mov     ah,3Fh
        VxDInt  21h
        call    FixUpDOSResult
  donefileread:
        pop     ebx
        ret

StopProc  FileRead

;----------------------------------------------------------------------------
;
; DWORD FileHugeRead ( HFILE hFile, PVOID pBuffer, DWORD dwBytes );
;
; Reads dwBytes from hFile and places them in pBuffer.
; Returns bytes read, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileHugeRead, < _hFile:DWORD, pBuffer:DWORD, dwBytes:DWORD >

        push    ebx
        xor     edx,edx
  hugereadmore:
        mov     eax,dwBytes
        test    eax,eax
        jz      hugereadall
        mov     ebx,0FFFFh - 10h
        cmp     eax,ebx
        jbe     hugereadmax
        mov     eax,ebx
  hugereadmax:
        push    eax
        push    edx
        INVOKE  FileRead, _hFile, pBuffer, eax
        add     pBuffer,eax
        sub     dwBytes,eax
        pop     edx
        pop     ebx
        cmp     eax,-1
        je      hugereaderror
        add     edx,eax
        cmp     eax,ebx
        je      hugereadmore
  hugereadall:
        mov     eax,edx
  hugereaderror:
        pop     ebx
        ret

StopProc  FileHugeRead

;----------------------------------------------------------------------------
;
; UINT FileWrite ( HFILE hFile, PVOID pBuffer, UINT uBytes );
;
; Writes uBytes from pBuffer to hFile.
; Returns bytes written, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileWrite, < _hFile:DWORD, pBuffer:DWORD, uBytes:UINT >

        push    ebx
        mov     eax,-1
        mov     ecx,uBytes
        cmp     ecx,0FFFFh
        ja      donefilewrite           ; no need to set LastError
        mov     ebx,_hFile
        mov     edx,pBuffer
        mov     ah,40h
        VxDInt  21h
        call    FixUpDOSResult
  donefilewrite:
        pop     ebx
        ret

StopProc  FileWrite

;----------------------------------------------------------------------------
;
; DWORD FileHugeWrite ( HFILE hFile, PVOID pBuffer, DWORD dwBytes );
;
; Writes dwBytes from pBuffer to hFile.
; Returns bytes written, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileHugeWrite, < _hFile:DWORD, pBuffer:DWORD, dwBytes:DWORD >

        push    ebx
        xor     edx,edx
  hugewritemore:
        mov     eax,dwBytes
        test    eax,eax
        jz      hugewriteall
        mov     ebx,0FFFFh - 10h
        cmp     eax,ebx
        jbe     hugewritemax
        mov     eax,ebx
  hugewritemax:
        push    eax
        push    edx
        INVOKE  FileWrite, _hFile, pBuffer, eax
        add     pBuffer,eax
        sub     dwBytes,eax
        pop     edx
        pop     ebx
        cmp     eax,-1
        je      hugewriteerror
        add     edx,eax
        cmp     eax,ebx
        je      hugewritemore
  hugewriteall:
        mov     eax,edx
  hugewriteerror:
        pop     ebx
        ret

StopProc  FileHugeWrite

;----------------------------------------------------------------------------
;
; UINT FileLockRegion ( HFILE hHandle, LONG lOffset, LONG lLength );
;
; Locks a portion of file hHandle, starting at position lOffset and lLength
; bytes in size.
; Returns 0 if OK or -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileLockRegion, < _hFile:DWORD, lOffset:DWORD, lLength:DWORD >

        push    ebx
        push    esi
        push    edi
        mov     eax,5C00h
        mov     ebx,_hFile
        mov     ecx,lOffset
        mov     edx,ecx
        shr     ecx,10h
        mov     esi,lLength
        mov     edi,esi
        shr     esi,10h
        VxDInt  21h
        jc      donefilelock
        xor     eax,eax
  donefilelock:
        call    FixUpDOSResult
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  FileLockRegion

;----------------------------------------------------------------------------
;
; UINT FileUnlockRegion ( HFILE hHandle, LONG lOffset, LONG lLength );
;
; Unlocks a portion of file hHandle, starting at position lOffset and
; lLength bytes in size.
; Returns 0 if OK or -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileUnlockRegion, < _hFile:DWORD, lOffset:DWORD, lLength:DWORD >

        push    ebx
        push    esi
        push    edi
        mov     eax,5C01h
        mov     ebx,_hFile
        mov     ecx,lOffset
        mov     edx,ecx
        shr     ecx,10h
        mov     esi,lLength
        mov     edi,esi
        shr     esi,10h
        VxDInt  21h
        jc      donefileunlock
        xor     eax,eax
  donefileunlock:
        call    FixUpDOSResult
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  FileUnlockRegion

;----------------------------------------------------------------------------
;
; LONG FileSeek ( HFILE hHandle, LONG lOffset, int iOrigin );
;
; Seeks lOffset bytes from iOrigin in hFile.
; Returns new file position or -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileSeek, < _hFile:DWORD, lOffset:DWORD, iOrigin:UINT >

        push    ebx
        mov     ebx,_hFile
        mov     dx,WORD PTR lOffset
        mov     cx,WORD PTR lOffset + 2
        mov     al,BYTE PTR iOrigin
        mov     ah,42h
        VxDInt  21h
        jnc     fileseekok
        call    FixUpDOSResult
        jmp     SHORT donefileseek
  fileseekok:
        call    FixUpDOSResult
        shl     edx,10h
        or      eax,edx
  donefileseek:
        pop     ebx
        ret

StopProc  FileSeek

;----------------------------------------------------------------------------
;
; LONG FileLength (HFILE hFile);
;
; Returns the length of a file or -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileLength, < _hFile:DWORD >

        LOCAL   dwCurrentPos:DWORD
        LOCAL   dwFileLength:DWORD

        push    ebx
        mov     dwFileLength,-1
        ;
        ; Get current file position
        ;
        mov     ebx,_hFile
        xor     edx,edx
        mov     ecx,edx
        mov     eax,4201h
        VxDInt  21h
        jc      donefilelength
        mov     WORD PTR dwCurrentPos,ax
        mov     WORD PTR dwCurrentPos + 2,dx
        ;
        ; Seek to the end of file and remember pointer
        ;
        mov     ebx,_hFile
        xor     edx,edx
        mov     ecx,edx
        mov     eax,4202h
        VxDInt  21h
        jc      donefilelength
        mov     WORD PTR dwFileLength,ax
        mov     WORD PTR dwFileLength + 2,dx
        ;
        ; Seek back to original position
        ;
        mov     ebx,_hFile
        mov     dx,WORD PTR dwCurrentPos
        mov     cx,WORD PTR dwCurrentPos + 2
        mov     eax,4200h
        VxDInt  21h
        jnc     donefilelength
        mov     dwFileLength,-1
  donefilelength:
        call    FixUpDOSResult
        mov     eax,dwFileLength
        pop     ebx
        ret

StopProc  FileLength

;----------------------------------------------------------------------------
;
; UINT FileDelete ( PCSTR pszFile );
;
; Deletes the file pszFile.
; Returns 0 if OK, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileDelete, < pszFile:DWORD >

        push    ebx
        push    esi
        mov     edx,pszFile
        xor     esi,esi
        mov     eax,7141h
        stc
        VxDInt  21h
        jnc     filedeleteok
        cmp     ax,7100h
        stc
        jne     donefiledelete
        mov     ah,41h
        VxDInt  21h
        jc      donefiledelete
  filedeleteok:
        xor     eax,eax
  donefiledelete:
        call    FixUpDOSResult
        pop     esi
        pop     ebx
        ret

StopProc  FileDelete

;----------------------------------------------------------------------------
;
; UINT FileRename ( PCSTR pszOldFile, PCSTR lpszNewFile );
;
; Renames the name of the file.
; Returns 0 if OK, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileRename, < pszOldFile:DWORD, pszNewFile >

        push    ebx
        push    edi
        mov     edx,pszOldFile
        mov     edi,pszNewFile
        mov     eax,7156h
        stc
        VxDInt  21h
        jnc     filerenameok
        cmp     ax,7100h
        stc
        jne     donefilerename
        mov     ah,56h
        VxDInt  21h
        jc      donefilerename
  filerenameok:
        xor     eax,eax
  donefilerename:
        call    FixUpDOSResult
        pop     edi
        pop     ebx
        ret

StopProc  FileRename

;----------------------------------------------------------------------------
;
; UINT FileCommit ( HFILE hFile );
;
; Commits the file's handle to disk.
; Returns 0 if OK, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileCommit, < _hFile:DWORD >

        push    ebx
        mov     ebx, _hFile
        mov     eax, 6800h
        VxDInt  21h
        call    FixUpDOSResult
        pop     ebx
        cld
        ret

StopProc  FileCommit

;----------------------------------------------------------------------------
;
; UINT FileDup ( LPHFILE lphDupFile, HFILE hFile );
;
; Dups a file's handle.
; Returns 0 if OK, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileDup, < lphDupFile:DWORD, _hFile:DWORD >

        push    ebx
        mov     ebx, _hFile
        mov     eax, 4500h
        VxDInt  21h
        mov     ebx, lphDupFile
        mov     DWORD PTR [ebx], eax
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  FileDup

;----------------------------------------------------------------------------
;
; UINT FileGetAttr ( PCSTR pszFileName, PUINT puAttr );
;
; Stores the attributes of pszFileName at puAttr.
;
; Returns 0 if ok, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileGetAttr, < pszFileName:DWORD, puAttr:DWORD >

        push    ebx
        mov     edx,pszFileName
        xor     bl,bl
        mov     eax,7143h
        stc
        VxDInt  21h
        jnc     donefilegetattr
        cmp     ax,7100h
        stc
        jne     donefilegetattr
        mov     eax,4300h
        VxDInt  21h
  donefilegetattr:
        call    FixUpDOSResult
        jc      endfilegetattr
        movzx   ecx,cx
        mov     edx,puAttr
        mov     [edx],ecx
        xor     eax,eax
  endfilegetattr:
        pop     ebx
        ret

StopProc  FileGetAttr

;----------------------------------------------------------------------------
;
; UINT FileSetAttr ( PCSTR pszFileName, UINT uAttr );
;
; Sets the attributes of pszFileName to uAttr.
;
; Returns 0 if ok, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  FileSetAttr, < pszFileName:DWORD, uAttr:UINT >

        push    ebx
        mov     edx,pszFileName
        mov     bl,1
        mov     ecx,uAttr
        mov     eax,7143h
        stc
        VxDInt  21h
        jnc     filesetattrok
        cmp     ax,7100h
        stc
        jne     donefilesetattr
        mov     eax,4301h
        VxDInt  21h
        jc      donefilesetattr
  filesetattrok:
        xor     eax,eax
  donefilesetattr:
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  FileSetAttr

;----------------------------------------------------------------------------
;
; UINT FileGetTime ( HFILE hFile, UINT uType, PFILETIME pFileTime );
;
; Gets hFile's date and time of uType.
;
; Returns 0 if ok, -1 if error.
;
; Note: If dwHighDateTime is 0, then dwLowDateTime contains the date/time
;       in standard DOS format (HIWORD is date, LOWORD is time.)  This
;       format will always be used in DOS versions which do not support
;       the 71A7h function or if the natural FILETIME format contains 0
;       in its dwHighDateTime member.
;
;----------------------------------------------------------------------------
StartProc  FileGetTime, < _hFile:DWORD, uType:UINT, pFileTime:DWORD >

        LOCAL   dwLowDateTimeUseDOS:DWORD

        push    ebx
        push    esi
        push    edi
        mov     eax,5700h
        mov     al,BYTE PTR uType
        xor     esi,esi
        mov     ebx,_hFile
        VxDInt  21h
        call    FixUpDOSResult
        jc      donefilegettime
        mov     [WORD PTR dwLowDateTimeUseDOS], cx
        mov     [WORD PTR dwLowDateTimeUseDOS + 2], dx
        mov     ebx,esi
        shl     ebx,8
        inc     ebx
        mov     edi,pFileTime
        mov     ax,71A7h
        VxDInt  21h
        mov     eax,0                   ; Caller doesn't care if it failed
        jc      filegettimeusedos
        cmp     [edi._FILETIME.dwHighDateTime],eax
        jne     donefilegettime
  filegettimeusedos:
        mov     ebx,dwLowDateTimeUseDOS
        mov     [edi._FILETIME.dwLowDateTime],ebx
  donefilegettime:
        pop     edi
        pop     esi
        pop     ebx
        cld
        ret

StopProc  FileGetTime

;----------------------------------------------------------------------------
;
; UINT FileSetTime ( HFILE hFile, UINT uType, PFILETIME pFileTime );
;
; Sets hFile's date and time of uType.
;
; Returns 0 if ok, -1 if error.
;
; Note: If dwHighDateTime is 0, then the function assumes that
;       dwLowDateTime contains the date/time in standard DOS format
;       (HIWORD is date, LOWORD is time)
;
;----------------------------------------------------------------------------
StartProc  FileSetTime, < _hFile:DWORD, uType:UINT, pFileTime:DWORD >

        push    ebx
        push    esi
        push    edi
        xor     ebx,ebx
        mov     esi,pFileTime
        cmp     [esi._FILETIME.dwHighDateTime],ebx
        je      filesettimespecial
        mov     eax,71A7h
        VxDInt  21h
        call    FixUpDOSResult
        jc      donefilesettime
        jnc     filesettimehavetime
  filesettimespecial:
        mov     cx,[WORD PTR esi._FILETIME.dwLowDateTime]
        mov     dx,[WORD PTR esi._FILETIME.dwLowDateTime + 2]
  filesettimehavetime:
        mov     ebx,_hFile
        mov     eax,5701h
        add     al,BYTE PTR uType
        VxDInt  21h
        call    FixUpDOSResult
        jc      donefilesettime
        xor     eax,eax
  donefilesettime:
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  FileSetTime

;----------------------------------------------------------------------------
;
; BYTE DOSGetDate ( UINT *pMonth, UINT *pDay, UINT *pYear );
;
; Gets the current DOS date and sets the variables at pMonth, pDay and
; pYear.
; Returns the current day of the week.
;
; Note: The return value will be correct if it was treated as WORD or DWORD.
; Note: The return value will be 0 if IFS' services were used
;
;----------------------------------------------------------------------------
StartProc  DOSGetDate, < pMonth:DWORD, pDay:DWORD, pYear:DWORD >

        pushad
        stc
        VxDcall IFSMgr_Get_Version
        jc      dosgetdate_usedos
        VxDcall IFSMgr_Get_DOSTime
        shr     eax,10h
        mov     ecx,eax
        and     eax,1Fh
        mov     esi,pDay
        mov     [esi],eax
        mov     eax,ecx
        shr     eax,5
        and     eax,0Fh
        mov     esi,pMonth
        mov     [esi],eax
        shr     ecx,9
        add     ecx,1980d
        mov     esi,pYear
        mov     [esi],ecx
        xor     eax,eax
        jmp     short dosgetdate_end
  dosgetdate_usedos:
        mov     ah,2Ah
        VxDInt  21h
        movzx   ecx,cx
        mov     esi,pYear
        mov     [esi],ecx
        movzx   cx,dh
        mov     esi,pMonth
        mov     [esi],ecx
        mov     cl,dl
        mov     esi,pDay
        mov     [esi],ecx
        movzx   eax,al
  dosgetdate_end:
        mov     [esp.Pushad_Struc.Pushad_EAX],eax
        popad
        cld
        ret

StopProc  DOSGetDate

;----------------------------------------------------------------------------
;
; VOID DOSGetTime ( UINT *pHour, UINT *pMin, UINT *pSec, UINT *pHund );
;
; Gets the current DOS time and sets the variables at pHour, pMin, pSec
; and pHund.
;
;----------------------------------------------------------------------------
StartProc  DOSGetTime, < pHour:DWORD, pMin:DWORD, pSec:DWORD, pHund:DWORD >

        pushad
        stc
        VxDcall IFSMgr_Get_Version
        jc      dosgettime_usedos
        VxDcall IFSMgr_Get_DOSTime
        movzx   ecx,ax
        and     eax,1Fh
        shl     eax,1
        cmp     edx,1000d
        jb      dosgettime_not2ndsec
        sub     edx,1000d
        inc     eax
  dosgettime_not2ndsec:
        mov     esi,pSec
        mov     [esi],eax
        mov     eax,ecx
        shr     eax,5
        and     eax,3Fh
        mov     esi,pMin
        mov     [esi],eax
        shr     ecx,0Bh
        mov     esi,pHour
        mov     [esi],ecx
        mov     eax,edx
        xor     edx,edx
        mov     ecx,10d
        div     ecx
        jmp     short dosgettime_end
  dosgettime_usedos:
        mov     ah,2Ch
        VxDInt  21h
        movzx   eax,ch
        mov     esi,pHour
        mov     [esi],eax
        mov     al,cl
        mov     esi,pMin
        mov     [esi],eax
        mov     al,dh
        mov     esi,pSec
        mov     [esi],eax
        mov     al,dl
  dosgettime_end:
        mov     esi,pHund
        mov     [esi],eax
        popad
        cld
        ret

StopProc  DOSGetTime

;----------------------------------------------------------------------------
;
; BOOL FileTimeToDosTime ( PFILETIME pFileTime, WORD *pwDate, WORD *pwTime );
;
; Converts Date/Time from FILETIME format to DOS format.
;
; Returns TRUE if OK, FALSE if error.
;
; Note: If dwHighDateTime is 0, then the function assumes that
;       dwLowDateTime contains the date/time in standard DOS format
;       (HIWORD is date, LOWORD is time) and returns those values without
;       conversion.
;
;----------------------------------------------------------------------------
StartProc  FileTimeToDosTime, < pFileTime:DWORD, pwDate:DWORD, pwTime:DWORD >

        push    ebx
        push    esi
        xor     ebx,ebx
        mov     esi,pFileTime
        cmp     [esi._FILETIME.dwHighDateTime],ebx
        je      filetimetodostimeusedos
        mov     eax,71A7h
        stc
        VxDInt  21h
        call    FixUpDOSResult
        inc     eax
        jc      donefiletimetodostime   ; Caller shouldn't do that!
        jnc     filetimetodostimegottime
  filetimetodostimeusedos:
        mov     cx,[WORD PTR esi._FILETIME.dwLowDateTime]
        mov     dx,[WORD PTR esi._FILETIME.dwLowDateTime + 2]
  filetimetodostimegottime:
        mov     esi,pwTime
        mov     [esi],cx
        mov     esi,pwDate
        mov     [esi],dx
        mov     eax,1
  donefiletimetodostime:
        pop     esi
        pop     ebx
        ret

StopProc  FileTimeToDosTime

;----------------------------------------------------------------------------
;
; BOOL DosTimeToFileTime ( WORD wDate, WORD wTime, PFILETIME pFileTime );
;
; Converts Date/Time from DOS format to FILETIME format.
;
; Returns TRUE if OK, FALSE if error.
;
; Note: If dwHighDateTime is 0, then dwLowDateTime contains the date/time
;       in standard DOS format (HIWORD is date, LOWORD is time.)  This
;       format will always be used in DOS versions which do not support
;       the 71A7h function or if the natural FILETIME format contains 0
;       in its dwHighDateTime member.
;
;----------------------------------------------------------------------------
StartProc  DosTimeToFileTime, < wDate:WORD, wTime:WORD, pFileTime:DWORD >

        push    ebx
        push    edi
        mov     eax,71A7h
        mov     ebx,1
        mov     cx,wTime
        mov     dx,wDate
        mov     edi,pFileTime
        stc
        VxDInt  21h
        jnc     dostimetofiletimecheck0
        cmp     ax,7100h
        je      dostimetofiletimeusedos
        stc
        call    FixUpDOSResult
        mov     eax,0
        jc      donedostimetofiletime
  dostimetofiletimecheck0:
        cmp     [edi._FILETIME.dwHighDateTime],0
        jne     dostimetofiletimesupported
  dostimetofiletimeusedos:
        mov     cx,wTime
        mov     dx,wDate
        mov     [WORD PTR edi._FILETIME.dwLowDateTime], cx
        mov     [WORD PTR edi._FILETIME.dwLowDateTime + 2], dx
        mov     [edi._FILETIME.dwHighDateTime],0
  dostimetofiletimesupported:
        mov     eax,1
  donedostimetofiletime:
        pop     edi
        pop     ebx
        cld
        ret

StopProc  DosTimeToFileTime

;----------------------------------------------------------------------------
;
; UINT TrueNameGet ( PCSTR pszRelativeName, PSTR pszTrueName );
;
; Returns in pszTrueName a canonicalized version of pszRelativeName.
; Returns 0 if OK, -1 if error.
;
;----------------------------------------------------------------------------
StartProc  TrueNameGet, < pszRelativeName:DWORD, pszTrueName:DWORD >

        push    ebx
        push    esi
        push    edi
        mov     eax,7160h
        mov     ecx,0
        mov     esi,pszRelativeName
        mov     edi,pszTrueName
        VxDInt  21h
        jnc     donetruenameget
        cmp     ax,7100h
        stc
        jne     keeperrortruenameget
        mov     ah,60h
        VxDInt  21h
        jc      keeperrortruenameget
  donetruenameget:
        xor     eax,eax
  keeperrortruenameget:
        call    FixUpDOSResult
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  TrueNameGet

;----------------------------------------------------------------------------
;
; BYTE DiskGet ( VOID );
;
; Gets the current DOS drive letter.
;
;----------------------------------------------------------------------------
StartProc  DiskGet, < >

        push    ebx
        mov     eax, 1900h
        VxDInt  21h
        add     al, 'A'
        pop     ebx
        cld
        ret

StopProc  DiskGet

;----------------------------------------------------------------------------
;
; BOOL __DiskIsNetwork(BYTE byDriveLetter);
;
; Tests the uppercase drive letter to see if it is a network drive.
;
;----------------------------------------------------------------------------
StartProc  __DiskIsNetwork, <byDriveLetter:BYTE>

        push    ebx
        mov     eax, 4409h
        mov     bl, byDriveLetter       ; Get the uppercase drive letter
        sub     bl,'A' - 1              ; Adjust drive letter to number
        xor     dx,dx                   ; Set to nul state
        VxDInt  21h
        test    dx, 1000h               ; Is it a networked drive?
        jnz     remoteDrive             ; Drive is remote

notRemote:
        xor     eax, eax                ; Return FALSE (local)
        jmp     short locRet            ; return to caller

remoteDrive:
        mov     eax, TRUE               ; Return TRUE (remote)

locRet:
        pop     ebx
        cld
        ret

StopProc  __DiskIsNetwork

;----------------------------------------------------------------------------
;
; BOOL __DiskIsRemovable(BYTE byDriveLetter);
;
; Tests the uppercase drive letter to see if it is a removable drive.
;
;----------------------------------------------------------------------------
StartProc  __DiskIsRemovable, <byDriveLetter:BYTE>

        push    ebx
        mov     eax, 4408h
        mov     bl, byDriveLetter       ; Get the uppercase drive letter
        sub     bl,'A' - 1              ; Adjust drive letter to number
        VxDInt  21h
        jc      notRemovable

        or      ax, ax                  ; Is it removable?
        jnz     notRemovable

        mov     eax, TRUE               ; Yes.
        jmp     short locRetIsRemovable

notRemovable:
        xor     eax, eax                ; No.

locRetIsRemovable:
        pop     ebx
        cld
        ret

StopProc  __DiskIsRemovable

;----------------------------------------------------------------------------
;
; BOOL __DiskGetAllocInfo (BYTE byDriveLetter,
;                         LPBYTE lpbyMediaDescriptor,
;                         LPBYTE lpbySectorsPerCluster,
;                         LPWORD lpwTotalClusters,
;                         LPWORD lpwBytesPerSector)
;
; Gets drive allocation information for an uppercase drive letter.
;
; NOTE: The media descriptor byte value will ALWAYS be set to 0.  Can't
; get it using this function.
;
;----------------------------------------------------------------------------
StartProc __DiskGetAllocInfo, < byDriveLetter:BYTE, \
                                lpbyMediaDescriptor:DWORD, \
                                lpbySectorsPerCluster:DWORD, \
                                lpwTotalClusters:DWORD, \
                                lpwBytesPerSector:DWORD >

        push    ebx

        mov     dl, byDriveLetter       ; Get uppercase drive letter
        sub     dl, 'A' - 1             ; Convert to a drive NUMBER

        xor     cx, cx                  ; Clear for testing later

        clc                             ; Set no error to OFF
        mov     eax, 1C00h              ; Get allocation info function
        VxDInt  21h
        jc      notValid                ; Not valid, inform user of error
                                        ;
        cmp     al, 0FFh                ; Is AL error state?
        je      notValid                ; Yes, error
                                        ;
        or      cx, cx                  ; Is CX still 0?
        je      notValid                ; Yes, error
                                        ;
        test    cx, 1                   ; Bytes/Sector MUST be even
        jnz     notValid                ; Not even, thus error
                                        ;
; Save returned drive information
                                        ;
        mov     ebx, lpbyMediaDescriptor
        mov     BYTE PTR [ebx], 0       ; Always set to 0
        mov     ebx, lpbySectorsPerCluster
        mov     BYTE PTR [ebx], al
        mov     ebx, lpwBytesPerSector
        mov     WORD PTR [ebx], cx
        mov     ebx, lpwTotalClusters
        mov     WORD PTR [ebx], dx
                                        ;
        mov     eax, TRUE               ; Set to valid disk information
        jmp     short locRetAllocInfo   ; Exit

notValid:
        xor     eax,eax                 ; Set to invalid disk information

locRetAllocInfo:
        pop     ebx
        cld
        ret

StopProc  __DiskGetAllocInfo

;----------------------------------------------------------------------------
;
; UINT DirMake(LPCSTR lpszDir);
;
; Makes the directory.
;
;----------------------------------------------------------------------------
StartProc  DirMake, <pszDir:DWORD>

        push    ebx
        mov     edx,pszDir
        mov     eax,7139h
        stc
        VxDInt  21h
        jnc     doneDirMake
        cmp     ax,7100h
        jne     doneDirMake

        mov     eax,3900h
        VxDInt  21h

doneDirMake:
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  DirMake

;----------------------------------------------------------------------------
;
; UINT DirGet ( BYTE chDrive, PSTR pszDestination );
;
; Returns the current directory of drive chDrive.
; Returns 0 if OK, -1 if error.
;
; Note:
;       The strings in this function should not be translated.
;
;----------------------------------------------------------------------------
StartProc  DirGet, < chDrive:BYTE, pszDestination:DWORD >

        push    esi
        mov     esi,pszDestination
        mov     edx,'\:?'
        mov     dl,chDrive
        cmp     dl,'a'
        jb      dirgetnouppercasing
        sub     dl,'a' - 'A'
  dirgetnouppercasing:
        mov     [esi],edx
        sub     dl,'A' - 1
        add     esi,3
        mov     eax,7147h
        stc
        VxDInt  21h
        jnc     donedirget
        cmp     ax,7100h
        stc
        jne     donedirget
        mov     ah,47h
        VxDInt  21h
  donedirget:
        jc      donedirgetkeeperror
        xor     eax,eax
  donedirgetkeeperror:
        call    FixUpDOSResult
        pop     esi
        ret

StopProc  DirGet

;----------------------------------------------------------------------------
;
; VOID DTASet(LPCVOID lpDTA);
;
; Sets the DTA.
;
;----------------------------------------------------------------------------
StartProc  DTASet, <lpDTA:DWORD>

        push    ebx

        mov     edx, lpDTA
        MOV     eax, 1A00h                  ; Set the disk transfer address
        VxDInt  21h

        pop     ebx
        cld
        ret

StopProc  DTASet

;----------------------------------------------------------------------------
;
; LPVOID DTAGet(VOID);
;
; Gets the DTA.
;
;----------------------------------------------------------------------------
StartProc  DTAGet, < >

        push    ebx

        MOV     eax, 2F00h              ; Set the disk transfer address
        VxDInt  21h

        mov     eax, ebx                ; Get DTA

        pop     ebx
        cld
        ret

StopProc  DTAGet

;----------------------------------------------------------------------------
;
; WORD __FileFindFirstOld(LPSTR lpszFile, UINT uAttr);
;
; Finds the first file, using old findFirst api.
;
;----------------------------------------------------------------------------
StartProc  __FileFindFirstOld, <lpszFile:DWORD, uAttr:DWORD >

        push    ebx

        mov     edx, lpszFile
        mov     ecx, uAttr
        MOV     eax, 4E00h
        VxDInt  21h
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  __FileFindFirstOld

;----------------------------------------------------------------------------
;
; BOOL __FileFindFirstNew(LPSTR lpszFile, UINT uAttr, LPFINDDATA lpFindData, HFIND FAR *lphFind);
;
; Finds the first file, using new findFirst api.
;
;----------------------------------------------------------------------------
StartProc  __FileFindFirstNew, <lpszFile:DWORD, uAttr:DWORD, lpFindData:DWORD, lphFind:DWORD>

        LOCAL   bSupported:BOOL

        push    ebx
        push    esi
        push    edi

        mov     bSupported, TRUE
        mov     eax,714Eh               ; Extended Find First
        mov     ecx,uAttr               ; Get attribute
        mov     edx,lpszFile
        mov     edi,lpFindData          ; Get the results buffer
        xor     esi,esi                 ; Return date/time as 64-bit format
        stc                             ; Assume not supported
        VxDInt  21h
        jnc     done
        cmp     ax,7100h                ; Unsupported?
        stc
        jne     done                    ; No, supported, but error
        mov     bSupported, FALSE       ; Not supported.  Use old method

done:
        mov     ebx,lphFind
        mov     DWORD PTR [ebx], eax
        jnc     validhandle
        mov     DWORD PTR [ebx], -1
validhandle:
        call    FixUpDOSResult

        mov     eax, bSupported

        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  __FileFindFirstNew

;----------------------------------------------------------------------------
;
; WORD __FileFindNextOld(VOID);
;
; Finds the next file, using old findFirst api.
;
;----------------------------------------------------------------------------
StartProc  __FileFindNextOld, <>

        push    ebx
        MOV     eax,4F00h
        VxDInt  21h
        call    FixUpDOSResult
        pop     ebx
        ret

StopProc  __FileFindNextOld

;----------------------------------------------------------------------------
;
; BOOL __FileFindNextNew(HFIND hFind, LPFINDDATA lpFindData);
;
; Finds the next file, using new findFirst api.
;
;----------------------------------------------------------------------------
StartProc  __FileFindNextNew, <hFind:DWORD, lpFindData:DWORD>

        LOCAL   bSupported:BOOL

        push    ebx
        push    esi
        push    edi

        mov     bSupported, TRUE
        mov     eax,714Fh               ; Extended Find Next
        mov     ebx,hFind
        mov     edi,lpFindData
        xor     esi,esi                 ; Return date/time as 64-bit format
        stc                             ; Assume not supported
        VxDInt  21h
        jnc     doneNext
        cmp     ax,7100h                ; Unsupported?
        stc
        jne     doneNext                ; No, supported, but error
        mov     bSupported, FALSE       ; Not supported.  Use old method
        clc
doneNext:
        call    FixUpDOSResult

        mov     eax, bSupported

        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  __FileFindNextNew

;----------------------------------------------------------------------------
;
; BOOL __FileFindCloseNew(HFIND hFind);
;
; Closes the HFind handle.
;
;----------------------------------------------------------------------------
StartProc  __FileFindCloseNew, <hFind:DWORD>

        LOCAL   bSupported:BOOL

        push    ebx

        mov     bSupported, TRUE
        mov     eax,71A1h               ; Extended Find Close
        mov     ebx,hFind
        stc                             ; Assume not supported
        VxDInt  21h
        jnc     doneClose
        cmp     ax,7100h                ; Unsupported?
        stc
        jne     doneClose               ; No, supported, but error
        mov     bSupported, FALSE       ; Not supported.  Use old method
        clc
doneClose:
        call    FixUpDOSResult

        mov     eax, bSupported

        pop     ebx
        ret

StopProc  __FileFindCloseNew

;----------------------------------------------------------------------------
;
; UINT VxDNameReturnShortName(LPCSTR lpszSpec, LPSTR lpszShortSpec)
;
; Returns the short name of a filename.
;
;----------------------------------------------------------------------------
StartProc  VxDNameReturnShortName, <lpszSpec:DWORD, lpszShortSpec:DWORD>

        push    ebx
        push    esi
        push    edi

        mov     eax,7160h
        mov     ecx,1
        mov     esi,lpszSpec
        mov     edi,lpszShortSpec
        stc                             ; Assume not supported
        VxDInt  21h
        jnc     doneShort
        cmp     ax,7100h                ; Unsupported?
        stc
        jne     doneShort

        ; Not supported.  Just copy from src to dest.
copyShort:
        cld
        lodsb
        stosb
        or      al,al                   ; EOS? + clc
        jnz     copyShort

doneShort:
        call    FixUpDOSResult
        sbb     eax,eax

        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  VxDNameReturnShortName

;----------------------------------------------------------------------------
;
; UINT VxDNameReturnLongName(LPCSTR lpszSpec, LPSTR lpszLongSpec)
;
; Returns the long name of a filename.
;
;----------------------------------------------------------------------------
StartProc  VxDNameReturnLongName, <lpszSpec:DWORD, lpszLongSpec:DWORD>

        push    ebx
        push    esi
        push    edi

        mov     eax,7160h
        mov     ecx,2
        mov     esi,lpszSpec
        mov     edi,lpszLongSpec
        stc                             ; Assume not supported
        VxDInt  21h
        jnc     doneLong
        cmp     ax,7100h                ; Unsupported?
        stc
        jne     doneLong

        ; Not supported.  Just copy from src to dest.
copyLong:
        cld
        lodsb
        stosb
        or      al,al                   ; EOS? + clc
        jnz     copyLong

doneLong:
        call    FixUpDOSResult
        sbb     eax,eax

        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  VxDNameReturnLongName

;----------------------------------------------------------------------------
;
; BOOL VolumeInformationGet (
;      LPCSTR lpRootPathName,
;      LPSTR lpVolumeNameBuffer,
;      DWORD dwVolumeNameSize,
;      LPDWORD lpVolumeSerialNumber,
;      LPDWORD lpMaximumComponentLength,
;      LPDWORD lpFileSystemFlags,
;      LPSTR  lpFileSystemNameBuffer,
;      DWORD dwFileSystemNameSize );
;
; Returns volume information.
;
;----------------------------------------------------------------------------
StartProc  VolumeInformationGet, < lpRootPathName:DWORD, lpVolumeNameBuffer:DWORD, dwVolumeNameSize:DWORD, lpVolumeSerialNumber:DWORD, lpMaximumComponentLength:DWORD, lpFileSystemFlags:DWORD, lpFileSystemNameBuffer:DWORD, dwFileSystemNameSize:DWORD >

        LOCAL   wMaximumComponentLength:WORD
        LOCAL   wFileNameSize:WORD
        LOCAL   wFlags:WORD

        push    ebx
        push    edi

        mov     wMaximumComponentLength,0
        movzx   ecx,WORD PTR dwFileSystemNameSize
        mov     wFileNameSize,cx
        mov     wFlags,0

        mov     edi,lpFileSystemNameBuffer
        mov     edx,lpRootPathName
        mov     eax,71A0h
        stc
        VxDInt  21h
        mov     eax,0
        jc      volinfoget_error
        inc     eax
        mov     wMaximumComponentLength,cx
        mov     wFlags,bx
  volinfoget_error:
        mov     edi,lpMaximumComponentLength
        test    edi,edi
        jz      volinfoget_nomaxcomplength
        mov     cx,wMaximumComponentLength
        mov     WORD PTR [edi],cx
  volinfoget_nomaxcomplength:
        mov     edi,lpFileSystemFlags
        test    edi,edi
        jz      volinfoget_nofilesystemflags
        mov     cx,wFlags
        mov     WORD PTR [edi],cx
  volinfoget_nofilesystemflags:
        pop     edi
        pop     ebx
        cld
        ret

StopProc  VolumeInformationGet

;----------------------------------------------------------------------------
;
; UINT DiskLockPhysicalVolume (
;      BYTE byDriveNumber,
;      BYTE byLockLevel,
;      BYTE byPermissions );
;
; Locks a physical volume.
;
;----------------------------------------------------------------------------
StartProc  DiskLockPhysicalVolume, < byDriveNumber:BYTE, byLockLevel:BYTE, byPermissions:BYTE >

        push    ebx
        push    esi
        push    edi
        mov     eax,440Dh
        movzx   ebx,byDriveNumber
        mov     bh,byLockLevel
        mov     ecx,84Bh
        movzx   edx,byPermissions
        VxDInt  21h
        call    FixUpDosResult
        mov     eax,0
        jnc     @f
        call    _GetLastError@0
  @@:
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  DiskLockPhysicalVolume

;----------------------------------------------------------------------------
;
; UINT DiskUnlockPhysicalVolume (
;      BYTE byDriveNumber );
;
; Unlocks a physical volume.
;
;----------------------------------------------------------------------------
StartProc  DiskUnlockPhysicalVolume, < byDriveNumber:BYTE >

        push    ebx
        push    esi
        push    edi
        mov     eax,440Dh
        movzx   ebx,byDriveNumber
        mov     ecx,86Bh
        VxDInt  21h
        call    FixUpDosResult
        mov     eax,0
        jnc     @f
        call    _GetLastError@0
  @@:
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  DiskUnlockPhysicalVolume

;----------------------------------------------------------------------------
;
; UINT DiskGetCurrentLockState (
;      BYTE  byDriveNumber,
;      PWORD pwLockLevel,
;      PWORD pwLockPermissions );
;
; Retrieves the current volume lock level and permissions on the
; specified drive
;
;----------------------------------------------------------------------------
StartProc  DiskGetCurrentLockState, < byDriveNumber:BYTE, pwLockLevel:DWORD, pwLockPermissions:DWORD >

        push    ebx
        push    esi
        push    edi
        mov     eax,440Dh
        movzx   ebx,byDriveNumber
        mov     ecx,870h
        VxDInt  21h
        call    FixUpDosResult
        jc      DiskGetCurLocks_err
        mov     esi,[pwLockLevel]
        mov     [esi],ax
        mov     esi,[pwLockPermissions]
        mov     [esi],cx
        xor     eax,eax
        jmp     short DiskGetCurLocks_done
  DiskGetCurLocks_err:
        call    _GetLastError@0
  DiskGetCurLocks_done:
        pop     edi
        pop     esi
        pop     ebx
        ret

StopProc  DiskGetCurrentLockState

StartProc FileEnableFileSharing, < bEnabled:BOOL >
        mov     eax, bEnabled
        mov     _bFileSharing, eax
        ret
StopProc  FileEnableFileSharing

;----------------------------------------------------------------------------
;
; Define necessary macros for the VxD wraps, then define the wraps
; themselves.  Currently only a few IFS manager functions and some of our
; own low-level functions don't have C-accessible wraps.
;
;----------------------------------------------------------------------------
MakeCPROC  MACRO  func, serv

OPTION LANGUAGE:SYSCALL

  PUBLIC  _&func

  _&func:
    VxDJmp  &serv
ENDM

MakeCPROC  UniToBCS, UniToBCS
MakeCPROC  BCSToUni, BCSToUni
MakeCPROC  UniToUpper, UniToUpper

        PUBLIC  IOS_Find_Int13_Drive

IOS_Find_Int13_Drive  PROC  STDCALL, uDevice:UINT

        push    ebx
        push    esi
        push    edi
        xor     edi,edi
        mov     al,BYTE PTR uDevice
        clc
        VxDCall IOS_Find_Int13_Drive
        mov     eax,0
        jc      no_bdd
        mov     eax,edi
  no_bdd:
        pop     edi
        pop     esi
        pop     ebx
        cld
        ret

IOS_Find_Int13_Drive  ENDP

VXD_PAGEABLE_CODE_ENDS

        END
