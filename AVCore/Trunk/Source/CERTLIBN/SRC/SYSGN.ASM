;-----------------------------------------------------------------------------
; SYS4_GET_NAMES
;
;       Return the names of the SYSTEM files that booted this
;       computer.
; 
; BOOL WINAPI SYS4_GET_NAMES(LPSTR lpBuffer);
;
; Input:
;       1)  Pointer to buffer to place file names in, Size = 1024 bytes
; Output:
;       1)  True  (1), buffer filled, double NULL for end.
;       2)  Error (0)
;
; Note:
;       Buffer filled like:
;               C:\MSDOS.SYS0C:\IOSYS.SYS0C:\COMMAND.COM00
;
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/sysgn.asv   1.7   07 Aug 1996 17:29:10   DALLEE  $
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/sysgn.asv  $
;  
;     Rev 1.7   07 Aug 1996 17:29:10   DALLEE
;  Now jumps to correct code to use default system file names when names not
;  found in boot sector.
;  This also fixes 16-bit platform crash when system names not found -- was
;  jumping to wrong spot without initializing DS.
;  
;     Rev 1.6   25 Apr 1995 17:27:12   SZIADEH
;  code to save the default set of system files in case the
;  files'names were not found in the boot sector.
;  
;     Rev 1.5   13 Feb 1995 18:10:34   SZIADEH
;  fixed lpEnvironment problem in OS_16 platform.
;  
;     Rev 1.4   13 Feb 1995 15:16:10   SZIADEH
;  changed elif with elseif
;  
;     Rev 1.3   13 Feb 1995 15:09:26   SZIADEH
;
;  
;     Rev 1.2   13 Feb 1995 12:58:56   SZIADEH
;  fixed the if-def construct around get environment variable stuff.
;  
;     Rev 1.1   27 Jan 1995 19:17:32   SZIADEH
;  work under WIN32 platform.
;-----------------------------------------------------------------------------
include CTSN.INC

UniqueDataSegment

szDOS           byte    'DOS',0
szSYS           byte    'SYS',0
szCOM           byte    'COM',0
szBIN           byte    'BIN',0
szEXE           byte    'EXE',0

szCOMSPEC       byte    'COMSPEC',0

szIoSys         byte    'C:\IO.SYS'     ,0
szMsdosSys      byte    'C:\MSDOS.SYS'  ,0
szIbmbioSys     byte    'C:\IBMBIO.SYS' ,0
szIbmdosSys     byte    'C:\IBMDOS.SYS' ,0

szSystemFile1   byte    15  dup(0)
szSystemFile2   byte    15  dup(0)

szCommandCom    byte    'C:\COMMAND.COM'  ,0
szNDOS          byte    'C:\NDOS.COM'     ,0
sz4DOS          byte    'C:\4DOS.COM'     ,0

                                            ;; new files for windows94
szDblspaceBin   byte    'C:\DBLSPACE.BIN' ,0
szDrvspaceBin   byte    'C:\DRVSPACE.BIN' ,0

                                            ;; this set of files is saved if SYSINTEG is
                                            ;; created under WINDOWS
                                            ;;szIoDos         byte    'C:\IO.DOS'     ,0
                                            ;;szMsdosDos      byte    'C:\MSDOS.DOS'  ,0
                                            ;;szIbmbioDos     byte    'C:\IBMBIO.DOS' ,0
                                            ;;szIbmdosDos     byte    'C:\IBMDOS.DOS' ,0

szCommandDos    byte    'C:\COMMAND.DOS',0

                                            ;; this set is saved if SYMINTEG is
                                            ;; created under DOS
szWinbootSys    byte    'C:\WINBOOT.SYS',0
szMsdosW40      byte    'C:\MSDOS.W40'  ,0
szCommandW40    byte    'C:\COMMAND.W40',0

UniqueDataSegmentEnd

;-----------------------------------------------------------
; External Prototypes
;-----------------------------------------------------------
if bSYM_WIN16
   GetDOSEnvironment           PROTO FAR
elseif bSYM_WIN32
   GetEnvironmentVariableA     PROTO NEAR, :LPSTR, :LPSTR, :DWORD
endif

