include CTS4.INC

.data
uBuffer         EQU     128
lpBuffer        db      uBuffer dup(0)
szTempFile      db      uBuffer dup(0)
szIniName       db      '\TEMP.INI',0
szOpenBracket   db      '[',0
szCloseBracket  db      ']',0
szEqual         db      '=',0

WriteEncrypt    db      FALSE
ReadEncrypt     db      FALSE
OverRide        db      FALSE

ENCRYPTINI      STRUCT
        Message db      32 dup(0)
                db      CR
                db      LF
                db      1Ah
        Verify  dw      0
        eKey    db      0
ENCRYPTINI      ENDS
sENCRYPTINI     EQU     SIZE ENCRYPTINI

eHeader         ENCRYPTINI<'Symantec INI File'>
tHeader         ENCRYPTINI<'Symantec INI File'>

UniqueSegment
;-----------------------------------------------------------------------------
; WRITEPRIVATEPROFILESTRING            3/16/93                         CAG
;
;       BOOL WritePrivateProfileString(LPCSTR lpszSection,LPCSTR lpszEntry
;                                      LPCSTR lpszString,LPCSTR lpszFilename);
;
; Input:
;       1)  Section to write to
;       2)  Entry to write to
;       3)  String to write
;       4)  File to write to
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Error
;
; Note:
;       This is a Windows Replacement function.  Work exactly like
;       WritePrivateProfileString except the file WILL NOT be cached.
;
; Note:
;       This function also has the ability to encrypt the INI file.
;
;       SetEncryptINI(Level);
;
;       Level = 0
;               On next access to a INI file, unencrypt it.
;       Level = 1
;               On next access to a INI file, encrypt it.
;       Level = 2
;               Leave files as they are.
;
;       Logic:
;       _____
;
;       Encryption starts out at LEVEL 3 (Leave as is).  If a user
;       selects PASSWORD protection, make a call to SetEncryptINI(2)
;       this will encrypt the file on the next access to it.
;       If a user disables the PASSWORD protection, make a call to 
;       SetEncryptINI(0).
;
;       When ENCRYPTION is ENABLE, EVERY INI file written to will be
;       encrypted.  To disable this feature, after the first write
;       to the INI file, SetEncryptINI(2).  This will let the
;       automatic detection determine if the encryption is on or not.
;
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
WRITEPRIVATEPROFILESTRING PROC USES XCX SDS XSI,
        lpszSection:LPCSTR,
        lpszEntry:LPCSTR,
        lpszString:LPCSTR,
        lpszFilename:LPCSTR


        LOCAL   uHandle:UINT            ; Handle for input file
        LOCAL   uTemp:UINT              ; Handle for the output file
        
        mov     ax,@data
        mov     ds,ax

        mov     uHandle,0
        mov     uTemp,0

; Parse directory out of lpszFilename
        INVOKE  FIL4_SPLIT_PATHNAME,lpszFilename,ADDR szTempFile,ADDR lpBuffer
        INVOKE  STR4_APPEND,ADDR szTempFile,ADDR szIniName

; Create TEMP file
        INVOKE  FIL4_OPEN,ADDR szTempFile,fCREATE
        cmp     ax, -1
        jz      IsError
        mov     uTemp,XAX

; Open file
;  If !PATH and !FILENAME
;       Create in Windows Directory.
;  If PATH and !FILENAME
;       Create in PATH directory.
;  If PATH and FILENAME
;       Open for read/write

        INVOKE  FIL4_OPEN,lpszFilename,fREAD_WRITE
        cmp     ax, -1
        jnz     FoundFile

; Test for directory, if found create, if not found, use Windows directory

        INVOKE  FIL4_OPEN,lpszFilename,fCREATE
        cmp     ax, -1
        jz      IsError

FoundFile:
        mov     uHandle,XAX

        INVOKE  EncryptHeader,uHandle,uTemp

; Search for SECTION
;  If !SECTION
;       Create Section

LookForHeader:
        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      CreateSection

; UnEncrypt
        INVOKE  UnEncrypt,ADDR lpBuffer

        cmp     ax,0                    ; Bytes read = 0, must be blank line
        je      WriteLineNext

        lea     si,lpBuffer
        cmp     [XSI],byte ptr '['
        jne     WriteLineNext

        inc     si
        INVOKE  MEM4_REPLACE,vSDS XSI,']',0,uBuffer

        INVOKE  STR4_ICOMPARE,vSDS XSI,lpszSection
        or      ax,ax
        jnz     HaveSection

