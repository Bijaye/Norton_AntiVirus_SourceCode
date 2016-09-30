include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; GETPRIVATEPROFILEINT                 2/16/93                         CAG
;
;       Get a INT from the private INI file.
;
; int WINAPI GetPrivateProfileINT(LPCSTR lpszSection,
;                                 LPCSTR lpszEntry,
;                                 int    Default,
;                                 LPCSTR lpszFilename);
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
;-----------------------------------------------------------------------------
GetPrivateProfileINT PROC USES SES XDI,
        lpszSection:LPSTR,
        lpszEntry:LPSTR,
        Default:WORD,
        lpszFilename:LPSTR

        LOCAL   szNothing[2]:BYTE
        LOCAL   lpBuffer[5]:BYTE

        mov     szNothing[0],0

        INVOKE  GetPrivateProfileString,lpszSection,lpszEntry,ADDR szNothing,ADDR lpBuffer,5,lpszFilename

        cmp     lpBuffer,byte ptr 0
        jz      UseDefault

        INVOKE  STR4_ASCII_TO_INT,ADDR lpBuffer
        jmp     GotIt
UseDefault:
        mov     ax,Default
GotIt:
        ret

GetPrivateProfileINT ENDP
;-----------------------------------------------------------------------------
END