;-----------------------------------------------------------
; Local Prototypes
;-----------------------------------------------------------
if FARCALL
   ChangeExtension PROTO FAR, :LPSTR, :LPSTR
   GetComSpecValue PROTO FAR
else
   ChangeExtension PROTO NEAR, :LPSTR, :LPSTR
   GetComSpecValue PROTO NEAR
endif
;-----------------------------------------------------------
;-----------------------------------------------------------

UniqueSegment
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
SYS4_GET_NAMES PROC USES XCX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LOCAL   strcPart:PARTENTRY
        LOCAL   lpEnvironment:DWORD

                                         ; Read BOOT Sector
        INVOKE  DIS4_GET_BOOTABLE_PARTITION,80h,ADDR strcPart,lpBuffer
        cmp     XAX, -1
        jz      IsError

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,strcPart.peBeginCylinder,strcPart.peBeginSector,strcPart.peBeginHead,80h
        or      XAX, XAX
        jnz     IsError

ifdef WIN16                              ; convert the segment into a selector
        mov     ax, word ptr lpBuffer+2
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      IsError
        mov     word ptr lpBuffer+2, ax
endif

        LES_DI  lpBuffer                 ; Where to store
                                         ; Look for SYS extension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szSYS,512
        or      XAX, XAX
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szCOM,512
        or      XAX, XAX
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szBIN,512
        or      XAX, XAX
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szEXE,512
        or      XAX, XAX
        jnz     FoundExtension

        jmp     UseDefaultSystemFiles
                                         ; Place first system file into buffer
                                         ; Conditions:  File MUST have eleven characters that don't contain
                                         ; a NULL.  Format:  "IBMBIO  COM"
FoundExtension:
        LES_DI  lpBuffer                 ; Where to store
        LDS_SI  lpBuffer                 ; setup ds:si for file name
        mov     XSI, XAX                 ; verification
        sub     XSI, 8                   ; Where NAME is located

        call    TestValidName
        jc      NotValidName

        push    XDI                      ;

        mov     XCX,8                    ; Max length of name
        mov     al,'C'                   ; Place C:slash into name buffer
        stosb
        mov     al,':'
        stosb
        mov     al,5Ch
        stosb
MoreName:
        lodsb
        or      al,al
        jz      EndName
        cmp     al,20h
        jz      EndName
        stosb
        loop    MoreName
EndName:
        lodsb                           ; Skip over space or zeros
        or      al,al
        jz      EndName
        cmp     al,20h
        je      EndName

        mov     al,'.'                  ; Place . after name
        stosb

        dec     XSI
        lodsb                           ; Place the EXTENSION in buffer
        stosb
        lodsb
        stosb
        lodsb
        stosb

        xor     XAX, XAX
        stosb                           ; Place NULL after name

; save first system file
        pop     XAX                     ; pop di which point to start of file name

if OS_16
        push    ds
        mov     bx, SEG szSystemFile1
        mov     ds, bx
endif

        INVOKE  STR4_COPY, vSES XAX, ADDR szSystemFile1
        INVOKE  ChangeExtension, ADDR szSystemFile1, ADDR szDOS

if OS_16
        pop     ds
endif
                                        ; Place second system file into buffer
                                        ; First test if there IS a second filename.  DR DOS doesn't
                                        ; have two system files, just one.
        call    TestValidName
        jc      NotValidName

FoundExtension2:
        push    XDI

        mov     XCX,8                   ; Max length of name
        mov     al,'C'                  ; Place C:slash into name buffer
        stosb
        mov     al,':'
        stosb
        mov     al,5Ch
        stosb
MoreName2:
        lodsb
        or      al,al
        jz      EndName2
        cmp     al,20h
        jz      EndName2
        stosb
        loop    MoreName2
EndName2:
        lodsb                           ; Skip over space or zeros
        or      al,al
        jz      EndName2
        cmp     al,20h
        je      EndName2

        mov     al,'.'                  ; Place . after name
        stosb

        dec     XSI
        lodsb                           ; Place the EXTENSION in buffer
        stosb
        lodsb
        stosb
        lodsb
        stosb

        xor     XAX, XAX
        stosb                           ; Place NULL after name

        pop     XAX

if OS_16
        mov     bx, SEG szSystemFile2
        mov     ds, bx