WriteLineNext:
; Encrypt
        INVOKE  Encrypt,ADDR lpBuffer

        INVOKE  FIL4_WRITELINE,uTemp,ADDR lpBuffer
        cmp     ax,-1
        je      IsError
        jmp     LookForHeader

CreateSection:
        INVOKE  STR4_COPY,ADDR szOpenBracket,ADDR lpBuffer
        INVOKE  STR4_APPEND,ADDR lpBuffer,lpszSection

HaveSection:
        INVOKE  STR4_APPEND,ADDR lpBuffer,ADDR szCloseBracket

; If ENTRY = NULL
;       Delete entire Section
; If ENTRY != NULL
;  Search for ENTRY
;   If !ENTRY
;       Create Entry

        cmp     word ptr lpszEntry+2,0
        jne     IsEntry
        cmp     word ptr lpszEntry,0
        jne     IsEntry

; Delete entire section
DeleteEntire:
        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoneRename

; UnEncrypt
        INVOKE  UnEncrypt,ADDR lpBuffer

        lea     XSI,lpBuffer
        cmp     [XSI],byte ptr '['      ; Next section
        jne     DeleteEntire

CopyEntire:
        INVOKE  FIL4_WRITELINE,uTemp,ADDR lpBuffer
        cmp     ax,-1
        je      IsError

        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoneRename
        jmp     CopyEntire

IsEntry:

; Encrypt
        INVOKE  Encrypt,ADDR lpBuffer

        INVOKE  FIL4_WRITELINE,uTemp,ADDR lpBuffer
        cmp     ax,-1
        je      IsError

        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      CreateEntry

; UnEncrypt
        INVOKE  UnEncrypt,ADDR lpBuffer

        cmp     ax,0                    ; Bytes read = 0, must be blank line
        je      WriteLineEntry

        lea     si,lpBuffer
        cmp     [XSI],byte ptr ';'
        je      WriteLineEntry
        cmp     [XSI],byte ptr '['
        je      CreateEntry

; Look for = or a space
LookMore:
        cmp     [XSI],byte ptr 0
        je      FoundIt
        cmp     [XSI],byte ptr '='
        je      FoundIt
        cmp     [XSI],byte ptr ' '
        je      FoundIt
        cmp     [XSI],byte ptr 9h
        je      FoundIt
        inc     XSI
        jmp     LookMore

WriteLineEntry:
        jmp     IsEntry

FoundIt:
        mov     cl,[XSI]
        mov     [XSI],byte ptr 0
        INVOKE  STR4_ICOMPARE,ADDR lpBuffer,lpszEntry
        mov     [XSI],cl
        or      ax,ax
        jz      WriteLineEntry

; If STRING = NULL
;       Delete the entry
; If STRING != NULL
;  Write new ENTRY=STRING

        cmp     word ptr lpszString+2,0
        jne     IsString
        cmp     word ptr lpszString,0
        jne     IsString

        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        jmp     CopyEntire

CreateEntry:
        cmp     word ptr lpszString+2,0
        jne     IsString
        cmp     word ptr lpszString,0
        jne     IsString
        jmp     IsError

IsString:
        INVOKE  STR4_COPY,lpszEntry,ADDR lpBuffer
        INVOKE  STR4_APPEND,ADDR lpBuffer,ADDR szEqual
        INVOKE  STR4_APPEND,ADDR lpBuffer,lpszString

; Encrypt
        INVOKE  Encrypt,ADDR lpBuffer

        INVOKE  FIL4_WRITELINE,uTemp,ADDR lpBuffer

        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoneRename

        jmp     CopyEntire

; Error handler

IsError:
        cmp     uTemp,0
        je      NoTemp
        INVOKE  FIL4_CLOSE,uTEMP
NoTemp:
        cmp     uHandle,0
        je      NoHandle
        INVOKE  FIL4_CLOSE,uHandle
NoHandle:
        INVOKE  FIL4_DELETE,ADDR szTempFile
        mov     ax,FALSE
        jmp     Done

; Success exit
DoneRename:
        cmp     uTemp,0
        je      NoTemp1
        INVOKE  FIL4_CLOSE,uTEMP
NoTemp1:
        cmp     uHandle,0
        je      NoHandle1
        INVOKE  FIL4_CLOSE,uHandle
