include CTS4.INC

ifdef WIN16
GetDOSEnvironment        PROTO FAR
endif

.data

szDOS           byte    'DOS',0
szSYS           byte    'SYS',0
szCOM           byte    'COM',0
szBIN           byte    'BIN',0
szEXE           byte    'EXE',0

szCOMSPEC       byte    'COMSPEC',0

szSystemFile1   byte    15  dup(0)
szSystemFile2   byte    15  dup(0)

szCommandCom    byte    'C:\COMMAND.COM'  ,0
szNDOS          byte    'C:\NDOS.COM'     ,0
sz4DOS          byte    'C:\4DOS.COM'     ,0

;; new files for windows94
szDblspaceBin   byte    'C:\DBLSPACE.BIN' ,0
szDrvspaceBin   byte    'C:\DRVSPACE.BIN'  ,0

;; this set of files is saved if SYSINTEG is
;; created under WINDOWS
;;szIoDos		  byte	  'C:\IO.DOS'     ,0
;;szMsdosDos	  byte	  'C:\MSDOS.DOS'  ,0
;;szIbmbioDos	  byte	  'C:\IBMBIO.DOS' ,0
;;szIbmdosDos	  byte	  'C:\IBMDOS.DOS' ,0

szCommandDos    byte    'C:\COMMAND.DOS',0

;; this set is saved if SYMINTEG is
;; created under DOS
szWinbootSys    byte    'C:\WINBOOT.SYS',0
szMsdosW40      byte    'C:\MSDOS.W40'  ,0
szCommandW40    byte    'C:\COMMAND.W40',0