endif

        INVOKE  STR4_COPY, vSES XAX, ADDR szSystemFile2
        INVOKE  ChangeExtension, ADDR szSystemFile2, ADDR szDOS

        jmp     DontUseDefault

NotValidName:

;-------------------------------------------------------------
UseDefaultSystemFiles:
        LES_DI  lpBuffer                 ; Where to store
        LDS_SI  lpBuffer                 ; setup ds:si for file name
if OS_16
        mov     ax,SEG szIoSys
        mov     ds,ax
endif
; this block of code was added to handle the case where the boot sector does
; not have the system files names (as in the NT boot sector) however, the files
; do exist in the root directory.
                                        ; look for "c:\io.sys"
        INVOKE  FIL4_ATTR,ADDR szIoSys,0,GET_INFO
        cmp     XAX,-1
        je      NoIoSysRoot
        INVOKE  STR4_COPY,ADDR szIoSys,vSES XDI
        add     XDI,XAX
        inc     XDI

        INVOKE  STR4_COPY,ADDR szIoSys,ADDR szSystemFile1
        INVOKE  ChangeExtension, ADDR szSystemFile1, ADDR szDOS
NoIoSysRoot:
                                        ; look for "c:\msdos.sys"
        INVOKE  FIL4_ATTR,ADDR szMsdosSys,0,GET_INFO
        cmp     XAX,-1
        je      NoMsdosSysRoot
        INVOKE  STR4_COPY,ADDR szMsdosSys,vSES XDI
        add     XDI,XAX
        inc     XDI

        INVOKE  STR4_COPY,ADDR szMsdosSys,ADDR szSystemFile2
        INVOKE  ChangeExtension, ADDR szSystemFile2, ADDR szDOS
NoMsdosSysRoot:
                                        ; look for "c:\ibmbio.sys"
        INVOKE  FIL4_ATTR,ADDR szIbmbioSys,0,GET_INFO
        cmp     XAX,-1
        je      NoIbmbioSysRoot
        INVOKE  STR4_COPY,ADDR szIbmbioSys,vSES XDI
        add     XDI,XAX
        inc     XDI

        INVOKE  STR4_COPY,ADDR szIbmbioSys,ADDR szSystemFile1
        INVOKE  ChangeExtension, ADDR szSystemFile1, ADDR szDOS
NoIbmbioSysRoot:
                                        ; look for "c:\ibmdos.sys"
        INVOKE  FIL4_ATTR,ADDR szIbmdosSys,0,GET_INFO
        cmp     XAX,-1
        je      NoIbmdosSysRoot
        INVOKE  STR4_COPY,ADDR szIbmdosSys,vSES XDI
        add     XDI,XAX
        inc     XDI

        INVOKE  STR4_COPY,ADDR szIbmdosSys,ADDR szSystemFile2
        INVOKE  ChangeExtension, ADDR szSystemFile2, ADDR szDOS

NoIbmdosSysRoot:

DontUseDefault:
;--------------------------------------------------
; Since this function may execute from a device
; driver, (LIKE NAVTSR), we need to specifically
; look for any command interperter we know about.
; And add it to the list.
;--------------------------------------------------
; See if COMMAND.COM is in the root and get it also
; along with whatever COMSPEC is pointing to.  This
; may be the same.
;--------------------------------------------------
; Look for COMMAND.COM
;--------------------------------------------------

        INVOKE  FIL4_ATTR,ADDR szCommandCom,0,GET_INFO
        cmp     XAX,-1
        je      NoCommandComRoot
        INVOKE  STR4_COPY,ADDR szCommandCom,vSES XDI
        add     XDI,XAX
        inc     XDI
NoCommandComRoot:
                                        ; Look for NDOS.COM
        INVOKE  FIL4_ATTR,ADDR szNDOS,0,GET_INFO
        cmp     XAX,-1
        je      NoNDOSRoot
        INVOKE  STR4_COPY,ADDR szNDOS,vSES XDI
        add     XDI, XAX
        inc     XDI
NoNDOSRoot:

                                        ; Look for 4DOS.COM
        INVOKE  FIL4_ATTR,ADDR sz4DOS,0,GET_INFO
        cmp     XAX,-1
        je      No4DOSRoot
        INVOKE  STR4_COPY,ADDR sz4DOS,vSES XDI
        add     XDI, XAX
        inc     XDI
