include CTS4.INC

.data
lpBuffer        db      128 dup(0)
uBuffer         dw      128

UniqueSegment
;-----------------------------------------------------------------------------
; GETPRIVATEPROFILESTRING              2/16/93                         CAG
;
;       Get a string from the private INI file.
;
; int WINAPI GetPrivateProfileString(LPCSTR lpszSection,
;                                    LPCSTR lpszEntry,
;                                    LPCSTR lpszDefault,
;                                    LPSTR  lpszReturnBuffer,
;                                    int    cbReturnBuffer,
;                                    LPCSTR lpszFilename);
;
; Input:
;       1)  Section of INI to read from [????]
;       2)  Item to read from INI file
;       3)  Default item to place in return if not found
;       4)  Buffer to fill in
;       5)  Size of buffer
;       6)  File to read from
; Output:
;       1)  Bytes copied to buffer, not including NULL
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
; Note:
;       This is an exact duplicate of the Windows SDK function,
;       and performs the sames actions the Windows version does.
;
;       lpszSection and lpszReturnBuffer CANNOT point to the same element.
;
;-----------------------------------------------------------------------------
GetPrivateProfileString PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpszSection:LPSTR,
        lpszEntry:LPSTR,
        lpszDefault:LPSTR,
        lpszReturnBuffer:LPSTR,
        cbReturnBuffer:WORD,
        lpszFilename:LPSTR

        LOCAL   uHandle:UINT
        LOCAL   uStatus:UINT

; Clear the return buffer first
        INVOKE  MEM4_SET,lpszReturnBuffer,0,cbReturnBuffer

        mov     ax,@data
        mov     ds,ax

        mov     uStatus,0
        mov     uHandle,0

        INVOKE  FIL4_OPEN,lpszFilename,fREAD
        cmp     ax, -1
        jz      DoDefault
        mov     uHandle,ax

        INVOKE  EncryptHeader,uHandle,0

LookForHeader:
        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoDefault

        INVOKE  UnEncrypt,ADDR lpBuffer

        cmp     ax,0                    ; Bytes read = 0, must be blank line
        je      LookForHeader

        lea     si,lpBuffer
        cmp     [XSI],byte ptr '['
        jne     LookForHeader

        inc     si
        INVOKE  MEM4_REPLACE,vSDS XSI,']',0,uBuffer

        INVOKE  STR4_ICOMPARE,vSDS XSI,lpszSection
        or      ax,ax
        jz      LookForHeader

; We're in the right section, now look for the Entry
        cmp     word ptr lpszEntry+2,0
        je      ReadItAll

LookForEntry:
IsComment:
        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoDefault

        INVOKE  UnEncrypt,ADDR lpBuffer

        cmp     ax,0                    ; Bytes read = 0, must be blank line
        je      LookForEntry

        lea     si,lpBuffer
        cmp     [XSI],byte ptr ';'
        je      IsComment
        cmp     [XSI],byte ptr '['
        je      DoDefault
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
FoundIt:
        mov     [XSI],byte ptr 0
        INVOKE  STR4_ICOMPARE,ADDR lpBuffer,lpszEntry
        or      ax,ax
        jz      LookForEntry

        INVOKE  STR4_SRCH_END,ADDR lpBuffer
        mov     XSI,XAX
SkipSpace:
        inc     XSI
        cmp     [XSI],byte ptr ' '
        je      SkipSpace
        cmp     [XSI],byte ptr 9h
        je      SkipSpace

        INVOKE  STR4_COPY,vSDS XSI,lpszReturnBuffer
        mov     uStatus,ax
        jmp     DoClose

ReadItAll:
IsAnotherComment:
IsBlankLine:
        INVOKE  FIL4_READLINE,uHandle,ADDR lpBuffer,uBuffer
        cmp     ax,-1
        je      DoClose

        INVOKE  UnEncrypt,ADDR lpBuffer

        cmp     ax,0                    ; Bytes read = 0, must be blank line
        je      IsBlankLine

        lea     si,lpBuffer

        cmp     [XSI],byte ptr '['
        je      DoClose
        cmp     [XSI],byte ptr ';'
        je      IsAnotherComment

LookMore2:
        cmp     [XSI],byte ptr 0
        je      FoundIt2
        cmp     [XSI],byte ptr '='
        je      FoundIt2
        cmp     [XSI],byte ptr ' '
        je      FoundIt2
        cmp     [XSI],byte ptr 9h
        je      FoundIt2
        inc     XSI
        jmp     LookMore2
FoundIt2:
        mov     [XSI],byte ptr 0

        INVOKE  STR4_ADD_LIST,ADDR lpBuffer,lpszReturnBuffer
        INVOKE  STR4_LENGTH,ADDR lpBuffer
        add     uStatus,XAX
        jmp     ReadItAll

DoDefault:
        INVOKE  STR4_COPY,lpszDefault,lpszReturnBuffer
        mov     uStatus,XAX
DoClose:
        cmp     uHandle,0
        je      OutOfHere
        INVOKE  FIL4_CLOSE,uHandle
OutOfHere:
        mov     XAX,uStatus

        ret

GetPrivateProfileString ENDP
;-----------------------------------------------------------------------------
END