if FARCALL
ChangeExtension PROTO FAR, :LPSTR, :LPSTR
else
ChangeExtension PROTO NEAR, :LPSTR, :LPSTR
endif

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_NAMES                       3/15/93                         CAG
;
;       Return the names of the SYSTEM files that booted this
;       computer.
; 
; BOOL WINAPI SYS4_GET_NAMES(LPSTR lpBuffer);
;
; Input:
;       1)  Pointer to buffer to place file names in, Size = 512 bytes
; Output:
;       1)  True (1), buffer filled, double NULL for end.
;       2)  Error (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Note:
;       Buffer filled like:
;               C:\MSDOS.SYS0C:\IOSYS.SYS0C:\COMMAND.COM00
;
;-----------------------------------------------------------------------------
SYS4_GET_NAMES PROC USES XCX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LOCAL   strcPart:PARTENTRY
        LOCAL   lpEnviroment:DWORD

; Read BOOT Sector 
        INVOKE  DIS4_GET_BOOTABLE_PARTITION,80h,ADDR strcPart,lpBuffer
        cmp     ax, -1
        jz      IsError

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,strcPart.peBeginCylinder,strcPart.peBeginSector,strcPart.peBeginHead,80h
        or      ax,ax
        jnz     IsError

        mov     ax, word ptr lpBuffer+2
ifdef WIN16
; convert the segment into a selector
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      IsError
        mov     word ptr lpBuffer+2, ax
endif

        LES_DI  lpBuffer                ; Where to store

; Look for SYS extension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szSYS,512
        or      ax,ax
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szCOM,512
        or      ax,ax
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szBIN,512
        or      ax,ax
        jnz     FoundExtension
        INVOKE  MEM4_SRCH_STRING,lpBuffer,ADDR szEXE,512
        or      ax,ax
        jnz     FoundExtension

        jmp     NoSystemButGetCommandCom


; Place first system file into buffer

; Conditions:  File MUST have eleven characters that don't contain
; a NULL.  Format:  "IBMBIO  COM"

FoundExtension:
        mov     si,ax
        sub     si,8                    ; Where NAME is located

        mov     ax,word ptr lpBuffer+2
        mov     SDS, ax                 ; set up DS register


        LES_DI  lpBuffer                ; Where to store

        call    TestValidName
        jc      NotValidName

        push    di                      ;

        mov     cx,8                    ; Max length of name
        mov     al,'C'                  ; Place C:slash into name buffer
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

        dec     si
        lodsb                           ; Place the EXTENSION in buffer
        stosb
        lodsb
        stosb
        lodsb
        stosb

        xor     ax,ax
        stosb                           ; Place NULL after name

; save first system file
        pop     ax
        push    ds
        mov     bx, SEG szSystemFile1
        mov     ds, bx

        INVOKE  STR4_COPY, vSES XAX, ADDR szSystemFile1
        INVOKE  ChangeExtension, ADDR szSystemFile1, ADDR szDOS
        pop     ds

; Place second system file into buffer
; First test if there IS a second filename.  DR DOS doesn't
; have two system files, just one.

        call    TestValidName
        jc      NotValidName

FoundExtension2:
        push    di

        mov     cx,8                    ; Max length of name
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

        dec     si
        lodsb                           ; Place the EXTENSION in buffer
        stosb
        lodsb
        stosb
        lodsb
        stosb

        xor     ax,ax
        stosb                           ; Place NULL after name

        pop     ax
        mov     bx, SEG szSystemFile2
        mov     ds, bx

        INVOKE  STR4_COPY, vSES XAX, ADDR szSystemFile2
        INVOKE  ChangeExtension, ADDR szSystemFile2, ADDR szDOS

NotValidName:


NoSystemButGetCommandCom:

        mov     ax,SEG szCommandCom
        mov     ds,ax

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
        cmp     ax,-1
        je      NoCommandComRoot
        INVOKE  STR4_COPY,ADDR szCommandCom,vSES XDI
        add     XDI,ax
        inc     XDI
NoCommandComRoot:

; Look for NDOS.COM
        INVOKE  FIL4_ATTR,ADDR szNDOS,0,GET_INFO
        cmp     ax,-1
        je      NoNDOSRoot
        INVOKE  STR4_COPY,ADDR szNDOS,vSES XDI
        add     XDI,ax
        inc     XDI
NoNDOSRoot:

; Look for 4DOS.COM
        INVOKE  FIL4_ATTR,ADDR sz4DOS,0,GET_INFO
        cmp     ax,-1
        je      No4DOSRoot
        INVOKE  STR4_COPY,ADDR sz4DOS,vSES XDI
        add     XDI,ax
        inc     XDI
No4DOSRoot:

;look for szDblspaceBin    'C:\DBLSPACE.BIN'
        INVOKE  FIL4_ATTR,ADDR szDblspaceBin,0,GET_INFO
        cmp     ax,-1
        je      NoDblspaceBin
        INVOKE  STR4_COPY,ADDR szDblspaceBin,vSES XDI
        add     XDI,ax
        inc     XDI

NoDblspaceBin:

;look for szDrvspaceBin    'C:\DRVSPACE.BIN
        INVOKE  FIL4_ATTR,ADDR szDrvspaceBin,0,GET_INFO
        cmp     ax,-1
        je      NoDrvspaceBin
        INVOKE  STR4_COPY,ADDR szDrvspaceBin,vSES XDI
        add     XDI,ax
        inc     XDI
NoDrvspaceBin:

;look 'C:\IO.DOS' or 'C:\IBMBIO.DOS', or whatever else
		INVOKE	FIL4_ATTR,ADDR szSystemFile1,0,GET_INFO
        cmp     ax,-1
        je      NoIoDos
		INVOKE	STR4_COPY,ADDR szSystemFile1,vSES XDI
        add     XDI,ax
        inc     XDI
NoIoDos:

;look 'C:\MSDOS.DOS' or 'C:\IBMDOS.DOS'
		INVOKE	FIL4_ATTR,ADDR szSystemFile2,0,GET_INFO
        cmp     ax,-1
        je      NoMsdosDos
		INVOKE	STR4_COPY,ADDR szSystemFile2,vSES XDI
        add     XDI,ax
        inc     XDI
NoMsdosDos:

;look 'C:\COMMAND.DOS'
        INVOKE  FIL4_ATTR,ADDR szCommandDos,0,GET_INFO
        cmp     ax,-1
        je      NoCommandDos
        INVOKE  STR4_COPY,ADDR szCommandDos,vSES XDI
        add     XDI,ax
        inc     XDI

NoCommandDos:
;look  'C:\WINBOOT.SYS'
        INVOKE  FIL4_ATTR,ADDR szWinbootSys,0,GET_INFO
        cmp     ax,-1
        je      NoWinbootSys
        INVOKE  STR4_COPY,ADDR szWinbootSys,vSES XDI
        add     XDI,ax
        inc     XDI

NoWinbootSys:

;look  'C:\MSDOS.W40'
        INVOKE  FIL4_ATTR,ADDR szMsdosW40,0,GET_INFO
        cmp     ax,-1
        je      NoMsdosW40
        INVOKE  STR4_COPY,ADDR szMsdosW40,vSES XDI
        add     XDI,ax
        inc     XDI
NoMsdosW40:

;look 'C:\COMMAND.W40'
        INVOKE  FIL4_ATTR,ADDR szCommandW40,0,GET_INFO
        cmp     ax,-1
        je      NoCommandW40
        INVOKE  STR4_COPY,ADDR szCommandW40,vSES XDI
        add     XDI,ax
        inc     XDI
NoCommandW40:

; Get the COMSPEC to store

ifdef WIN16
        INVOKE  GetDOSEnvironment

        mov     word ptr lpEnviroment+2,dx
        mov     word ptr lpEnviroment,ax
else
        mov     ah,62h                  ; Get PSP of program
        int     21h

        push    es
        mov     es,bx
        mov     dx,es:[2Ch]             ; Segment of ENVIROMENT
        mov     word ptr lpEnviroment+2,dx
        mov     word ptr lpEnviroment,0
        pop     es
endif

        INVOKE  MEM4_SRCH_STRING,lpEnviroment,ADDR szCOMSPEC,8000h
        add     word ptr lpEnviroment,ax; Where COMSPEC is found

        INVOKE  STR4_SRCH_CHAR,lpEnviroment,'='
        or      ax,ax
        jz      DoEnd
        mov     word ptr lpEnviroment,ax

        push    es
        push    di
MoreSpace:
        LES_DI  lpEnviroment
        inc     XDI
        mov     word ptr lpEnviroment,XDI
        cmp     XES[XDI],byte ptr ' '
        je      MoreSpace

        pop     di
        pop     es

; Test to see if we got it already.
        INVOKE  STR4_COMPARE,lpEnviroment,ADDR szCommandCom
        cmp     ax,TRUE
        je      GotItAlready
        INVOKE  STR4_COMPARE,lpEnviroment,ADDR szNDOS
        cmp     ax,TRUE
        je      GotItAlready
        INVOKE  STR4_COMPARE,lpEnviroment,ADDR sz4DOS
        cmp     ax,TRUE
        je      GotItAlready

        push    es
        push    di

        LES_DI  lpEnviroment
        cmp     XES[XDI+1],byte ptr ':'
        jne     NoCommandCom

        mov     al,XES[XDI]
        and     al,5Fh
        sub     al,'A'
        cmp     al,2
        jb      NoCommandCom            ; Can't use from drive A: or B:

        pop     di
        pop     es
        jmp     CopyCommandCom

NoCommandCom:
        pop     di
        pop     es
        jmp     DoEnd

CopyCommandCom:
        INVOKE  STR4_COPY,lpEnviroment,vSES XDI
        add     di, ax                  ; Increment pointer past the copied
                                        ; string. Martin
DoEnd:
GotItAlready:
        xor     ax,ax
        stosw                           ; Place Double NULL at end
        mov     ax,TRUE
        jmp     Done

IsError:
        xor     ax,ax
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

        push    si
        push    cx

        mov     cx,11                   ; Look at eleven characters for a NULL
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

        pop     cx
        pop     si
        ret

TestValidName ENDP
;-----------------------------------------------------------------------------
ChangeExtension PROC USES SDS XSI SES XDI,
        FileName:LPSTR,
        NewExt:LPSTR

        INVOKE  STR4_SRCH_CHAR, FileName, '.'

        LES_DI   FileName
        LDS_SI   NewExt

        mov   di, ax
        inc   di

        lodsb
        stosb

        lodsb
        stosb

        lodsb
        stosb

        ret
ChangeExtension ENDP

END