No4DOSRoot:
                                        ;look for szDblspaceBin    'C:\DBLSPACE.BIN'
        INVOKE  FIL4_ATTR,ADDR szDblspaceBin,0,GET_INFO
        cmp     XAX,-1
        je      NoDblspaceBin
        INVOKE  STR4_COPY,ADDR szDblspaceBin,vSES XDI
        add     XDI, XAX
        inc     XDI

NoDblspaceBin:
                                        ;look for szDrvspaceBin    'C:\DRVSPACE.BIN
        INVOKE  FIL4_ATTR,ADDR szDrvspaceBin,0,GET_INFO
        cmp     XAX,-1
        je      NoDrvspaceBin
        INVOKE  STR4_COPY,ADDR szDrvspaceBin,vSES XDI
        add     XDI, XAX
        inc     XDI
NoDrvspaceBin:
                                        ;look 'C:\IO.DOS' or 'C:\IBMBIO.DOS', or whatever else
		INVOKE	FIL4_ATTR,ADDR szSystemFile1,0,GET_INFO
        cmp     XAX,-1
        je      NoIoDos
		INVOKE	STR4_COPY,ADDR szSystemFile1,vSES XDI
        add     XDI, XAX
        inc     XDI
NoIoDos:
                                        ;look 'C:\MSDOS.DOS' or 'C:\IBMDOS.DOS'
		INVOKE	FIL4_ATTR,ADDR szSystemFile2,0,GET_INFO
        cmp     XAX,-1
        je      NoMsdosDos
		INVOKE	STR4_COPY,ADDR szSystemFile2,vSES XDI
        add     XDI, XAX
        inc     XDI
NoMsdosDos:

                                        ;look 'C:\COMMAND.DOS'
        INVOKE  FIL4_ATTR,ADDR szCommandDos,0,GET_INFO
        cmp     XAX,-1
        je      NoCommandDos
        INVOKE  STR4_COPY,ADDR szCommandDos,vSES XDI
        add     XDI, XAX
        inc     XDI

NoCommandDos:
                                        ;look  'C:\WINBOOT.SYS'
        INVOKE  FIL4_ATTR,ADDR szWinbootSys,0,GET_INFO
        cmp     XAX,-1
        je      NoWinbootSys
        INVOKE  STR4_COPY,ADDR szWinbootSys,vSES XDI
        add     XDI, XAX
        inc     XDI

NoWinbootSys:

                                        ;look  'C:\MSDOS.W40'
        INVOKE  FIL4_ATTR,ADDR szMsdosW40,0,GET_INFO
        cmp     XAX,-1
        je      NoMsdosW40
        INVOKE  STR4_COPY,ADDR szMsdosW40,vSES XDI
        add     XDI, XAX
        inc     XDI
NoMsdosW40:
                                        ;look 'C:\COMMAND.W40'
        INVOKE  FIL4_ATTR,ADDR szCommandW40,0,GET_INFO
        cmp     XAX,-1
        je      NoCommandW40
        INVOKE  STR4_COPY,ADDR szCommandW40,vSES XDI
        add     XDI, XAX
        inc     XDI

NoCommandW40:
;-----------------------------------------------------------------
; check COMSPEC
; for W32 platform, call WIN API
;-----------------------------------------------------------------

if bSYM_WIN32
        mov     eax, edi
        add     eax, 512                  ; because no buffer was allocated for lpEnvironment
        mov     lpEnvironment, eax
        INVOKE  GetEnvironmentVariableA, ADDR szCOMSPEC, lpEnvironment, 256
        jz      DoEnd                     ; jump on error

elseif bSYM_WIN16                           ; value returned in dx:ax
        INVOKE  GetDOSEnvironment
elseif bSYM_DOS                             ; value returned in dx:ax
        INVOKE  DOS4_GetEnvironment
endif

if bSYM_WIN16 or bSYM_DOS
        INVOKE  GetComSpecValue
        jc      DoEnd

        mov     word ptr lpEnvironment, ax
        mov     word ptr lpEnvironment+2, dx