NoHandle1:
        INVOKE  FIL4_DELETE,lpszFilename
        INVOKE  FIL4_RENAME,ADDR szTempFile,lpszFilename
        mov     ax,TRUE
Done:

        ret

WRITEPRIVATEPROFILESTRING ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
SetEncryptINI   PROC,
        bFlag:BOOL

; bFlag = 0
;       Disable the encryption
; bFlag = 1
;       Enable the encryption
; bFlag = 2
;       Leave files as they are

        cmp     bFlag,0
        je      MakeUnencrypt

        cmp     bFlag,1
        je      MakeEncrypt

; Disable the encryption
        mov     ReadEncrypt,FALSE
        mov     WriteEncrypt,FALSE
        mov     OverRide,FALSE
        jmp     Done

MakeUnencrypt:
        mov     OverRide,TRUE
        jmp     Done

MakeEncrypt:
        mov     WriteEncrypt,TRUE

Done:
        ret

SetEncryptINI   ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
EncryptHeader   PROC,
        uInput:UINT,
        uOutput:UINT

; Read the INPUT file for a encrypt header
; If exists set ReadEncrypt to TRUE

        mov     ReadEncrypt,FALSE
        INVOKE  FIL4_READ,uInput,ADDR eHeader,sEncryptINI
        or      ax,ax
        jz      NoEncryptInput
        cmp     ax,-1
        je      NoEncryptInput

        cmp     eHeader.Verify,0E5E5h
        jne     NoEncryptInput2

        mov     ReadEncrypt,TRUE

; If OverRide = TRUE
;       set WriteEncrypt = FALSE
; else
;       set WriteEncrypt = TRUE

        mov     WriteEncrypt,FALSE
        cmp     OverRide,TRUE
        je      Done
        mov     WriteEncrypt,TRUE

; If WriteEncrypt = TRUE
;       Write Encrypt Header to Temp file
MakeEncrypt:
        cmp     uOutput,0
        je      Done

        INVOKE  FIL4_WRITE,uOutput,ADDR eHeader,sEncryptINI
        jmp     Done

NoEncryptInput:

        cmp     uOutput,0
        je      NoEncrypt

; Test if we're suppose to encrypt
        cmp     WriteEncrypt,TRUE
        jne     NoEncrypt


        INVOKE  TIM4_ENCODE
        INVOKE  MEM4_COPY,ADDR tHeader,ADDR eHeader,sEncryptINI
        mov     eHeader.eKey,al
        mov     eHeader.Verify,0E5E5h
        jmp     MakeEncrypt

NoEncryptInput2:
        cmp     WriteEncrypt,TRUE
        jne     NoEncrypt

        INVOKE  TIM4_ENCODE
        INVOKE  MEM4_COPY,ADDR tHeader,ADDR eHeader,sEncryptINI
        mov     eHeader.eKey,al
        mov     eHeader.Verify,0E5E5h
        INVOKE  FIL4_WRITE,uOutput,ADDR eHeader,sEncryptINI

DoOverRide:
NoEncrypt:
        INVOKE  FIL4_SEEK,uInput,0,fSEEK_BEG

Done:

        ret

EncryptHeader   ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
Encrypt PROC USES XAX SDS SES XSI XDI,
        lpszBuffer:LPSTR

        cmp     WriteEncrypt,TRUE
        jne     NotEncrypted


        LDS_SI  lpszBuffer
        LES_DI  lpszBuffer

DoMore:
        lodsb
        or      al,al
        jz      Done

        xor     al,eHeader.eKey
        cmp     al,32
        ja      IsMore                  ; Can't be below 32
        xor     al,eHeader.eKey

IsMore:
        stosb
        jmp     DoMore

NotEncrypted:
Done:

        ret

Encrypt ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
UnEncrypt PROC USES XAX SDS SES XSI XDI,
        lpszBuffer:LPSTR

        cmp     ReadEncrypt,TRUE
        jne     NotEncrypted

        LDS_SI  lpszBuffer
        LES_DI  lpszBuffer

DoMore:
        lodsb
        or      al,al
        jz      Done

        xor     al,eHeader.eKey
        cmp     al,32
        ja      IsMore                  ; Can't be below 32
        xor     al,eHeader.eKey

IsMore:
        stosb
        jmp     DoMore

NotEncrypted:
Done:

        ret

UnEncrypt ENDP
;-----------------------------------------------------------------------------
END