endif
                                        ; Test to see if we got it already.
        INVOKE  STR4_COMPARE, lpEnvironment,ADDR szCommandCom
        cmp     XAX,TRUE
        je      GotItAlready
        INVOKE  STR4_COMPARE, lpEnvironment,ADDR szNDOS
        cmp     XAX,TRUE
        je      GotItAlready
        INVOKE  STR4_COMPARE, lpEnvironment,ADDR sz4DOS
        cmp     XAX,TRUE
        je      GotItAlready

        INVOKE  STR4_COPY, lpEnvironment,vSES XDI
        add     XDI, XAX                ; Increment pointer past the copied

GotItAlready:
DoEnd:
        xor     XAX, XAX
        stosw                           ; Place Double NULL at end
        mov     XAX,TRUE
        jmp     Done

IsError:
        xor     XAX, XAX
Done:
        ret

SYS4_GET_NAMES ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; TestValidName
;
;       Test if filename can be a valid filename
;
; Input:
;       1) DS:SI file name to test.
; Output:
;       1) CLC, is legal
;       2) STC, is illegal
;-----------------------------------------------------------------------------
TestValidName PROC

        push    XSI
        push    XCX

        mov     XCX,11                   ; Look at eleven characters for a NULL
LookMoreForNull:
        cmp     [XSI],byte ptr 0
        je      FoundNULL
        inc     XSI
        loop    LookMoreForNull
        clc
        jmp     NoNullFound
FoundNULL:
        stc
NoNullFound:

        pop     XCX
        pop     XSI
        ret

TestValidName ENDP
;-----------------------------------------------------------------------------
ChangeExtension PROC USES SDS XSI SES XDI,
        FileName:LPSTR,
        NewExt:LPSTR

        INVOKE  STR4_SRCH_CHAR, FileName, '.'

        LES_DI   FileName
        LDS_SI   NewExt

        mov   XDI, XAX
        inc   XDI

        lodsb
        stosb

        lodsb
        stosb

        lodsb
        stosb

        ret
ChangeExtension ENDP

;-----------------------------------------------------------
; entering: dx points to the COMSPEC segment
; return DX:AX pointer to COMSPEC value
;-----------------------------------------------------------
GetComSpecValue PROC USES SES XDI SDS XSI,

        LOCAL   lpComSpec:DWORD

        mov     word ptr lpComSpec+2,dx
        mov     word ptr lpComSpec,0

        INVOKE  MEM4_SRCH_STRING,lpComSpec,ADDR szCOMSPEC,8000h
                                         ; Where COMSPEC is found
        mov     word ptr lpComSpec, ax

        INVOKE  STR4_SRCH_CHAR,lpComSpec,'='
        or      ax, ax
        jz      ComSpecError

        mov     word ptr lpComSpec,ax

        LES_DI  lpComSpec
MoreSpace:
        inc     di                      ; skip over '='
        cmp     XES[XDI],byte ptr ' '   ; and spaces
        je      MoreSpace

        mov     word ptr lpComSpec, di
        cmp     XES[XDI+1],byte ptr ':' ; skip drive letter

        jne     ComSpecError            ; can't find ':'

        mov     al,XES[XDI]             ; make sure it's not on A or B
        cmp     al,'A'
        je      ComSpecError

        cmp     al, 'B'
        jne     ComSpecDone             ; command com found

ComSpecError:
        xor     ax,ax
        xor     dx,dx
        stc
        jmp     ComSpecRet
ComSpecDone:
        mov     ax, word ptr lpComSpec
        mov     dx, word ptr lpComSpec+2
        clc
ComSpecRet:
        ret
GetComSpecValue ENDP
;-----------------------------------------------------------
;-----------------------------------------------------------

UniqueSegmentEnd
END


;-----------------------------------------------------------
; not used
;-----------------------------------------------------------
MyGetEnvironmentVariable PROC USES SDS SES XSI XDI XCX XBX XAX XDX,
       varname:LPSTR,
       buffer:LPSTR,
       buffsize :DWORD

       LDS_SI varname
       LES_DI buffer
       mov    XCX, buffsize
       rep    movsb

       mov    XAX, 10

       ret

MyGetEnvironmentVariable ENDP


